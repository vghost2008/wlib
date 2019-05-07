/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")

#include <string>
#include <memory>
#include "wiodevice.h"

class WFDIO:public WIODevice
{
	public:
		using WIODevice::WIODevice;
		~WFDIO();
		virtual bool close()override;
		virtual bool isValid()const override;
		virtual bool isOpen()const override;
		virtual int raw_write(const WBuffer& data)override;
		virtual int raw_read(WBuffer* data)override;
};
