/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <wabstractsocket.h>
#include <cassert>
#include <type_traits>
#include <sstream>
#include <wlogmacros.h>
#include <time.h>
#ifndef POSIX_API
#include <QHostAddress>
#include <boost/endian/conversion.hpp>
#endif
using namespace std;
struct sockaddr_in __to_addr_in(unsigned addr,unsigned port)
{
	struct sockaddr_in res;

	bzero(&res,sizeof(res));
	res.sin_family        =   AF_INET;
	res.sin_addr.s_addr   =   addr;
	res.sin_port          =   port;

	return res;
}

WAbstractSocket::WAbstractSocket(unsigned type)
:WFDIO(type)
{
#if __cplusplus >= 201103L
	static_assert(std::is_pod<SocketOption>::value,"Error type");
#endif
	memset(&socket_opt_,0x00,sizeof(socket_opt_));

}
int WAbstractSocket::setLocalAddrAndPort(const string& address,unsigned short port,struct SocketOption* option)
{
	Q_ASSERT_X(nullptr != option,__func__,"Error option");
	option->local_port      =   htons(port);
	return setLocalAddr(address,option);
}
int WAbstractSocket::setLocalAddr(const string& address,struct SocketOption* option)
{
	Q_ASSERT_X(nullptr != option,__func__,"Error option");
	bzero(&option->local_addr,sizeof(option->local_addr));
    option->local_addr = inet_pton(address);

    return 0;
}
int WAbstractSocket::setLocalPort(unsigned short port,struct SocketOption* option)
{
	option->local_port = htons(port);
	return 0;
}
int WAbstractSocket::setLocalAddr(const string& address)
{
	return setLocalAddr(address,&socket_opt_);
}
int WAbstractSocket::setRemoteAddr(const string& address)
{
	return setRemoteAddr(address,&socket_opt_);
}
int WAbstractSocket::setLocalPort(unsigned short port)
{
	socket_opt_.local_port = htons(port);
	local_addr_ = __to_addr_in(socket_opt_.local_addr,socket_opt_.local_port);
	return 0;
}
int WAbstractSocket::setRemotePort(unsigned short port)
{
	socket_opt_.remote_port = htons(port);
	remote_addr_ = __to_addr_in(socket_opt_.remote_addr,socket_opt_.remote_port);
	return 0;
}
int WAbstractSocket::setRemotePort(unsigned short port,struct SocketOption* option)
{
	option->remote_port      =   htons(port);
	return 0;
}
int WAbstractSocket::setRemoteAddrAndPort(const string& address,unsigned short port,struct SocketOption* option)
{
	Q_ASSERT_X(nullptr != option,__func__,"Error option");
	option->remote_port      =   htons(port);
	return setRemoteAddr(address,option);
}
int WAbstractSocket::setRemoteAddr(const string& address,struct SocketOption* option)
{
	Q_ASSERT_X(nullptr != option,__func__,"Error option");

	bzero(&option->remote_addr,sizeof(option->remote_addr));
    option->remote_addr = inet_pton(address);
    return 0;
}
int WAbstractSocket::setMulticastAddr(const string& address,struct SocketOption* option)
{
	Q_ASSERT_X(nullptr != option,__func__,"Error option");
	bzero(&option->multicast_addr,sizeof(option->multicast_addr));
    option->multicast_addr = inet_pton(address);
    return 0;
}
int WAbstractSocket::setLocalAddrAndPort(const string& address,unsigned short port)
{
	return setLocalAddrAndPort(address,port,&socket_opt_);
}
int WAbstractSocket::setRemoteAddrAndPort(const string& address,unsigned short port)
{
	return setRemoteAddrAndPort(address,port,&socket_opt_);
}
int WAbstractSocket::setMulticastAddr(const string& address)
{
	return setMulticastAddr(address,&socket_opt_);
}
int WAbstractSocket::getOption(WIODeviceOption option,void* data)const
{
	switch(option) {
		case WIODO_SOCKET:
			*(SocketOption*)data  = socket_opt_;
			break;
		default:
			return WIODevice::getOption(option,data);
			break;
	}
	return true;
}
int WAbstractSocket::setOption(WIODeviceOption option,void* data)
{
	switch(option) {
		case WIODO_SOCKET:
			socket_opt_ = *(SocketOption*)data;
			local_addr_ = __to_addr_in(socket_opt_.local_addr,socket_opt_.local_port);
			remote_addr_ = __to_addr_in(socket_opt_.remote_addr,socket_opt_.remote_port);
			break;
		default:
			return WIODevice::setOption(option,data);
			break;
	}
	return true;
}
string WAbstractSocket::strInfo()const
{
	string info = WIODevice::strInfo()+"\n";
	info += socketOptionToStr(socket_opt_);
	return info;
}
string WAbstractSocket::socketOptionToStr(const SocketOption& opt)
{
	stringstream ss;

    ss<<"LocalAddress:"<<inet_ntop(opt.local_addr)<<","<<ntohs(opt.local_port)<<endl;
    ss<<"RemoteAddress:"<<inet_ntop(opt.remote_addr)<<":"<<ntohs(opt.remote_port);

	return ss.str();
}
unsigned WAbstractSocket::inet_pton(const string& addr)
{
#ifdef POSIX_API
    unsigned res = 0;
	::inet_pton(AF_INET,addr.c_str(),&res);
    return res;
#else
    QHostAddress ha(QString(addr.c_str()));
    return boost::endian::endian_reverse(ha.toIPv4Address());
#endif

}
string WAbstractSocket::inet_ntop(unsigned addr)
{
#ifdef POSIX_API
	char buffer[32];
	::inet_ntop(AF_INET,&addr,buffer,sizeof(buffer));
    return buffer;
#else
    return QHostAddress(boost::endian::endian_reverse(addr)).toString().toUtf8().data();
#endif
}
SocketOption WAbstractSocket::socket_opt_cast(const FSocketOption& opt)
{
	SocketOption res;
	setLocalAddrAndPort(opt.local_addr,opt.local_port,&res);
	setRemoteAddrAndPort(opt.remote_addr,opt.remote_port,&res);
	setMulticastAddr(opt.multicast_addr,&res);
	return res;
}
void WAbstractSocket::setSocketOpt(const SocketOption& opt)
{
	setOption(WIODO_SOCKET,const_cast<SocketOption*>(&opt));
}
void WAbstractSocket::setSocketOpt(const FSocketOption& opt)
{
	setSocketOpt(socket_opt_cast(opt));
}
