/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#include "wiodevice.h"
#include <fcntl.h>
#include "packetmodule.h"
#include "wmacros.h"
#include <wlogmacros.h>
#include <sstream>
#include <errno.h>

using namespace std;
int WIODevice::IOBridge::read(WBuffer* data,WIODevice* /*io*/)
{
	return io_dev_.read(data);
}
int WIODevice::IOBridge::write(WBuffer* data,WIODevice* /*io*/)
{
	return io_dev_.write(data);
}

WIODevice::WIODevice(unsigned id)
:id_(id)
,fd_(-1)
,flag_(0)
,io_monitor_func_(nullptr)
,timeout_{32,32,200}
,io_bridge_(*this)
{
}
WIODevice::~WIODevice()
{
	if(isOpen())
		close();
}
bool WIODevice::open()
{
	for(auto it=protocols_.rbegin(); it!=protocols_.rend(); ++it) {
		it->start();
	}
	return true;
}
bool WIODevice::close()
{
	for(auto it=protocols_.begin(); it!=protocols_.end(); ++it) {
		it->stop();
	}
	return true;
}
bool WIODevice::isOpen()const
{
	return true;
}
int WIODevice::pauseRead()
{
	return 0;
}
int WIODevice::read(WBuffer* data)
{
	int res = raw_read(data);
#ifdef ENABLE_IO_DEVICE_MONITOR
	if(nullptr != io_monitor_func_ && res>0)io_monitor_func_(data->data(),res,WIODD_READ,id_,iom_private_data_);
#endif
	return res;
}
int WIODevice::write(WBuffer* data)
{
	auto res = raw_write(*data);
#ifdef ENABLE_IO_DEVICE_MONITOR
	if(nullptr != io_monitor_func_ && res>0)io_monitor_func_(data->data(),res,WIODD_WRITE,id_,iom_private_data_);
#endif
	return res;
}
int WIODevice::send(const WBuffer& d)
{
	auto data = d;
	return send(&data);
}
int WIODevice::send(WBuffer* data)
{
	if(!protocols_.empty()) {
		return protocols_.front().write(data,this);
	} else {
		return write(data);
	}
	return 0;
}
int WIODevice::recv(WBuffer* data)
{
	data->clear();

	if(!protocols_.empty()) {
		return protocols_.front().read(data,this);
	} else {
		return read(data);
	}
	return 0;
}
void WIODevice::clearCache() 
{
	if(!protocols_.empty()) 
	protocols_.front().clearCache();
}
int WIODevice::getOption(WIODeviceOption option,void *data)const
{
	switch(option) {
		case WIODO_TIMEOUT:
			*(IOTimeout*)data = timeout_;
			break;
		default:
			LOG(LL_ERROR,"Get option faild, option=%d",option);
			return -1;
			break;
	}
	return 0;
}
int WIODevice::setOption(WIODeviceOption option,void *data)
{
	switch(option) {
		case WIODO_TIMEOUT:
			timeout_ = *(IOTimeout*)data;
			break;
		default:
			LOG(LL_ERROR,"Set option faild, option=%d",option);
			return -1;
			break;
	}
	return 0;
}
std::string WIODevice::directionToString(WIODeviceDirection direction)
{
	switch(direction) {
		case WIODD_READ:
			return "读";
			break;
		case WIODD_WRITE:
			return "写";
			break;
		default:
			return "Unknow";
			break;
	}
	return "";
}
string WIODevice::strInfo()const
{
	ostringstream ss;

	ss<<"Id="<<id_<<endl;
	ss<<"Isvalid="<<isValid()<<endl;

	return ss.str();
}
void WIODevice::dumpInfo()const
{
#if WDEBUG_MESSAGE_LEVEL >= WDBG_LEVEL
	cout<<"-------------------------------------->>\n";
	cout<<strInfo();
	cout<<"<<--------------------------------------\n";
#endif
}
int WIODevice::raw_write(const WBuffer& data)
{
	ERROR_LOG("Unexpected.");
	return -1;
}
int WIODevice::raw_read(WBuffer* data)
{
	ERROR_LOG("Unexpected.");
	return -1;
}
int WIODevice::setFDFlag(int flag)
{
	if(fd_ < 0) return -1;
	int flags = fcntl(fd_,F_GETFL,0);
	if(-1 == flags ) {
		LOG(LL_ERROR,"F_GETFL Faild,error=%s",strerror(errno));
		return -1;
	} else {
		fcntl(fd_,F_SETFL,flags|flag);
	}
	return 0;
}
int WIODevice::resetFDFlag(int flag)
{
	if(fd_ < 0) return -1;
	int flags = fcntl(fd_,F_GETFL,0);
	if(-1 == flags ) {
		LOG(LL_ERROR,"F_GETFL Faild,error=%s",strerror(errno));
		return -1;
	} else {
		fcntl(fd_,F_SETFL,flags&(~flag));
	}
	return 0;
}
int WIODevice::getFDFlag()const
{
	if(fd_ < 0) return -1;
	int flags = fcntl(fd_,F_GETFL,0);
	return flags;
}
int WIODevice::setBlock(bool isblock)
{
	if(isblock)
		return resetFDFlag(O_NONBLOCK);
	else
		return setFDFlag(O_NONBLOCK);
	return -1;
}
bool WIODevice::waitForReadyRead(int time_out)
{
	fd_set         rset;
	auto           maxfdp1 = fd()+1;
	struct timeval timeout;

	if(wunlikely(maxfdp1<=0)) {
		return false;
	}
	timeout.tv_sec   =  time_out/1000;
	timeout.tv_usec  =  (time_out%1000)*1000;

	FD_ZERO(&rset);
	FD_SET(fd(),&rset);
	auto res = select(maxfdp1,&rset,nullptr,nullptr,&timeout);

	if(-1 == res) {
		return false;
	} else if(res > 0
			&& wlikely(FD_ISSET(fd(),&rset))) {
		return true;
	} else if(res > 0) {
		ERROR_LOG("Unexpected.");
		return false;
	}
	return false;
}
WAbstractIOProtocol* WIODevice::clone()const
{
	return nullptr;
}
void WIODevice::clearProtocols()
{
	protocols_.clear();
}
void WIODevice::__pushProtocolBack(WAbstractIOProtocol* protocol)
{
	protocols_.push_back(protocol);
	WAbstractIOProtocol::setProtocols(&protocols_,&io_bridge_);
}
void WIODevice::__pushProtocolFront(WAbstractIOProtocol* protocol)
{
	protocols_.insert(protocols_.begin(),protocol);
	WAbstractIOProtocol::setProtocols(&protocols_,&io_bridge_);
}
