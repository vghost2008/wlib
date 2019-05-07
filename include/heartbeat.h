_Pragma("once")
#include <stdint.h>
#include <thread>
#include <mtudpsocket.h>
#include <wabstractsocket.h>
#include <wthreadtools.h>

struct WHBConfig
{
	uint32_t id                 = 0;
	uint32_t heartbeat_interval = 200; //ms
	uint32_t max_miss_nr        = 10;
	int32_t  priority           = 0;
	bool     is_major           = 0;
	char     reserved[3];
};
static_assert(sizeof(WHBConfig)==20,"error size");
struct HBSockConfig
{
	std::string local_addr;
	std::string remote_addr;
	std::string multicast_addr;
	int         local_port;
	int         remote_port;
	unsigned    mode;
};
class WHPKModel:public WAbstractIOProtocol
{
	public:
		WHPKModel(uint32_t id);
		virtual int write(WBuffer* data,WIODevice* io);
		virtual int read(WBuffer* data,WIODevice* io);
	private:	
		uint32_t id_;
};
class WHeartbeat
{
	private:
		using IODev = WMTUDPSocket;
		static constexpr auto kMaxPriority =10000;
	public:
		WHeartbeat(const HBSockConfig& sok_config,const WHBConfig& hb_config=WHBConfig());
		~WHeartbeat();
	public:
		inline bool is_running()const { return is_running_; }
		bool isMajor()const;
		/*
		 * 伙伴状态是否正常
		 */
		bool isBuddyOk()const;
		inline const WHBConfig& config()const { return hb_config_; }
	protected:
		/*
		 * 主周期性执行的事务，不允许有大的延时
		 */
		virtual void majorWork();
		/*
		 * 备周期性执行的事务，不允许有大的延时
		 */
		virtual void minorWork();
		/*
		 * 备切换到主时进行的操作，可以有大延时
		 */
		virtual void toMajor();
	private:
		void run();
		void init_run(unsigned timeout=2000);
		uint32_t makeId()const;
	private:
		WHBConfig                   hb_config_;
		IODev                       io_device_;
		mutable std::spinlock_mutex mtx_;                 //保护hb_config_.is_major
		std::thread                 thread_;
		std::thread                 work_thread_;
		std::atomic_bool            is_running_   {true};
		std::atomic_bool            buddy_state_  {true};
};
