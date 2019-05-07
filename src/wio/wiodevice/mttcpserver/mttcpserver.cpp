/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "mttcpserver.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "wmacros.h"
#include <algorithm>
#include <toolkit.h>
#ifdef POSIX_API
#include <arpa/inet.h>
#endif

using namespace std;
WMTTCPClientCBS::WMTTCPClientCBS(int fd,unsigned id)
:WMTAbstractTCPClient(id)
{
	fd_ = fd;
}
/******************************************************************************/
bool WMTTCPServer::open()
{
	constexpr auto QLEN   = 16;
	auto           l_addr = local_addr();

	if((fd_ = socket(AF_INET,SOCK_STREAM,0)) == -1) {
		ERR("Create tcp server socket faild:%s",strerror(errno));
		return false;
	}

	if(bind(fd_,(const struct sockaddr*)(&l_addr),sizeof(l_addr)) == -1) {
		ERR("bind fd faild:%s",strerror(errno));
		return false;
	}
	if(listen(fd_,QLEN) < 0) {
		ERR("Listen faild:%s.",strerror(errno));
		return false;
	}
	return true;
}
int WMTTCPServer::raw_read(WBuffer* buffer)
{
	struct sockaddr_in addr;
	socklen_t          len;
	auto               fd  = 0;

	buffer->clear();

	if((fd = ::accept(fd_,(struct sockaddr*)&addr,&len)) < 0) {
		return -1;
	}

	if(fd < 0) return -1;

	lock_guard<mutex> guard{mtx_};	

	auto ptr = new WMTTCPClientCBS(fd,id());
	client_buffer_.emplace_back(ptr);
	buffer->append(fd);

	return 4;
}
int WMTTCPServer::raw_write(const WBuffer& /*data*/)
{
	return -1;
}
std::unique_ptr<WMTTCPClientCBS> WMTTCPServer::accept()
{
	lock_guard<mutex> guard{mtx_};

	if(client_buffer_.empty()) return nullptr;

	auto res = std::move(client_buffer_.front());

	client_buffer_.pop_front();

	return res;
}
