/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <cstddef>
#include <type_traits>
namespace WSpace
{
	/*
	 * v0与v1最低两位中不同的位数
	 */
	inline size_t bitsdiff_of_low2(unsigned char v0, unsigned char v1) 
	{
		return (((v0^v1)&0x03)+1)>>1;
	}
	/*
	 * v0与v1最低四位中不同的位数
	 */
	inline size_t bitsdiff_of_low4(unsigned char v0,unsigned char v1) 
	{
		return bitsdiff_of_low2(v0,v1)+bitsdiff_of_low2(v0>>2,v1>>2);
	}
	/*
	 * v0与v1中不同的位数
	 */
	inline size_t bitsdiff(unsigned char v0, unsigned char v1)
	{
		return bitsdiff_of_low4(v0,v1)+bitsdiff_of_low4(v0>>4,v1>>4);
	}
	/*
	 * 输入序列[begin0,end0)与[begin1,end1)中不同的bit数
	 * 两个序列长度相同
	 */
	size_t bitsdiff(const unsigned char* begin0, const unsigned char* end0, const unsigned char* begin1);
	/*
	 * 数据大小为1个byte
	 */
	template<typename T>
		inline size_t bitsdiff_imp(const T& v0, const T& v1,std::true_type) {
			return bitsdiff((unsigned char)v0,(unsigned char)v1);
		}
	/*
	 * 数据大小不为1个byte
	 */
	template<typename T>
		inline size_t bitsdiff_imp(const T& v0, const T& v1,std::false_type) {
			return bitsdiff((const unsigned char*)&v0,((const unsigned char*)&v0)+sizeof(T),(const unsigned char*)&v1);
		}
	/*
	 * 测试数据v0,v1中的不同bit数
	 */
	template<typename T>
		inline size_t bitsdiff(const T& v0,const T& v1) {
			return bitsdiff_imp(v0,v1,std::is_same<std::integral_constant<size_t,1>,std::integral_constant<size_t,sizeof(T)>>());
		}
}
