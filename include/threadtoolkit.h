/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <thread>
#include <mutex>
#include <wmacros.h>
namespace WSpace
{
	class thread_group
	{
		public:
			thread_group();
			~thread_group();
			void join_all();
			template<typename F>
				std::thread* create_thread(F&& threadfunc) {
					std::lock_guard<std::mutex> guard{mtx_};
					std::thread* res = nullptr;
					try {
						auto t = std::thread(std::forward<F>(threadfunc));
						thread_list_.push_back(move(t));
						res = &t;
					} catch(...) {
						return nullptr;
					}
					return  res;
				}
			template<typename F,typename ...Argus>
				std::thread* create_thread(F&& threadfunc, Argus&& ...argus) {
					std::lock_guard<std::mutex> guard{mtx_};
					std::thread* res = nullptr;
					try {
						auto t = std::thread(std::forward<F>(threadfunc),std::forward<Argus>(argus)...);
						thread_list_.push_back(t);
					} catch(...) {
						return nullptr;
					}
					return  res;
				}
		private:
			std::list<std::thread> thread_list_;
			std::mutex       mtx_;
			DISABLE_COPY_AND_ASSIGN(thread_group);
	};
}
