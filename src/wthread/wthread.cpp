/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <wthread.h>
#include <wlogmacros.h>
#include <errno.h>
#include <toolkit.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
using namespace std;

WThread::WThread()
{
	
}
WThread::~WThread()
{
	detach();
}
int WThread::start()
{
	if(thread_.get_id() != thread::id{}) {
		ERROR_LOG("线程已经启动");
		return -1;
	}
	thread_ = move(std::thread(WThread::s_run,this));
	return 0;
}
void WThread::s_run(WThread* t)
{
	if(nullptr == t) {
		ERROR_LOG("参数错误");
		return ;
	}

	t->run();
	
	return ;
}
int WThread::detach()
{
	if(thread_.joinable()) {
		try {
			thread_.detach();
		} catch(...) {
		}
		return 0;
	}
	return -1;
}
int WThread::join()
{
	if(thread_.joinable())
		thread_.join();
	return 0;
}
