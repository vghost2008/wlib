#include <heartbeat.h>
#include <boost/crc.hpp>
#include <wlogmacros.h>
#include <wthreadtools.h>
#include <chrono>
#include <sys/types.h>
#include <unistd.h>
#include <boost/scope_exit.hpp>
#include <QTime>

using crc_type = boost::crc_32_type;
using namespace std;
using namespace WSpace;

WHPKModel::WHPKModel(uint32_t id)
:id_(id)
{
}
int WHPKModel::write(WBuffer* data,WIODevice* io)
{
	crc_type crc;
	crc.process_bytes(data->data(),data->size());
	data->append((uint32_t)crc.checksum());
	return next()->write(data,io);
}
int WHPKModel::read(WBuffer* data,WIODevice* io)
{
	auto res = next()->read(data,io);

	if((res<=0) || (data->size()<sizeof(uint32_t)+sizeof(WHBConfig)))
		return res;
	crc_type crc;
	crc.process_bytes(data->data(),data->size()-4);

	if(*data->cast<const uint32_t*>(data->size()-4) != crc.checksum()) {
		ERROR_LOG("Check crc sum faild.");
		return -1;
	}
	auto& config = *data->cast<const WHBConfig*>();
	if(config.id == id_) return 0;

	return  data->size();
}
/******************************************************************************/
WHeartbeat::WHeartbeat(const HBSockConfig& sok_config,const WHBConfig& hb_config)
:hb_config_(hb_config)
{
	io_device_.setLocalAddrAndPort(sok_config.local_addr,sok_config.local_port);
	io_device_.setRemoteAddrAndPort(sok_config.remote_addr,sok_config.remote_port);
	io_device_.setMulticastAddr(sok_config.multicast_addr);
	io_device_.setUDPMode(static_cast<WMTUDPSocketMode>(sok_config.mode));

	hb_config_.id = makeId();

	io_device_.setProtocol<WHPKModel>(hb_config_.id);

	if(!io_device_.open()) {
		throw std::runtime_error("open socket faild.");
	}
	io_device_.setBlock(false);

	init_run(2000);

	thread_ = std::move(thread([this](){ run(); }));
	is_running_ = true;
}
WHeartbeat::~WHeartbeat()
{
	is_running_ = false;
	if(thread_.joinable())
		thread_.join();
	if(work_thread_.joinable())
		work_thread_.join();
}
void WHeartbeat::init_run(unsigned timeout)
{
	WBuffer        data;
	constexpr auto kmin_interval      = 5;
	LoopDelay      delayer;
	constexpr auto kMaxRandomPriority = (kMaxPriority>>2);
	auto           deadline           = chrono::steady_clock::now()+chrono::milliseconds(timeout);
	lock_guard<spinlock_mutex> guard{mtx_};

	BOOST_SCOPE_EXIT_ALL(this){
		if(hb_config_.is_major) hb_config_.priority = kMaxPriority;
	};

	
	while(is_running_) {
		data.clear();
		data.append(hb_config_);
		delayer.delay(chrono::milliseconds(kmin_interval));
		io_device_.send(data);

		if(io_device_.recv(&data)<=0) {
			if(chrono::steady_clock::now() > deadline) {
				hb_config_.is_major = true;
				return;
			}
			continue;
		}

		do {
			auto& config = *data.cast<const WHBConfig*>();

			if(config.is_major != hb_config_.is_major) return;
			if((hb_config_.is_major && (hb_config_.priority<config.priority))
					|| ((!hb_config_.is_major) && (hb_config_.priority>config.priority))) {
				hb_config_.is_major = !hb_config_.is_major;
				break;
			}
			if(hb_config_.priority == config.priority) {
				hb_config_.priority = rand()%kMaxRandomPriority;
			}
			data.clear();
		} while(io_device_.recv(&data)>0);
	}
}
void WHeartbeat::run()
{
	WBuffer   data;
	int       miss_count = 0;
	LoopDelay delayer;

	while(is_running_) {
		data.clear();
		data.append(hb_config_);
		delayer.delay(chrono::milliseconds(hb_config_.heartbeat_interval));

		io_device_.send(data);
		if(io_device_.recv(&data) <= 0) {
			++miss_count;
		} else {
			miss_count = 0;
		}
		do {} while(io_device_.recv(&data)>0);

		if(hb_config_.is_major)
			majorWork();
		else
			minorWork();

		buddy_state_ = (miss_count<hb_config_.max_miss_nr);

		auto& config = *data.cast<const WHBConfig*>();

		if(hb_config_.is_major&&config.is_major) {
			/*
			 * 不应该出现
			 */
			ERROR_LOG("Two major!!!");
		}

		if(hb_config_.is_major || buddy_state_) 
			continue;
		/*
		 * 对方可能已经出现故障，先将自己设为主
		 */
		hb_config_.is_major = true;

		init_run(600);

		if(hb_config_.is_major) {
			thread t([this](){
				toMajor();
			});
			work_thread_ = std::move(t);
		}
	}
}
void WHeartbeat::minorWork()
{
	cout<<QTime::currentTime().toString("hh:mm:ss.zzz").toUtf8().data()<<"Idle..."<<isBuddyOk()<<endl;
}
void WHeartbeat::majorWork()
{
	cout<<QTime::currentTime().toString("hh:mm:ss.zzz").toUtf8().data()<<"Work..."<<isBuddyOk()<<endl;
}
void WHeartbeat::toMajor()
{
}
uint32_t WHeartbeat::makeId()const
{
	const auto id_p0 = io_device_.socket_opt().local_addr>>24;
	const auto id_p1 = uint32_t(getpid());

	return (id_p1&0xffffff)|(id_p0<<24);
}
bool WHeartbeat::isMajor()const
{
	lock_guard<spinlock_mutex> guard{mtx_};

	return hb_config_.is_major;
}
bool WHeartbeat::isBuddyOk()const
{
	return buddy_state_;
}
