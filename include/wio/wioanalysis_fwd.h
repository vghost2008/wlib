/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include "wiodevice_fwd.h"
#include "iomonitor_fwd.h"
#include <QString>
#include <QTime>
#include <wbuffer.h>
/*
 * 分析项描述
 */
struct WIOAnalysisItem
{
	bool            show;
	unsigned        monitor_type;
	WIODataViewMode view_mode;
	QString         title;
	unsigned        read_size;
	unsigned        write_size;
};
struct WIOADataItem
{
	WIOADataItem(const QTime& _t,unsigned _id, unsigned _direct,const QString& _title,const void* d, int s)
		:time(_t),id(_id),direction(_direct),title(_title),data(d,s){}
	QTime    time;
	unsigned id;
	unsigned direction;
	QString  title;
	WBuffer  data;
};
