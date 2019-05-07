/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#include <QtGlobal>
#include "toolkit.h"
#include <wmacros.h>
#include <math.h>
#include <time.h>
#include <fstream>
#include <mutex>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#if defined(Q_OS_LINUX)
#include <sys/time.h>
#include <unistd.h>
#include <sys/vfs.h>
#elif defined(Q_OS_WIN32)
#include <Windows.h>
inline float FileTimeToFloat(FILETIME ft) {
    return float(ft.dwHighDateTime<<22)+float(ft.dwLowDateTime>>10);
}
#else
#error "Error system";
#endif //system define

using namespace std;

namespace WSpace {
	unsigned log2(unsigned v) {
		unsigned tv = 1;
		unsigned r = 0;
		while(tv<v) {
			tv <<=1;
			++r;
		}
		return r;
	}
	unsigned long clock() {
		static bool initialized = false;
#if defined(Q_OS_LINUX)
		static struct timespec start_time;
		if(!initialized) {
			clock_gettime(CLOCK_REALTIME_COARSE,&start_time);
			initialized = true;
			return 0;
		}
		struct timespec stop_time;
		clock_gettime(CLOCK_REALTIME_COARSE,&stop_time);
		return (stop_time.tv_sec-start_time.tv_sec)*1000+(stop_time.tv_nsec-start_time.tv_nsec)/1000000;
#else
		static LARGE_INTEGER sys_frequency;
		static LARGE_INTEGER start_time;
		static LARGE_INTEGER stop_time;
		bool                 flag          = false;

		if(!initialized) {
			flag = QueryPerformanceFrequency(&sys_frequency);
			if(!flag || sys_frequency.QuadPart==0) return 0;
			flag = QueryPerformanceCounter(&start_time);
			if(!flag) return 0;
			initialized = true;
			return 0;
		}
		flag = QueryPerformanceCounter(&stop_time);
		if(!flag) return 0;
		return (stop_time.QuadPart-start_time.QuadPart)*1000/sys_frequency.QuadPart;
#endif
	}
	bool sleep(unsigned msec)
	{
#ifdef Q_OS_LINUX
		if(msec>1000) {
			::sleep(msec/1000);
			::usleep((msec%1000)*1000);
		} else {
			::usleep(msec*1000);
		}
#else
		::Sleep(msec);
#endif
		return true;
	}
	int getCPUUsage()
	{
#ifdef Q_OS_LINUX
		static unsigned last_total_cpu_time = 0;
		static unsigned last_usage_cpu_time = 0;
		unsigned        total_cpu_time      = 0;
		unsigned        usage_cpu_time      = 0;
		int             user                = 0;
		int             nice                = 0;
		int             system              = 0;
		int             idle                = 0;
		int             ret_val             = 0;
		string          temp;
		ifstream        file;

		file.open("/proc/stat");

		if(!file.is_open()) return 0;
		file>>temp>>user>>nice>>system>>idle;
		usage_cpu_time        =   user+nice+system;
		total_cpu_time        =   usage_cpu_time +idle;
		last_total_cpu_time   =   total_cpu_time - last_total_cpu_time;
		last_usage_cpu_time   =   usage_cpu_time - last_usage_cpu_time;
		ret_val               =   100*last_usage_cpu_time/last_total_cpu_time;
		last_total_cpu_time   =   total_cpu_time;
		last_usage_cpu_time   =   usage_cpu_time;
		file.close();
		return ret_val;
#else    
		static float last_total_cpu_time = 0;
		static float last_usage_cpu_time = 0;
		float        total_cpu_time      = 0;
		float        usage_cpu_time      = 0;
		FILETIME     idle_time;
		FILETIME     kernel_time;
		FILETIME     user_time;
		unsigned     ret_val             = 0;

		if(GetSystemTimes(&idle_time,&kernel_time,&user_time)) {
			/*
			 * KernelTime包括IdleTime
			 */
			total_cpu_time        =   FileTimeToFloat(kernel_time)+FileTimeToFloat(user_time);
			usage_cpu_time        =   total_cpu_time-FileTimeToFloat(idle_time);
			last_total_cpu_time   =   total_cpu_time - last_total_cpu_time;
			last_usage_cpu_time   =   usage_cpu_time - last_usage_cpu_time;
			if(last_total_cpu_time <= 0) return 0;
			ret_val               =   100*last_usage_cpu_time/last_total_cpu_time;
			last_total_cpu_time   =   total_cpu_time;
			last_usage_cpu_time   =   usage_cpu_time;
			return ret_val;
		} else {
			return 0;
		}
#endif
		return 0;
	}
	int getMemUsage()
	{
#ifdef Q_OS_LINUX
		int      total_mem = 0;
		int      free_mem  = 0;
		string   temp;
		ifstream file;

		file.open("/proc/meminfo");

		if(!file.is_open()) return 0;

		file>>temp>>total_mem>>temp>>temp>>free_mem;
		if(total_mem <= 0) {
			file.close();
			return 0;
		}
		file.close();
		return 100-100*free_mem/total_mem;
#else
		MEMORYSTATUSEX memory_status;
		memory_status.dwLength = sizeof(memory_status);
		GlobalMemoryStatusEx(&memory_status);
		return memory_status.dwMemoryLoad;
#endif
		return 0;
	}
	int getDiskUsage(const string& disk_path)
	{
#ifdef Q_OS_LINUX
		struct statfs fs;
		if(statfs(disk_path.c_str(),&fs) >= 0) {
			if(fs.f_blocks == 0) return 0;
			return 100-100.0f*fs.f_bfree/fs.f_blocks-0.5f;
		}
#else
		DWORD   temp;
		DWORD   free_clusters;
		DWORD   total_clusters;
		QString disk           = QFileInfo(disk_path.c_str()).absoluteFilePath();

		if(disk.size()>2)disk=disk.left(2);
		if(GetDiskFreeSpaceA(disk.toUtf8().data(),&temp,&temp,&free_clusters,&total_clusters)) {
			if(total_clusters >0) return 100.0f-100.0f*free_clusters/total_clusters;
			return 0;
		}
		return 0;
#endif
		return 0;
	}
	time_t toTime_t(int y,int M,int d, int h,int m,int s,int time_zone)noexcept
	{
#if 0
		auto v0 = 0u;
		{
			auto              t   = ::time(nullptr);
			static std::mutex mtx;
			struct tm         tm;

			localtime_r(&t,&tm);
			tm.tm_year  =  y-1900;
			tm.tm_mon   =  M-1;
			tm.tm_mday  =  d;
			tm.tm_hour  =  h;
			tm.tm_min   =  m;
			tm.tm_sec   =  s;

			std::lock_guard<std::mutex> guard{mtx};
			v0 = mktime(&tm);
			return v0;
		}
#else
		try {
			using namespace boost::gregorian;
			using namespace boost::posix_time;

			const ptime epoch(date(1970,1,1),time_duration(0,0,0)); 
			ptime t(date(y,M,d),time_duration(h,m,s));
			return (t-epoch).total_seconds()-time_zone*3600;
		} catch(...) {
			return 0;
		}
#endif
	}
	size_t strftime(char* outstr,size_t maxsize,const char* format,time_t time)
	{
		struct tm tm;

		localtime_r(&time,&tm);
		return ::strftime(outstr,maxsize,format,&tm);
	}
	void assign(char* data,const std::string& str,size_t size)
	{
		if(size>0)
			snprintf(data,size,"%s",str.c_str());
		else
			sprintf(data,"%s",str.c_str());
	}
	void assign(char* data,long v,size_t size)
	{
		if(size>0)
			snprintf(data,size,"%ld",v);
		else
			sprintf(data,"%ld",v);
	}
	void assign(char* data,const char* data_i,size_t size)
	{
		if(0 == size)
			strcpy(data,data_i);
		else
			strncpy(data,data_i,size);
	}
	std::vector<int> range(int begin, int end, int step) 
	{
		using namespace std;

		if(end==begin) return vector<int>();
		if((end-begin-1)/step < 0) return vector<int>(); 

		vector<int> res((end-begin-1)/step+1);

		generate(res.begin(),res.end(),[&begin,step]() { 
			auto r = begin;
			begin += step;
			return r;
		});

		return res;
	}
	string toHexString(const unsigned char* data,int size,int unit_nr,int column)
	{
		int          i;
		int          j;
		stringstream ret_val;
		const int    elements_every_row = unit_nr *column; //每行的字符数

		ret_val<<setfill('0')<<hex;
		for(i=0; i<size;) {
			for(j=0; j<elements_every_row&&i<size; ++j,++i) {
				if(j%unit_nr== 0 && j) ret_val<<"    ";
				ret_val<<setw(2)<<((uint)data[i])<<" ";
			}
			ret_val<<"\n";
		}
		return ret_val.str();
	}
}
