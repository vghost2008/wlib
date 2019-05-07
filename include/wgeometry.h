/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QRect>
#include <QPoint>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/register/box.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/accumulators.hpp>

namespace boost { namespace geometry { namespace traits {
	template<> struct tag<QPoint>
	{ typedef point_tag type; };

	template<> struct coordinate_type<QPoint>
	{ typedef int type; };

	template<> struct coordinate_system<QPoint>
	{ typedef cs::cartesian type; };

	template<> struct dimension<QPoint> : boost::mpl::int_<2> {};

	template<>
		struct access<QPoint, 0>
		{
			static int get(QPoint const& p) { return p.x(); }

			static void set(QPoint& p, int const& value) { p.setX(value); }
		};

	template<>
		struct access<QPoint, 1>
		{
			static int get(QPoint const& p) { return p.y(); }
			static void set(QPoint& p, int const& value) { p.setY(value); }
		};
	/******************************************************************************/
	template<> struct tag<QPointF>
	{ typedef point_tag type; };

	template<> struct coordinate_type<QPointF>
	{ typedef qreal type; };

	template<> struct coordinate_system<QPointF>
	{ typedef cs::cartesian type; };

	template<> struct dimension<QPointF> : boost::mpl::int_<2> {};

	template<>
		struct access<QPointF, 0>
		{
			static qreal get(QPointF const& p) { return p.x(); }
			static void set(QPointF& p, qreal const& value) { p.setX(value); }
		};

	template<>
		struct access<QPointF, 1>
		{
			static qreal get(QPointF const& p) { return p.y(); }
			static void set(QPointF& p, qreal const& value) { p.setY(value); }
		};
	/******************************************************************************/
	template<> struct tag<QRect> 
	{ typedef box_tag type; };

	template<> struct point_type<QRect> 
	{ typedef QPoint type; };

	template <size_t Dimension>
		struct indexed_access<QRect, min_corner,Dimension>
		{
			static inline int get(QRect const& b)
			{
				return access<QPoint,Dimension>::get(b.topLeft());
			}

			static inline void set(QRect& b, int const& value)
			{
				auto p = b.topLeft();
				access<QPoint,Dimension>::set(p,value);
				b.setTopLeft(p);
			}
		};

	template <size_t Dimension>
		struct indexed_access<QRect, max_corner, Dimension>
		{
			static inline int get(QRect const& b)
			{
				return access<QPoint,Dimension>::get(b.bottomRight());

			}

			static inline void set(QRect& b, int const& value)
			{
				auto p = b.bottomRight();
				access<QPoint,Dimension>::set(p,value);
				b.setBottomRight(p);
			}
		};
	/******************************************************************************/
	template<> struct tag<QRectF> 
	{ typedef box_tag type; };

	template<> struct point_type<QRectF> 
	{ typedef QPointF type; };

	template <size_t Dimension>
		struct indexed_access<QRectF, min_corner,Dimension>
		{
			static inline int get(QRectF const& b)
			{
				return access<QPointF,Dimension>::get(b.topLeft());
			}

			static inline void set(QRectF& b, int const& value)
			{
				auto p = b.topLeft();
				access<QPointF,Dimension>::set(p,value);
				b.setTopLeft(p);
			}
		};

	template <size_t Dimension>
		struct indexed_access<QRectF, max_corner, Dimension>
		{
			static inline int get(QRectF const& b)
			{
				return access<QPointF,Dimension>::get(b.bottomRight());

			}

			static inline void set(QRectF& b, int const& value)
			{
				auto p = b.bottomRight();
				access<QPointF,Dimension>::set(p,value);
				b.setBottomRight(p);
			}
		};
}}} // namespace boost::geometry::traits
namespace WSpace
{
	/*
	 * 根据点与点之间的距离进行自动简单聚类
	 * @点的容器
	 * @Container1: 容器的容器
	 * 如:Container0=std::vector<point>,Container1=std::list<std::vector<point>>
	 */
	struct QuadDistance
	{
		template<typename T>
			double operator()(const T& lhv,const T& rhv)const {
				return (std::get<0>(lhv)-std::get<0>(rhv))*(std::get<0>(lhv)-std::get<0>(rhv))+(std::get<1>(lhv)-std::get<1>(rhv))*(std::get<1>(lhv)-std::get<1>(rhv));
			}
	};
	template<typename Container0,typename Container1,typename Distance=QuadDistance>
		void pointsClusteringByDistance(const Container0& _data_i, Container1* data_o,Distance get_dis=Distance(),bool mybe_circle=true) 
		{
			using namespace boost::accumulators;
			using namespace std;

			data_o->clear();
			if(_data_i.size()<=2) return;

			auto data_i = _data_i;
			accumulator_set<float,stats<tag::variance(lazy)>> acc;
			vector<float> distance;

			distance.reserve(data_i.size());

			auto end=prev(data_i.end());
			for(auto it = data_i.begin(); it!=end; ++it) {
				auto jt = next(it);
				distance.push_back(get_dis(*jt,*it));
				acc(distance.back());
			}

			if(mybe_circle) {
				auto it = data_i.begin();
				auto jt = prev(data_i.end());

				distance.push_back(get_dis(*it,*jt));
			} 
			const auto delta = sqrt(variance(acc))*3.3;
			const auto ave   = mean(acc);

			if(!mybe_circle) 
				distance.push_back(delta*3+ave);

			auto    it  = find_if(distance.begin(),distance.end(),[delta,&ave](float v){ return v>delta+ave;});

			if((it != distance.end()) && (it!=std::prev(distance.end()))) {
				auto dis = std::distance(distance.begin(),it)+1;
				rotate(distance.begin(),std::next(it),distance.end());
				rotate(data_i.begin(),next(data_i.begin(),dis),data_i.end());
			}
			
			auto process_end = data_i.begin();
			it    = find_if(distance.begin(),distance.end(),[delta,&ave](float v){ return v>delta+ave;});
			while(it!=distance.end()) {
				auto dis    = std::distance(distance.begin(),it);
				auto old_pe = process_end;
				process_end = std::next(data_i.begin(),dis+1);
				data_o->emplace_back(old_pe,process_end);
				it    = find_if(std::next(it),distance.end(),[delta,&ave](float v){ return v>delta+ave;});
			}
			if(process_end != data_i.end()) {
				data_o->emplace_back(process_end,data_i.end());
			}
		}
}
