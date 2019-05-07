/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
#include <wendian.h>
namespace WSpace
{
	 template class float_endian_buffer<order::big,float>;
	 template class float_endian_buffer<order::big,double>;
	 template class endian_buffer<order::big, uint16_t, 16, align::no>;
	 template class endian_buffer<order::big, uint32_t, 32, align::no>;
	 template class endian_buffer<order::big, uint64_t, 64, align::no>;
	 template class endian_buffer<order::big, int16_t, 16, align::no>;
	 template class endian_buffer<order::big, int32_t, 32, align::no>;
	 template class endian_buffer<order::big, int64_t, 64, align::no>;
}
std::ostream& operator<<(std::ostream& os,const WSpace::big_float_buf_t& v)
{
	os<<float(v);
	return os;
}
std::ostream& operator<<(std::ostream& os,const WSpace::big_double_buf_t& v)
{
	os<<double(v);
	return os;
}
