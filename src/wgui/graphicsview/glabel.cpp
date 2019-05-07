/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <glabel.h>
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

WGLabel::WGLabel(const QString& text,WOrientation orientation,QGraphicsItem* parent)
:QGraphicsItem(parent)
,text_(text)
,orientation_(orientation)
{
	init();
}
void WGLabel::init()
{
	auto text_width = QApplication::fontMetrics().width(text_)+2*margins_;
	auto text_height = QApplication::fontMetrics().height()+2*margins_;

	if(Horizontal == orientation_)
		rect_=QRectF(0,0,text_width,text_height);
	else
		rect_=QRectF(0,0,text_height,text_width);
}
void WGLabel::setBackgroundColor(const QColor& color)
{
	background_color_ = color;
	update();
}
void WGLabel::setTextColor(const QColor& color)
{
	text_color_ = color;
	update();
}
QRectF WGLabel::boundingRect()const
{
	return rect_;
}
void WGLabel::setText(const QString& text)
{
	text_ = text;
	init();
	update();
}
void WGLabel::paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* /*widget*/)
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
		transform.translate(rect_.width()/2,rect_.height()/2);
		transform.rotate(90);
		transform.translate(-rect_.width()/2,-rect_.height()/2);
		painter->setTransform(transform,true);
		painter->drawText(margins_,rect_.width()/2+text_height/2-option->fontMetrics.descent(),text_);
	} else { 
		painter->drawText(rect_.left()+(rect_.width()-text_width)/2,rect_.bottom()-(rect_.height()-text_height)/2-option->fontMetrics.descent(),text_);
	}
	painter->restore();
}
