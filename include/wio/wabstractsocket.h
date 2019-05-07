/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include "wiodevice.h"
#include "fdio.h"
#ifdef POSIX_API
#include <arpa/inet.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <string.h>
/*
 * 网络字节序
 */
struct SocketOption
{
	unsigned       local_addr;
	unsigned       remote_addr;
	unsigned       multicast_addr;
	unsigned short local_port;
	unsigned short remote_port;
};
/*
 * 本地字节序
 */
struct FSocketOption
{
	std::string    local_addr;
	std::string    remote_addr;
	std::string    multicast_addr;
	unsigned short local_port;
	unsigned short remote_port;
};
class WAbstractSocket:public WFDIO 
{
	public:
		WAbstractSocket(unsigned id=0);
		/*
		 * 函数参数为本地字节序
		 */
		static int setLocalAddrAndPort(const std::string& address,unsigned short port,struct SocketOption* option);
		static int setLocalAddr(const std::string& address,struct SocketOption* option);
		static int setLocalPort(unsigned short port,struct SocketOption* option);
		static int setRemoteAddrAndPort(const std::string& address,unsigned short port,struct SocketOption* option);
		static int setRemoteAddr(const std::string& address,struct SocketOption* option);
		static int setRemotePort(unsigned short port,struct SocketOption* option);
		static int setMulticastAddr(const std::string& address,struct SocketOption* option);
		int setLocalAddrAndPort(const std::string& address,unsigned short port);
		int setRemoteAddrAndPort(const std::string& address,unsigned short  port);
		int setMulticastAddr(const std::string& address);
		int setLocalPort(unsigned short port);
		int setRemotePort(unsigned short port);
		int setLocalAddr(const std::string& address);
		int setRemoteAddr(const std::string& address);
		/*
		 * 以下为网络字节序
		 */
		inline int setLocalAddrAndPort(unsigned addr,unsigned short port)
		{
			socket_opt_.local_addr = addr;
			socket_opt_.local_port = port;
			if(isOpen()) {
				bzero(&local_addr_,sizeof(local_addr_));
				local_addr_.sin_family        =   AF_INET;
				local_addr_.sin_addr.s_addr   =   socket_opt_.local_addr;
				local_addr_.sin_port          =   socket_opt_.local_port;
			}
			return 0;
		}
		inline int setRemoteAddrAndPort(unsigned addr,unsigned short  port)
		{
			socket_opt_.remote_addr = addr;
			socket_opt_.remote_port = port;
			if(isOpen()) {
				bzero(&remote_addr_,sizeof(remote_addr_));
				remote_addr_.sin_family        =   AF_INET;
				remote_addr_.sin_addr.s_addr   =   socket_opt_.remote_addr;
				remote_addr_.sin_port          =   socket_opt_.remote_port;
			}
			return 0;
		}

		static SocketOption socket_opt_cast(const FSocketOption& opt);
		void setSocketOpt(const SocketOption& opt);
		void setSocketOpt(const FSocketOption& opt);
		virtual int getOption(WIODeviceOption option,void* data)const override;
		virtual int setOption(WIODeviceOption option,void* data)override;
		static std::string socketOptionToStr(const SocketOption& opt);
		static std::string inet_ntop(unsigned addr);
		static unsigned inet_pton(const std::string& addr);
		inline sockaddr_in local_addr()const { return local_addr_; }
		inline sockaddr_in remote_addr()const { return remote_addr_; }
		inline SocketOption socket_opt()const { return socket_opt_; }
	public:
		virtual std::string strInfo()const override;
	protected:
		SocketOption       socket_opt_;
		struct sockaddr_in local_addr_;
		struct sockaddr_in remote_addr_;
};
