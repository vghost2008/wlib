/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#ifndef NO_LOG
#include "wlog.h"

#define LOG(level,arg,...) do{ \
	char temp_desc_str[512]; \
	snprintf(temp_desc_str,sizeof(temp_desc_str),arg,##__VA_ARGS__); \
	WLogData w_log_data = {WLog::now(),level,__FILE__,__func__,__LINE__,temp_desc_str,LWM_ALL}; \
	try { \
    WLog::instance().log(std::move(w_log_data)); \
	} catch(...) { \
	} \
}while(0)
#define MASK_LOG(level,mask,arg,fmt...) do{ \
	char temp_desc_str[512]; \
	snprintf(temp_desc_str,sizeof(temp_desc_str),arg,##fmt); \
	WLogData w_log_data = {WLog::now(),level,__FILE__,__func__,__LINE__,temp_desc_str,mask}; \
	try { \
    WLog::instance().log(std::move(w_log_data)); \
	} catch(...) { \
	} \
}while(0)

#define QALL_LOG(level,msg) do{ \
	WLogData w_log_data = {WLog::now(),level,__FILE__,__func__,__LINE__,QString(msg).toUtf8().data(),LWM_ALL}; \
	try { \
    WLog::instance().log(std::move(w_log_data)); \
	} catch(...) { \
	} \
}while(0)

#define LZLOG(id,level,arg,fmt...) do{ \
	char temp_desc_str[512]; \
	snprintf(temp_desc_str,sizeof(temp_desc_str),"LZ: " arg,##fmt); \
	WLogData w_log_data = {WLog::now(),level,__FILE__,__func__,__LINE__,temp_desc_str,LWM_ALL}; \
	try { \
    WLog::instance().lazeLog(id,std::move(w_log_data)); \
	} catch(...) { \
	} \
}while(0)

#define QLZLOG(id,level,msg) do{ \
	WLogData w_log_data = {WLog::now(),level,__FILE__,__func__,__LINE__,std::string("LZ: ")+QString(msg).toUtf8().data(),LWM_ALL}; \
	try { \
    WLog::instance().lazeLog(id,std::move(w_log_data)); \
	} catch(...) { \
	} \
}while(0)

#define SIMPLE_LOG(level,arg,fmt...) do{ \
	char temp_desc_str[512]; \
	snprintf(temp_desc_str,sizeof(temp_desc_str),arg,##fmt); \
	WLogData w_log_data = {WLog::now(),level,__FILE__,__func__,__LINE__,temp_desc_str,LWM_NO_GUI}; \
	try { \
    WLog::instance().log(std::move(w_log_data)); \
	} catch(...) { \
	} \
}while(0)
#define SIMPLE_LOG_TO(level,target,arg,fmt...) do{ \
	char temp_desc_str[512]; \
	snprintf(temp_desc_str,sizeof(temp_desc_str),arg,##fmt); \
	WLogData w_log_data = {WLog::now(),level,__FILE__,__func__,__LINE__,temp_desc_str,target}; \
	try { \
    WLog::instance().log(std::move(w_log_data)); \
	} catch(...) { \
	} \
}while(0)

#define QSIMPLE_LOG(level,msg) do{ \
	WLogData w_log_data = {WLog::now(),level,__FILE__,__func__,__LINE__,QString(msg).toUtf8().data(),LWM_NO_GUI}; \
	try { \
    WLog::instance().log(std::move(w_log_data)); \
	} catch(...) { \
	} \
}while(0)



/*
 * 如果日志已经初始化就记录日志，否则忽略
 */
#define TRY_LOG(level,arg,fmt...) do{ \
	if(WLog::isInited()) { \
	char temp_desc_str[512]; \
	snprintf(temp_desc_str,sizeof(temp_desc_str),arg,##fmt); \
	WLogData w_log_data = {WLog::now(),level,__FILE__,__func__,__LINE__,temp_desc_str,LWM_ALL}; \
	try { \
    WLog::instance().log(std::move(w_log_data)); \
	} catch(...) { \
	} \
	} \
}while(0)

#define SIMPLE_HEX_LOG(level,data,size,arg,fmt...) do{ \
	char temp_desc_str[512]; \
	snprintf(temp_desc_str,sizeof(temp_desc_str),arg "\n",##fmt); \
	WLogData w_log_data = {WLog::now(),level,__FILE__,__func__,__LINE__,std::string(temp_desc_str)+WLog::toHex(data,size),LWM_NO_GUI}; \
	try { \
    WLog::instance().log(std::move(w_log_data)); \
	} catch(...) { \
	} \
} while(0) 

#define LOG_SHOW_VERSION(version) do{ \
try { \
WLog::instance().write("\n"); \
WLog::instance().write("----------------------------------------------------------------------"); \
WLog::instance().write(version); \
WLog::instance().write("----------------------------------------------------------------------"); \
WLog::instance().write("\n"); \
} catch(...) { \
} \
} while(0)

#else  //NO_LOG
/******************************************************************************/
/*
 * 不记录日志使用
 * 所有宏定义为空
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

#define LOG(level,arg,fmt...) do{ \
}while(0)
#define MASK_LOG(level,mask,arg,fmt...) do{ \
}while(0)

#define QALL_LOG(level,msg) do{ \
}while(0)

#define LZLOG(id,level,arg,fmt...) do{ \
}while(0)

#define QLZLOG(id,level,msg) do{ \
}while(0)

#define SIMPLE_LOG(level,arg,fmt...) do{ \
}while(0)
#define SIMPLE_LOG_TO(level,target,arg,fmt...) do{ \
}while(0)

#define QSIMPLE_LOG(level,msg) do{ \
}while(0)

#define TRY_LOG(level,arg,fmt...) do{ \
}while(0)

#define SIMPLE_HEX_LOG(level,data,size,arg,fmt...) do{ \
} while(0) 

#define LOG_SHOW_VERSION(version) do{ \
} while(0)

#endif //NO_LOG


#define QERROR_LOG(arg...) do{ \
QSIMPLE_LOG(LL_ERROR,arg); \
}while(0)
#define QWARNING_LOG(arg...) do{ \
QSIMPLE_LOG(LL_WARNING,arg); \
}while(0)
#define QINFO_LOG(arg...) do{ \
QSIMPLE_LOG(LL_INFO,arg); \
}while(0)
#define QDEBUG_LOG(arg...) do{ \
QSIMPLE_LOG(LL_DEBUG,arg); \
}while(0)
#define ERROR_LOG(arg,fmt...) do{ \
MASK_LOG(LL_ERROR,LWM_NO_GUI,arg,##fmt); \
}while(0)
#define WARNING_LOG(arg,fmt...) do{ \
MASK_LOG(LL_WARNING,LWM_NO_GUI,arg,##fmt); \
}while(0)
#define INFO_LOG(arg,...) do{ \
MASK_LOG(LL_INFO,LWM_NO_GUI,arg,##__VA_ARGS__); \
}while(0)
#define DEBUG_LOG(arg,fmt...) do{ \
MASK_LOG(LL_DEBUG,LWM_NO_GUI,arg,##fmt); \
}while(0)
#define AERROR_LOG(arg,fmt...) do{ \
LOG(LL_ERROR,arg,##fmt); \
}while(0)
#define AWARNING_LOG(arg,fmt...) do{ \
LOG(LL_WARNING,arg,##fmt); \
}while(0)
#define AINFO_LOG(arg,fmt...) do{ \
LOG(LL_INFO,arg,##fmt); \
}while(0)
