/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

_Pragma("once")
#include <memory>
#include <type_traits>
#include <vector>
#include "cmacros.h"
#include <iostream>

enum GPSCoordinateType
{
	GCT_AUTO   ,   
	GCT_LONG   ,   
	GCT_LAT    ,   
};
namespace std
{
#if __cplusplus<201400
	template<typename T, typename... Args>
		unique_ptr<T> make_unique(Args&&... args)
		{
			return unique_ptr<T>{new T{std::forward<Args>(args)...}};
		}
	template<typename T>
		using remove_reference_t=typename remove_reference<T>::type;
	template<typename T>
		using remove_const_t=typename remove_const<T>::type;
	template<typename T>
		using remove_all_extents_t=typename remove_all_extents<T>::type;
	template<typename T>
		using remove_extent_t=typename remove_extent<T>::type;
	template<typename T>
		using remove_pointer_t=typename remove_pointer<T>::type;
	template<typename T>
		using remove_cv_t=typename remove_cv<T>::type;
	template<typename T>
		using remove_const_t=typename remove_const<T>::type;
	template<typename T>
		using remove_volatile_t=typename remove_volatile<T>::type;
	template< class T >
		using decay_t = typename decay<T>::type;
	template< bool b,class T =void>
		using enable_if_t= typename enable_if<b,T>::type;
	template< typename T>
		using result_of_t = typename result_of<T>::type;
	template<int v,typename T0,typename T1>
		using conditional_t = typename conditional<v,T0,T1>::type;
#endif
}
namespace WSpace {
	template<typename T>
		using remove_cvr_t = std::remove_cv_t<std::remove_reference_t<T>>;
	template<typename T>
		struct ValueGuard
		{
			public:
				ValueGuard(T& v,const T& new_value):v_(v),old_v_(v){
					v_ = new_value;
				}
				~ValueGuard(){
					v_ = old_v_;
				}
			private:
				T& v_;
				T old_v_;
		};
	template<typename DataType>
		bool checkRange(DataType* v,const DataType min,const DataType max) {
			if(*v < min) { *v = min; return false; }
			if(*v > max) { *v = max; return false; }
			return true;
		}
	template<typename DataType>
		bool checkRange(DataType* v,const DataType* range) {
			if(*v < range[0]) { *v = range[0]; return false; }
			if(*v > range[1]) { *v = range[1]; return false; }
			return true;
		}
	template<typename DataType>
		bool isInRange(DataType v,const DataType min,const DataType max) {
			return v>=min&&v<=max;
		}
	template<typename DataType>
		bool isInRange(DataType v,const DataType* limit) {
			return v>=limit[0]&&v<=limit[1];
		}
	template<typename DataType>
		DataType ceilingAlignTo(DataType data,DataType align) {
			static_assert(std::is_integral<DataType>::value==true,"Error data Type");
			assert(align>0);
			return ((data+align-1)/align)*align;
		}
	template<typename DataType>
		DataType floorAlignTo(DataType data,DataType align) {
			static_assert(std::is_integral<DataType>::value==true,"Error data Type");
			assert(align>0);
			return (data/align)*align;
		}
	template<typename DataType0,typename DataType1>
		bool overlap(const DataType0& lhv_min,const DataType0& lhv_max,const DataType1& rhv_min,const DataType1& rhv_max) {
			if(rhv_max< lhv_min|| rhv_min>lhv_max) return false;
			return true;
		}
	template<typename DataType0,typename DataType1>
		bool overlap(const std::pair<DataType0,DataType0>& lhv,const std::pair<DataType1,DataType1>& rhv) {
			return overlap(lhv.first,lhv.second,rhv.first,rhv.second);
		}
	/*
	 * 返回 r
	 * 2^(r-1) < v <= 2^r
	 */
	unsigned log2(unsigned v);
	/*
	 * 绝对计时，以毫秒为单位
	 */
	unsigned long clock();
	/*
	 * 睡眠msec毫秒
	 */
	bool sleep(unsigned msec);
	/*
	 * 获取CPU使用率，返回值[1-100]
	 */
	int getCPUUsage();
	/*
	 * 获取内在使用率,返回值[1-100]
	 */
	int getMemUsage();
	/*
	 * 获取磁盘使用率
	 */
	int getDiskUsage(const std::string& disk_path);
	template<unsigned byte_num,typename ValueType>
		ValueType endian_reverse(ValueType v) {
			ValueType      res = 0;
			unsigned char *ps  = (unsigned char*)&v;
			unsigned char *pd  = (unsigned char*)&res;

			static_assert(sizeof(ValueType)>=byte_num,"error size");

			for(int i=0; i<byte_num; ++i) {
				pd[i] = ps[byte_num-1-i];
			}
			return res;
		}
	/*
	 * 线程安全，将年月日时分秒转换为time_t
	 * time_zone:时区，东区用+,西区用-
	 */
	time_t toTime_t(int y,int M,int d, int h,int m,int s,int time_zone=8)noexcept;

	/*
	 * format:日期格式，如:"%Y-%m-%d %H:%M:%S"
	 */
	size_t strftime(char* outstr, size_t maxsize,const char* format,time_t time);
	inline std::string strftime(const char* format, time_t time) {
		char data[64];
		auto res      = strftime(data,sizeof(data),format,time);

		if(res<=0) return std::string();

		data[res] = '\0';

		return std::string(data);
	}
	template<typename T, int N>
		inline constexpr size_t array_size(const T (&)[N]) noexcept { return N; }
	template<typename T>
		inline constexpr size_t array_size(const T& )noexcept { return 1; }

	template<typename T, typename TM>
		inline size_t mem_array_size(const TM& v) {
			auto p = reinterpret_cast<const T*>(0);
			return array_size(p->*v);
		}
	void assign(char* data,const std::string& str,size_t size=0);
	void assign(char* data,long v,size_t size=0);
	void assign(char* data,const char* data_i,size_t size=0);
	inline void assign(char* data,const unsigned char* data_i,size_t size=0)
	{
		return assign(data,(const char*)(data_i),size);
	}
	std::vector<int> range(int begin, int end, int step);
	inline std::vector<int> range(int begin, int end) {
		if(end<begin) return range(begin,end,-1);
		return range(begin,end,1);
	}
	inline std::vector<int> range(int end) {
		if(end>0) return range(0,end,1);
		return range(0,end,-1);
	}
	/*
	 * 显示时一行分为column列，每列unit_nr个字符数据
	 */
	std::string toHexString(const unsigned char* data,int size,int unit_nr=8,int column=3);
}
