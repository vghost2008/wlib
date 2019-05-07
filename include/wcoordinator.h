/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <wthreadtools.h>
#include <chrono>

namespace WSpace
{
	class WCoordinator:public lockable_adapter<std::mutex>
	{
		public:
			WCoordinator();
			~WCoordinator();
			inline void notify_one() { cv_.notify_one();}
			inline void notify_all() { cv_.notify_all();}
			inline bool should_stop()const { return should_stop_;}
			inline void reset() { should_stop_ = false; }
			void request_stop();
			template<class Rep,class Period>
				void wait_for(const std::chrono::duration<Rep,Period>& t) {
					std::unique_lock<std::mutex> guard{lockable()};
					cv_.wait_for(guard,t);
				}
			template<class Rep,class Period,class Predicate>
				void wait_for(const std::chrono::duration<Rep,Period>& t,Predicate p) {
					std::unique_lock<std::mutex> guard{lockable()};
					cv_.wait_for(guard,t,p);
				}
			template< class Clock, class Duration>
				void wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time) {
					std::unique_lock<std::mutex> guard{lockable()};
					cv_.wait_until(guard,timeout_time);
				}
			template< class Clock, class Duration, class Predicate >
				void wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time,Predicate p) {
					std::unique_lock<std::mutex> guard{lockable()};
					cv_.wait_until(guard,timeout_time,p);
				}
			inline void wait() {
				std::unique_lock<std::mutex> guard{lockable()};
				cv_.wait(guard);
			}
			template< class Predicate >
				void wait(Predicate p) {
					std::unique_lock<std::mutex> guard{lockable()};
					cv_.wait(guard,p);
				}
			inline void wait_stop(){ wait([this]()->bool{ return should_stop_;});}
			template< class Clock, class Duration>
				void wait_until_stop(const std::chrono::time_point<Clock, Duration>& timeout_time) {
					wait_until(timeout_time,[this]()->bool{ return should_stop_;});
				}
			template<class Rep,class Period>
				void wait_for_stop(const std::chrono::duration<Rep,Period>& t) {
					wait_for(t,[this]()->bool{ return should_stop_; });
				}
			inline void reset_delayer() { tp_ = std::chrono::steady_clock::now(); }
			/*
			 * 与LoopDelay作用相同
			 */
			template<class Rep,class Period>
				void loop_delay(const std::chrono::duration<Rep,Period>& delay) {
					auto delay_time = std::chrono::duration_cast<std::chrono::milliseconds>(delay-(std::chrono::steady_clock::now()-tp_));

					if(delay_time.count() < 0) {
						/*
						 * 已经超时
						 */
						reset_delayer();
						return;
					}

					tp_ = tp_+delay;
					std::unique_lock<std::mutex> lock{lockable()};
					cv_.wait_for(lock,delay_time);
				}
		private:
			std::condition_variable               cv_;
			std::chrono::steady_clock::time_point tp_;
			std::atomic_bool                      should_stop_  {false};
	};
}
