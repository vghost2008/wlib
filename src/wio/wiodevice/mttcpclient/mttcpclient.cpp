/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "mttcpclient.h"
#include <fcntl.h>
#include <unistd.h>
#include <wlogmacros.h>
#include <errno.h>
#include <QtGlobal>
#ifdef Q_OS_LINUX
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#endif //OS test
#include "toolkit.h"
#include "wmacros.h"

int WMTAbstractTCPClient::raw_read(IOBuffer* buffer)
{
	buffer->clear();

	if(fd_ < 0) return -1;

	int ret = ::read(fd_,buffer->data(),buffer->dataCapacity());
	if(ret > 0) {
		buffer->resize(ret);
	} else if(0 == ret) {
		buffer->clear();
		INFO("Buddy closed.");
		close();
		return -1;
	} else if(-1 == ret) {
		buffer->clear();
		switch(errno) {
			case EAGAIN:
				break;
			case EBADF:
				ERR("Read faild,error=%s",strerror(errno));
				close();
				return -1;
				break;
			default:
				ERR("Read faild,error=%s",strerror(errno));
				return 0;
				break;
		}
	}
	return buffer->size();
}
/******************************************************************************/
bool WMTTCPClient::open()
{
	return tryOpen(timeout_.connect);
}
bool WMTTCPClient::tryOpen(int time_out)
{
	int                res         = 0;
	int                on          = 0;
	int                flags       = 0;
	int                error;
	fd_set             rset;
	fd_set             wset;
	socklen_t          len;
	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;
	struct timeval     tval;

	bzero(&local_addr,sizeof(local_addr));
	local_addr.sin_family        =   AF_INET;
	local_addr.sin_addr.s_addr   =   socket_opt_.local_addr;
	local_addr.sin_port          =   socket_opt_.local_port;

	bzero(&remote_addr,sizeof(remote_addr));
	remote_addr.sin_family        =   AF_INET;
	remote_addr.sin_addr.s_addr   =   socket_opt_.remote_addr;
	remote_addr.sin_port          =   socket_opt_.remote_port;

	if(-1 != fd_) {
		::close(fd_);
		fd_ = -1;
	}
	fd_ = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == fd_) {
		LOG(LL_ERROR,"socket faild,error = %s",strerror(errno));
		goto out0;
	}

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
	if(0==socket_opt_.local_addr || 0==socket_opt_.local_port) goto label0;
	res = bind(fd_,(sockaddr*)&local_addr,sizeof(local_addr));

	if(-1 == res) {
		switch(errno) {
			case EADDRINUSE:
				LOG(LL_ERROR,"本地地址已经在使用");
				break;
			default:
				LOG(LL_ERROR,"Bind Faild,error=%s",strerror(errno));
				break;
		}
		dumpInfo();
		goto out0;
	}
label0:
#ifndef Q_OS_WIN32
	flags = fcntl(fd_,F_GETFL,0);
	if(-1 == flags ) {
		LOG(LL_ERROR,"F_GETFL Faild,error=%s",strerror(errno));
	} else {
		fcntl(fd_,F_SETFL,flags|O_NONBLOCK);
	}
#endif

	if((res = connect(fd_,(struct sockaddr*)&remote_addr,sizeof(remote_addr))) < 0) {
		if(errno != EINPROGRESS) {
			LOG(LL_ERROR,"Connect faild,error=%s",strerror(errno));
			goto out0;
		}
	} 
	if(0 == res) goto out1;

	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_SET(fd_,&rset);
	FD_SET(fd_,&wset);
	tval.tv_sec = time_out/1000;
	tval.tv_usec = (time_out%1000)*1000;
	if((res = select(fd_+1,&rset,&wset,nullptr,&tval)) == 0) {
		LOG(LL_ERROR,"Connect time out");
		goto out0;
	}
	if(FD_ISSET(fd_,&rset) || FD_ISSET(fd_,&wset)) {
		len = sizeof(error);
		error = 0;
		if((res=getsockopt(fd_,SOL_SOCKET,SO_ERROR,(char*)&error,&len))==0
			&& error != 0) {
			LOG(LL_ERROR,"Connect faild,error=%s",strerror(error));
			goto out0;
		} 
	} else {
		LOG(LL_ERROR,"Unexpected , Connect faild");
		goto out0;
	}
out1:
#ifndef Q_OS_WIN32
	fcntl(fd_,F_SETFL,flags);
#endif
	clearCache();
	INFO_LOG("Connect to host success");
	return true;
out0:
	if(-1 != fd_) {
		::close(fd_);
		fd_ = -1;
	}
	return false;
}
