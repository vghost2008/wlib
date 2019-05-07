/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <gline.h>
#include <wmacros.h>
#include <algorithm>
#include <wgui/wgui.h>
#include <toolkit.h>
#include <math.h>
#include <wgui/guitools.h>
#include <wglobal.h>
#include <wgraphics.h>

using namespace std;
using namespace WGui;
using namespace WSpace;

void WGRoundLinkLine::paint(QPainter* painter,const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
	if(rect_.width()<1 && rect_.height()<1) return;

	QPen        pen(color_);
	const float line_radius2   = 2.0f *line_radius_;
	const float arrow_size     = line_width_ *3;
	QPointF     temp_end_point;

	painter->save();
	painter->setRenderHint(QPainter::Antialiasing,true);
	pen.setWidth(line_width_);
	painter->setPen(pen);

	if(rect_.width() < 1)  {
		if(begin_point_.y() < end_point_.y()) {
				temp_end_point = QPointF(begin_point_.x(),end_point_.y()-arrow_size*0.8);
		} else {
				temp_end_point = QPointF(begin_point_.x(),end_point_.y()+arrow_size*0.8);
		}
	} else {
		if(begin_point_.x() < end_point_.x()) {
				temp_end_point = QPointF(end_point_.x()-arrow_size*0.8,end_point_.y());
		} else {
				temp_end_point = QPointF(end_point_.x()+arrow_size*0.8,end_point_.y());
		}
	}
	if(rect_.width() < 1 || rect_.height()<1) {
		painter->drawLine(begin_point_,temp_end_point);
	} else {
		const float mid_x = rect_.center().x();
		if(rect_.width()<line_radius2||rect_.height()<line_radius2) {
			painter->drawLine(begin_point_,QPointF(mid_x,begin_point_.y()));
			painter->drawLine(QPointF(mid_x,begin_point_.y()),QPointF(mid_x,temp_end_point.y()));
			painter->drawLine(QPointF(mid_x,temp_end_point.y()),temp_end_point);
		} else {

			const float    xradio = begin_point_.x()<temp_end_point.x()?1.0f:-1.0f;
			const float    yradio = begin_point_.y()<temp_end_point.y()?1.0f:-1.0f;
			const QPointF &lxp    = begin_point_.x()<temp_end_point.x()?begin_point_:temp_end_point;
			const QPointF &bxp    = begin_point_.x()>temp_end_point.x()?begin_point_:temp_end_point;

			painter->drawLine(begin_point_,QPointF(mid_x-xradio*line_radius_,begin_point_.y()));
			painter->drawLine(QPointF(mid_x,begin_point_.y()+yradio*line_radius_),QPointF(mid_x,temp_end_point.y()-yradio*line_radius_));
			painter->drawLine(QPointF(mid_x+xradio*line_radius_,temp_end_point.y()),temp_end_point);

			if(lxp.y()<bxp.y()) {
				painter->drawArc(QRect(mid_x-line_radius2,lxp.y(),line_radius2,line_radius2), 0<<4,90<<4);
				painter->drawArc(QRect(mid_x,bxp.y()-line_radius2,line_radius2,line_radius2), 180<<4,90<<4);
			} else {
				painter->drawArc(QRect(mid_x-line_radius2,lxp.y()-line_radius2,line_radius2,line_radius2), 270<<4,90<<4);
				painter->drawArc(QRect(mid_x,bxp.y(),line_radius2,line_radius2), 90<<4,90<<4);
			}
		}
	}

	painter->setPen(Qt::NoPen);
	painter->setBrush(QBrush(color_));
	drawArrow(painter,end_point_,arrow_size,direction_);
	painter->restore();
}
/*================================================================================*/
WGLinkLine::WGLinkLine(const QPointF& begin_point,const QPointF& end_point,QGraphicsItem* parent)
:QGraphicsItem(parent)
,begin_point_(begin_point)
,end_point_(end_point)
{
	setZValue(-1);
	updateData();
}
void WGLinkLine::paint(QPainter* painter,const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
	if(rect_.width()<1 && rect_.height()<1) return;

	QPen        pen(color_);
	const float arrow_size     = line_width_ *3;
	const float arrow_height   = arrow_size *0.8f;
	const float mid_x          = rect_.center().x();
	QPointF     temp_end_point;

	painter->save();
	painter->setRenderHint(QPainter::Antialiasing,true);
	pen.setWidth(line_width_);
	painter->setPen(pen);


	switch(direction_) {
		case Direction::D_EAST:
			temp_end_point = end_point_-QPointF(arrow_height,0);
			break;
		case Direction::D_NORTH:
			temp_end_point = end_point_-QPointF(0,-arrow_height);
			break;
		case Direction::D_WEST:
			temp_end_point = end_point_-QPointF(-arrow_height,0);
			break;
		case Direction::D_SOUTH:
			temp_end_point = end_point_-QPointF(0,arrow_height);
			break;
		default:
			ERR("Unexpected");
			break;
	}
	if(G_MID == graphics_place_) {
		painter->drawLine(begin_point_,temp_end_point);
		goto label0;
	}
	painter->drawLine(begin_point_,QPointF(mid_x,begin_point_.y()));
	painter->drawLine(QPointF(mid_x,begin_point_.y()),QPointF(mid_x,temp_end_point.y()));
	painter->drawLine(QPointF(mid_x,temp_end_point.y()),temp_end_point);
label0:
	painter->setPen(Qt::NoPen);
	painter->setBrush(QBrush(color_));
	drawArrow(painter,end_point_,arrow_size,direction_);
	painter->restore();
}
QRectF WGLinkLine::boundingRect()const
{
	return rect_;
}
void WGLinkLine::updateData()
{
	prepareGeometryChange();
	rect_ = QRect(std::min(begin_point_.x(),end_point_.x()),std::min(begin_point_.y(),end_point_.y()),
			abs(begin_point_.x()-end_point_.x()),abs(begin_point_.y()-end_point_.y()));

	const float angle = atan2(begin_point_.y()-end_point_.y(),end_point_.x()-begin_point_.x());//graphicsview中y轴从上到下

	if(angle>=-M_PI_2&& angle<=M_PI_2) {
		direction_ = Direction::D_EAST;
	} else {
		direction_ = Direction::D_WEST;
	}
	switch(direction_) {
		case Direction::D_EAST:
			if(rect_.height() <= 1) 
				graphics_place_ = G_MID;
			else if(end_point_.y() > begin_point_.y())
				graphics_place_ = G_LEFT;
			else
				graphics_place_ = G_RIGHT;
			break;
		case Direction::D_NORTH:
			if(rect_.width() < 1)
				graphics_place_ = G_MID;
			else if(end_point_.x() < begin_point_.x())
				graphics_place_ = G_LEFT;
			else
				graphics_place_ = G_RIGHT;
			break;
		case Direction::D_WEST:
			if(rect_.height() <= 1) 
				graphics_place_ = G_MID;
			else if(end_point_.y() < begin_point_.y())
				graphics_place_ = G_LEFT;
			else
				graphics_place_ = G_RIGHT;
			break;
		case Direction::D_SOUTH:
			if(rect_.width() < 1)
				graphics_place_ = G_MID;
			else if(end_point_.x() > begin_point_.x())
				graphics_place_ = G_LEFT;
			else
				graphics_place_ = G_RIGHT;
			break;
		default:
			ERR("Unexpected");
			break;
	}
}
QPointF WGLinkLine::leftCenterPoint(const QGraphicsItem& item) 
{
	return item.pos()+QPointF(item.boundingRect().left()-2,0);
}
QPointF WGLinkLine::rightCenterPoint(const QGraphicsItem& item) 
{
	return item.pos()+QPointF(item.boundingRect().right()+2,0);
}
/*================================================================================*/
WGTreeLine::WGTreeLine(const QPointF& begin_point,const list<QPointF>& end_point_list,QGraphicsItem* parent)
:QGraphicsItem(parent)
,begin_point_(begin_point)
,end_point_list_(end_point_list)
,color_(0,0,0)
,line_width_(5)
{
	setZValue(-1);
	updateData();
}
void WGTreeLine::updateData()
{
	float min_x      = begin_point_.x(),min_y = begin_point_.y(),max_x = begin_point_.x(),max_y = begin_point_.y();
	float sum_end_x  = 0.0f,sum_end_y = 0.0f;
	bool  have_left  = false;
	bool  have_right = false;

	if(end_point_list_.empty()) return;

	prepareGeometryChange();

	list<QPointF>::iterator it;
	for(it=end_point_list_.begin(); it!=end_point_list_.end(); ++it) {
		min_x = std::min<float>(min_x,it->x());
		min_y = std::min<float>(min_y,it->y());
		max_x = std::max<float>(max_x,it->x());
		max_y = std::max<float>(max_y,it->y());
		sum_end_x += it->x();
		sum_end_y += it->y();
	}
	getPointsRect(end_point_list_,&end_points_rect_);

	sum_end_x /= end_point_list_.size();
	sum_end_y /= end_point_list_.size();
	const float angle = atan2(begin_point_.y()-sum_end_y,sum_end_x-begin_point_.x()); //graphicsview中y轴从上到下

	if(angle>=-M_PI_4 && angle<=M_PI_4) {
		node_point_.setX((begin_point_.x()+end_points_rect_.left())/2);
		node_point_.setY(begin_point_.y());
		node_point_.setX(std::max(node_point_.x(),begin_point_.x()));

		direction_ = Direction::D_EAST;
	} else if(angle>=M_PI_4 && angle<=0.75f*M_PI) {
		node_point_.setX(begin_point_.x());
		node_point_.setY((begin_point_.y()+end_points_rect_.bottom())/2);
		node_point_.setY(std::min(node_point_.y(),begin_point_.y()));

		direction_ = Direction::D_NORTH;
	} else if(angle >= 0.75*M_PI || angle <= -0.75*M_PI) {
		node_point_.setX((begin_point_.x()+end_points_rect_.left())/2);
		node_point_.setY(begin_point_.y());
		node_point_.setX(std::min(node_point_.x(),begin_point_.x()));

		direction_ = Direction::D_WEST;
	} else {
		node_point_.setX(begin_point_.x());
		node_point_.setY((begin_point_.y()+end_points_rect_.bottom())/2);
		node_point_.setY(std::max(node_point_.y(),begin_point_.y()));

		direction_ = Direction::D_SOUTH;
	}
	switch(direction_) {
		case Direction::D_EAST:
			for(QPointF& p:end_point_list_) {
				if(p.y() > begin_point_.y()) 
					have_left = true;
				else if(p.y() < begin_point_.y())
					have_right = true;

			}
			break;
		case Direction::D_NORTH:
			for(QPointF& p:end_point_list_) {
				if(p.x() < begin_point_.x()) 
					have_left = true;
				else if(p.x() > begin_point_.x())
					have_right = true;

			}
			break;
		case Direction::D_WEST:
			for(QPointF& p:end_point_list_) {
				if(p.y() < begin_point_.y()) 
					have_left = true;
				else if(p.y() > begin_point_.y())
					have_right = true;

			}
			break;
		case Direction::D_SOUTH:
			for(QPointF& p:end_point_list_) {
				if(p.x() > begin_point_.x()) 
					have_left = true;
				else if(p.x() < begin_point_.x())
					have_right = true;

			}
			break;
	}

	if(have_left&have_right)
		graphics_place_ = G_ALL;
	else if(have_left)
		graphics_place_ = G_LEFT;
	else if(have_right)
		graphics_place_ = G_RIGHT;
	else 
		graphics_place_ = G_MID;

	rect_ = QRectF(min_x,min_y,max_x-min_x,max_y-min_y);
}
QRectF WGTreeLine::boundingRect()const 
{
	return rect_;
}
void WGTreeLine::paint(QPainter* painter,const QStyleOptionGraphicsItem* /*option*/,QWidget* /*widget*/)
{
	if(rect_.width()<1 && rect_.height()<1) return;

	const float arrow_size   = line_width_ *3;
	const float arrow_height = arrow_size *0.8f;
	const int   delta        = line_width_/2;

	painter->save();
	painter->setRenderHint(QPainter::Antialiasing,true);
	QPen pen(color_);
	pen.setWidth(line_width_);
	painter->setPen(pen);
	painter->setBrush(Qt::NoBrush);

	if(G_MID == graphics_place_) {
		switch(direction_) {
			case Direction::D_WEST:
				for(const QPointF& p:end_point_list_) {
					painter->drawLine(begin_point_,p-QPointF(arrow_height,0));
				}
				break;
			case Direction::D_NORTH:
				for(const QPointF& p:end_point_list_) {
					painter->drawLine(begin_point_,p-QPointF(0,-arrow_height));
				}
				break;
			case Direction::D_EAST:
				for(const QPointF& p:end_point_list_) {
					painter->drawLine(begin_point_,p-QPointF(-arrow_height,0));
				}
				break;
			case Direction::D_SOUTH:
				for(const QPointF& p:end_point_list_) {
					painter->drawLine(begin_point_,p-QPointF(0,arrow_height));
				}
				break;
		}
		goto label0;
	} 
	switch(direction_) {
		case Direction::D_EAST:
			painter->drawLine(begin_point_,node_point_-QPointF(delta,0));
			switch(graphics_place_) {
				case G_ALL:
					painter->drawLine(node_point_.x(),end_points_rect_.top(),node_point_.x(),end_points_rect_.bottom());
					break;
				case G_LEFT:
					painter->drawLine(node_point_.x(),end_points_rect_.top(),node_point_.x(),node_point_.y());
					break;
				case G_RIGHT:
					painter->drawLine(node_point_.x(),end_points_rect_.bottom(),node_point_.x(),node_point_.y());
					break;
				default:
					ERR("unexpected");
					break;
			}
			for(const QPointF& p:end_point_list_) {
				painter->drawLine(QPointF(node_point_.x()+delta,p.y()),p-QPointF(arrow_height,0));
			}
			break;
		case Direction::D_NORTH:
			painter->drawLine(begin_point_,node_point_-QPointF(0,-delta));
			switch(graphics_place_) {
				case G_ALL:
					painter->drawLine(end_points_rect_.left(),node_point_.y(),end_points_rect_.right(),node_point_.y());
					break;
				case G_LEFT:
					painter->drawLine(end_points_rect_.left(),node_point_.y(),node_point_.x(),node_point_.y());
					break;
				case G_RIGHT:
					painter->drawLine(end_points_rect_.right(),node_point_.y(),node_point_.x(),node_point_.y());
					break;
				default:
					ERR("unexpected");
					break;
			}
			for(const QPointF& p:end_point_list_) 
				painter->drawLine(QPointF(p.x(),node_point_.y()-delta),p-QPointF(0,-arrow_height));
			break;
		case Direction::D_WEST:
			painter->drawLine(begin_point_,node_point_-QPointF(-delta,0));
			switch(graphics_place_) {
				case G_ALL:
					painter->drawLine(node_point_.x(),end_points_rect_.top(),node_point_.x(),end_points_rect_.bottom());
					break;
				case G_LEFT:
					painter->drawLine(node_point_.x(),end_points_rect_.bottom(),node_point_.x(),node_point_.y());
					break;
				case G_RIGHT:
					painter->drawLine(node_point_.x(),end_points_rect_.top(),node_point_.x(),node_point_.y());
					break;
				default:
					ERR("unexpected");
					break;
			}
			for(const QPointF& p:end_point_list_) {
				painter->drawLine(QPointF(node_point_.x()-delta,p.y()),p+QPointF(arrow_height,0));
			}
			break;
		case Direction::D_SOUTH:
			painter->drawLine(begin_point_,node_point_-QPointF(0,delta));
			switch(graphics_place_) {
				case G_ALL:
					painter->drawLine(end_points_rect_.left(),node_point_.y(),end_points_rect_.right(),node_point_.y());
					break;
				case G_LEFT:
					painter->drawLine(end_points_rect_.right(),node_point_.y(),node_point_.x(),node_point_.y());
					break;
				case G_RIGHT:
					painter->drawLine(end_points_rect_.left(),node_point_.y(),node_point_.x(),node_point_.y());
					break;
				default:
					ERR("unexpected");
					break;
			}
			for(const QPointF& p:end_point_list_) 
				painter->drawLine(QPointF(p.x(),node_point_.y()+delta),p-QPointF(0,-arrow_height));
			break;
		default:
			ERR("Unexptected");
			break;
	}
label0:
	painter->setPen(Qt::NoPen);
	painter->setBrush(color_);
	for(const QPointF& p:end_point_list_) {
		WGui::drawArrow(painter,p,arrow_size,direction_);
	}
	painter->restore();
	return;
}

