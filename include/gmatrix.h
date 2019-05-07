/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <Eigen/Core>
#include <wmatrix.h>
#include <initializer_list>

namespace WSpace{ 
	typedef  Eigen::Matrix<float,4,4> GMatrixBase;
	typedef Eigen::Matrix<float,4,1>  GVectorBase;
	class GVector;
	class GMatrix:public GMatrixBase {
		public:
			using GMatrixBase::GMatrixBase;
			inline const value_type*data()const { 
				return GMatrixBase::data() ;
			}
			inline value_type*data(){ 
				return GMatrixBase::data();
			}
			GMatrix inverse()const;
			static GMatrix translateMatrix(value_type x, value_type y,value_type z);
			static GMatrix scaleMatrix(value_type x, value_type y, value_type z);
			/*
			 * t为角度,360度为一个周期
			 */
			static GMatrix rotatexMatrix(value_type t);
			static GMatrix rotateyMatrix(value_type t);
			static GMatrix rotatezMatrix(value_type t);
			static GMatrix mirrorxMatrix();
			static GMatrix mirroryMatrix();
			static GMatrix mirrorzMatrix();
			static GMatrix identityMatrix();
	};
	class GVector:public GVectorBase
	{
		public:
			using GVectorBase::GVectorBase;
			GVector():GVectorBase(4){
				this->operator()(3) = 1.0f;
			}
			template<typename T>
			GVector(const std::initializer_list<T>& list) {
				const auto end = list.size()>4?std::next(list.begin(),4):list.end();
				auto       i   = 0;
				for(auto it=list.begin(); it!=end; ++i,++it)
					this->operator()(i) = *it;
				if(list.size()<4) {
					for(auto i=list.size(); i<3; ++i)
						this->operator()(i) = 0;
					this->operator()(3) = 1.0;
				}
			}
			GVector(float x, float y, float z=0.0f,float a=1.0f):GVectorBase(4){
				this->operator()(0) = x;
				this->operator()(1) = y;
				this->operator()(2) = z;
				this->operator()(3) = a;
			}
			value_type& operator[](int i) {
				return this->operator()(i);
			}
			static GVector cross_prod(const GVector& lhv,const GVector& rhv);
			const value_type& operator[](int i)const {
				return this->operator()(i);
			}
			inline const value_type* data()const { 
				/*
				 * 依赖于boost库的实现
				 */
				return GVectorBase::data() ;
			}
			inline value_type* data(){ 
				/*
				 * 依赖于boost库的实现
				 */
				return GVectorBase::data();
			}
	};
}
