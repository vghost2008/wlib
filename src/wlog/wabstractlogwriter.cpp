/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <wabstractlogwriter.h>
#include <sstream>
#include <QtWidgets>
#include <iomanip>
#include "wmacros.h"
#include "wlog.h"
#include "toolkit.h"

using namespace std;

WAbstractLogWriter::WAbstractLogWriter(WLog* log)
:log_(log)
,mask_(0)
{
}
bool WAbstractLogWriter::write(const WLogData& ld)
{
	if(!isCompatible(ld)) return false;
	const string str = logDataToSimpleString(ld);
	return write(str);
}
bool WAbstractLogWriter::write(const string& /*str*/)
{
	return false;
}
bool WAbstractLogWriter::levelWrite(unsigned level,const string& str)
{
	if(level>log_->log_config_.level[maskToIndex()]) return false;
	return write(str);
}
void WAbstractLogWriter::close()
{
}
bool WAbstractLogWriter::isCompatible(const WLogData& ld)const
{
	Q_ASSERT_X(mask_ != 0,__func__,"Error mask");
	if(ld.level<LL_ERROR || ld.level>=LL_NR) {
		ERR("Error level: %d",ld.level);
		return false;
	}
	if(ld.level>log_->log_config_.level[maskToIndex()]) return false;
	if(!(ld.mask&mask_)) return false;
	return true;
}
unsigned WAbstractLogWriter::maskToIndex()const
{
	return WSpace::log2(mask_);
}
string WAbstractLogWriter::time(const WLogData& ld)const 
{
	ostringstream ss;
	auto          date = ld.time.date();
	auto          time = ld.time.time_of_day();

	ss<<setfill('0')<<setw(2)<<int(date.month())<<'/'<<setw(2)<<date.day()<<'/'<<setw(4)<<date.year()<<' '<<time.hours()<<':'<<time.minutes()<<':'<<time.seconds();
	return ss.str();
}
string WAbstractLogWriter::level(const WLogData& ld)const
{
	const static string level_name_array[] = {"","ERROR","WARNING","INFO","DEBUG"};
	if(ld.level<0 || ld.level>=ARRAY_SIZE(level_name_array)) {
		ERR("Error level: %d",ld.level);
		return string();
	}
	return level_name_array[ld.level];
}
string WAbstractLogWriter::codeFileName(const WLogData& ld)const
{
	return ld.file_name;
}
string WAbstractLogWriter::codeFuncName(const WLogData& ld)const
{
	return ld.func_name;
}
string WAbstractLogWriter::codeLine(const WLogData& ld)const
{
	return std::to_string(ld.line);
}
string WAbstractLogWriter::message(const WLogData& ld)const
{
	return ld.msg;
}
QString WAbstractLogWriter::fileSuffix()const
{
	return QString();
}
string WAbstractLogWriter::logDataToSimpleString(const WLogData& ld)const
{
	string       ret_val;

	if(!isCompatible(ld)) return string();
	const WLogConfig& log_config = log_->log_config_;
	ret_val = time(ld) + ": "+level(ld);
	if(log_config.flag&LC_FLAG_SHOW_FILENAME)
		ret_val += string(" FILE=")+codeFileName(ld);
	if(log_config.flag&LC_FLAG_SHOW_FUNC) 
		ret_val += string(" FUNC=")+codeFuncName(ld);
	if(log_config.flag&LC_FLAG_SHOW_LINE) 
		ret_val += string(" LINE=")+codeLine(ld);
	return ret_val+" "+message(ld);
}
QString WAbstractLogWriter::dateTimeToFileName(const QDateTime& date_time)
{
	return QString("LOG_")+date_time.toString("yyMMddhhmmss");
}
QDateTime WAbstractLogWriter::fileNameToDateTime(const QString& file_name)
{
	QDate date;
	QTime time;
	if(file_name.size()<16) return QDateTime();

	date.setDate(file_name.mid(4,2).toInt()+2000,file_name.mid(6,2).toInt(),file_name.mid(8,2).toInt());
	time.setHMS(file_name.mid(10,2).toInt(),file_name.mid(12,2).toInt(),file_name.mid(14,2).toInt());
	return QDateTime(date,time);
}
bool WAbstractLogWriter::deleteLogNrDaysAgo(int nr)
{

	if(fileSuffix().isEmpty()) return false;

	log_->checkLogDirPath();

	int            i              = 0;
	QDateTime      date_line      = QDateTime::currentDateTime().addDays(-nr);
	WLogConfig    &log_config     = log_->log_config_;
	QDir           dir(log_config.dir_path);
	QFileInfoList  file_info_list = dir.entryInfoList(QStringList()<<QString("*")+fileSuffix());

	if(file_info_list.isEmpty()) return false;
	close();	
	for(i=0; i<file_info_list.size(); ++i) {
		const QDateTime temp_date_time = fileNameToDateTime(file_info_list.at(i).baseName());
		if(!temp_date_time.isValid())continue;
		if(temp_date_time  < date_line) {
			QFile::remove(file_info_list.at(i).absoluteFilePath());
		}
	}
	return true;
}
QString WAbstractLogWriter::getLogFilePath()
{
	WLogConfig& log_config = log_->log_config_;
	log_->checkLogDirPath();

	int           i                = 0;
	int           index            = -1;
	QDateTime     max_date         = QDateTime::currentDateTime().addYears(-2);
	QDir          dir(log_config.dir_path);
	QFileInfoList file_info_list   = dir.entryInfoList(QStringList()<<(QString("*")+fileSuffix()));

	if(EVERY_TIME == log_config.create_file_policy) { //每次都使用一个新的文件
		goto out0;
	}

	if(file_info_list.isEmpty()) goto out0;
	/*
	 * 查找最近使用的文件名
	 */
	for(i=0; i<file_info_list.size(); ++i) {
		const QDateTime temp_date_time = fileNameToDateTime(file_info_list.at(i).baseName());
		if(!temp_date_time.isValid())continue;
		if(temp_date_time > max_date) {
			max_date = temp_date_time;
			index = i;
		}
	}

	if(1 == log_config.create_file_policy
			&& max_date.date().day() != QDateTime::currentDateTime().date().day())
		goto out0;

	if(max_date > QDateTime::currentDateTime().addDays(-log_config.create_file_policy)
			&& -1 != index ) {
		return file_info_list.at(index).absoluteFilePath();
	}
out0:
	/*
	 * 生成一个新的文件
	 */
	return log_config.dir_path+QString(QDir::separator())+dateTimeToFileName(QDateTime::currentDateTime())+fileSuffix();
}
bool WAbstractLogWriter::sync()
{
	return true;
}
