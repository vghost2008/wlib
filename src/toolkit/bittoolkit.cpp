/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#include <bittoolkit.h>
namespace WSpace
{
	size_t bitsdiff(const unsigned char* begin0, const unsigned char* end0, const unsigned char* begin1)
	{
		auto it   = begin0;
		auto jt   = begin1;
		auto res  = 0;

		while(it != end0) {
			res += bitsdiff(*it++,*jt++);
		}
		return res;
	}
}
