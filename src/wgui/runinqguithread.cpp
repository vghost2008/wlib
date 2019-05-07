#include <wgui/runinqguithread.h>
#include<QDebug>
#include<QTimer>

using namespace std;
WRuninQGUIThreadImp* WRuninQGUIThreadImp::s_instance = nullptr;
WRuninQGUIThreadImp::WRuninQGUIThreadImp()
{
	connect(this,&WRuninQGUIThreadImp::sigRun,this,&WRuninQGUIThreadImp::slotRun,Qt::QueuedConnection);
}
void WRuninQGUIThreadImp::slotRun()
{
	if(busy_) {
		QTimer::singleShot(1000,this,&WRuninQGUIThreadImp::slotRun);
		return ;
	}

	lock_guard<mutex> guard{mtx_};
	busy_ = true;
	try {
		for(auto& f:func_list_) {
			f();
		}
		func_list_.clear();
	} catch(...) {
	}
	busy_ = false;
}
void WRuninQGUIThreadImp::sync()
{
	lock_guard<mutex> guard{mtx_};
	busy_ = true;
	try {
		for(auto& f:func_list_) {
			f();
		}
		func_list_.clear();
	} catch(...) {
	}
	busy_ = false;
}
void winit_runin_qgui_thread()
{
	WRuninQGUIThreadImp::init();
}
void wrunin_qgui_sync()
{
	WRuninQGUIThreadImp::instance().sync();
}
