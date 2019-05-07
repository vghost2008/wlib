/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "udpsocket.h"
#include "wmacros.h"
#include <QtGlobal>
WUDPSocket::WUDPSocket(unsigned id)
:WAbstractSocket(id)
,mode_(UM_UNICAST)
{
}
bool WUDPSocket::open()
{
	if(UM_MULTICAST == mode_) {
		int            ttl  = 4;
		//socket_.setSocketOption(QAbstractSocket::MulticastLoopbackOption,0);
		if(!socket_.bind(QHostAddress(ntohs(socket_opt_.local_addr)),ntohs(socket_opt_.local_port),QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint)) {
			QSIMPLE_LOG(LL_ERROR,QString("UDP initconnect failed,")+socket_.errorString()+QString(",error=%1").arg(socket_.error()));
			return false;
		}
		socket_.setSocketOption(QAbstractSocket::MulticastLoopbackOption,1);
		socket_.setSocketOption(QAbstractSocket::MulticastTtlOption,ttl);
		if(!socket_.joinMulticastGroup(QHostAddress(ntohl(socket_opt_.multicast_addr)))) {
			QALL_LOG(LL_ERROR,QString("Join multicast faild:")+socket_.errorString());
		}
	} else {
		if(!socket_.bind(QHostAddress(ntohl(socket_opt_.local_addr)),ntohs(socket_opt_.local_port),QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint)) {
			QSIMPLE_LOG(LL_ERROR,QString("UDP initconnect failed,")+socket_.errorString()+QString(",error=%1").arg(socket_.error()));
			return false;
		}
	}
	connect(&socket_,&QUdpSocket::readyRead,this,&WUDPSocket::readyRead);
	return true;
}
bool WUDPSocket::close() 
{
	socket_.disconnectFromHost();
	socket_.close();
	return true;
}
int WUDPSocket::raw_write(const IOBuffer& data)
{
	int ret_val = 0;
	switch(mode_) {
		case UM_UNICAST:
		case UM_MULTICAST:
			ret_val = socket_.writeDatagram(data.data(),data.size(),QHostAddress(ntohl(socket_opt_.remote_addr)),ntohs(socket_opt_.remote_port));
			break;
		case UM_BROADCAST:
			ret_val = socket_.writeDatagram(data.data(),data.size(),QHostAddress::Broadcast,ntohs(socket_opt_.remote_port));
			break;
		default:
			SIMPLE_LOG(LL_ERROR,"Error mode");
			return 0;
			break;
	}
#if WDEBUG_MESSAGE_LEVEL >= WDBG_LEVEL && defined(TEST_IO)
	if(-1 == ret_val) {
		QSIMPLE_LOG(LL_DEBUG,socket_.errorString()+QString(",error=%1").arg(socket_.error()));
	}
#endif
	return ret_val;
}
int WUDPSocket::raw_read(IOBuffer* buffer)
{
	int size = 0;

	if(!socket_.hasPendingDatagrams()) return 0;
	size = socket_.readDatagram(buffer->data(),buffer->dataCapacity());
	buffer->resize(size);
	return buffer->size();
}
bool WUDPSocket::isValid()const 
{ 
	return socket_.isValid();
}
