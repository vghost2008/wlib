/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include "sfifo.h"
#include <atomic>
#include <stdexcept>
#include <wsingleton.h>
#include "wlog_fwd.h"
#include "cmacros.h"
#include <wthreadtools.h>

class WLog;
class WLogThread:public QThread
{
	public:
		WLogThread(WLog* log);
		bool stop();
	private:
		WLog             *log_;
		const unsigned    queue_size_limit_;
		std::atomic_bool  run_;
		friend            WLog;
	private:
		void run()override;
};
class WAbstractLogWriter;
class WLog:public WSingleton<WLog>
{
	public:
		WLogConfig          log_config_;
	public:
		static bool readConfig(const QString& config_path,const QString& log_dir_path,WLogConfig* config);
		static std::string toHex(const std::string& ba);
		static std::string toHex(const void* data,size_t size);
		template<class T,typename=typename std::enable_if<std::is_same<WLogData,typename std::decay<T>::type>::value>::type>
			bool log(T&& data)
			{
				if(!work_thread_.run_) {
					log_data_queue_.clear();
					ERR("Work thread is going to stop.");
					goto out0;
				}
				log_data_queue_.enqueue(std::forward<T>(data));
				if(!work_thread_.isRunning()) work_thread_.start();
out0:
				return true;
			}
		template<class T,typename=typename std::enable_if<std::is_same<WLogData,typename std::decay<T>::type>::value>::type>
			bool lazeLog(int& id,T&& data)
			{
				if(!testLazeId(id)) return false;
				return log(std::forward<T>(data));
			}
		bool deleteLogNrDaysAgo(int nr);
		bool write(const std::string& str);
		bool insertLogWriter(WAbstractLogWriter* log_writer);
		void initGUIWriter();
		void initDatabaseWriter();
		bool sync();
		bool checkLogDirPath();
		bool clearLog();
		bool checkConfig(WLogConfig* config);
		/*
		 * 分配一个慢写入logid,
		 */
		static int allocLazeId();
		static bool releaseLazeId(int id);
		static void setLazeTimeInterval(unsigned v);
		static bool testLazeId(int& id);
		~WLog();
		template<class Writer>
			Writer& getWriter() {
				for(auto w:log_writer_list_) {
					auto res = dynamic_cast<Writer*>(w);
					if(nullptr != res) return *res;
				}
				throw std::runtime_error("null writer");
			}
		void removeAllWriter();
	public:
		static bool sameLogWriter(const WAbstractLogWriter* log_writer0,const WAbstractLogWriter* log_writer1);
		static boost::posix_time::ptime now();
	private:
		WLogThread        work_thread_;
		WSFIFO<WLogData>  log_data_queue_;
		unsigned          laze_time_interval_;   //慢写入log的时间间隔
		static WLog      *s_instance;
		std::shared_mutex  shared_lock_; //保护log_writer_list_,last_laze_write_clock_
		std::map<int,unsigned> last_laze_write_clock_;
		std::list<WAbstractLogWriter*>                    log_writer_list_;
		friend WLogThread;
		friend WSingleton<WLog>; 
	private:
		WLog(const WLogConfig& config);
		bool rawWriteLog(const WLogData& data);
		void logWriterChanged();
		virtual int minit()override;
};
