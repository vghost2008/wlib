/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#include <gmatrix.h>
#include <math.h>
#include <Eigen/LU>
#include <Eigen/Geometry>

namespace WSpace { 
	using namespace Eigen;
	GMatrix GMatrix::inverse()const 
	{
		static_assert(std::is_same<value_type,float>::value,"error type");
		GMatrix res = GMatrixBase::inverse();
		return res;
	}
	GMatrix GMatrix::translateMatrix(value_type x, value_type y,value_type z) 
	{
		GMatrix res = identityMatrix();
		res(0,3) = x;
		res(1,3) = y;
		res(2,3) = z;
		return res;
	}
	GMatrix GMatrix::scaleMatrix(value_type x, value_type y, value_type z)
	{
		GMatrix res = identityMatrix();

		res(0,0) = x;
		res(1,1) = y;
		res(2,2) = z;

		return res;
	}
	GMatrix GMatrix::rotatexMatrix(value_type t) 
	{
		GMatrix          res = identityMatrix();
		const value_type tzt = t *M_PI/180.0f;

		res(1,1)  =  res(2,2)        =  ::cos(tzt);
		res(2,1)  =  ::sin(tzt);
		res(1,2)  =  -1.0f*res(2,1);
		return res;
	}
	GMatrix GMatrix::rotateyMatrix(value_type t)
	{
		const value_type tzt = t *M_PI/180.0f;
		GMatrix          res = identityMatrix();

		res(0,0)  =  res(2,2)        =  ::cos(tzt);
		res(0,2)  =  ::sin(tzt);
		res(2,0)  =  -1.0f*res(0,2);
		return res;
	}
	GMatrix GMatrix::rotatezMatrix(value_type t)
	{
		const value_type tzt = t *M_PI/180.0f;
		GMatrix          res = identityMatrix();

		res(1,1)  =  res(0,0)           =  ::cos(tzt);
		res(1,0)  =  ::sin(tzt);
		res(0,1)  =  -1.0f*res(1,0);
		return res;
	}
	GMatrix GMatrix::mirrorxMatrix()
	{
		GMatrix res = identityMatrix();
		res(0,0) = -1.0f;
		return res;
	}
	GMatrix GMatrix::mirroryMatrix()
	{
		GMatrix res = identityMatrix();
		res(1,1) = -1.0f;
		return res;
	}
	GMatrix GMatrix::mirrorzMatrix()
	{
		GMatrix res = identityMatrix();
		res(2,2) = -1.0f;
		return res;
	}
	GMatrix GMatrix::identityMatrix()
	{
		return GMatrixBase::Identity();
	}
	GVector cross_prod(const GVector& lhv,const GVector& rhv)
	{
		static_assert(std::is_same<GVector::value_type,float>::value,"error type");
		GVector res = lhv.cross3(rhv);
		res[3] = 1.0f;
		return res;
#if 0
		res[0]=lhv[1]*rhv[2]-lhv[2]*rhv[1];
		res[1]=lhv[2]*rhv[0]-lhv[0]*rhv[2];
		res[2]=lhv[0]*rhv[1]-lhv[1]*rhv[0];
		return res;
#endif
	}
}
