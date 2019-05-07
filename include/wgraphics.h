/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <limits>
#include <wmath.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/geometries/ring.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <wgeometry.h>
#include <algorithm>
#include <iostream>
#include <QGraphicsItem>
namespace WSpace
{
	template< typename Point,typename Rect>
		inline Point rectTopMid(const Rect& r) {
			return Point(r.left()+r.width()/2,r.top());
		}
	inline QPointF rectTopMid(const QGraphicsItem& item) {
		return rectTopMid<QPointF>(item.boundingRect());
	}
	template< typename Point,typename Rect>
		inline Point rectBottomMid(const Rect& r) {
			return Point(r.left()+r.width()/2,r.bottom());
		}
	inline QPointF rectBottomMid(const QGraphicsItem& item) {
		return rectBottomMid<QPointF>(item.boundingRect());
	}
	template< typename Point,typename Rect>
		inline Point rectLeftMid(const Rect& r) {
			return Point(r.left(),r.bottom()+r.height()/2);
		}
	inline QPointF rectLeftMid(const QGraphicsItem& item) {
		return rectLeftMid<QPointF>(item.boundingRect());
	}
	template< typename Point,typename Rect>
		inline Point rectRightMid(const Rect& r) {
			return Point(r.right(),r.bottom()+r.height()/2);
		}
	inline QPointF rectRightMid(const QGraphicsItem& item) {
		return rectRightMid<QPointF>(item.boundingRect());
	}
	template< typename Point,typename Rect>
		inline Point rectCenter(const Rect& r) {
			return Point(r.left()+r.width()/2,r.bottom()+r.height()/2);
		}
	inline QPointF rectCenter(const QGraphicsItem& item) {
		return rectCenter<QPointF>(item.boundingRect());
	}
		/*
		 * 获取用于连接矩形之间的点列表
		 */
	template<typename Rect, typename Point>
		bool getPointsForRectConnectLine(const Rect& r,Point* points,unsigned points_nr) {
			if(points_nr < 4) return false;
			points[0] = rectTopMid<Point>(r);
			points[1] = rectRightMid<Point>(r);
			points[2] = rectBottomMid<Point>(r);
			points[3] = rectLeftMid<Point>(r);

			if(points_nr < 8) return true;

			points[4] = r.topLeft();
			points[5] = r.topRight();
			points[6] = r.bottomLeft();
			points[7] = r.bottomRight();
			return true;
		}
	/*
	 * 获取p0_io与p1_io之间的连线
	 * 使用几何体r0,r1剪切直线(p0_io,p1_io),即删除与几何体r0,r1重叠的部分
	 * 生成一个新的直线
	 */
	template<typename Geometry, typename IPoint>
		bool cutLine(const Geometry& r0, const Geometry& r1,IPoint* p0_io,IPoint* p1_io) {

			namespace bg      =  boost::geometry;
			using Point       =  typename bg::traits::point_type<Geometry>::type;
			using linestring  =  bg::model::linestring<Point>;
			using ring        =  bg::model::ring<Point>;

			ring       ring0;
			ring       ring1;
			std::array<IPoint,2> points{*p0_io,*p1_io};
			linestring ls(points.begin(),points.end());

			bg::convert(r0,ring0);
			bg::convert(r1,ring1);

			std::vector<Point> out;

			out.clear();
			if(bg::intersection(ring0,ls,out)
					&& !out.empty()) {
				*p0_io = out[0];
			}

			out.clear();
			if(bg::intersection(ring1,ls,out)
					&& !out.empty()) {
				*p1_io = out[0];
			}
			return true;
		}
	template<typename Rect, typename GeometryOut>
		bool getRectConnectLine(const Rect& r0, const Rect& r1,GeometryOut geometry_out) {
			namespace bg=boost::geometry;
			using Point       =  typename bg::traits::point_type<Rect>::type;
			Point p00;
			Point p01;

			bg::centroid(r0,p00);
			bg::centroid(r1,p01);

			auto res = cutLine(r0,r1, &p00,&p01);

			geometry_out++ = p00;
			geometry_out++ = p01;
			return res;
		}
	template<typename Rect, typename GeometryOut,typename IPoint>
		bool getRectConnectLineWithOffset(const Rect& r0, const Rect& r1,GeometryOut geometry_out,const IPoint& p0_offset,const IPoint& p1_offset ) {
			namespace bg=boost::geometry;
			using Point       =  typename bg::traits::point_type<Rect>::type;
			Point p00;
			Point p01;

			bg::centroid(r0,p00);
			bg::centroid(r1,p01);
			bg::add_point(p00,p0_offset);
			bg::add_point(p01,p1_offset);

			auto res = cutLine(r0,r1, &p00,&p01);

			geometry_out++ = p00;
			geometry_out++ = p01;
			return res;
		}
}
