/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <unordered_map>
#include <stdexcept>
#include <wcoordinator.h>
#include <toolkit.h>
#include <memory>
#include <sys/epoll.h>
#include <thread>
#include "wiodevice.h"
/*
 * 用于管理多路读操作
 * CallBack: void (IODevice&,WBuffer& data)
 * 如果io操作出错或io已经关闭会自动从管理器中移走
 */
template<class IOPtr=std::shared_ptr<WIODevice>>
class WIOMultiplexing
{
	public:
		using self_type = WIOMultiplexing<IOPtr>;
	public:
		template<class CB>
			WIOMultiplexing(CB cb){
				epfd_ = epoll_create1(EPOLL_CLOEXEC);
				if(epfd_ < 0) throw std::runtime_error("create epfd faild.");
				t_ = std::move(std::thread([this,cb]{ run(cb);}));
			}
		~WIOMultiplexing();
		bool addIO(IOPtr io);
		bool removeIO(IOPtr io) {
			if(nullptr == io) return false;
			return removeIO(io->fd());
		}
		bool removeIO(IOPtr io,const std::lock_guard<std::mutex>& guard) {
			if(nullptr == io) return false;
			return removeIO(io->fd(),guard);
		}
		bool removeIO(int fd) {
			std::lock_guard<std::mutex> guard{mtx_};
			return removeIO(fd,guard);
		}
		bool removeIO(int fd,const std::lock_guard<std::mutex>& /*guard*/);
		void removeIOById(unsigned id);
		void request_stop() { coordinator_.request_stop(); }
	private:
		template<class CallBack>
			void run(CallBack call_back)
			{
				struct epoll_event events[16];
				auto               res        = 0;
				const auto         timeout    = 2e3; //milliseconds
				WBuffer            data(8192);

				coordinator_.wait([this]{ 
						return coordinator_.should_stop()|| !fd_to_io_.empty(); //fd_to_io_.empty()不加锁仅会产生一些额外时延
						});

				while(!coordinator_.should_stop()) {
					res = epoll_wait(epfd_,events,WSpace::array_size(events),timeout);
					if(0 == res) continue;
					if(-1 == res) {
						ERR("epoll_wait faild:%s",strerror(errno));
						continue;
					}

					std::lock_guard<std::mutex> guard{mtx_};

					for(auto i=0; i<res; ++i) {
						auto& e = events[i];
						if(e.events&(EPOLLERR|EPOLLHUP)) {
							auto it = fd_to_io_.find(e.data.fd);

							ERR("epoll error(id=%d):%s",it->second->id(),strerror(errno));
							if(fd_to_io_.end() == it) continue; 
							it->second->close();
							fd_to_io_.erase(it);
							continue;
						}
						if(!(e.events&(EPOLLIN|EPOLLPRI))) continue;
						auto it = fd_to_io_.find(e.data.fd);
						if(it == fd_to_io_.end()) continue;
						while(it->second->isValid() && (it->second->recv(&data)>0)) {
							call_back(*it->second,data);
						}
						if(!it->second->isValid()) {
							removeIO(it->first,guard);
						}
					}
				}
			}
	private:
		WSpace::WCoordinator coordinator_;
		std::mutex           mtx_;             //保护fd_to_io_
		int                  epfd_        = 0;
		std::thread          t_;
		std::unordered_map<int,IOPtr> fd_to_io_;
};
template<class IOPtr>
bool WIOMultiplexing<IOPtr>::addIO(IOPtr io) {
	epoll_event e;
	if(nullptr == io) return false;
	if(io->fd()<0) return false;
	bzero(&e,sizeof(e));
	e.events = EPOLLIN|EPOLLPRI;
	e.data.fd = io->fd();
	io->setBlock(false);

	std::lock_guard<std::mutex> guard{mtx_};
	if(0 != epoll_ctl(epfd_,EPOLL_CTL_ADD,io->fd(),&e)) {
		ERROR_LOG("epoll_ctl faild:%s",strerror(errno));
		return false;
	}
	fd_to_io_[io->fd()] = io;
	coordinator_.notify_one();
	return true;
}
template<class IOPtr>
WIOMultiplexing<IOPtr>::~WIOMultiplexing()
{
	coordinator_.request_stop();
	if(t_.joinable())
		t_.join();
	if(epfd_>=0) {
		::close(epfd_);
		epfd_ = -1;
	}
}
template<class IOPtr>
bool WIOMultiplexing<IOPtr>::removeIO(int fd,const std::lock_guard<std::mutex>& /*guard*/) {
	auto it = fd_to_io_.find(fd);

	if(fd_to_io_.end() != it)
		fd_to_io_.erase(it);
	epoll_ctl(epfd_,EPOLL_CTL_DEL,fd,nullptr);
	coordinator_.notify_one();
	return true;
}
template<class IOPtr>
void WIOMultiplexing<IOPtr>::removeIOById(unsigned id) {
	std::lock_guard<std::mutex> guard{mtx_};
	for(auto it = fd_to_io_.begin(); it!=fd_to_io_.end(); ++it) {
		if(it->second->id() == id) 
			fd_to_io_.erase(id);
	}
}
template<class IOPtr=std::shared_ptr<WIODevice>>
using WIOMPtr=std::unique_ptr<WIOMultiplexing<IOPtr>>;

template<class CallBack,class IOPtr=std::shared_ptr<WIODevice>>
WIOMPtr<IOPtr> make_iomultiplexing(CallBack func,IOPtr =nullptr)
{
	return std::make_unique<WIOMultiplexing<IOPtr>>(func);
}
/*
 * 使用类及类的成员函数作为回调函数
 */
template<class ClassT, class MemFunc,class IOPtr=std::shared_ptr<WIODevice>>
WIOMPtr<IOPtr> make_iomultiplexing(ClassT* c, MemFunc func,IOPtr _p=nullptr)
{
	auto cb = [c,func](decltype(*_p)& io,WBuffer& data){ (c->*func)(io,data);};
	return std::make_unique<WIOMultiplexing<IOPtr>>(cb);
}
