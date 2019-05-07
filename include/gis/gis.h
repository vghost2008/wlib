/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <boost/geometry.hpp> 
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
#include <toolkit.h>
#include <iostream>
#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include <QString>

struct GPSPoint
{
	double x; //经度
	double y; //纬度
	double z; //高程m
};
inline std::ostream& operator<<(std::ostream& s,const GPSPoint& p) {
	s<<"("<<p.x<<","<<p.y<<","<<p.z<<")";
	return s;
}
/*
 * 星下点数据标志定义
 */
enum {
	SPF_INRANGE      =  0x00000001, //可视范围内
	SPF_INRANGE_TOP  =  0x00000002, //过顶
};
/*
 * 星下点数据
 */
struct WSubStarPoint
{
	long     time  = 0; //北京时
	long     pass  = 0; //圈号
	double   longi = 0; //星下点经度
	double   lati  = 0; //星下点纬度
	uint32_t flag  = 0; //标志
};
using WSubStarPoints=std::vector<WSubStarPoint>;
BOOST_GEOMETRY_REGISTER_POINT_2D(WSubStarPoint, double, cs::cartesian, longi, lati);
BOOST_GEOMETRY_REGISTER_LINESTRING(WSubStarPoints);
inline std::ostream& operator<<(std::ostream& s,const WSubStarPoint& p) {
	s<<std::dec<<"("<<WSpace::strftime("%Y-%m-%d %H:%M:%S",p.time)<<","<<p.pass<<","<<p.longi<<","<<p.lati<<", flag="<<std::hex<<p.flag<<std::dec<<")";
	return s;
}
/*
 * 条带上的一个点
 */
using WSubStarStripePoint=WSubStarPoint;
/*
 * 星下点条带
 */
struct WSubStarStripe
{
	float                            color[4];
	int                              sat_index;
	std::vector<WSubStarStripePoint> data;
};
inline std::ostream& operator<<(std::ostream& s,const WSubStarStripe& p) {
	s<<"Color:"<<p.color[0]<<","<<p.color[1]<<","<<p.color[2]<<","<<p.color[3]<<std::endl;
	s<<"SatIndex:"<<p.sat_index<<std::endl;
	s<<"DataSize:"<<p.data.size()<<std::endl;
	for(auto& v:p.data) {
		s<<v<<std::endl;
	}
	return s;
}
using WSubStarStripes=std::list<WSubStarStripe>;

namespace boost { namespace geometry { 
	namespace traits {

		template<> struct tag<GPSPoint>
		{ typedef point_tag type; };

		template<> struct coordinate_type<GPSPoint>
		{ typedef double type; };

		template<> struct coordinate_system<GPSPoint>
		{ typedef cs::geographic<boost::geometry::degree> type; };

		template<> struct dimension<GPSPoint> : boost::mpl::int_<2> {};

		template<>
			struct access<GPSPoint, 0> {
				static double get(GPSPoint const& p) { return p.x; }
				static void set(GPSPoint& p, double const& value) { p.x = value; }
			};

		template<>
			struct access<GPSPoint, 1> {
				static double get(GPSPoint const& p) { return p.y; }
				static void set(GPSPoint& p, double const& value) { p.y = value; }
			};
	}
}} // namespace boost::geometry::traits
namespace WSpace
{
	class GMatrix;
	/*
	 * 地球半径，单位米
	 */
	constexpr double kEarthRadius =  6371004.0f;
	QString toGPSString(double v,GPSCoordinateType type=GCT_LONG);
	double  toGPSValue(const QString& str,GPSCoordinateType type=GCT_AUTO);
	/*
	 * longi,sati:输入经度，纬度
	 * distance:距离，单位m
	 * azimuth:方位，正北0,正东90,正南180
	 * longo,lato:输出经度，纬度
	 */
	bool getGPSPointByDistanceAndAzimuth(double longi,double lati,double distance,double azimuth,double* longo,double* lato);
	inline bool getGPSPointByDistanceAndAzimuth(const GPSPoint& pi,double distance,double azimuth,GPSPoint* po) {
		return getGPSPointByDistanceAndAzimuth(pi.x,pi.y,distance,azimuth,&po->x,&po->y);
	}
	inline bool getGPSPointByDistanceAndAzimuth(const WSubStarPoint& pi,double distance,double azimuth,WSubStarPoint* po) {
		return getGPSPointByDistanceAndAzimuth(pi.longi,pi.lati,distance,azimuth,&po->longi,&po->lati);
	}
	/*
	 * 获取地球上等半径的圆
	 * @long0,sat0圆中心GPS坐标
	 * @radius:圆半径，单位m
	 * @container:支持push_back的的容器
	 * @[begin,end):输出迭代器
	 */
	template<class Container>
				   size_t getCircleOnEarth(double long0,double lat0,double radius,Container& container,size_t size)
	{
		namespace bg=boost::geometry;

		double azimuth = 0.0;
		double long1   = 0;
		double lat1    = 0;

		if(0 == size) return 0;

		const auto   delta_azimuth = 360.0/size;

		using value_type = typename Container::value_type;

		container.clear();

		while(azimuth < 360.0) {
			if(getGPSPointByDistanceAndAzimuth(long0,lat0,radius,azimuth,&long1,&lat1))
				container.push_back(value_type{long1,lat1,0});
			azimuth += delta_azimuth;
		}
		return container.size();
	}
	template<class Container>
		inline size_t getCircleOnEarth(const GPSPoint& pi,double radius,Container& container,size_t size) {
			return getCircleOnEarth(pi.x,pi.y,radius,container,size);
		}
	/*
	 * 根据卫星的星下点轨迹与卫星的侦察区域大小计算卫星的条带
	 * @trace:星下点轨迹
	 * @radius:侦察半径
	 * @strip:输出的条带多边形
	 * @round_end:开始与结束位置是否使用半圆
	 */
	bool getSubStarStripe(const WSubStarPoints& trace,double radius,WSubStarStripe* stripe,bool round_end=true);
	/*
	 * 使用经度-180与180分割条带
	 */
	bool splitSubStarStripe(const WSubStarStripe& strip,std::list<WSubStarStripe>* strips);
	bool splitSubStarPoints(const WSubStarPoints& strip,std::list<WSubStarPoints>* strips);
	/*
	 * 获取p0到p1的方位，正北为0正东为90
	 */
	double azimuth(const GPSPoint& p0, const GPSPoint& p1);
	double azimuth(const WSubStarPoint& p0, const WSubStarPoint& p1);
	double azimuth(double x0,double y0,double x1,double y1);
	/*
	 * 根据参考星下点信息恢复指定星下点的附加信息
	 * 通常in_and_out为ref的一个子集
	 */
	void correct(const WSubStarStripe& ref, WSubStarStripe* in_and_out);
	void correct(const WSubStarPoints& ref, WSubStarPoints* in_and_out);
	GPSPoint& correct(GPSPoint* v);
	/*
	 * a:轨道半径
	 * 输出卫星运行周期，单位为秒
	 */
	double satPeriod(double a)noexcept(false);
	/*
	 * 姿态角变换
	 * h:航向
	 * fai:俯仰
	 * theta:横滚(弧度)
	 * a:方位
	 * b:俯仰
	 */
	bool attitudeTrans(const GMatrix& m,double a,double b,double* a_o,double* b_o);
	/*
	 * 使用角度为单位
	 */
	bool attitudeTransD(const GMatrix& m,double a,double b,double* a_o,double* b_o);
	/*
	 * 方位俯仰使用东，北，天向量给出
	 * (使用方位府仰时内部会自动转换为e,n,u)
	 */
	bool attitudeTrans(const GMatrix& m,double e,double n,double u,double* e_o,double* n_o,double* u_o);
	/*
	 * 获取姿态角校正矩阵
	 * 以下参数为设备自身的偏置
	 * h:航向
	 * fai:俯仰
	 * theta:横滚(弧度)
	 */
	GMatrix attitudeTransMatrix(double h,double fai, double theta);
	GMatrix attitudeTransMatrixD(double h,double fai, double theta);
}
namespace std
{
	template<std::size_t index>
		inline double get(const GPSPoint& p) {
			return 0;
		}
	template<> inline double get<0>(const GPSPoint& p) {
		return p.x;
	}
	template<> inline double get<1>(const GPSPoint& p) {
		return p.y;
	}
	template<> inline double get<2>(const GPSPoint& p) {
		return p.z;
	}
	template<std::size_t index>
		inline double get(const WSubStarPoint& p) {
			return 0;
		}
	template<> inline double get<0>(const WSubStarPoint& p) {
		return p.longi;
	}
	template<> inline double get<1>(const WSubStarPoint& p) {
		return p.lati;
	}
}
