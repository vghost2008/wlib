/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <toolkit.h>
#include "wiodevice.h"
#include "wabstractsocket.h"
class WMTAbstractTCPClient:public WAbstractSocket
{
	public:
		using WAbstractSocket::WAbstractSocket;
		virtual int raw_read(IOBuffer* data)override;
};
class WMTTCPClient:public WMTAbstractTCPClient
{
	public:
		using WMTAbstractTCPClient::WMTAbstractTCPClient;
		~WMTTCPClient() { close(); }
		virtual bool open()override;
	private:
		/*
		 * time_out:连接到服务器的超时时间，单位毫秒
		 */
		bool tryOpen(int time_out);
		DISABLE_COPY_AND_ASSIGN(WMTTCPClient);
};
template<typename SOption>
std::unique_ptr<WMTTCPClient> make_tcpclient(const SOption& opt,unsigned id=0)
{
	auto res = std::make_unique<WMTTCPClient>(id);
	res->setSocketOpt(opt);
	res->open();
	return res;
}
