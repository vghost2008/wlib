_Pragma("once")
#include <QObject>
#include <future>
#include <mutex>
#include <atomic>
#include <wsingleton.h>

class WRuninQGUIThreadImp:public QObject,public WSingleton<WRuninQGUIThreadImp>
{
	Q_OBJECT
	public:
		template <class Func>
			std::future<bool> doRun(Func&& func) {
				std::lock_guard<std::mutex> guard{mtx_};
				std::packaged_task<bool()> task(std::forward<Func>(func));
				auto fu = task.get_future();
				func_list_.emplace_back(std::move(task));
				emit sigRun();
				return fu;
			}
		void sync();
	private:
		std::mutex                  mtx_;
		std::atomic_bool            busy_{false};
		static WRuninQGUIThreadImp *s_instance;
	private:
		friend WSingleton<WRuninQGUIThreadImp>;
		WRuninQGUIThreadImp();
		void slotRun();
		std::list<std::packaged_task<bool()>> func_list_;
signals:
		void explain();
		void sigRun();
};
/*
 * 初始化，必须在Qt GUI线程中调用
 */
void winit_runin_qgui_thread();
/*
 * 立即执行所有操作
 */
void wrunin_qgui_sync();
/*
 * 让可调用对象func在Qt GUI线程中执行，函数本身立即返回
 * func稍后执行
 * 可用future等待func执行完成
 * 执行的操作为非紧急操作，可能会有秒级的时延
 */
template<class Func>
std::future<bool> wrunin_qgui_thread(Func&& func)
{
	return WRuninQGUIThreadImp::instance().doRun(std::forward<Func>(func));
}
