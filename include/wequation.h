/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

_Pragma("once")
#include <wmatrix.h>
#include <Eigen/LU>

namespace WSpace
{
	/*
	 * 解方程
	 * m_*v = b;
	 */
	template<class M, class V>
		bool solve(const M& m_,const V& b,V* v) noexcept  {
			try {
				*v = m_.lu().solve(b);
			} catch(...) {
				return false;
			}
			return true;
		}
}
