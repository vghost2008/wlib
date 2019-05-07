/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

_Pragma("once")

#include <list>
#include <toolkit.h>
#include "wabstractsocket.h"
#include "mttcpclient.h"

class WMTTCPClientCBS:public WMTAbstractTCPClient
{
	public:
		WMTTCPClientCBS(int fd,unsigned id=0);
};
class WMTTCPServer:public WAbstractSocket
{
	public:
		using WAbstractSocket::WAbstractSocket;
		virtual bool open()override;
		std::unique_ptr<WMTTCPClientCBS> accept();
		virtual int raw_read(WBuffer* data)override;
		virtual int raw_write(const WBuffer& data)override;
	private:
		std::string path_;
		std::mutex  mtx_;    //保护client_buffer_
		std::list<std::unique_ptr<WMTTCPClientCBS>> client_buffer_;
};

template<typename SOption>
std::unique_ptr<WMTTCPServer> make_tcpserver(const SOption& opt,unsigned id=0)
{
	auto res = std::make_unique<WMTTCPServer>(id);
	res->setSocketOpt(opt);
	res->open();
	return res;
}
