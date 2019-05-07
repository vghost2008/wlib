/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QUdpSocket>
#include <QHostAddress>
#include "wabstractsocket.h"
/*
 * udp广播模式
 */
enum UdpMode
{
	UM_UNICAST     ,   //向指定地址发送数据
	UM_MULTICAST   ,   //组播方式发送数据
	UM_BROADCAST   ,   //向所有地址广播
};

class WUDPSocket:public QObject,public WAbstractSocket
{
	Q_OBJECT
	public:
		WUDPSocket(unsigned type=0);
		~WUDPSocket() { close();}
		virtual bool open()override;
		virtual bool close()override;
		virtual bool isValid()const override;
		virtual int raw_write(const IOBuffer& data)override;
		virtual int raw_read(IOBuffer* data)override;
		inline void setUDPMode(UdpMode mode){ mode_ = mode;}
	private:
		QUdpSocket     socket_;
		UdpMode        mode_;
	private:
		DISABLE_COPY_AND_ASSIGN(WUDPSocket);
signals:
		void explain();
		void readyRead();
};
