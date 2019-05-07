/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <wmath.h>
#include <math.h>
#include <toolkit.h>
#include <iostream>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <QDebug>

namespace bg=boost::geometry;
using namespace std;
namespace WSpace
{
	bool getPosOnShpere(double alpha0,double beta0,double distance,double azimuth,double* alpha1,double* beta1)
	{
		typedef bg::model::point<double,2,bg::cs::spherical_equatorial<bg::radian>> point;
		/*
		 * 新增一个辅助点(alpha0,M_PI_2)
		 */
		/*
		 * 球面三角余弦定理计算
		 */
		auto           sin_beta1         = sin(beta0) *cos(distance)+cos(beta0) *sin(distance) *cos(azimuth);
		constexpr auto error_delta       = M_PI/180.0f;
		constexpr auto e_error_delta     = 4 *error_delta;
		auto           error             = 0.0;
		constexpr auto angle_error_delta = 1E-8;

		*beta1 = asin(sin_beta1); //*beta1 [-PI/2,PI/2],没有二义性

		/*
		 * 在极点，经度固定为0
		 */
		if((fabs(*beta1-M_PI_2)<angle_error_delta)
			|| (fabs(*beta1+M_PI_2)<angle_error_delta)) {
			*alpha1 = 0;
			return true;
		}
		/*
		 * 球面三角正弦定理计算
		 */
		auto sin_alpha1_dec_alpha0 = sin(azimuth)*sin(distance)/cos(*beta1);

		*alpha1  = alpha0+asin(sin_alpha1_dec_alpha0);
		correctRRef(*alpha1);

		if((error=fabs(bg::distance(point(alpha0,beta0),point(*alpha1,*beta1))-distance)) > error_delta) {
			auto old_alpha1 = *alpha1;
			*alpha1 = M_PI+2*alpha0-old_alpha1;
			correctRRef(*alpha1);
			const auto n_error = fabs(bg::distance(point(alpha0,beta0),point(*alpha1,*beta1))-distance);
			if(fabs(n_error-error)<e_error_delta) return false;
			if(n_error > error) //使用另一个解后误差变大
				*alpha1 = old_alpha1;
		}
		return true;
	}
}
