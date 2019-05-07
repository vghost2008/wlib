#include <abstractioprotocol.h>
#include <wbuffer.h>
#include <wiodevice.h>

WAbstractIOProtocol::WAbstractIOProtocol()
:errno_(0)
{
}
WAbstractIOProtocol::~WAbstractIOProtocol()
{
}
int WAbstractIOProtocol::write(WBuffer* data,WIODevice* io)
{
	return next_write(data,io);
}
int WAbstractIOProtocol::read(WBuffer* data,WIODevice* io)
{
	return next_read(data,io);
}
bool WAbstractIOProtocol::start()
{
	return true;
}
bool WAbstractIOProtocol::stop()
{
	return true;
}
int WAbstractIOProtocol::next_write(WBuffer* data,WIODevice* io)
{
	if(nullptr != next_) return next_->write(data,io);
	return -1;
}
int WAbstractIOProtocol::next_read(WBuffer* data,WIODevice* io)
{
	if(nullptr != next_) return next_->read(data,io);
	return -1;
}
void WAbstractIOProtocol::setProtocols(WIOProtocols* _protocols,WAbstractIOProtocol* io)
{
	auto& protocols = *_protocols;

	if(protocols.empty()) return;

	for(int i=0; i<protocols.size()-1; ++i) {
		protocols[i].next_ = &protocols[i+1];
	}

	protocols.back().next_ = io;
}
void WAbstractIOProtocol::clearCache()
{
	if(nullptr != next_) next_->clearCache();
	doClearCache();
}
void WAbstractIOProtocol::doClearCache()
{
}
WAbstractIOProtocol* WAbstractIOProtocol::clone()const
{
	throw std::runtime_error("undefined clone function");
}
