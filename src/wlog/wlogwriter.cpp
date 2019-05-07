/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "wlogwriter.h"
#include <QApplication>
#include <QtGui>
#ifdef WLOG_ENABLE_DATABASE_LOG
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#endif
#include "wmacros.h"
#include "wlog.h"
#include "toolkit.h"
#include <sstream>

using namespace std;

WFileLogWriter::WFileLogWriter(WLog* log)
:WAbstractLogWriter(log)
{
	mask_ = LWM_FILE;
}
bool WFileLogWriter::write(const string& str)
{
	if(!file_.isOpen()) {
		file_path_ = getLogFilePath();
		file_.setFileName(file_path_);
		if(!file_.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text)) {
			ERR("Open Log file faild");
			return false;
		}
	}
	file_.write(str.data(),str.size());
	file_.write("\n",1);
#if WDEBUG_MESSAGE_LEVEL >= WDBG_LEVEL
	file_.flush();
#endif
	return true;
}
QString WFileLogWriter::filePath()const
{
	return file_path_;
}
void WFileLogWriter::close()
{
	if(file_.isOpen())file_.close();
}
QString WFileLogWriter::fileSuffix()const
{
	return ".log";
}
WFileLogWriter::~WFileLogWriter()
{
	if(file_.isOpen()) file_.close();
}
/*================================================================================*/
WTerminalLogWriter::WTerminalLogWriter(WLog* log)
:WAbstractLogWriter(log)
{
	mask_ = LWM_TERMINAL;
}
bool WTerminalLogWriter::write(const string& str)
{
	qDebug().noquote()<<str.c_str();
	return true;
}
/*================================================================================*/
#ifdef WLOG_ENABLE_DATABASE_LOG
#define SQL_VALUE(x) "\'"+(x)+"\'"
WDatabaseLogWriter* WDatabaseLogWriter::s_database_log_writer = nullptr;
WDatabaseLogWriter::WDatabaseLogWriter(WLog* log)
:WAbstractLogWriter(log)
,table_name_("Log")
,db_(nullptr)
{
	mask_ = LWM_DATABASE;
	connectDataBase();
}
bool WDatabaseLogWriter::connectDataBase()
{
	log_->checkLogDirPath();
	const QString database_name = log_->log_config_.dir_path+"/log.db";
	const QString user_name     = "root";
	const QString password      = "root";

	db_    =   new QSqlDatabase;
	*db_   =   QSqlDatabase::addDatabase("QSQLITE");
	db_->setDatabaseName(database_name);
	db_->setUserName(user_name);
	db_->setPassword(password);

	if(!QFile::exists(database_name)) {
		if(!db_->open()) {
			ERR("Open database log faild!");
			qDebug()<<db_->lastError().text();
			return false;
		}
		QSqlQuery query(*db_);
		QString   cmd = "CREATE TABLE "+table_name_+" (DateTime SMALLDATETIME, Level VARCHAR(10), FileName VARCHAR(80), FuncName VARCHAR(64), Line TINYINT, Message VARCHAR(256) )";
		if(!query.exec(cmd)) {
			ERR("Create log table faild");
			qDebug()<<query.lastError().text();
			db_->close();
			return false;
		}
	} else {
		if(!db_->open()) {
			ERR("Open database log faild!");
			qDebug()<<db_->lastError().text();
			return false;
		}
	}
	return true;
}
bool WDatabaseLogWriter::write(const WLogData& ld)
{
	if(!isCompatible(ld)) return false;
	if((nullptr == db_ || !db_->isOpen())
			&& !connectDataBase()) return false;
	QSqlQuery query(*db_);
	QString   cmd = QString("INSERT INTO ")+table_name_+" VALUES("+SQL_VALUE(time(ld).c_str())+","+SQL_VALUE(level(ld).c_str())+","+SQL_VALUE(codeFileName(ld).c_str())+","+SQL_VALUE(codeFuncName(ld).c_str())+","+codeLine(ld).c_str()+","+SQL_VALUE(message(ld).c_str())+")";
	if(!query.exec(cmd)) {
		ERR("Inert data faild");
		qDebug()<<query.lastError().text();
		qDebug()<<"CMD="<<cmd;
		return false;
	}
	return true;
}
string WDatabaseLogWriter::dateTimeToString(const boost::posix_time::ptime& dt)const
{
	ostringstream  ss;
	auto           date = dt.date();
	auto           time = dt.time_of_day();

	ss<<date.month()<<'/'<<date.day()<<'/'<<date.year()<<' '<<time.hours()<<':'<<time.minutes()<<':'<<time.seconds();
	return ss.str();
}
string WDatabaseLogWriter::time(const WLogData& ld)const
{
	return dateTimeToString(ld.time);
}
bool WDatabaseLogWriter::deleteLogNrDaysAgo(int nr)
{
	if((nullptr == db_ || !db_->isOpen())
			&&!connectDataBase()) return false;
	auto date_time = WLog::now()-boost::gregorian::date_duration(nr);
	QSqlQuery query(*db_);
	QString cmd = QString("DELETE FROM ")+table_name_+" WHERE DateTime<"+SQL_VALUE(dateTimeToString(date_time).c_str());
	if(!query.exec(cmd)) {
		ERR("Delete recorder faild!");
		qDebug()<<query.lastError().text();
		qDebug()<<"CMD="<<cmd;
		return false;
	}
	return true;
}
WDatabaseLogWriter::~WDatabaseLogWriter()
{
	if(nullptr != db_) {
		if(db_->isOpen())db_->close();
		delete db_;
		db_ = nullptr;
	}
}
WDatabaseLogWriter* WDatabaseLogWriter::instance(WLog* log)
{
	if(nullptr == s_database_log_writer) {
		Q_ASSERT_X(nullptr != log,__func__,"Error WLog");
		s_database_log_writer = new(std::nothrow) WDatabaseLogWriter(log);
	}
	return s_database_log_writer;
}
#endif //WLOG_ENABLE_DATABASE_LOG
/*================================================================================*/
