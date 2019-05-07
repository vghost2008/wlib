/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <memory>
#include <type_traits>

enum WTaskFlag{
	TF_NONE          =   0x0000 ,
};
enum WTaskReturnFlag
{
	TF_NO_OPERATION   ,   
	TF_REPEAT         ,   
};
class WTaskManager;
class WTask:public std::enable_shared_from_this<WTask>
{
	public:
		WTask(const QDateTime& begin_time,int flag=0);
		WTask(const QTime& begin_time,int flag=0);
		WTask(int flag=0);
		virtual ~WTask();
		virtual WTaskReturnFlag run()=0;
		void setBeginTime(const QDateTime& begin_time);
		virtual bool terminal();
		inline bool isRunning()const { return running_; }
		/*
		 * 如果任务已经在运行，管理器会调用terminal终止任务
		 */
		bool removeFromManager();
		bool isInManager()const {
			return nullptr != manager_;
		}
		inline int taskFlag()const { return flag_; }
		inline void setTaskFlag(int flag) { flag_ = flag; }
	public:
		virtual QString info()const;
		inline void setManager(WTaskManager* manager) {
			manager_ = manager;
		}
		inline QDateTime begin_time()const { return begin_time_; }
	protected:
		volatile bool running_;
		QDateTime     begin_time_;
	private:
		int           flag_    = 0;
		WTaskManager *manager_ = nullptr;
		friend class  WTaskManager;
};
/*================================================================================*/
template<class Func>
class FuncTask:public WTask
{
	public:
		FuncTask(Func func,const QDateTime& begin_time=QDateTime(),WTaskFlag flag=TF_NONE):WTask(begin_time,flag),func_(func){}
	protected:
		Func func_;
		virtual WTaskReturnFlag run()override;
		WTaskReturnFlag invoke(std::false_type) {
			func_();
			return TF_NO_OPERATION;
		}
		WTaskReturnFlag invoke(std::true_type) {
			return func_();
		}
};
template<class Func>
WTaskReturnFlag FuncTask<Func>::run() 
{
	WTaskReturnFlag ret_flag;
	running_ = true;
	ret_flag = invoke(std::is_same<typename std::result_of<Func()>::type,WTaskReturnFlag>());
	running_ = false;
	return ret_flag;
}
template<typename Func>
std::shared_ptr<FuncTask<Func>> makeFuncTask(Func&& func, const QDateTime& t)
{
	using TaskType = FuncTask<Func>;
	return std::make_shared<TaskType>(std::forward<Func>(func),t);
}
/******************************************************************************/
template<class Func>
class LazyFuncTask:public FuncTask<Func>
{
	public:
		LazyFuncTask(Func func,int time_threshold=2,const QDateTime& begin_time=QDateTime()):FuncTask<Func>(func,begin_time),time_threshold_(time_threshold){}
	protected:
		virtual WTaskReturnFlag run()override;
		const int time_threshold_ = 0;
};
template<class Func>
WTaskReturnFlag LazyFuncTask<Func>::run()
{
	static time_t last_run_time = 0;
	if(time(nullptr)-last_run_time<time_threshold_) return TF_NO_OPERATION;
	last_run_time = time(nullptr);
	return FuncTask<Func>::run();
}
template<typename Func>
std::shared_ptr<LazyFuncTask<Func>> makeLazyFuncTask(Func&& func, int time_threshold=2,const QDateTime& t=QDateTime())
{
	using TaskType = LazyFuncTask<Func>;

	return std::make_shared<TaskType>(std::forward<Func>(func),time_threshold,t);
}
/*================================================================================*/
