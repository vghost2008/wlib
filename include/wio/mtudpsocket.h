/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <toolkit.h>
#include "wabstractsocket.h"
/*
 * udp广播模式
 */
enum WMTUDPSocketMode
{
	MTUM_UNICAST     ,   //向指定地址发送数据
	MTUM_MULTICAST   ,   //组播方式发送数据
	MTUM_BROADCAST   ,   //向所有地址广播
};
class WMTUDPSocket:public WAbstractSocket
{
	public:
		WMTUDPSocket(unsigned id=0);
		~WMTUDPSocket() { close();}
		virtual bool open()override;
		virtual bool close()override;
		virtual bool isValid()const override;
		virtual int raw_write(const IOBuffer& data)override;
		virtual int raw_read(IOBuffer* data)override;
		int sendTo(const IOBuffer& data,struct sockaddr_in addr);
		inline void setUDPMode(WMTUDPSocketMode mode){ mode_ = mode;}
		virtual bool isOpen()const override;
	private:
		WMTUDPSocketMode   mode_;
	private:
		DISABLE_COPY_AND_ASSIGN(WMTUDPSocket);
};
template<typename SOption>
std::unique_ptr<WMTUDPSocket> make_udpsocket(const SOption& opt,unsigned id=0,WMTUDPSocketMode mode=MTUM_UNICAST)
{
	auto res = std::make_unique<WMTUDPSocket>(id);
	res->setSocketOpt(opt);
	res->setUDPMode(mode);
	res->open();
	return res;
}
