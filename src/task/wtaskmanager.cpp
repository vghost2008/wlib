/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#include <wtaskmanager.h>
#include "wmacros.h"
#include <toolkit.h>
#include <algorithm>
using namespace std;
const int   kTaskManagerTimerInterval   = 200;

WTaskManager* WTaskManager::s_instance = nullptr;
WTaskManager::WTaskManager()
:exclusive_(false)
{
	connect(&timer_,&QTimer::timeout,this,&WTaskManager::runTask);
	timer_.start(kTaskManagerTimerInterval);
}
WTaskManager::~WTaskManager()
{
	timer_.stop();
}
void WTaskManager::runTask()
{
	WTaskReturnFlag    ret_val;
	unique_lock<mutex> guard{mtx_};

	if(task_list_.empty()) {
		return;
	}

	const QDateTime date_time         = QDateTime::currentDateTime();
	const auto      process_dead_line = date_time.addMSecs(kTaskManagerTimerInterval);

	while(date_time>=task_list_.front()->begin_time()) {

		auto task = getFrontTask();

		current_running_task_ = task;
		guard.unlock();

		ret_val=task->run();

		guard.lock();
		current_running_task_.reset();
		task->manager_          =   nullptr;
		guard.unlock();

		switch(ret_val) {
			case TF_REPEAT:
				insertTask(task);
				break;
			case TF_NO_OPERATION:
				break;
			default:
				ERR("Error return value");
				break;
		}

		guard.lock();
		if(task_list_.empty() 
				|| (QDateTime::currentDateTime()>=process_dead_line)) break;
	}
}
shared_ptr<WTask> WTaskManager::getFrontTask()
{
	if(task_list_.empty()) return nullptr;

	auto task = std::move(task_list_.front());

	task_list_.erase(task_list_.begin());

	return task;
}
int WTaskManager::insertTask(shared_ptr<WTask> task)
{
	int ret_val = TEC_NO_ERROR;

	lock_guard<mutex> guard{mtx_};

	if(task_list_.empty()) {
		task_list_.push_front(task);
	} else {
		if(exclusive_) {
			if(find_if(task_list_.begin(),task_list_.end(),[task](shared_ptr<WTask>& t)->bool{ return typeid(*t)==typeid(*task);}) !=task_list_.end()) {
				ERROR_LOG("已经有相同的任务存在");
				ret_val |= TEC_ALREADY_HAVE_SAME_TASK;
				return -1;
			}
		}
		auto it = find_if(task_list_.begin(),task_list_.end(),[task](const shared_ptr<WTask>& t)->bool{ return t->begin_time()>task->begin_time();});
		task_list_.insert(it,task);
	}
	task->setManager(this);
	return ret_val;
}
int WTaskManager::forceInsertTask(shared_ptr<WTask> task)
{
	if(!exclusive_) return insertTask(task);

	{
		unique_lock<mutex> guard{mtx_};
		auto same_task =  find_if(task_list_.begin(),task_list_.end(),[task](shared_ptr<WTask>& t)->bool{ return typeid(*t)==typeid(*task);});
		if(same_task != task_list_.end()) {
			auto t = *same_task;
			guard.unlock();
			removeTask(t);
		} 
	}
	return insertTask(task);
}
int WTaskManager::removeTask(shared_ptr<WTask> task)
{
	{
		lock_guard<mutex> guard{mtx_};
		auto it = find(task_list_.begin(),task_list_.end(),task);
		if(it == task_list_.end()) {
			return TEC_TASK_DOSENT_EXISTS;
		}
		task_list_.erase(it);
		if(task->isRunning())task->terminal();
		task->setManager(nullptr);
	}
	return TEC_NO_ERROR;
}
QString WTaskManager::info()const
{
	QString           res;
	lock_guard<mutex> guard{mtx_};

	res += "Task Num:"+QString::number(task_list_.size())+"\n";
	for(auto task:task_list_) {
		res += "----------->\n";
		res += task->info();
		res += "<-----------\n";
	}
	return res;
}
void WTaskManager::dumpInfo()const
{
	qDebug()<<__func__<<",Size="<<task_list_.size();
	qDebug()<<info();
}
