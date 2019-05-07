/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "wtask.h"
#include "wmacros.h"
#include <toolkit.h>
#include <algorithm>
#include <wtaskmanager.h>

using namespace std;

WTask::WTask(const QDateTime& begin_time,int flag)
:flag_(flag)
,running_(false)
,manager_(nullptr)
{
	setBeginTime(begin_time);
}
WTask::WTask(const QTime& begin_time,int flag)
:flag_(flag)
,running_(false)
,manager_(nullptr)
{
	if(begin_time.isNull()) {
		setBeginTime(QDateTime());
	} else {
		QDateTime begin_date_time(QDate::currentDate(),begin_time);
		if(begin_date_time < QDateTime::currentDateTime())
			begin_date_time.addDays(1);
		setBeginTime(begin_date_time);
	}
}
void WTask::setBeginTime(const QDateTime& begin_time)
{
	if(begin_time.isNull()) 
		begin_time_ = QDateTime::currentDateTime();
	else
		begin_time_ = begin_time;
}
WTask::~WTask()
{
	if(isRunning()) terminal();
}
bool WTask::terminal()
{
	return false;
}
bool WTask::removeFromManager()
{
	if(!isInManager()) return false;
	return manager_->removeTask(shared_from_this())==TEC_NO_ERROR;
}
QString WTask::info()const
{
	QString res;
	res += "BeginTime:"+begin_time_.toString("yy-MM-dd hh:mm:ss")+"\n";
	res += "\naddr=0x"+QString::number(long(this),16)+"\n";
	return res;
}
/*================================================================================*/
