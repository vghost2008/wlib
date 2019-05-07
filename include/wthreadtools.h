/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QReadWriteLock>
#include <atomic>
#include <thread>
#include <mutex>
#include <future>
#include <functional>
#include <chrono>
#include <list>

namespace std
{
	class spinlock_mutex
	{
		public:
			spinlock_mutex();
			void lock();
			void unlock();
		private:
			std::atomic_flag flag_;
	};
	class shared_mutex
	{
		public:
			shared_mutex();
			void lock();
			void unlock();
			void lock_shared();
			void unlock_shared();
			bool try_lock();
			bool try_lock_shared();
		private:
			QReadWriteLock imp_;
	};
	template<class shared_mutex>
		class shared_lock
		{
			public:
				typedef shared_mutex mutex_t;
			public:
				shared_lock(){}
				shared_lock(mutex_t& m):mutex_(&m),is_locked_(true) {
					mutex_->lock_shared();
				}
				shared_lock(mutex_t& m,std::defer_lock_t ):mutex_(&m),is_locked_(false) {
				}
				shared_lock(mutex_t& m,std::adopt_lock_t):mutex_(&m),is_locked_(true) {
				}
				shared_lock(shared_lock&& lock){
					mutex_           =  lock.mutex_;
					is_locked_       =  lock.is_locked_;
					lock.mutex_      =  nullptr;
					lock.is_locked_  =  false;
				}
				~shared_lock() {
					if(is_locked_&&mutex_) {
						mutex_->unlock_shared();
					}
				}
				shared_lock& operator=(shared_lock<mutex_t>&& lock) {
					if(is_locked_&&mutex_) {
						mutex_->unlock_shared();
					}
					mutex_           =  lock.mutex_;
					is_locked_       =  lock.is_locked_;
					lock.mutex_      =  nullptr;
					lock.is_locked_  =  false;
					return *this;
				}
				void lock() {
					if(mutex_&& !is_locked_) {
						mutex_->lock_shared();
						is_locked_ = true;
					}
				}
				void unlock() {
					if(mutex_&& is_locked_) {
						mutex_->unlock_shared();
						is_locked_ = false;
					}
				}
			private:
				mutex_t        *mutex_     = nullptr;
				volatile bool   is_locked_ = false;
				shared_lock(const shared_lock&)=delete;
				shared_lock& operator=(const shared_lock&)=delete;
		};
}
namespace WSpace
{
	//[basic_lockable_adapter
	template <typename BasicLockable>
		class basic_lockable_adapter
		{
			public:
				typedef BasicLockable mutex_type;
			protected:
				mutex_type& lockable() const
				{
					return lockable_;
				}
				mutable mutex_type lockable_; /*< mutable so that it can be modified by const functions >*/
			public:
				basic_lockable_adapter()
				{}

				void lock() const
				{
					lockable().lock();
				}
				void unlock() const
				{
					lockable().unlock();
				}
			private:
				basic_lockable_adapter(const basic_lockable_adapter&)=delete;
				basic_lockable_adapter& operator=(const basic_lockable_adapter&)=delete;

		};
	//]

	//[lockable_adapter
	template <typename Lockable>
		class lockable_adapter : public basic_lockable_adapter<Lockable>
	{
		public:
			typedef Lockable mutex_type;
	};
	//[shared_lockable_adapter
	template <typename SharableLock>
		class shared_lockable_adapter: public lockable_adapter<SharableLock>
	{
		public:
			typedef SharableLock mutex_type;

			void lock_shared() const
			{
				this->lockable().lock_shared();
			}
			void unlock_shared() const
			{
				this->lockable().unlock_shared();
			}
			bool try_lock()const {
				return this->lockable().try_lock();
			}
			bool try_lock_shared()const {
				return this->lockable().try_lock_shared();
			}
	};
	class thread_pool
	{
		public:
			/*
			 * 立即创建thread_num个线程，用于执行可能的任务
			 * 如果没有任务则睡眠
			 */
			thread_pool(int thread_num=std::thread::hardware_concurrency());
			/*
			 * 会等待所有的任务都完成
			 */
			void join_all();
			~thread_pool();
			/*
			 * 向线程池提交一个任务
			 */
			template<class Func,typename...Args>
				inline void submit(Func&& func,Args&&... args) {
					{
						std::lock_guard<std::mutex> guard{mtx_};
						tasks_.emplace_back(std::bind(std::forward<Func>(func),std::forward<Args>(args)...));
					}
					cond_.notify_one();
				}
		private:
			std::mutex mtx_;
			std::condition_variable cond_;
			std::list<std::function<void()>> tasks_; 
			std::vector<std::thread> threads_;
			std::atomic_bool running_;
	};
	template<> inline void thread_pool::submit<std::function<void()>>(std::function<void()>&& func)
	{
		{
			std::lock_guard<std::mutex> guard{mtx_};
			tasks_.push_back(func);
		}
		cond_.notify_one();
	}
	/*
	 * 使用多个线程（具体数量自动选择）中，在一个序列上执行f
	 */
	template <typename Func,typename Iter >
		void parallel_for_each(Iter start, Iter end, const Func& f) {

			const auto count        = std::distance(start,end);
			const auto nthreads     = std::max<int>(4,std::thread::hardware_concurrency());
			const auto blockSize    = std::max<int>(1,count/ nthreads);
			const auto loop_size_m1 = std::min<int>(count,nthreads)-1;

			if(0 == count) return;

			std::vector<std::future<void> > futures;

			auto blockStart = start;
			auto blockEnd   = std::next(blockStart,blockSize);

			if (blockEnd > end) blockEnd = end;

			for (int i = 0; i < loop_size_m1; i++) {
				futures.push_back(
						std::move(std::async(std::launch::async, [blockStart, blockEnd, &f] {
								std::for_each(blockStart,blockEnd,f);
								})));

				blockStart = blockEnd;
				blockEnd = std::next(blockStart, blockSize);
			}
			blockEnd = end;
			futures.push_back(
					std::move(std::async(std::launch::async, [blockStart, blockEnd, &f] {
							std::for_each(blockStart,blockEnd,f);
							})));

			for (auto &future : futures) future.wait();
		}
	/*
	 * 在一个独立的线程中执行parallel_for_each
	 */
	template <typename Func,typename Iter >
		std::thread thread_parallel_for_each(Iter start, Iter end, const Func& f) {
			std::thread t(parallel_for_each,start,end,std::ref(f));
			return t;
		}
	template <typename Func,typename Iter >
		void thread_pool_for_each(thread_pool& tp,Iter start, Iter end, const Func& f) {

			const auto count        = std::distance(start,end);
			const auto nthreads     = std::max<int>(4,std::thread::hardware_concurrency());
			const auto blockSize    = std::max<int>(1,count/ nthreads);
			const auto loop_size_m1 = std::min<int>(count,nthreads)-1;

			if(0 == count) return;

			std::vector<std::future<void> > futures;

			auto blockStart = start;
			auto blockEnd   = std::next(blockStart,blockSize);

			if (blockEnd > end) blockEnd = end;

			for (int i = 0; i < loop_size_m1; i++) {
				std::packaged_task<void()> task([blockStart, blockEnd, &f] {
								std::for_each(blockStart,blockEnd,f);
								});
				futures.push_back(task.get_future());
				tp.submit(std::move(task));
				blockStart = blockEnd;
				blockEnd = std::next(blockStart, blockSize);
			}
			blockEnd = end;
			futures.push_back(
					std::move(std::async(std::launch::async, [blockStart, blockEnd, &f] {
							std::for_each(blockStart,blockEnd,f);
							})));

			for (auto &future : futures) future.wait();
		}
	template <typename Func,typename Iter >
		std::thread ttp_for_each(thread_pool& tp,Iter start, Iter end, const Func& f) {
			std::thread t(thread_pool_for_each,tp,start,end,std::ref(f));
			return t;
		}
	/*
	 * 在一个循环中每次都延迟指定的长度(delay)
	 * delay包含了循环中执行其它指令的时间,可以用于保持循环以固定的节拍执行，而不受其它操作(如IO)时间不确定的影响
	 */
	class LoopDelay
	{
		public:
			inline LoopDelay() {
				reset();
			}
			template<class Rep,class Period>
				void delay(const std::chrono::duration<Rep,Period>& delay) {
					auto delay_time = std::chrono::duration_cast<std::chrono::milliseconds>(delay-(std::chrono::steady_clock::now()-tp_));

					if(delay_time.count() < 0) {
						/*
						 * 已经超时
						 */
						reset();
						return;
					}

					tp_ = tp_+delay;
					std::unique_lock<std::mutex> lock{mtx_};
					cv_.wait_for(lock,delay_time);
				}
			inline void reset() { tp_ = std::chrono::steady_clock::now(); }
		private:
			std::chrono::steady_clock::time_point tp_;
			std::mutex                            mtx_;
			std::condition_variable               cv_;
	};
}
