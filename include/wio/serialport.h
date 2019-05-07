/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <string>
#include <toolkit.h>
#include "wiodevice.h"
#ifdef POSIX_API
#define USE_LINUX_LOCAL_SERIALPORT
#endif
#ifdef USE_LINUX_LOCAL_SERIALPORT
#include <termios.h>
typedef speed_t BaudRateType;
#else
#include "qextserialport.h"
#endif
/*
 * 串口停止位
 */
enum SerialPortStopBit
{
	SPSB_1    ,
	SPSB_2    ,
};

/*
 * 串口校验模式
 */
enum SerialPortParen
{
	SPP_NONE   ,
	SPP_ODD    ,
	SPP_EVEN   ,
};
/*
 * 串口设置
 */
struct SerialPortSetup
{
	std::string       dev; //Linux为/dev/ttyS[012345...],windows为COM[1234567...]
	unsigned          baudrate; //波特率
	unsigned          databit; //数据位
	SerialPortStopBit stopbit; //停止位
	SerialPortParen   paren; //校验
	bool              hardware_flow_control; //是否启用硬件流控
};

class WSerialPort:public WIODevice
{
	public:
		WSerialPort(const SerialPortSetup& setup,unsigned id);
		virtual bool open()override;
		virtual bool close()override;
		virtual int raw_write(const IOBuffer& ba)override;
		virtual int raw_read(IOBuffer* ba)override;
		virtual bool isValid()const override;
		virtual bool isOpen()const override;
	private:
		SerialPortSetup setup_;
#ifndef USE_LINUX_LOCAL_SERIALPORT
		QextSerialPort serial_port_;
#endif
	private:
		static bool isNormalBaudrate(unsigned baudrate,BaudRateType* speed=nullptr);
		DISABLE_COPY_AND_ASSIGN(WSerialPort);
};
inline std::unique_ptr<WSerialPort> make_serialport(const SerialPortSetup& setup,unsigned id=0)
{
	auto res = std::make_unique<WSerialPort>(setup,id);
	res->open();
	return res;
}
