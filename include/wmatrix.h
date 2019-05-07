/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

_Pragma("once")
#include <cmacros.h>
#include <math.h>
#include <limits>
#include <Eigen/LU>
#include <type_traits>
#include <toolkit.h>

namespace WSpace{
	const float kMatrixZero = 1E-10;
	template<class M>
		inline int rowSize(const M& m) { return m.rows(); }
	template<class M>
		inline int columnSize(const M& m) { return m.cols(); }
	template<class M,typename T>
		inline T abs(const M& m,int i, int j) { return fabs(m(i,j)); }
	template<class M>
		bool inverse(const M& m_,M* m_o) 
		{
			const int            row_size     = rowSize(m_);
			const int            column_size  = columnSize(m_);

			static_assert(std::is_const<M>::value==false,"error type");
			if(row_size != column_size) return false;
			try {
				*m_o = m_.inverse();
			} catch(...) {
				return false;
			}
			return true;
		}
}
