/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <wlib_configs.h>
#include "wabstractlogwriter.h"
class WLog;
/*
 * 文件日志写入器
 */
class WFileLogWriter:public WAbstractLogWriter
{
	public:
		WFileLogWriter(WLog* log);
		virtual QString fileSuffix()const override;
		virtual bool write(const std::string& str) override;
		virtual void close();
		QString filePath()const;
		~WFileLogWriter();
	private:
		QFile file_;
		QString file_path_;
};
/*
 * 终端日志写入器
 */
class WTerminalLogWriter:public WAbstractLogWriter
{
	public:
		WTerminalLogWriter(WLog* log);
		virtual bool write(const std::string& str)override;
};
/*
 * 数据库日志写入器
 */
#ifdef WLOG_ENABLE_DATABASE_LOG
class QSqlDatabase;
class WDatabaseLogWriter:public WAbstractLogWriter
{
	public:
		~WDatabaseLogWriter();
		virtual bool write(const WLogData& ld)override;
		virtual bool deleteLogNrDaysAgo(int nr)override;
		virtual std::string time(const WLogData& ld)const override;
		static WDatabaseLogWriter* instance(WLog* log=nullptr);
	private:
		const QString              table_name_;
		static WDatabaseLogWriter *s_database_log_writer;
		QSqlDatabase              *db_;
	private:
		bool connectDataBase();
		std::string dateTimeToString(const boost::posix_time::ptime& dt)const;
		WDatabaseLogWriter(WLog* log);
};
#endif
