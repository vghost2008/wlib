/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "mtudpsocket.h"
#include <unistd.h>
#include <wmacros.h>
#include <wlogmacros.h>
#include <errno.h>
#ifdef POSIX_API
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#endif //OS test

WMTUDPSocket::WMTUDPSocket(unsigned id)
:WAbstractSocket(id)
,mode_(MTUM_UNICAST)
{
	timeout_.read = 3000;
}
bool WMTUDPSocket::open()
{
	int                res          = 0;
	int                on           = 0;
	const int          is_broadcast = (mode_==MTUM_BROADCAST)?1:0;

	bzero(&local_addr_,sizeof(local_addr_));
	local_addr_.sin_family        =   AF_INET;
	local_addr_.sin_addr.s_addr   =   (mode_==MTUM_MULTICAST)?INADDR_ANY:socket_opt_.local_addr;
	local_addr_.sin_port          =   socket_opt_.local_port;

	bzero(&remote_addr_,sizeof(remote_addr_));
	remote_addr_.sin_family        =   AF_INET;
	remote_addr_.sin_addr.s_addr   =   socket_opt_.remote_addr;
	remote_addr_.sin_port          =   socket_opt_.remote_port;

	if(fd_ < 0) fd_ = socket(AF_INET,SOCK_DGRAM,0);
	on = 1;
	res = setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on));
	if(-1 == res) {
		LOG(LL_ERROR,"Reuse addr faild,error=%s",strerror(errno));
	}
#ifndef Q_OS_WIN32
	res = setsockopt(fd_,SOL_SOCKET,SO_REUSEPORT,(char*)&on,sizeof(on));
	if(-1 == res) {
		LOG(LL_ERROR,"Reuse port faild,error=%s",strerror(errno));
	}
#endif

	res = setsockopt(fd_,SOL_SOCKET,SO_BROADCAST,(char*)&is_broadcast,sizeof(is_broadcast));
	if(-1 == res) {
		LOG(LL_ERROR,"设置广播选项失败:%s",strerror(errno));
	}

	res = bind(fd_,(sockaddr*)&local_addr_,sizeof(local_addr_));
	if(-1 == res) {
		switch(errno) {
			case EADDRINUSE:
				LOG(LL_ERROR,"地址已经在使用");
				break;
			default:
				LOG(LL_ERROR,"Bind Faild,error=%s",strerror(errno));
				dumpInfo();
				break;
		}
		goto out0;
	}
	switch(mode_) {
		case MTUM_MULTICAST:
			{
				int            ttl  = 4;
				int            loop = 1;
				struct ip_mreq mreq;


				res = setsockopt(fd_,IPPROTO_IP,IP_MULTICAST_LOOP,(char*)&loop,sizeof(loop));
				if(-1 == res) {
					ERROR_LOG("Set socket multicast loop faild");
				}
				res = setsockopt(fd_,IPPROTO_IP,IP_MULTICAST_TTL,(char*)&ttl,sizeof(ttl));
				if(-1 == res) {
					ERROR_LOG("Set socket multicast TTL faild");
				}
				bzero(&mreq,sizeof(mreq));
				mreq.imr_multiaddr.s_addr = (socket_opt_.multicast_addr);
				mreq.imr_interface.s_addr = (socket_opt_.local_addr);
				//mreq.imr_interface.s_addr = (INADDR_ANY);
				res = setsockopt(fd_,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&mreq,sizeof(mreq));
				if(-1 == res) {
#ifdef Q_OS_LINUX
					switch(errno) {
						case ENOPROTOOPT:
							ERROR_LOG("Unsupport socket operation");
							break;
						case EADDRNOTAVAIL:
							ERROR_LOG("Socket address not available");
							break;
						default:
							LOG(LL_ERROR,"Unknow socket error:%s",strerror(errno));
							break;
					}
#else
					ERROR_LOG("Unsupport socket operation");
#endif
				}
			} 
			break;
		case MTUM_UNICAST:
			break;
		case MTUM_BROADCAST:
			remote_addr_.sin_addr.s_addr = ~0u;
			break;
	}
#if 0
	res = connect(fd_,(const struct sockaddr*)&remote_addr_,sizeof(remote_addr_));
	if(-1 == res) {
		LOG(LL_ERROR,"Connect faild:%s",strerror(errno));
		goto out0;
	}
#endif
	INFO_LOG("Open success");
	return WAbstractSocket::open();
out0:
	if(fd_ >= 0) {
		::close(fd_);
		fd_ = -1;
	}
	return false;
}
bool WMTUDPSocket::close() 
{
	if(fd_ < 0) return true;
	::close(fd_);
	fd_ = -1;
	return WAbstractSocket::close();
}
int WMTUDPSocket::raw_write(const IOBuffer& data)
{
	if(fd_ < 0) return -1;
	int ret_val = 0;
	switch(mode_) {
		case MTUM_UNICAST:
		case MTUM_MULTICAST:
		case MTUM_BROADCAST:
			ret_val = ::sendto(fd_,(char*)data.data(),data.size(),0,(sockaddr*)&remote_addr_,sizeof(remote_addr_));
			break;
		default:
			LOG(LL_ERROR,"Error mode");
			return 0;
			break;
	}
#if 1 //WDEBUG_MESSAGE_LEVEL >= WDBG_LEVEL && defined(TEST_IO)
	if(-1 == ret_val) {
		LOG(LL_ERROR,"Write faild,errno=%s",strerror(errno));
	}
#endif
	return ret_val;
}
int WMTUDPSocket::raw_read(IOBuffer* buffer)
{
	buffer->clear();

	if(fd_ < 0) return -1;

	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
#ifdef REWORKS
	int len = sizeof(addr);
#else
	socklen_t len = sizeof(addr);
#endif
	int ret = recvfrom(fd_,buffer->data(),buffer->dataCapacity(),MSG_DONTWAIT,(sockaddr*)&addr,&len);
	buffer->setSrcInfo(addr);

	if(ret > 0) {
		buffer->resize(ret);
	} else if(ret < 0) {
		buffer->clear();
		//LOG(LL_ERROR,"Read faild,errno=%s,%d",strerror(errno),errno);
		return -1;
	}

	return buffer->size();
}
int WMTUDPSocket::sendTo(const IOBuffer& data,struct sockaddr_in addr)
{
	if(fd_ < 0) return -1;
	return ::sendto(fd_,(char*)data.data(),data.size(),0,(sockaddr*)&addr,sizeof(addr));
}
bool WMTUDPSocket::isValid()const 
{ 
	return fd_>=0;
}
bool WMTUDPSocket::isOpen()const
{
	return fd_>=0;
}
