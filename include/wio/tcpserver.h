/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QTcpServer>
#include <QTcpSocket>
#include <list>
#include "wabstractsocket.h"
class WTCPServer:public QObject,public WAbstractSocket
{
	Q_OBJECT
	public:
		WTCPServer(unsigned type=0);
		~WTCPServer() { close(); }
		virtual bool open()override;
		virtual bool close()override;
		virtual bool isValid()const override{ return !client_socket_list_.empty(); }
		inline bool isListening()const { return server_.isListening();}
		virtual int raw_write(const IOBuffer& data)override;
		virtual int raw_read(IOBuffer* data)override;
	protected:
		QTcpSocket* recently_read_socket_;
		friend class Telecontrol;
	private:
		std::list<QTcpSocket*>       client_socket_list_;
		QTcpServer            		 server_;
	private:
		DISABLE_COPY_AND_ASSIGN(WTCPServer);
	private:
		void slotNewConnection();
		void slotDisconnected();
	signals:
		void explain();
		void readyRead();
};
