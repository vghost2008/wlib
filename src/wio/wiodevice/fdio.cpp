#include <fdio.h>
#include <errno.h>
#include <string.h>
#include <wlogmacros.h>

WFDIO::~WFDIO()
{
	if(isOpen())
		close();
}
bool WFDIO::close()
{
	if(fd_ < 0) return true;
	::close(fd_);
	fd_ = -1;
	return true;
}
bool WFDIO::isValid()const
{
	return fd_>=0;
}
bool WFDIO::isOpen()const
{
	return fd_>=0;
}
int WFDIO::raw_write(const WBuffer& data)
{
	const int ret_val = ::write(fd_,data.data(),data.size());
	if(-1 == ret_val) {
		switch(errno) {
			case EINTR:
				return raw_write(data);
			case EAGAIN:
				DBG("EAGAIN");
				return 0;
			case EBADF:
				fd_ = -1;
				close();
				return -1;
		}
		ERR("Write faild,error=%s",strerror(errno));
	}
	return ret_val;
}
int WFDIO::raw_read(WBuffer* buffer)
{
	buffer->clear();

	if(fd_ < 0) return -1;

	int ret = ::read(fd_,buffer->data(),buffer->dataCapacity());
	if(ret > 0) {
		buffer->resize(ret);
	} else if(0 == ret) {
		return 0;
	} else if(-1 == ret) {
		buffer->clear();
		switch(errno) {
			case EAGAIN:
				break;
			case EBADF:
				ERR("Read faild,error=%s",strerror(errno));
				close();
				return -1;
				break;
			default:
				ERR("Read faild,error=%s",strerror(errno));
				return 0;
				break;
		}
	}
	return buffer->size();
}
