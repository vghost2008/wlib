/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

_Pragma("once")
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <wglobal.h>
#include <wgui.h>
#include <memory>
#include "wgui_fwd.h"

class WGLinkLine:public QGraphicsItem
{
	public:
		WGLinkLine(const QPointF&begin_point=QPointF(),const QPointF& end_point=QPointF(),QGraphicsItem* parent=nullptr);
		inline void setColor(const QColor& color) {
			color_ = color;
			update();
		}
		inline void setLineWidth(unsigned width) {
			line_width_ = width;
			update();
		}
		inline void setBeginPoint(const QPointF& begin_point) {
			begin_point_ = begin_point;
			updateData();
			update();
		}
		inline void setEndPoint(const QPointF& end_point) {
			end_point_ = end_point;
			updateData();
			update();
		}
		void updateData();
		static QPointF leftCenterPoint(const QGraphicsItem& item);
		static QPointF rightCenterPoint(const QGraphicsItem& item);
	protected:
		QColor            color_;
		unsigned          line_width_;
		QPointF           begin_point_;
		QPointF           end_point_;
		QRectF            rect_;
		WSpace::Direction direction_;
		GraphicsPlace     graphics_place_;
	protected:
		virtual void   paint(QPainter *painter,const QStyleOptionGraphicsItem*option,QWidget*widget)override;
		virtual QRectF boundingRect()const   override;
};
class WGRoundLinkLine:public WGLinkLine 
{
	public:
		using WGLinkLine::WGLinkLine;
	private:
		const unsigned line_radius_     = 20;
	private:
		virtual void   paint(QPainter *painter,const QStyleOptionGraphicsItem*option,QWidget*widget)override;
};
class WGTreeLine:public QGraphicsItem
{
	public:
		WGTreeLine(const QPointF& begin_point,const std::list<QPointF>& end_point_list,QGraphicsItem* parent=nullptr);
		inline void setColor(const QColor& color) {
			color_ = color;
			update();
		}
		inline void setLineWidth(unsigned width) {
			line_width_ = width;
			update();
		}
		inline void setBeginPoint(const QPointF& begin_point) {
			begin_point_ = begin_point;
			updateData();
			update();
		}
		inline void setEndPointList(const std::list<QPointF>& end_point_list) {
			end_point_list_.assign(end_point_list.begin(),end_point_list.end());
			updateData();
			update();
		}
		void updateData();
	private:
		QColor             color_;
		unsigned           line_width_;
		QPointF            begin_point_;
		std::list<QPointF> end_point_list_;
		QRectF             rect_;
		const unsigned     line_radius_     = 20;
		WSpace::Direction  direction_;
		GraphicsPlace      graphics_place_;
		QRectF             end_points_rect_;
		QPointF            node_point_;
	private:
		virtual void   paint(QPainter *painter,const QStyleOptionGraphicsItem*option,QWidget*widget)override;
		virtual QRectF boundingRect()const   override;
};
