/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "wlog.h"
#include <wlogwriter.h>
#include <functional>
#include "toolkit.h"
#include <xmldata.h>
#include <xmlstream.h>
#include <QApplication>
#include <wguilogwriter.h>
using namespace std;

WLog* WLog::s_instance = nullptr;

WLog::WLog(const WLogConfig& config)
:work_thread_(this)
,laze_time_interval_(2000)
,log_config_(config)
{
}
int WLog::minit()
{
	insertLogWriter(new WFileLogWriter(this));
	insertLogWriter(new WTerminalLogWriter(this));

	write("");
	write("");
	write("*================================================================================");
	write("*    初始化日志");
	write(string("*    日期: ")+QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8().data());
	write("*================================================================================");
	write("");
	return 0;
}
void WLog::setLazeTimeInterval(unsigned v)
{
	WSpace::checkRange(&v,50u,3600u);
    instance().laze_time_interval_ = v;
}
boost::posix_time::ptime WLog::now()
{
	return boost::posix_time::second_clock::local_time();
}
void WLog::initGUIWriter()
{
#ifdef WLOG_ENABLE_GUI_LOG
    insertLogWriter(&WGUILogWriter::instance_i(this));
#else
	printf("********************************************************************************\n");
	printf("GUI Log disabled.\n");
	printf("********************************************************************************\n");
#endif
}
void WLog::initDatabaseWriter()
{
#ifdef WLOG_ENABLE_GUI_LOG
    insertLogWriter(&WGUILogWriter::instance_i(this));
#endif
}
bool WLog::insertLogWriter(WAbstractLogWriter* log_writer)
{
	lock_guard<shared_mutex> guard{shared_lock_};

	if(find_if(log_writer_list_.begin(),log_writer_list_.end(),bind2nd(ptr_fun(sameLogWriter),log_writer)) != log_writer_list_.end()) {
		ERR("This log writer is already in the list.");
		return false;
	}
	log_writer_list_.push_back(log_writer);
	return true;
}
bool WLog::sameLogWriter(const WAbstractLogWriter* log_writer0,const WAbstractLogWriter* log_writer1)
{
	return typeid(*log_writer0)==typeid(*log_writer1);
}
WLog::~WLog()
{
	work_thread_.stop();
	removeAllWriter();
}
bool WLog::write(const string& str)
{
	shared_lock<shared_mutex> guard{shared_lock_};
	for(auto it=log_writer_list_.begin(); it!=log_writer_list_.end(); ++it) {
		(*it)->write(str);
	}
	return true;
}
bool WLog::deleteLogNrDaysAgo(int nr)
{
	shared_lock<shared_mutex> guard{shared_lock_};
	for(auto it=log_writer_list_.begin(); it!=log_writer_list_.end(); ++it) {
		(*it)->deleteLogNrDaysAgo(nr);
	}
	return true;
}
bool WLog::testLazeId(int& id)
{
	if(0 == id) id = allocLazeId();

	if(id<0) return false;

	shared_lock<shared_mutex> guard{instance().shared_lock_};
    map<int,unsigned>& last_laze_write_clock = instance().last_laze_write_clock_;
	auto it = last_laze_write_clock.find(id);

	if(it == last_laze_write_clock.end()) {
		ERR("Can't find laze log id=%d",id);
		return false;
	}

	const unsigned t = WSpace::clock();

    if(t-it->second < instance().laze_time_interval_) return false;

	it->second = t;
	return true;
}
int WLog::allocLazeId()
{
	int                     id     = 1;
	const int               max_id = 256;
	lock_guard<shared_mutex> guard{instance().shared_lock_};
    map<int,unsigned>& last_laze_write_clock = instance().last_laze_write_clock_;

	while(last_laze_write_clock.find(id) != last_laze_write_clock.end()
		&& id<max_id) 
		++id;
	if(id >= max_id) return -1;
	last_laze_write_clock[id] = 0u;
	return id;
}
bool WLog::releaseLazeId(int id)
{
    return instance().last_laze_write_clock_.erase(id) > 0;
}
bool WLog::rawWriteLog(const WLogData& data)
{
	shared_lock<shared_mutex> guard{shared_lock_};
	for(auto it=log_writer_list_.begin(); it!=log_writer_list_.end(); ++it) {
		if(!work_thread_.run_) return false;
		(*it)->write(data);
	}
	return true;
}
bool WLog::sync()
{
	shared_lock<shared_mutex> guard{shared_lock_};
	for(auto it=log_writer_list_.begin(); it!=log_writer_list_.end(); ++it) {
		(*it)->sync();
	}
	return true;
}
std::string WLog::toHex(const string& ba)
{
	return  WSpace::toHexString((const unsigned char*)ba.data(),ba.size());
}
std::string WLog::toHex(const void* data,size_t size)
{
	return WSpace::toHexString((const unsigned char*)data,size);
}
bool WLog::checkLogDirPath()
{
	if(log_config_.dir_path.size() <= 1
			|| !QDir().exists(log_config_.dir_path)) {
		log_config_.dir_path = QApplication::applicationDirPath()+"/log";
		if(!QDir().exists(log_config_.dir_path)) {
			QDir().mkpath(log_config_.dir_path);
		}
	}
	/*
	 * 删除路径尾部的文件夹分隔符
	 */
	if(log_config_.dir_path.right(1) == "/"
			|| log_config_.dir_path.right(1) == "\\") {
		log_config_.dir_path = log_config_.dir_path.left(log_config_.dir_path.size()-1);
	}
	return QDir().exists(log_config_.dir_path);
}
bool WLog::clearLog()
{
	if(log_config_.clear_log_time == 0) return true;
	return deleteLogNrDaysAgo(log_config_.clear_log_time);
}
bool WLog::readConfig(const QString& config_path,const QString& default_log_dir,WLogConfig* _config)
{
	int         id          = 0;
	WXmlData    xml_data;
	WXmlStream  xml_stream(&xml_data);
	WLogConfig &config      = *_config;

	if(!xml_stream.readFile(config_path)) {
		ERR("Read Log config faild!");
		goto use_default;
	}
	if(xml_data.groups().empty()) {
		ERR("Read Log config faild!");
		goto use_default;
	}

	config.flag = 0x0000;
	for(WXmlData::iterator it=xml_data.begin(); it!=xml_data.end(); ++it) {
		id = it->id();
		if(id<0) {
			ERR("error id");
			qDebug()<<id;
			continue;
		}

		switch(id) {
			case 1:
				switch(it->toInt()) {
					case 0:
						config.create_file_policy = EVERY_TIME;
						break;
					case 1:
						config.create_file_policy = ONE_DAY;
						break;
					default:
					case 2:
						config.create_file_policy = ONE_WEEK;
						break;
				}
				break;
			case 2:
				config.clear_log_time = it->toInt();
				break;
			case 3:
				config.dir_path = it->toString();
				if(config.dir_path.isEmpty()
						|| !QDir(config.dir_path).exists()
						|| !QDir().mkpath(config.dir_path)) {
					config.dir_path = default_log_dir;
				}
				if(!QDir(config.dir_path).exists()
						&& !QDir().mkpath(config.dir_path)) {
					ERR("创建日志目录失败");
					qDebug()<<"日志目录:"<<config.dir_path;
				}
#ifdef Q_OS_LINUX
				if(-1 == access(config.dir_path.toUtf8().data(),W_OK)) {
					config.dir_path = default_log_dir;
				}
				if(-1 == access(config.dir_path.toUtf8().data(),W_OK)) {
					ERR("日志目录没有写权限");
					qDebug()<<"日志目录:"<<config.dir_path;
				}
#endif
				break;
			case 4:
				if(it->toBool())config.flag |= LC_FLAG_SHOW_FILENAME;
				break;
			case 5:
				if(it->toBool())config.flag |= LC_FLAG_SHOW_FUNC;
				break;
			case 6:
				if(it->toBool())config.flag |= LC_FLAG_SHOW_LINE;
				break;
			case 10:
				{
					auto level = static_cast<LogLevel>(it->toInt());
					if(level<0
							|| level>=LL_NR)
						config.level[WLT_OUTPUT_FILE] = LL_ERROR;
					else
						config.level[WLT_OUTPUT_FILE] = level;
				}
				break;
			case 11:
				{
					LogLevel level = static_cast<LogLevel>(it->toInt());
					if(level<0
							|| level>=LL_NR)
						config.level[WLT_OUTPUT_TERMINAL] = LL_ERROR;
					else
						config.level[WLT_OUTPUT_TERMINAL] = level;
				}
				break;
			case 12:
				{
					LogLevel level = static_cast<LogLevel>(it->toInt());
					if(level<0
							|| level>=LL_NR)
						config.level[WLT_GUI] = LL_ERROR;
					else
						config.level[WLT_GUI] = level;
				}
				break;
			case 13:
				{
					LogLevel level = static_cast<LogLevel>(it->toInt());
					if(level<0
							|| level>=LL_NR)
						config.level[WLT_DATABASE] = LL_ERROR;
					else
						config.level[WLT_DATABASE] = level;
				}
				break;
			default:
				ERR("ERROR id=%d",id);
				break;

		}
	}
	INFO("Read Log config success");
	return true;
use_default:
	config.dir_path = default_log_dir;
	if(!QFile::exists(config.dir_path)) {
		config.dir_path = "./log";
	}
	for(auto& l:config.level) {
		l = LL_NULL;
	}
	config.level[WLT_OUTPUT_TERMINAL]   =   LL_DEBUG;
	config.level[WLT_OUTPUT_FILE]       =   LL_DEBUG;
	return false;
}
bool WLog::checkConfig(WLogConfig* config)
{
	if(!QFile::exists(config->dir_path)) {
		config->dir_path = "./log";
	}
	if(!QDir(config->dir_path).exists()) {
		QDir().mkpath(config->dir_path);
	}
	return true;
}
void WLog::removeAllWriter()
{
	lock_guard<shared_mutex> guard{shared_lock_};

	for(auto it=log_writer_list_.begin(); it!=log_writer_list_.end(); ++it) {
		(*it)->close();
		delete *it;
	}
	log_writer_list_.clear();
}
/*================================================================================*/
WLogThread::WLogThread(WLog* log)
:log_(log)
,queue_size_limit_(40960)
,run_(true)
{
}
bool WLogThread::stop()
{
	if(!isRunning()) return true;

	run_ = false;
	log_->log_data_queue_.stop();
	log_->log_data_queue_.clear();
	wait(3000);
	return !isRunning();
}
void WLogThread::run()
{
	list<WLogData>   log_data_list;
	auto& log_data_queue = log_->log_data_queue_;

	run_ = true;
	while(run_) {
		msleep(500);
		if(log_data_queue.size() == 0)continue;
		if(log_data_queue.size() >= queue_size_limit_) {
			WLogData log_data = {WLog::now(),LL_ERROR,__FILE__,__func__,__LINE__,"队列中的日志超过限制，强制清空日志队列",LWM_ALL}; 
			log_->rawWriteLog(log_data);
			log_data_queue.clear();
			continue;
		}
		if(log_data_queue.getAllData(&log_data_list) <= 0) continue;
		for(auto it=log_data_list.begin(); it!=log_data_list.end(); ++it) {
			if(!log_->rawWriteLog(*it))break;
		}
		log_->sync();
	}
}
