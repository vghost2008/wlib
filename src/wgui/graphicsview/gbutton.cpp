/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <gbutton.h>
#include <gline.h>
#include <wmacros.h>
#include <algorithm>
#include <wgui/wgui.h>
#include <toolkit.h>
#include <math.h>
#include <wgui/guitools.h>
#include <wglobal.h>

using namespace std;
using namespace WGui;
using namespace WSpace;

WGAbstractButton::WGAbstractButton(unsigned id,QGraphicsItem* parent)
:QGraphicsItem(parent)
,id_(id)
,flag_(0)
{
	setAcceptedMouseButtons(Qt::LeftButton);
	setAcceptHoverEvents(true);
}
WGAbstractButton::~WGAbstractButton()
{
}
QRectF WGAbstractButton::boundingRect()const
{
	return rect_;
}
void WGAbstractButton::hoverEnterEvent(QGraphicsSceneHoverEvent* /*event*/)
{
	flag_ |= F_HOVER;
	emit sigHovered(id_,true);
	update();
}
void WGAbstractButton::hoverLeaveEvent(QGraphicsSceneHoverEvent* /*event*/)
{
	flag_ &= (~F_HOVER);
	emit sigHovered(id_,false);
	update();
}
void WGAbstractButton::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if(event->button()==Qt::LeftButton) {
		flag_ |= F_LBUTTON_DOWN;
		update();
	}
}
void WGAbstractButton::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if((event->button()==Qt::LeftButton)
			&& (flag_&F_LBUTTON_DOWN)) {
		flag_ &= (~F_LBUTTON_DOWN);
		if(checkable_) {
			flag_ ^= F_CHECKED;
			setChecked(flag_&F_CHECKED);
		}
		emit sigClicked(id_,flag_&F_CHECKED);
		update();
	}
}
QPointF WGAbstractButton::leftCenterPoint()const
{
	return WGLinkLine::leftCenterPoint(*this);
}
QPointF WGAbstractButton::rightCenterPoint()const
{
	return WGLinkLine::rightCenterPoint(*this);
}
/*================================================================================*/
WGButtonGroup::WGButtonGroup()
{
}
WGAbstractButton* WGButtonGroup::button(unsigned index)
{
	if(index>= button_array_.size()) return nullptr;
	return button_array_[index];
}
void WGButtonGroup::addButton(WGAbstractButton* button)
{
	button_array_.push_back(button);
	connect(button,&WGAbstractButton::sigClicked,this,&WGButtonGroup::slotButtonClicked);
}
void WGButtonGroup::slotButtonClicked(unsigned id,bool ischecked)
{
	if(ischecked) {
		for(auto it=button_array_.begin(); it!=button_array_.end(); ++it) {
			if((*it)->id_ != id && (*it)->isChecked()) (*it)->setChecked(false);
		}
	} else {
		for(auto it=button_array_.begin(); it!=button_array_.end(); ++it) {
			if((*it)->id_ == id) {
				(*it)->setChecked(true);
				break;
			}
		}
	}
}
unsigned WGButtonGroup::checkedID()const
{
	for(auto it=button_array_.begin(); it!=button_array_.end(); ++it) {
		if((*it)->isChecked()) return (*it)->id();
	}
	return -1;
}
/*================================================================================*/
WGButton::WGButton(const QString& title,unsigned id,QGraphicsItem* parent)
:WGAbstractButton(id,parent)
,title_(title)
{
	const float width = QApplication::fontMetrics().width(title)+2*margins_;
	const float height = QApplication::fontMetrics().height()+2*margins_;

	rect_ = QRect(0,0,width,height);
}
void WGButton::paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* /*widget*/)
{
	int       i;
	const int text_width = option->fontMetrics.width(title_);

	painter->save();
	painter->setPen(QColor(140,140,140,100));

	QLinearGradient gradient(rect_.x(),rect_.y(),rect_.x(),rect_.bottom());
	if(flag_&F_LBUTTON_DOWN) {
		GradientNode node_array[] = {
			{0      ,   QColor(40,   220,   70)}   ,
			{0.6      ,   QColor(10,   150,   40)}   ,
			{1      ,   QColor(40,   220,   70)}   ,
		};
		for(i=0; i<ARRAY_SIZE(node_array); ++i) {
			gradient.setColorAt(node_array[i].position,node_array[i].color.darker(180));
		}
		painter->setBrush(gradient);
	} else  if(flag_&F_HOVER) {
		GradientNode node_array[] = {
			{0      ,   QColor(40,   220,   70)}   ,
			{0.6      ,   QColor(10,   150,   40)}   ,
			{1      ,   QColor(40,   220,   70)}   ,
		};
		for(i=0; i<ARRAY_SIZE(node_array); ++i) {
			gradient.setColorAt(node_array[i].position,node_array[i].color.lighter());
		}
		painter->setBrush(gradient);
	} else {
		GradientNode node_array[] = {
			{0      ,   QColor(40,   220,   70)}   ,
			{0.6      ,   QColor(10,   150,   40)}   ,
			{1      ,   QColor(40,   220,   70)}   ,
		};
		for(i=0; i<ARRAY_SIZE(node_array); ++i) {
			gradient.setColorAt(node_array[i].position,node_array[i].color);
		}
		painter->setBrush(gradient);
	}
	painter->drawRoundedRect(rect_,round_radius_,round_radius_);
	painter->setPen(QColor(0,0,15));
	painter->drawText(rect_.left()+(rect_.width()-text_width)/2,
			rect_.bottom()-(rect_.height()-option->fontMetrics.height())/2-option->fontMetrics.descent(),
			title_);
	painter->restore();
}
/*================================================================================*/
WGIconButton::WGIconButton(const QString& title,const QString& icon,GraphicsPlace icon_pos,unsigned id,QGraphicsItem* parent)
:WGAbstractButton(id,parent)
,title_(title)
,icon_size_(64,64)
,pix_(icon)
,icon_pos_(icon_pos)
{
	const int text_width  = QApplication::fontMetrics().width(title_);
	const int text_height = QApplication::fontMetrics().height();

	switch(icon_pos_) {
		case G_LEFT:
			{
				auto s = std::max<int>(32,text_height*1.4);
				icon_size_ = QSize(s,s);
				if(pix_.size() != icon_size_) {
					pix_ = pix_.scaled(icon_size_,Qt::KeepAspectRatio,Qt::SmoothTransformation);
				}
				QSize size(text_width+3*margins_+icon_size_.width(),icon_size_.height()+2*margins_);
				rect_ = QRectF(-size.width()/2,-size.height()/2,size.width(),size.height());
			}
			break;
		case G_TOP:
		default:
			{
				if(pix_.size() != icon_size_) {
					pix_ = pix_.scaled(icon_size_,Qt::KeepAspectRatio,Qt::SmoothTransformation);
				}
				const QSize size(std::max(text_width,icon_size_.width())+2*margins_,pix_.height()+text_height+spacing_+2*margins_);
				rect_ = QRectF(-size.width()/2,-size.height()/2,size.width(),size.height());
			}
			break;
	}
}
void WGIconButton::paintIconLeft(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* /*widget*/)
{
	int       i;
	const int text_height = option->fontMetrics.height();

	painter->save();
	painter->setPen(QColor(140,140,140,100));

	QLinearGradient gradient(rect_.x(),rect_.y(),rect_.x()+rect_.width()*0.8f,rect_.bottom());
	if(flag_&F_HOVER) {
		GradientNode node_array[] = {
			{0      ,   QColor(255   ,   255   ,   247)}   ,
			{0.41   ,   QColor(255   ,   214   ,   73)}    ,
			{0.73   ,   QColor(255   ,   216   ,   87)}    ,
			{1      ,   QColor(255   ,   231   ,   151)}   ,
		};
		for(i=0; i<ARRAY_SIZE(node_array); ++i) {
			gradient.setColorAt(node_array[i].position,node_array[i].color);
		}
		painter->setBrush(gradient);
	} else  if(flag_&F_CHECKED) {
		GradientNode node_array[] = {
			{0      ,   QColor(201   ,   152   ,   97)}    ,
			{0.41   ,   QColor(253   ,   163   ,   97)}    ,
			{0.45   ,   QColor(251   ,   142   ,   60)}    ,
			{1      ,   QColor(255   ,   208   ,   134)}   ,
		};
		for(i=0; i<ARRAY_SIZE(node_array); ++i) {
			gradient.setColorAt(node_array[i].position,node_array[i].color);
		}
		painter->setBrush(gradient);
	} else {
		painter->setBrush(QColor(140,140,140,100));
	}
	painter->drawRoundedRect(rect_,round_radius_,round_radius_);
	painter->drawPixmap(rect_.left()+margins_,rect_.top()+(rect_.height()-icon_size_.height())/2,pix_);
	painter->setPen(QColor(0,0,15));
	painter->drawText(rect_.left()+icon_size_.width()+margins_*2,rect_.top()+(rect_.height()+text_height)/2,title_);
	painter->restore();
}
void WGIconButton::paintIconTop(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* /*widget*/)
{
	int       i;
	const int text_width = option->fontMetrics.width(title_);

	painter->save();
	painter->setPen(QColor(140,140,140,100));

	QLinearGradient gradient(rect_.x(),rect_.y(),rect_.x()+rect_.width()*0.8f,rect_.bottom());
	if(flag_&F_HOVER) {
		GradientNode node_array[] = {
			{0      ,   QColor(255   ,   255   ,   247)}   ,
			{0.41   ,   QColor(255   ,   214   ,   73)}    ,
			{0.73   ,   QColor(255   ,   216   ,   87)}    ,
			{1      ,   QColor(255   ,   231   ,   151)}   ,
		};
		for(i=0; i<ARRAY_SIZE(node_array); ++i) {
			gradient.setColorAt(node_array[i].position,node_array[i].color);
		}
		painter->setBrush(gradient);
	} else  if(flag_&F_CHECKED) {
		GradientNode node_array[] = {
			{0      ,   QColor(201   ,   152   ,   97)}    ,
			{0.41   ,   QColor(253   ,   163   ,   97)}    ,
			{0.45   ,   QColor(251   ,   142   ,   60)}    ,
			{1      ,   QColor(255   ,   208   ,   134)}   ,
		};
		for(i=0; i<ARRAY_SIZE(node_array); ++i) {
			gradient.setColorAt(node_array[i].position,node_array[i].color);
		}
		painter->setBrush(gradient);
	} else {
		painter->setBrush(QColor(140,140,140,100));
	}
	painter->drawRoundedRect(rect_,round_radius_,round_radius_);
	painter->drawPixmap(rect_.left()+(rect_.width()-icon_size_.width())/2,rect_.top()+margins_,pix_);
	painter->setPen(QColor(0,0,15));
	painter->drawText(rect_.left()+(rect_.width()-text_width)/2,rect_.top()+rect_.height()-margins_,title_);
	painter->restore();
}
void WGIconButton::paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget)
{
	if(G_TOP == icon_pos_)
		paintIconTop(painter,option,widget);
	else
		paintIconLeft(painter,option,widget);
}
/*================================================================================*/
WGStateButton::WGStateButton(const QStringList& state_icon_list,QGraphicsItem* parent)
:QGraphicsItem(parent)
,state_(0)
,state_icon_list_(state_icon_list)
,rect_(-21,-21,42,42)
{
}
QRectF WGStateButton::boundingRect()const 
{
	return rect_;
}
bool WGStateButton::setState(unsigned state)
{
	if(state >= pix_list_.size()) return false;
	state_ = state;
	update();
	return true;
}
void WGStateButton::paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* /*widget*/)
{
	if(pix_list_.isEmpty()) {
		const float ratio     = 1.2f;
		const QSize icon_size = QSize(option->fontMetrics.height() *ratio,option->fontMetrics.height()*ratio);
		int         i;
		for(i=0; i<state_icon_list_.size(); ++i) {
			pix_list_<<QPixmap(state_icon_list_.at(i)).scaled(icon_size, Qt::KeepAspectRatio,Qt::SmoothTransformation);
		}
		rect_ = QRect(0,0,icon_size.width(),icon_size.height());
	}
	if(state_ >= pix_list_.size()) return;
	painter->drawPixmap(0,0,pix_list_.at(state_));
}
/******************************************************************************/
WGTextButton::WGTextButton(const QString& text,unsigned id,WOrientation orientation,QGraphicsItem* parent)
:WGAbstractButton(id,parent)
,text_(text)
,orientation_(orientation)
,background_color_(QColor(200,200,215))
{
	init();
}
void WGTextButton::init()
{
	auto text_width = QApplication::fontMetrics().width(text_)+2*margins_;
	auto text_height = QApplication::fontMetrics().height()+2*margins_;

	if(Horizontal == orientation_)
		rect_=QRectF(-text_width/2,-text_height/2,text_width,text_height);
	else
		rect_=QRectF(-text_height/2,-text_width/2,text_height,text_width);
}
void WGTextButton::setBackgroundColor(const QColor& color)
{
	background_color_ = color;
	update();
}
void WGTextButton::setTextColor(const QColor& color)
{
	text_color_ = color;
	update();
}
QRectF WGTextButton::boundingRect()const
{
	return rect_;
}
void WGTextButton::setText(const QString& text)
{
	text_ = text;
	init();
	update();
}
void WGTextButton::paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* /*widget*/)
{
	const float text_width = option->fontMetrics.width(text_);
	const float text_height = option->fontMetrics.height();

	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setRenderHint(QPainter::Antialiasing,true);
	painter->setBrush(background_color_);
	painter->drawRoundedRect(rect_,round_radius_,round_radius_);
	painter->setPen(text_color_);
	if(Vertical == orientation_) {
		QTransform transform;
		transform.rotate(90);
		painter->setTransform(transform,true);
		painter->drawText(-rect_.height()/2+margins_,text_height/2-option->fontMetrics.descent(),text_);
	} else { 
		painter->drawText((rect_.width()-text_width)/2,rect_.bottom()-(rect_.height()-text_height)/2-option->fontMetrics.descent(),text_);
	}
	painter->restore();
}
