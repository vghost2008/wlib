/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include "wabstractsocket.h"
#include <QtCore>
#include <QTcpSocket>
#include <QHostAddress>
class WTCPClient:public QObject,public WAbstractSocket
{
	Q_OBJECT
	public:
		WTCPClient(unsigned type=0);
		~WTCPClient() { close(); }
		virtual bool open()override;
		virtual bool close()override;
		virtual bool isValid()const override;
		virtual int raw_write(const IOBuffer& data)override;
		virtual int raw_read(IOBuffer* data)override;
		inline QTcpSocket& socket() { return socket_;}
	protected:
		QTcpSocket     socket_;
		friend class   Telecontrol;
	private:
		int timer_id_;
	private:
		void timerEvent(QTimerEvent* event);
		/*
		 * time_out:连接到服务器的超时时间，单位毫秒
		 */
		bool tryOpen(int time_out);
		DISABLE_COPY_AND_ASSIGN(WTCPClient);
	private:
		void slotDisconnected();
	signals:
		void explain();
		void readyRead();
};
