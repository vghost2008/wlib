/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <list>
#include <wsingleton.h>
#include <wtask.h>
#include <mutex>
#include <algorithm>
#include <memory>

enum WTaskErrorCode 
{
	TEC_NO_ERROR                 =   0x00000000,
	TEC_ALREADY_HAVE_SAME_TASK   =   0x00000001,
	TEC_TASK_DOSENT_EXISTS       =   0x00000002,
};
class WTask;
/*
 * 保证所有的任务都在主线程中执行
 */
class WTaskManager:public QObject,public WSingleton<WTaskManager>
{
	public:
		WTaskManager();
		~WTaskManager();
		/*
		 * 线程安全
		 */
		int insertTask(std::shared_ptr<WTask> task);
		int forceInsertTask(std::shared_ptr<WTask> task);
		/*
		 * 如果任务已经在运行，管理器会调用terminal终止任务
		 */
		int removeTask(std::shared_ptr<WTask> task);
		/*
		 * 默认不互斥
		 */
		inline void setExclusive(bool bexclu) {
			exclusive_ = bexclu;
		}
		template<typename F>
			std::list<std::shared_ptr<WTask>> remove_task_if(F f) {
				std::list<std::shared_ptr<WTask>> res;
				std::lock_guard<std::mutex> guard_{mtx_};
				auto it = stable_partition(task_list_.begin(),task_list_.end(),f);
				res.assign(task_list_.begin(),it);
				task_list_.erase(task_list_.begin(),it);
				return res;
			}
		/*
		 * 删除所有类型为T的任务
		 */
		template<typename T>
			std::list<std::shared_ptr<WTask>> remove_task_type() {
				return remove_task_if([](const std::shared_ptr<WTask>& t)->bool {
					return typeid(*t.get()) == typeid(T); 
				});
			}
	public:
		QString info()const;
		void dumpInfo()const;
	private:
		mutable std::mutex   mtx_;                          //保护task_list_
		bool                 exclusive_            = false;
		std::weak_ptr<WTask> current_running_task_;
		static WTaskManager *s_instance;
		QTimer               timer_;
		std::list<std::shared_ptr<WTask>>  task_list_;
		friend class         WSingleton<WTaskManager>;
	private:
		/*
		 * 将最近需要执行的任务出队列并返回
		 */
		std::shared_ptr<WTask> getFrontTask();
	private:
		void runTask();
};
