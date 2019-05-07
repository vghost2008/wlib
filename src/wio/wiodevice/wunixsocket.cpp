#include <wunixsocket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <wlogmacros.h>

using namespace std;

struct sockaddr_un to_un_addr(const string& name)
{
	struct sockaddr_un addr;

	bzero(&addr,sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strncpy(addr.sun_path,name.data(),std::min<size_t>(sizeof(addr.sun_path)-1,name.size()));

	return addr;
}
size_t un_addr_len(const struct sockaddr_un& addr)
{
	return offsetof(struct sockaddr_un,sun_path)+strlen(addr.sun_path);
}
/******************************************************************************/
WUnixSocketClientCBS::WUnixSocketClientCBS(int fd,unsigned id)
:WFDIO(id)
{
	fd_ = fd;
}
/******************************************************************************/
WUnixSocketServer::WUnixSocketServer(const std::string& path,unsigned id)
:WFDIO(id)
,path_(path)
{
}
bool WUnixSocketServer::open()
{
	constexpr auto     QLEN = 1;
	struct sockaddr_un addr = to_un_addr(path_);

	if((fd_ = socket(AF_UNIX,SOCK_STREAM,0)) == -1) {
		ERR("Create unix socket faild:%s",strerror(errno));
		return false;
	}

	unlink(addr.sun_path);

	auto len = un_addr_len(addr);

	if(bind(fd_,(const struct sockaddr*)(&addr),len) == -1) {
		ERR("bind fd faild:%s",strerror(errno));
		return false;
	}
	if(listen(fd_,QLEN) < 0) {
		ERR("Listen faild:%s.",strerror(errno));
		return false;
	}
	return true;
}
int WUnixSocketServer::raw_read(WBuffer* buffer)
{
	struct sockaddr_un un;
	socklen_t          len;
	auto               fd  = 0;

	buffer->clear();

	if((fd = ::accept(fd_,(struct sockaddr*)&un,&len)) < 0) {
		return -1;
	}

	if(fd < 0) return -1;

	lock_guard<mutex> guard{mtx_};	
	auto res = new WUnixSocketClientCBS(fd,id());
	client_buffer_.emplace_back(res);
	buffer->append(fd);

	return 4;
}
int WUnixSocketServer::raw_write(const WBuffer& /*data*/)
{
	return -1;
}
std::unique_ptr<WUnixSocketClientCBS> WUnixSocketServer::accept()
{
	lock_guard<mutex> guard{mtx_};
	if(client_buffer_.empty()) return nullptr;
	auto res = std::move(client_buffer_.front());
	client_buffer_.pop_front();
	return res;
}
/******************************************************************************/
WUnixSocketClient::WUnixSocketClient(const string& path,unsigned id)
:WFDIO(id)
,path_(path)
{
}
bool WUnixSocketClient::open()
{
	struct sockaddr_un addr = to_un_addr(path_);

	if((fd_ = socket(AF_UNIX,SOCK_STREAM,0)) == -1) {
		ERR("Create unix socket faild:%s",strerror(errno));
		return false;
	}

	if(connect(fd_,(const struct sockaddr*)&addr,sizeof(addr)) < 0) {
		ERR("Connect to server faild.");
		return false;
	}

	return true;
}
/******************************************************************************/
WUnixSocket::WUnixSocket(const string& lpath,const string& rpath,unsigned id)
:WFDIO(id)
,l_addr_(to_un_addr(lpath))
,r_addr_(to_un_addr(rpath))
{
}
bool WUnixSocket::open()
{
	if((fd_ = socket(AF_UNIX,SOCK_DGRAM,0)) == -1) {
		ERR("Create unix socket faild:%s",strerror(errno));
		return false;
	}

	unlink(l_addr_.sun_path);

	auto len = un_addr_len(l_addr_);

	if(bind(fd_,(const struct sockaddr*)(&l_addr_),len) == -1) {
		ERR("bind fd faild:%s",strerror(errno));
		return false;
	}

	return true;
}
int WUnixSocket::raw_write(const WBuffer& data)
{
	return ::sendto(fd_,(char*)data.data(),data.size(),0,(sockaddr*)&r_addr_,sizeof(r_addr_));
}
