/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include "wlog_fwd.h"
#include <QtCore>
class WLog;
/*
 * 抽象日志写入器
 */
class WAbstractLogWriter
{
	public:
		WAbstractLogWriter(WLog* log);
		virtual bool write(const WLogData& ld);
		virtual bool levelWrite(unsigned level,const std::string& str);
		virtual bool write(const std::string& str);
		virtual ~WAbstractLogWriter(){}
		virtual void close();
		virtual bool isCompatible(const WLogData& ld)const;
		virtual std::string level(const WLogData& ld)const;
		virtual std::string time(const WLogData& ld)const;
		virtual std::string codeFileName(const WLogData& ld)const;
		virtual std::string codeFuncName(const WLogData& ld)const;
		virtual std::string codeLine(const WLogData& ld)const;
		virtual std::string message(const WLogData& ld)const;
		/*
		 * 文件后辍，包含"."
		 */
		virtual QString fileSuffix()const;
		virtual std::string logDataToSimpleString(const WLogData& ld)const;
		virtual bool deleteLogNrDaysAgo(int nr);
		virtual bool sync();
		unsigned maskToIndex()const;
	public:
		QString getLogFilePath();
		static QString dateTimeToFileName(const QDateTime& date_time);
		static QDateTime fileNameToDateTime(const QString& file_name);
	protected:
		WLog     *log_ = nullptr;
		/*
		 * 当前logwirter的LogWriterMask
		 */
		unsigned mask_ = 0;
};
