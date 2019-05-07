/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <wthread.h>
#include <string>
#include "wabstractsocket.h"

class WNTCPServer:public WAbstractSocket
{
	public:
		using WAbstractSocket::WAbstractSocket;
		virtual ~WNTCPServer();
		virtual bool open()override;
		virtual bool close()override;
		virtual bool isValid()const override;
	private:
		bool initServer();
};
