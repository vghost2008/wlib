/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "serialport.h"
#include <stdio.h>
#include <wmacros.h>
#define DEFAULT_BAUDRATE B38400

#ifdef USE_LINUX_LOCAL_SERIALPORT
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#define SERIAL_CASE(x) case x:\
			if(nullptr != speed)*speed = B##x;\
			return true; \
			break
#else
#define SERIAL_CASE(x) case x:\
			if(nullptr != speed)*speed = BAUD##x;\
			return true; \
			break
#endif

WSerialPort::WSerialPort(const SerialPortSetup& setup,unsigned type)
:WIODevice(type)
,setup_(setup)
{
}
#ifdef USE_LINUX_LOCAL_SERIALPORT
bool WSerialPort::open()
{
	int                  flag;
	struct termios       options;
	struct serial_struct ss;
	BaudRateType         speed;

	if(fd_ > 0)  {
		SIMPLE_LOG(LL_INFO,"Serial port already opened!"); 
		goto out1;
	}
	if((fd_ = ::open(setup_.dev.c_str(),O_RDWR|O_NOCTTY|O_NDELAY)) == -1) {
		SIMPLE_LOG(LL_ERROR,"Open serial port faild!");
		goto out1;
	}
	tcgetattr(fd_,&options);
	fcntl(fd_,F_SETFL,0);
	if(!isNormalBaudrate(setup_.baudrate,&speed)) {
		cfsetispeed(&options,DEFAULT_BAUDRATE);
		cfsetospeed(&options,DEFAULT_BAUDRATE);
	} else {
		cfsetispeed(&options,speed);
		cfsetospeed(&options,speed);
	}

	options.c_iflag &= ~(ICRNL|IXON);
	options.c_cflag |= (CLOCAL|CREAD);
	switch(setup_.paren) {
		case SPP_NONE:
			options.c_cflag &= (~PARENB);
			options.c_iflag &= (~INPCK);
			break;
		case SPP_ODD:
			options.c_iflag |= INPCK;
			options.c_cflag |= (PARENB|PARODD);
			break;
		case SPP_EVEN:
			options.c_iflag |= INPCK;
			options.c_cflag |= (PARENB);
			break;
		default:
			SIMPLE_LOG(LL_ERROR,"Error parity");
			break;
	}
	switch(setup_.stopbit) {
		case SPSB_1:
			options.c_cflag &= (~CSTOPB);
			break;
		case SPSB_2:
			options.c_cflag |= (CSTOPB);
			break;
		default:
			SIMPLE_LOG(LL_ERROR,"Error stop bits");
			break;
	}
	options.c_cflag &= (~CSIZE);
	switch(setup_.databit) {
		case 5:
			options.c_cflag |= CS5;
			break;
		case 6:
			options.c_cflag |= CS6;
			break;
		case 7:
			options.c_cflag |= CS7;
			break;
		case 8:
			options.c_cflag |= CS8;
			break;
		default:
			SIMPLE_LOG(LL_ERROR,"Error data bits");
			break;
	}
	if(setup_.hardware_flow_control)
		options.c_cflag |= CRTSCTS; //hand shake
	else
		options.c_cflag &= (~CRTSCTS); //hand shake
	options.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
	options.c_oflag &= ~OPOST;

	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 1;

	flag = tcsetattr(fd_,TCSANOW,&options);
	if(flag < 0) {
		SIMPLE_LOG(LL_ERROR,"Set tscanow failed!");
		goto out0;
	}

	tcflush(fd_,TCIFLUSH);

	if(!isNormalBaudrate(setup_.baudrate)) {
		if(ioctl(fd_,TIOCGSERIAL,&ss) < 0) {
			SIMPLE_LOG(LL_ERROR,"Error to get the serial_struct info:%s",strerror(errno));
			goto out0;
		}
		ss.flags = ASYNC_SPD_CUST;
		ss.custom_divisor = ss.baud_base/setup_.baudrate;
		if(ioctl(fd_,TIOCSSERIAL,&ss) < 0) {
			SIMPLE_LOG(LL_ERROR,"Error to set sreial_struct:%s",strerror(errno));
			goto out0;
		}
	}
	clearCache();
	return true;
out0:
	close();
out1:
	return false;
}
bool WSerialPort::isOpen()const
{
	return fd_ >= 0;
}
bool WSerialPort::isValid()const 
{ 
	return fd_ > 0;
}
bool WSerialPort::close()
{
	if(fd_ > 0) {
		::close(fd_);
		fd_ = -1;
	}
	return true;
}
int WSerialPort::raw_write(const IOBuffer& data)
{
	if(fd_ < 0) {
		ERR("Device dosen't open!");
		return 0;
	}
	return ::write(fd_,data.data(),data.size());
}
#else
bool WSerialPort::open()
{
	if(serial_port_.isOpen())serial_port_.close();
#ifdef Q_OS_WIN32
	serial_port_.setPortName(setup_.dev.c_str());
#else
	serial_port_.setPortName(setup_.dev.c_str());
#endif
	BaudRateType speed;
	if(!isNormalBaudrate(setup_.baudrate,&speed)) {
		SIMPLE_LOG(LL_ERROR,"Error baudrate");
		goto out1;
	}
	serial_port_.setQueryMode(QextSerialPort::Polling);
	if(!serial_port_.open(QIODevice::ReadWrite)) {
		SIMPLE_LOG(LL_ERROR,"Open serialport faild");
		goto out1;
	}
	serial_port_.setBaudRate(speed);
	switch(setup_.databit) {
		case 5: serial_port_.setDataBits(DATA_5); break;
		case 6: serial_port_.setDataBits(DATA_6); break;
		case 7: serial_port_.setDataBits(DATA_7); break;
		case 8: serial_port_.setDataBits(DATA_8); break;
		default: SIMPLE_LOG(LL_ERROR,"ERROR stop bit"); break;
	}
	switch(setup_.paren) {
		case SPP_NONE:serial_port_.setParity(PAR_NONE);break;
		case SPP_ODD:serial_port_.setParity(PAR_ODD);break;
		case SPP_EVEN:serial_port_.setParity(PAR_EVEN);break;
		default: SIMPLE_LOG(LL_ERROR,"ERROR parity mode"); break;
	}
	switch(setup_.stopbit) {
		case SPSB_1:serial_port_.setStopBits(STOP_1);break;
		case SPSB_2:serial_port_.setStopBits(STOP_2);break;
		default: SIMPLE_LOG(LL_ERROR,"ERROR stop bits"); break;
	}
	serial_port_.setFlowControl(FLOW_OFF);
	serial_port_.setTimeout(30);
	return true;
out1:
	return false;
}
bool WSerialPort::isOpen()const
{
	return serial_port_.isOpen();
}
bool WSerialPort::isValid()const 
{ 
	return serial_port_.isOpen();
}
bool WSerialPort::close()
{
	serial_port_.close();
	return true;
}
int WSerialPort::raw_write(const IOBuffer& data)
{
	return serial_port_.write(data.data(),data.size());
}
#endif //USE_LINUX_LOCAL_SERIALPORT

int WSerialPort::raw_read(IOBuffer* data)
{
	data->clear();
#ifdef USE_LINUX_LOCAL_SERIALPORT
	if(fd_ < 0) {
		ERR("Device dosen't open!");
		return 0;
	}
	const int size = ::read(fd_,data->data(),data->dataCapacity());
#else
	const int size = serial_port_.read(data->data(),data->dataCapacity());
#endif
	if(size >= 0) {
		data->resize(size);
	} else {
		data->clear();
		return size;
	}
	return data->size();
}
bool WSerialPort::isNormalBaudrate(unsigned baudrate,BaudRateType* speed)
{
	switch(baudrate) {
		SERIAL_CASE(9600);
		SERIAL_CASE(19200);
		SERIAL_CASE(38400);
		SERIAL_CASE(57600);
		SERIAL_CASE(115200);
	}
	return false;
}
