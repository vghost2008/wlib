/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <string>
#include <memory>
#include <toolkit.h>
#include <sys/un.h>
#include "fdio.h"

class WUnixSocketServer;
/*
 * Stream方式Unix socket client
 * 用于表示当外部连接进入后通过WUnixSocketServer自动创建的socket
 */
class WUnixSocketClientCBS:public WFDIO
{
	private:
		WUnixSocketClientCBS(int fd,unsigned id=0);
		friend WUnixSocketServer;
};
/*
 * Stream方式Unix socket server
 * 打开已经通过raw_read接收外部连接
 * 用户通过accept获取Server已经收到的连接
 */
class WUnixSocketServer:public WFDIO
{
	public:
		WUnixSocketServer(const std::string& path,unsigned id=0);
		virtual bool open()override;
		std::unique_ptr<WUnixSocketClientCBS> accept();
		virtual int raw_read(WBuffer* data)override;
		virtual int raw_write(const WBuffer& data)override;
	private:
		std::string path_;
		std::mutex  mtx_;    //保护client_buffer_
		std::list<std::unique_ptr<WUnixSocketClientCBS>> client_buffer_;
};
/*
 * Stream方式的unix socket client
 */
class WUnixSocketClient:public WFDIO
{
	public:
		WUnixSocketClient(const std::string& path,unsigned id=0);
		virtual bool open()override;
	private:
		std::string path_;
};
/*
 * 数据报方式的unix socket
 */
class WUnixSocket:public WFDIO
{
	public:
		WUnixSocket(const std::string& lpath,const std::string& rpath,unsigned id=0);
		virtual bool open()override;
		virtual int raw_write(const WBuffer& data)override;
	private:
		struct sockaddr_un l_addr_;
		struct sockaddr_un r_addr_;
};
/*
 * 工厂函数
 */
inline std::unique_ptr<WUnixSocketServer> make_unixsocketserver(const std::string& path,unsigned id)
{
	auto res = std::make_unique<WUnixSocketServer>(path,id);
	res->open();
	return res;
}
inline std::unique_ptr<WUnixSocketClient> make_unixsocketclient(const std::string& path,unsigned id)
{
	auto res = std::make_unique<WUnixSocketClient>(path,id);
	res->open();
	return res;
}
inline std::unique_ptr<WUnixSocket> make_unixsocket(const std::string& lpath,const std::string& rpath,unsigned id)
{
	auto res = std::make_unique<WUnixSocket>(lpath,rpath,id);
	res->open();
	return res;
}
