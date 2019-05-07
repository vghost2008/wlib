/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "tcpserver.h"
#include <boost/foreach.hpp>
#include "wmacros.h"
#include <algorithm>
#ifdef POSIX_API
#include <arpa/inet.h>
#endif
using namespace std;
WTCPServer::WTCPServer(unsigned id)
:WAbstractSocket(id)
,recently_read_socket_(nullptr)
{
}
bool WTCPServer::open()
{
	if(!server_.listen(QHostAddress(ntohl(socket_opt_.local_addr)),ntohs(socket_opt_.local_port))) {
		SIMPLE_LOG(LL_ERROR,"Start listen failed!");
		return false;
	}
	client_socket_list_.clear();
	recently_read_socket_ = nullptr;
	connect(&server_,&QTcpServer::newConnection,this,&WTCPServer::slotNewConnection);
	clearCache();
	return true;
}
bool WTCPServer::close()
{
	for(auto it=client_socket_list_.begin(); it!=client_socket_list_.end(); ++it) {
		disconnect(*it,&QTcpSocket::readyRead,this,&WTCPServer::readyRead);
		disconnect(*it,&QTcpSocket::disconnected,this,&WTCPServer::slotDisconnected);
		(*it)->disconnectFromHost();
	}
	client_socket_list_.clear();
	if(!isListening()) return true;
	server_.close();
	return false;
}
void WTCPServer::slotNewConnection()
{
	while(server_.hasPendingConnections()) {
		recently_read_socket_ = server_.nextPendingConnection();
		if(recently_read_socket_) {
			client_socket_list_.push_back(recently_read_socket_);
			connect(recently_read_socket_,&QTcpSocket::readyRead,this,&WTCPServer::readyRead);
			connect(recently_read_socket_,&QTcpSocket::disconnected,this,&WTCPServer::slotDisconnected);
			INFO_LOG("Get connection");
		}
	}
}
void WTCPServer::slotDisconnected()
{
	QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
	list<QTcpSocket*>::iterator it = find(client_socket_list_.begin(),client_socket_list_.end(),socket);
	if(it == client_socket_list_.end()) return ;
	client_socket_list_.erase(it);
	socket->deleteLater();
	DBG("Disconnected by client!");
}
int WTCPServer::raw_write(const IOBuffer& buffer)
{
	
	if(!isValid())  return 0;
	for(auto it = client_socket_list_.begin(); it!=client_socket_list_.end(); ++it) {
		(*it)->write(buffer.data(),buffer.size());
	}
	return 0;
}
int WTCPServer::raw_read(IOBuffer* buffer)
{
	if(!isValid())  return 0;
	buffer->clear();
	int size   = 0;
	buffer->clear();
	BOOST_FOREACH(QTcpSocket* socket,client_socket_list_) {
		while(socket->bytesAvailable()) {
			size = socket->read(buffer->end(),buffer->free_size());
			if(size <= 0) break;
			buffer->add_size(size);
		}
		if(buffer->size() > 0) {
			recently_read_socket_ = socket;
			goto out0;
		}
	}
out0:
	return buffer->size();
}
