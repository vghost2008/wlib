/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <ntcpserver.h>
#include <wmacros.h>
#include <unistd.h>
#include <mttcpclient.h>
#ifdef POSIX_API
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#endif //OS test
#define QLEN 8

WNTCPServer::~WNTCPServer()
{
	if(fd_ >= 0)
		close();
}
bool WNTCPServer::open()
{
	if(fd_>=0) {
		ERROR_LOG("已经启动");
		return false;
	}
	if(fd_ >= 0) {
		::close(fd_);
		fd_ = -1;
	}
	if(0 != initServer()) {
		return false;
	}
	return true;
}
bool WNTCPServer::close()
{
	if(fd_ >= 0) { ::close(fd_); }
	return true;
}
bool WNTCPServer::initServer()
{
	int                on           = 0;
	struct sockaddr_in local_addr;

	bzero(&local_addr,sizeof(local_addr));
	local_addr.sin_family        =   AF_INET;
	local_addr.sin_addr.s_addr   =   socket_opt_.local_addr;
	local_addr.sin_port          =   socket_opt_.local_port;

	if(wunlikely(fd_>=0)) {
		::close(fd_); fd_ = -1;
	}
	if((fd_ = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		ERROR_LOG("Create socket faild:%s",strerror(errno));
		return false;
	}
	on = 1;
	if(setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on)) < 0) {
		LOG(LL_ERROR,"Reuse addr faild,error=%s",strerror(errno));
	}
	if(bind(fd_,(const sockaddr*)&local_addr,sizeof(local_addr)) < 0) {
		switch(errno) {
			case EADDRINUSE:
				LOG(LL_ERROR,"地址已经在使用");
				break;
			default:
				LOG(LL_ERROR,"Bind Faild,error=%s",strerror(errno));
				break;
		}
		goto faild_out0;
	}
	if(listen(fd_,QLEN) < 0) {
		ERROR_LOG("Listen faild:%s",strerror(errno));
		goto faild_out0;
	}
	return true;
faild_out0:
	if(fd_ >= 0) { ::close(fd_); fd_ = -1;}
	return false;
}
bool WNTCPServer::isValid()const
{
	return fd_>=0;
}
