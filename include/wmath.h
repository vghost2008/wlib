/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <wequation.h>
#include <iterator>
#include <math.h>
#include <boost/geometry.hpp> 

namespace WSpace
{
	/*
	 * 弧度转换为角度
	 */
	template<typename value_type>
		inline value_type r2d(value_type v) { return 180.0*v/M_PI; }
	/*
	 * 角度转换为弧度
	 */
	template<typename value_type>
		inline value_type d2r(value_type v) { return v*M_PI/180.0; }
	template<typename value_type>
		inline value_type correctR(value_type v) { 
			constexpr auto tpi = 2*M_PI;
			if(v < -M_PI)  {
				const int k = (M_PI-v)/tpi;
				v += (k*tpi);
			} else if(v > M_PI) {
				const int k = (M_PI+v)/tpi;
				v -= (k*tpi);
			}
			return v;
		}
	template<typename value_type>
		inline value_type correctD(value_type v) { 
			constexpr auto tpi = 360.0;
			if(v < -180)  {
				const int k = (180-v)/tpi;
				v += (k*tpi);
			} else if(v > 180) {
				const int k = (180+v)/tpi;
				v -= (k*tpi);
			}
			return v;
		}
	template<typename value_type>
		inline value_type& correctRRef(value_type& v) { 
			v = correctR(v);
			return v;
		}
	template<typename value_type>
		inline value_type& correctDRef(value_type& v) { 
			v = correctD(v);
			return v;
		}
	/*out=v0 X v1*/
	template<typename ValueType>
		void cross_prod(const ValueType* v0,const ValueType* v1,ValueType* out)
		{
			out[0]=v0[1]*v1[2]-v0[2]*v1[1];
			out[1]=v0[2]*v1[0]-v0[0]*v1[2];
			out[2]=v0[0]*v1[1]-v0[1]*v1[0];
		}
	/*
	 * 0,1,2按逆时针方向输入
	 * out = (vin1-vin0)X(vin2-vin1)
	 */
	template<typename ValueType>
		void cross_prod(const ValueType* vin0,const ValueType* vin1,const ValueType* vin2,ValueType* out)
		{
			ValueType v0[3];
			ValueType v1[3];

			v0[0]   =   vin1[0]-vin0[0];
			v0[1]   =   vin1[1]-vin0[1];
			v0[2]   =   vin1[2]-vin0[2];

			v1[0]   =   vin2[0]-vin1[0];
			v1[1]   =   vin2[1]-vin1[1];
			v1[2]   =   vin2[2]-vin1[2];
			cross_prod(v0,v1,out);
		}
	/*
	 * 在单位球体上获取两点间的距离
	 * alpha为经度，[-180,180],单位弧度
	 * beta为续度，[-90,90],单位弧度
	 * azimuth:为方位，单位为弧度，正北为0，正东为M_PI_2，正南为M_PI,正西为M_PI*3/2
	 * distance:为两点间距离
	 */
	bool getPosOnShpere(double alpha0,double beta0,double distance,double azimuth,double* alpha1,double* beta1);
	/*
	 * 二次贝塞尔曲线
	 */
	template<typename Point>
		Point bezierQuad(const Point& p0, const Point& p1,const Point& p2,float t) {
			namespace bg=boost::geometry;
			constexpr int x_tag = 0;
			constexpr int y_tag = 1;
			auto          omt   = 1.0f-t;
			Point         res;

			bg::set<x_tag>(res,omt*omt*bg::get<x_tag>(p0)+2*t*omt*bg::get<x_tag>(p1)+t*t*bg::get<x_tag>(p2));
			bg::set<y_tag>(res,omt*omt*bg::get<y_tag>(p0)+2*t*omt*bg::get<y_tag>(p1)+t*t*bg::get<y_tag>(p2));
			return res;
		}
	template<typename Point>
		Point bezierQuad(const Point* p,float t) {
			return bezierQuad(p[0],p[1],p[2],t);
		}
	/*
	 * 三次贝塞尔曲线
	 */
	template<typename Point>
		Point bezierCubic(const Point& p0, const Point& p1,const Point& p2,const Point& p3,float t) {
			namespace bg=boost::geometry;
			constexpr int x_tag = 0;
			constexpr int y_tag = 1;
			auto          omt   = 1.0f-t;
			Point         res;

			bg::set<x_tag>(res,omt*omt*omt*bg::get<x_tag>(p0)+ 3*t*omt*omt*bg::get<x_tag>(p1)+ 3*t*t*omt*bg::get<x_tag>(p2)+ t*t*t*bg::get<x_tag>(p3));
			bg::set<y_tag>(res,omt*omt*omt*bg::get<y_tag>(p0)+ 3*t*omt*omt*bg::get<y_tag>(p1)+ 3*t*t*omt*bg::get<y_tag>(p2)+ t*t*t*bg::get<y_tag>(p3));
			return res;
		}
	template<typename Point>
		Point bezierCubic(const Point* p,float t) {
			return bezierCubic(p[0],p[1],p[2],p[3],t);
		}
	/*
	 * 四次贝塞尔曲线
	 */
	template<typename Point>
		Point bezierBiquad(const Point& p0, const Point& p1,const Point& p2,const Point& p3,const Point& p4,float t) {
			namespace bg=boost::geometry;
			constexpr int x_tag = 0;
			constexpr int y_tag = 1;
			auto          omt   = 1.0f-t;
			Point         res;

			bg::set<x_tag>(res,omt*omt*omt*omt*bg::get<x_tag>(p0)+ 4*t*omt*omt*omt*bg::get<x_tag>(p1)+ 6*t*t*omt*omt*bg::get<x_tag>(p2)+4*t*t*t*omt*bg::get<x_tag>(p3)+t*t*t*t*bg::get<x_tag>(p4));
			bg::set<y_tag>(res,omt*omt*omt*omt*bg::get<y_tag>(p0)+ 4*t*omt*omt*omt*bg::get<y_tag>(p1)+ 6*t*t*omt*omt*bg::get<y_tag>(p2)+4*t*t*t*omt*bg::get<y_tag>(p3)+t*t*t*t*bg::get<y_tag>(p4));
			return res;
		}
	template<typename Point>
		Point bezierBiquad(const Point* p,float t) {
			return bezierBiquad(p[0],p[1],p[2],p[3],p[4],t);
		}
	inline float int26p62f(int32_t v) { 
		auto mask=(1<<6)-1;
		return float(v>>6)+float(v&mask)/(mask+1);
	}
	inline int32_t f2int26p6(float v) {
		auto mask=(1<<6)-1;
		return (int32_t(v)<<6)+(v-int32_t(v))*(mask+1);
	}
	/*
	 * 从容器c中取n个元素进行组合
	 * ContainerO为元素为容器的容器
	 */
	template<typename T,class ContainerI,class ContainerO>
		bool combination(const ContainerI& c,unsigned n,ContainerO* out) 
		{
			using out_ctype=typename ContainerO::value_type;

			static_assert(std::is_same<typename ContainerI::value_type,T>::value,"error type");
			static_assert(std::is_same<typename out_ctype::value_type,T>::value,"error type");

			out->clear();

			if(n>c.size()||c.empty()) return false;

			if(c.size() == n) {
				out_ctype oc;
				oc.assign(c.begin(),c.end());
				out->push_back(std::move(oc));
				return true;
			}
			if(1 == n) {
				for(auto& v:c) {
					out_ctype oc;
					oc.push_back(v);
					out->push_back(std::move(oc));
				}
				return true;
			}

			const ContainerI temp_in(c.begin(),std::prev(c.end()));
			ContainerO       temp_out;

			if(combination<T>(temp_in,n-1,&temp_out)) {
				for(auto& co:temp_out) {
					co.push_back(c.back());
					out->push_back(std::move(co));
				}
			}
			if(combination<T>(temp_in,n,&temp_out)) {
				for(auto& co:temp_out) {
					out->push_back(std::move(co));
				}
			}
			return true;
		}
}
