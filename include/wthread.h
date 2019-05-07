/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <thread>
#include <wmacros.h>
#include <string>
#include <atomic>
#include <errno.h>
#include <wthreadtools.h>
#define DEFAULT_THREAD_PRIO 200
class WThread
{
	public:
		WThread();
		virtual ~WThread();
		int start();
		/*
		 * 在start返回之后，run返回之前调用
		 */
		int detach();
		int join();
		inline std::thread& thread() { return thread_; }
		inline const std::thread& thread()const { return thread_; }
	protected:
		std::thread thread_;
	protected:
		/*
		 * 真正的线程函数
		 */
		virtual void run()=0;
	private:
		/*
		 * 线程入口函数
		 */
		static void s_run(WThread* t);
		DISABLE_COPY_AND_ASSIGN(WThread);
};
