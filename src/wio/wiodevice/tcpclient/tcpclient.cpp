/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "tcpclient.h"
#ifdef POSIX_API
#include <arpa/inet.h>
#endif
#include "wmacros.h"
#define TCP_TIMER_INTERVAL 8000
WTCPClient::WTCPClient(unsigned type)
:WAbstractSocket(type)
,timer_id_(-1)
{

}
bool WTCPClient::open()
{
	return tryOpen(timeout_.connect);
}
bool WTCPClient::tryOpen(int time_out)
{
	socket_.connectToHost(QHostAddress(ntohl(socket_opt_.remote_addr)),ntohs(socket_opt_.remote_port));
	if(!socket_.waitForConnected(time_out)) {
		ERR("Connect to host failed!");
		goto out0;
	}
	connect(&socket_,&QTcpSocket::readyRead,this,&WTCPClient::readyRead);
	connect(&socket_,&QTcpSocket::disconnected,this,&WTCPClient::slotDisconnected);
	clearCache();
	if(timer_id_ > 0) {
		killTimer(timer_id_);
		timer_id_ = -1;
	}
	INFO_LOG("Connect to host success");
	return true;
out0:
	if(timer_id_ <= 0) {
		timer_id_ = startTimer(TCP_TIMER_INTERVAL);
	}
	return false;
}
bool WTCPClient::close()
{
	if(timer_id_ > 0) {
		killTimer(timer_id_);
		timer_id_ = -1;
	}
	if(!isValid()) return true;
	socket_.close();
	if(!socket_.waitForDisconnected(10000)) {
		ERR("Disconnect failed!");
		return false;
	}
	return true;
}
int WTCPClient::raw_write(const IOBuffer& data)
{
	const int ret_val = socket_.write(data.data(),data.size());
	if(-1 == ret_val) {
		QSIMPLE_LOG(LL_DEBUG,QString("Write to socket faild,")+socket_.errorString()+QString(",error=%1").arg(socket_.error()));
	}
	socket_.waitForBytesWritten(timeout_.write);
	return ret_val;
}
int WTCPClient::raw_read(IOBuffer* data)
{
	int size = 0;
	data->clear();
	socket_.waitForReadyRead(timeout_.read);
	while(socket_.bytesAvailable()&&data->free_size()) {
		size = socket_.read(data->end(),data->free_size());
		if(size <= 0) break;
		data->add_size(size);
	}
	return data->size();
}
void WTCPClient::slotDisconnected()
{
	socket_.close();
	DBG("Disconnected by server.");
}
void WTCPClient::timerEvent(QTimerEvent* /*event*/)
{
	if(isValid())  return; 
	tryOpen(50);
}
bool WTCPClient::isValid()const 
{ 
	return socket_.state()==QAbstractSocket::ConnectedState; 
}
