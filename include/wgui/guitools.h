/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QPainter>
#include <wglobal.h>
#include <toolkit.h>
#include <math.h>
#include <algorithm>
#define WGUI_FLOAT_ZERO 0.0001f
namespace WGui
{
	template<class Point>
		void getArrowPoint(const Point& pos,float size,WSpace::Direction direction,Point* points)
		{
			const float height = size;
			const float size_2 = size*0.577f;
			points[0] = pos;
			switch(direction) {
				case WSpace::Direction::D_EAST:
					points[1].setX(pos.x()-height);
					points[1].setY(pos.y()+size_2);
					points[2].setX(pos.x()-height);
					points[2].setY(pos.y()-size_2);
					break;
				case WSpace::Direction::D_NORTH:
					points[1].setX(pos.x()-size_2);
					points[1].setY(pos.y()+height);
					points[2].setX(pos.x()+size_2);
					points[2].setY(pos.y()+height);
					break;
				case WSpace::Direction::D_WEST:
					points[1].setX(pos.x()+height);
					points[1].setY(pos.y()-size_2);
					points[2].setX(pos.x()+height);
					points[2].setY(pos.y()+size_2);
					break;
				case WSpace::Direction::D_SOUTH:
					points[1].setX(pos.x()+size_2);
					points[1].setY(pos.y()-height);
					points[2].setX(pos.x()-size_2);
					points[2].setY(pos.y()-height);
					break;
				default:
					ERR("Error direction=%d",int(direction));
					break;
			}
		}
	template<class Point>
		void drawRoundCorner(QPainter* painter,const Point& pos,float radius,WSpace::Corner corner) {

			if(fabs(radius)<WGUI_FLOAT_ZERO) return;

			const float width  = radius *2;
			const float height = radius *2;

			switch(corner) {
				case WSpace::Corner::C_TopLeft:
					{
						QRectF rect(pos.x(),pos.y(),width,height);
						painter->drawArc(rect,90<<4,90<<4);
					}
					break;
				case WSpace::Corner::C_BottomLeft:
					{
						QRectF rect(pos.x(),pos.y()-height,width,height);
						painter->drawArc(rect,180<<4,90<<4);
					}
					break;
				case WSpace::Corner::C_BottomRight:
					{
						QRectF rect(pos.x()-width,pos.y()-height,width,height);
						painter->drawArc(rect,270<<4,90<<4);
					}
					break;
				case WSpace::Corner::C_TopRight:
					{
						QRectF rect(pos.x()-width,pos.y(),width,height);
						painter->drawArc(rect,0,90<<4);
					}
					break;
				default:
					ERR("Error corner=%d",int(corner));
					break;
			}
		}
	template<class Point>
		void drawArrow(QPainter* painter,const Point& pos,float size,WSpace::Direction direction)
		{
			Point points[3];
			getArrowPoint(pos,size,direction,points);
			painter->drawPolygon(points,3);
		}
	template<class PointContainer,class Rect>
		bool getPointsRect(const PointContainer& points,Rect* rect) {
			if(points.empty()) return false;
			float min_x      = points.front().x(),min_y = points.front().y(),max_x = points.front().x(),max_y = points.front().y();
			for(auto it=points.begin(); it!=points.end(); ++it) {
				min_x = std::min<float>(min_x,it->x());
				min_y = std::min<float>(min_y,it->y());
				max_x = std::max<float>(max_x,it->x());
				max_y = std::max<float>(max_y,it->y());
			}
			*rect = Rect(min_x,min_y,max_x-min_x,max_y-min_y);
			return true;
		}
	/*
	 * 获取中文text的拼音首字母键
	 * dataset为拼音首字母与中文的对应
	 */
	QString getPinYingKey(const QString& text,const std::map<char,QString>& dataset);
}
