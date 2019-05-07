/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <wthreadtools.h>
#include <stdexcept>
#include <unistd.h>

namespace std
{
	spinlock_mutex::spinlock_mutex()
	:flag_(ATOMIC_FLAG_INIT)
	{
	}
	void spinlock_mutex::lock()
	{
		while(flag_.test_and_set(std::memory_order_acquire));
	}
	void spinlock_mutex::unlock()
	{
		if(!flag_.test_and_set(std::memory_order_acquire))throw std::runtime_error("unlock spinlock without locked");
		flag_.clear(std::memory_order_release);
	}

	shared_mutex::shared_mutex()
	{
	}
	void shared_mutex::lock()
	{
		imp_.lockForWrite();
	}
	void shared_mutex::unlock()
	{
		imp_.unlock();
	}
	void shared_mutex::lock_shared()
	{
		imp_.lockForRead();
	}
	void shared_mutex::unlock_shared()
	{
		imp_.unlock();
	}
	bool shared_mutex::try_lock()
	{
		return imp_.tryLockForWrite();
	}
	bool shared_mutex::try_lock_shared()
	{
		return imp_.tryLockForRead();
	}
}
namespace WSpace
{
	using namespace std;

	thread_pool::thread_pool(int thread_num) 
	{
		if(thread_num <= 0) thread_num = 2;
		running_ = true;
		for(int i=0; i<thread_num; ++i) {
			threads_.emplace_back([this](){
				while(running_) {
					std::function<void()> fun;
					{
					unique_lock<mutex> guard{mtx_};
					if(tasks_.empty()) {
						cond_.wait_for(guard,chrono::seconds(1));
						continue;
					}
					fun = tasks_.front();
					tasks_.pop_front();
					}
					if(!running_) break;
					fun();
				}
			});
		}
	}
	void thread_pool::join_all()
	{
		{
			unique_lock<mutex> guard{mtx_};
			while(!tasks_.empty()) {
				guard.unlock();
				usleep(3E3);
				guard.lock();
			}
		}

		running_ = false;
		cond_.notify_all();
		for(auto& t:threads_)
			t.join();
	}
	thread_pool::~thread_pool()
	{
		if(running_) throw std::runtime_error("thread is still running");
	}
}
