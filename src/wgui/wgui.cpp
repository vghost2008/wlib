/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "wgui.h"
#include <toolkit.h>
#include <wmath.h>
#include <algorithm>
#include <wglobal.h>
#include <gmatrix.h>
using namespace std;
using namespace WSpace;
WGuiSeparator::WGuiSeparator(WOrientation orientation)
:orientation_(orientation)
{
	const int kSize = 3;
	switch(orientation_) {
		case Horizontal:
			setFixedHeight(kSize);
			break;
		case Vertical:
			setFixedWidth(kSize);
			break;
	}
}
void WGuiSeparator::paintEvent(QPaintEvent* /*event*/)
{
	//const QColor line_color(50,50,80);
	const QColor line_color(100,100,125);
	QPainter painter(this);
	switch(orientation_) {
		case Horizontal:
			painter.setPen(QPen(QColor(255,255,255)));
			painter.drawLine(0,0,width(),0);
			painter.drawLine(0,2,width(),2);
			painter.setPen(QPen(line_color));
			painter.drawLine(0,1,width(),1);
			break;
		case Vertical:
			painter.setPen(QPen(QColor(255,255,255)));
			painter.drawLine(0,0,0,height());
			painter.drawLine(2,0,2,height());
			painter.setPen(QPen(line_color));
			painter.drawLine(1,0,1,height());
			break;
	}
}
/*================================================================================*/
WCheckBox::WCheckBox(const QString& title,unsigned id)
:QCheckBox(title)
,id_(id)
{
	connect(this,&QCheckBox::clicked,this,&WCheckBox::slotClicked);
}
void WCheckBox::slotClicked(bool ischecked)
{
	emit sigClicked(id_,ischecked);
}
/*================================================================================*/
WPixmapLabel::WPixmapLabel(const QString& text,const QString& pixmap_name)
:text_(text)
,offset_(10)
{
	if(QFile::exists(pixmap_name))
		background_pix_.load(pixmap_name);
	else
		background_pix_.load(imgP("pixmaplabel_background"));
	QFont f = font();
	f.setPixelSize(48);
	setFont(f);
	setFixedHeight(1.34f*fontMetrics().height());
}
void WPixmapLabel::paintEvent(QPaintEvent* /*event*/)
{
	int             x;
	int             y;
	QPainter        painter(this);
	const QPalette &pal           = palette();

	painter.drawPixmap(rect(),background_pix_,background_pix_.rect());
	painter.setRenderHint(QPainter::Antialiasing,true);
	y = (height()+fontMetrics().height())/2-fontMetrics().descent();
	x = offset_;
	painter.setPen(QColor(240,240,240));
	painter.drawText(x,y,text_);
	x = width()-2;
	y = height()-2;
	painter.setPen(pal.mid().color());
	painter.drawLine(2,y,x,y);
	x = width()-1;
	y = height()-1;
	painter.drawLine(1,y,x,y);
	painter.setPen(pal.base().color());
	painter.setRenderHint(QPainter::Antialiasing,false);
}
/*================================================================================*/
WColorLabel::WColorLabel(const QString& text,const QColor& color)
:text_(text)
,label_color_(color)
,offset_(10)
,margin_(5)
,color_box_height_(fontMetrics().height()/4)
{

	const unsigned fh = fontMetrics().height();
	QFont f = font();
	f.setBold(true);
	setFont(f);
	setFixedHeight(fh+color_box_height_+3*margin_);
}
void WColorLabel::paintEvent(QPaintEvent* /*event*/)
{
	const unsigned fh = fontMetrics().height();
	QRect rect(0,fh+margin_+2,width()*0.8,color_box_height_);
	QPoint point_array[] = {QPoint(rect.right(),rect.top()),QPoint(rect.right(),rect.bottom()+1),QPoint(rect.right()+color_box_height_*2.0,rect.bottom()+1)};
	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing,true);
	painter.drawText(offset_,margin_+fh-fontMetrics().descent(),text());
	painter.setPen(Qt::NoPen);
	painter.setBrush(QBrush(label_color_));
	painter.drawRect(rect);
	painter.drawPolygon(point_array,ARRAY_SIZE(point_array));
	painter.setRenderHint(QPainter::Antialiasing,false);
}
void WColorLabel::setText(const QString& text)
{
	text_ = text;
	update();
}
/******************************************************************************/
WColorWidget::WColorWidget(const QColor& color,QWidget* parent)
:QWidget(parent)
,color_(color)
{
	setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}
void WColorWidget::paintEvent(QPaintEvent* /*event*/)
{
	QPainter painter(this);
	painter.fillRect(rect(),QBrush(color()));
}
void WColorWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
	if(e->button() != Qt::LeftButton) return QWidget::mouseDoubleClickEvent(e);
	color_ = QColorDialog::getColor(color(),this,"选择颜色");
	update();
}
void WColorWidget::setColor(const QColor& color)
{
	color_ = color;
	update();
}
/*================================================================================*/
WDateTimeLabel::WDateTimeLabel()
:timer_id_(-1)
,timer_interval_(500)
{
	timer_id_ = startTimer(timer_interval_);
}
void WDateTimeLabel::timerEvent(QTimerEvent* /*event*/)
{
	if(!isVisible()) return;
	setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}
void WDateTimeLabel::setUpdateable(bool update)
{
	if(update) {
		if(timer_id_ < 0) timer_id_ = startTimer(timer_interval_);
	} else {
		if(timer_id_ > 0)killTimer(timer_id_);
		timer_id_ = -1;
	}
}
WDateTimeLabel::~WDateTimeLabel()
{
	setUpdateable(false);
}
/******************************************************************************/
WComboBox::WComboBox(QWidget* parent)
:QComboBox(parent)
,read_only_(false)
{
}
void WComboBox::mousePressEvent(QMouseEvent* event)
{
	if(read_only_)return;
	return QComboBox::mousePressEvent(event);
}
void WComboBox::mouseReleaseEvent(QMouseEvent* event)
{
	if(read_only_) return;
	return QComboBox::mouseReleaseEvent(event);
}

/*================================================================================*/
ArrowLine::ArrowLine()
:x0_(0)
,y0_(0)
,x1_(0)
,y1_(0)
,arrow_size_(0)
,arrow_pos_(0)
{
}
ArrowLine::ArrowLine(float x0,float y0,float x1,float y1,float arrow_size,unsigned arrow_pos)
:x0_(x0)
,y0_(y0)
,x1_(x1)
,y1_(y1)
,arrow_size_(arrow_size)
,arrow_pos_(arrow_pos)
{
	update();
}
ArrowLine::ArrowLine(const QPointF& p0,const QPointF& p1,float arrow_size,unsigned arrow_pos)
:x0_(p0.x())
,y0_(p0.y())
,x1_(p1.x())
,y1_(p1.y())
,arrow_size_(arrow_size)
,arrow_pos_(arrow_pos)
{
	update();
}
void ArrowLine::update()
{
	GVector a0 = {arrow_size_,0.0f,0.0f,1.0f};
	GVector a1={0.0f,arrow_size_*0.4f,0.0f,1.0f};
	GVector a2={0.0f,-arrow_size_*0.4f,0.0f,1.0f};
	int         arrow_num = 0;
	float       angle     = atan2(y1_-y0_,x1_-x0_);
	const float line_size = sqrt((y1_-y0_) *(y1_-y0_)+(x1_-x0_)*(x1_-x0_));

	if(arrow_pos_&WSpace::LP_BEGIN) ++arrow_num;
	if(arrow_pos_&WSpace::LP_END)++arrow_num;
	if(line_size<arrow_size_*arrow_num) {
		point0_[0] = x0_;
		point0_[1] = y0_;
		point1_[0] = x1_;
		point1_[1] = y1_;
		for(int i=0; i<3; ++i) {
			memcpy(arrow_points0_+i*2,point0_,sizeof(float)*2);
			memcpy(arrow_points1_+i*2,point1_,sizeof(float)*2);
		}
		return ;
	}

	const float delta_x = arrow_size_*cos(angle);
	const float delta_y = arrow_size_*sin(angle);
	if(arrow_pos_&WSpace::LP_BEGIN) {
		point0_[0] = x0_+delta_x;
		point0_[1] = y0_+delta_y;
		const GMatrix trans = GMatrix::translateMatrix(point0_[0],point0_[1],0)*GMatrix::rotatezMatrix(WSpace::r2d(angle-M_PI));
		GVector new_p = trans*a0;
		memcpy(arrow_points0_,new_p.data(),sizeof(float)*2);
		new_p = trans*a1;
		memcpy(arrow_points0_+2,new_p.data(),sizeof(float)*2);
		new_p = trans*a2;
		memcpy(arrow_points0_+4,new_p.data(),sizeof(float)*2);
	} else {
		point0_[0] = x0_;
		point0_[1] = y0_;
	}
	if(arrow_pos_&WSpace::LP_END) {
		point1_[0] = x1_-delta_x;
		point1_[1] = y1_-delta_y;

		const GMatrix trans = GMatrix::translateMatrix(point1_[0],point1_[1],0)*GMatrix::rotatezMatrix(WSpace::r2d(angle));
		GVector new_p = trans*a0;
		memcpy(arrow_points1_,new_p.data(),sizeof(float)*2);
		new_p = trans*a1;
		memcpy(arrow_points1_+2,new_p.data(),sizeof(float)*2);
		new_p = trans*a2;
		memcpy(arrow_points1_+4,new_p.data(),sizeof(float)*2);
	} else {
		point1_[0] = x1_;
		point1_[1] = y1_;
	}
}
void ArrowLine::paint(QPainter* painter)
{
	painter->drawLine(point0_[0],point0_[1],point1_[0],point1_[1]);
	painter->save();
	painter->setPen(Qt::NoPen);
	if(arrow_pos_&WSpace::LP_BEGIN) {
		const QPointF points[3] = {
			QPointF(arrow_points0_[0],arrow_points0_[1]),
			QPointF(arrow_points0_[2],arrow_points0_[3]),
			QPointF(arrow_points0_[4],arrow_points0_[5]),
		};
		painter->drawPolygon(points,3);
	}
	if(arrow_pos_&WSpace::LP_END) {
		const QPointF points[3] = {
			QPointF(arrow_points1_[0],arrow_points1_[1]),
			QPointF(arrow_points1_[2],arrow_points1_[3]),
			QPointF(arrow_points1_[4],arrow_points1_[5]),
		};
		painter->drawPolygon(points,3);
	}
	painter->restore();
}
QPainterPath ArrowLine::clipPath()const
{
	QPainterPath painter_path;
	painter_path.moveTo(point0_[0],point0_[1]);
	painter_path.lineTo(point1_[0],point1_[1]);
	if(arrow_pos_&WSpace::LP_BEGIN) {
		const QVector<QPointF> points = {
			QPointF(arrow_points0_[0],arrow_points0_[1]),
			QPointF(arrow_points0_[2],arrow_points0_[3]),
			QPointF(arrow_points0_[4],arrow_points0_[5]),
		};
		painter_path.addPolygon(QPolygonF(points));
		painter_path.closeSubpath();
	}
	if(arrow_pos_&WSpace::LP_END) {
		const QVector<QPointF> points = {
			QPointF(arrow_points1_[0],arrow_points1_[1]),
			QPointF(arrow_points1_[2],arrow_points1_[3]),
			QPointF(arrow_points1_[4],arrow_points1_[5]),
		};
		painter_path.addPolygon(QPolygonF(points));
		painter_path.closeSubpath();
	}
	return painter_path;
}
/*================================================================================*/
float LineStripWithArrow::zero_ = 0.01f;
LineStripWithArrow::LineStripWithArrow()
{
}
void LineStripWithArrow::paint(QPainter* painter)
{
	if(arrow_pos_&WSpace::LP_BEGIN) {
		begin_arrow_.paint(painter);
	} 
	if(arrow_pos_&WSpace::LP_END) {
		end_arrow_.paint(painter);
	} 
	if(points_.empty()) return;
	QPointF* p0 = &points_.front();
	auto it = points_.begin();
	++it;
	for(;it!=points_.end(); ++it) {
		painter->drawLine(*p0,*it);
		p0 = &(*it);
	}
}
QPainterPath LineStripWithArrow::clipPath()const
{
	QPainterPath painter_path;

	if(arrow_pos_&WSpace::LP_BEGIN) {
		painter_path.addPath(begin_arrow_.clipPath());
	} 
	if(arrow_pos_&WSpace::LP_END) {
		painter_path.addPath(end_arrow_.clipPath());
	} 

	if(points_.size()<=1) return painter_path;
	auto it = points_.begin();
	painter_path.moveTo(points_.front());
	++it;
	for(;it!=points_.end(); ++it) {
		painter_path.lineTo(*it);
		painter_path.closeSubpath();
		painter_path.moveTo(*it);
	}
	painter_path.closeSubpath();
	return painter_path;
}
auto LineStripWithArrow::getDirection(const QPointF& p0, const QPointF& p1)->LineDirection
{
	const auto p     = p1-p0;
	const auto angle = atan2f(-p.y(),p.x());

	if((angle>=-M_PI/4) &&(angle<M_PI/4)) return LD_RIGHT;
	if((angle>=M_PI/4) && (angle<M_PI*3/4)) return LD_UP;
	if((angle>=M_PI*3/4) || (angle<-M_PI*3/4)) return LD_LEFT;
	return LD_DOWN;
}
int LineStripWithArrow::noreverseDirection(LineDirection d)
{
	static_assert(LD_RIGHT==reverseDirection<LD_LEFT>(),"error reverseDirection");
	switch(d) {
		case LD_UP: return LD_ANY&(~reverseDirection<LD_UP>());
		case LD_DOWN: return LD_ANY&(~reverseDirection<LD_DOWN>());
		case LD_LEFT: return LD_ANY&(~reverseDirection<LD_LEFT>());
		case LD_RIGHT: return LD_ANY&(~reverseDirection<LD_RIGHT>());
		case LD_ANY: return LD_ANY&(~reverseDirection<LD_ANY>());
		case 0: return LD_ANY&(~reverseDirection<0>());
		default: return 0;
	}
}

