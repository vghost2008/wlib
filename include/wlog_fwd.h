/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QString>
#include <string>
#include <QDateTime>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
/*
 * 不同的日志写入器可以使用相同的掩码，使用相同掩码的写入器共离相同的日志
 * 级别设置和相同的定向写入指定
 */
enum LogWriterMask
{
	LWM_FILE       =   0x0001,
	LWM_TERMINAL   =   0x0002,
	LWM_GUI        =   0x0004,
	LWM_DATABASE   =   0x0008,
	LWM_ALL        =   0xFFFF,
	LWM_NO_GUI     =   (LWM_ALL&(~LWM_GUI)),
};
/*
 * 配置文件使用
 */
enum WLogType
{
	WLT_OUTPUT_FILE       ,   
	WLT_OUTPUT_TERMINAL   ,   
	WLT_GUI               ,   
	WLT_DATABASE          ,   
	WLT_NR                ,   
};
enum LogLevel
{
	LL_NULL      ,   //不记录日志
	LL_ERROR     ,   
	LL_WARNING   ,   
	LL_INFO      ,   
	LL_DEBUG     ,   
	LL_NR        ,   
};
enum WLogFlag {
	LC_FLAG_SHOW_FILENAME  =    0x0001   ,   
	LC_FLAG_SHOW_FUNC      =    0x0002   ,   
	LC_FLAG_SHOW_LINE      =    0x0004   ,   
};
enum FileCreatePolicy{
	EVERY_TIME =      0,
	ONE_DAY    =      1,
	ONE_WEEK   =      7,
};
struct WLogConfig
{
	LogLevel         level[WLT_NR];
	FileCreatePolicy create_file_policy;   //创建文件的策略，值表示每多少天创建一个文件
	QString          dir_path;
	int              flag;
	unsigned         clear_log_time;       //最多保留多少天的日志
};
struct WLogData
{
	boost::posix_time::ptime time;
	LogLevel                 level;
	std::string              file_name;
	std::string              func_name;
	int                      line;
	std::string              msg;
	unsigned                 mask;
};
