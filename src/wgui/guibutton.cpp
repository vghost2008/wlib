/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <guibutton.h>
#include <toolkit.h>
#include <algorithm>
using namespace std;
using namespace WSpace;
WAbstractButton::WAbstractButton(unsigned id)
:id_(id)
,state_(NORMAL)
{
	connect(this,&WAbstractButton::clicked,this,&WAbstractButton::slotClicked);
}
WAbstractButton::~WAbstractButton()
{
}
void WAbstractButton::mousePressEvent(QMouseEvent* event)
{
	if(Qt::LeftButton != event->button()) return;
	state_ |= PRESSED;
	BaseButton::mousePressEvent(event);
	update();
}
void WAbstractButton::mouseReleaseEvent(QMouseEvent* event)
{
	if(Qt::LeftButton != event->button()) return;
	state_ &= (~PRESSED);
	BaseButton::mouseReleaseEvent(event);
	update();
}
void WAbstractButton::enterEvent(QEvent* /*event*/)
{
	state_ |= HOVER;
	repaint();
	update();
}
void WAbstractButton::leaveEvent(QEvent* /*event*/)
{
	state_ &= (~HOVER);
	repaint();
	update();
}
void WAbstractButton::slotClicked()
{
	emit sigClicked(id_,isChecked());
}
bool WAbstractButton::setShortcut(const QString& shortcut)
{
	if(shortcut.isEmpty()) {
		if(nullptr != shortcut_) {
			shortcut_->deleteLater();
			shortcut_ = nullptr;
		}
		return true;
	}
	if(nullptr != (shortcut_ = new QShortcut(shortcut,this))) {
		connect(shortcut_,&QShortcut::activated,this,&WAbstractButton::slotShortcut);
		setToolTip(shortcut);
	}
	return nullptr != shortcut_;
}
void WAbstractButton::slotShortcut()
{
	if(isCheckable()) {
		if(isChecked())
			setChecked(false);
		else
			setChecked(true);
	}
	emit sigClicked(id_,isChecked());
}
void WAbstractButton::setChecked(bool checked)
{
	QAbstractButton::setChecked(checked);
	repaint();
	update();
}
/*================================================================================*/
const int WIconButton::s_base_offset = 8;
const int WIconButton::s_base_margin = 4;
WIconButton::WIconButton(const QString& title,const QString& icon_name,unsigned id,WOrientation orientation)
:WAbstractButton(id)
,title_(title)
,orientation_(orientation)
,background_color_(92,113,134)
,checked_background_color_(170,64,64)
{
	const unsigned fh          = fontMetrics().height();
	const unsigned v_icon_size = std::max<unsigned>(32u,fh *1.6f);
	const unsigned h_icon_size = std::max<unsigned>(22u,fh *1.1f);
	const QSize horizontal_icon_size(h_icon_size,h_icon_size);
	const QSize vertical_icon_size(v_icon_size,v_icon_size);

	setMinimumSize(minimumSize(orientation));
	switch(orientation_) {
		case Horizontal:
			setIcon(icon_name,horizontal_icon_size);
			break;
		case Vertical:
			setIcon(icon_name,vertical_icon_size);
			break;
		default:
			Q_ASSERT_X(0,__func__,"Error orientation");
			break;
	}
}
QSize WIconButton::minimumSize(WOrientation orientation)
{
	const unsigned fh          = QFontMetrics(qApp->font()).height();
	const unsigned v_icon_size = std::max<unsigned>(32u,fh *1.6f);
	const unsigned h_icon_size = std::max<unsigned>(22u,fh *1.1f);
	switch(orientation) {
		case Horizontal:
			return QSize(h_icon_size+s_base_margin+2*s_base_offset,h_icon_size+2*s_base_margin);
			break;
		default:
			return QSize(v_icon_size+2*s_base_margin,2*s_base_offset+v_icon_size+s_base_margin+fh);
			break;
	}
}
void WIconButton::setIcon(const QString& icon_name,const QSize& icon_size)
{
	const int   s_margins      = s_base_offset*2;
	const int   l_margins      = s_base_offset*2+s_base_margin;

	if(!icon_pixmap_.load(icon_name)) {
		ERR("Load pixmap faild:%s.",icon_name.toUtf8().data());
	}
	Q_ASSERT_X(!icon_pixmap_.isNull(),__func__,"Error pixmap");
	switch(orientation_) {
		case Horizontal:
			icon_size_   =   icon_size;
			size_        =   QSize(icon_size_.width()+fontMetrics().width(title_)+l_margins,icon_size_.height()+s_margins);
			setFixedHeight(size_.height());
			setMinimumWidth(size_.width());
			break;
		case Vertical:
			icon_size_   =   icon_size;
			size_        =   QSize(icon_size_.width()+s_margins,icon_size_.height()+fontMetrics().height()+l_margins);
			setFixedWidth(max(size_.width(),fontMetrics().width(title_)+4));
			setMinimumHeight(size_.height());
			break;
		default:
			Q_ASSERT_X(0,__func__,"Error orientation");
			break;
	}
	if(icon_pixmap_.size() != icon_size_) icon_pixmap_ = icon_pixmap_.scaled(icon_size_,Qt::KeepAspectRatio,Qt::SmoothTransformation);
	initPainterPath();
}
void WIconButton::paintEvent(QPaintEvent* /*event*/)
{
	int       i;
	int       icon_x_offset = 0;
	int       icon_y_offset = 0;
	int       text_x_offset = 0;
	int       text_y_offset = 0;
	QLinearGradient gradient(rect().x(),rect().y(),rect().x()+rect().width()*0.8,rect().bottom());
	QPainter painter(this);
	if(HOVER&state_&&!(PRESSED&state_)) {
		GradientNode node_array[] = {
			{0      ,   QColor(255   ,   255   ,   247)}   ,   
			{0.41   ,   QColor(255   ,   214   ,   73)}    ,   
			{0.73   ,   QColor(255   ,   216   ,   87)}    ,   
			{1      ,   QColor(255   ,   231   ,   151)}   ,   
		};
		for(i=0; i<ARRAY_SIZE(node_array); ++i) {
			gradient.setColorAt(node_array[i].position,node_array[i].color);
		}
	} else  if(PRESSED&state_||isChecked()) {
		GradientNode node_array[] = {
			{0      ,   QColor(201   ,   152   ,   97)}    ,   
			{0.41   ,   QColor(253   ,   163   ,   97)}    ,   
			{0.45   ,   QColor(251   ,   142   ,   60)}    ,   
			{1      ,   QColor(255   ,   208   ,   134)}   ,   
		};
		for(i=0; i<ARRAY_SIZE(node_array); ++i) {
			gradient.setColorAt(node_array[i].position,node_array[i].color);
		}
	}

	QFontMetrics mt = fontMetrics(); 
	switch(orientation_) {
		case Horizontal:
			icon_y_offset = (rect().height()-icon_pixmap_.height())/2;
			icon_x_offset = s_base_offset;
			text_x_offset = icon_x_offset+icon_pixmap_.width()+s_base_margin;
			text_y_offset = (rect().height()+mt.height())/2-mt.descent();
			break;
		case Vertical:
			icon_x_offset = (rect().width()-icon_pixmap_.width())/2;
			icon_y_offset = s_base_offset;
			text_x_offset = (width()-fontMetrics().width(title_))/2;
			text_y_offset = icon_y_offset+icon_pixmap_.height()+s_base_margin+mt.height()-mt.descent();
			break;
	}
	painter.setRenderHint(QPainter::Antialiasing,true);
	if(NORMAL==state_&&!isChecked()) {
		painter.fillPath(background_painter_path_,QBrush(QColor(255,255,255,0)));
	} else {
		painter.fillPath(background_painter_path_,QBrush(gradient));
		painter.fillPath(background_top_painter_path_,QBrush(QColor(255,255,255,100)));
	}
	painter.drawPixmap(rect().x()+icon_x_offset,rect().y()+icon_y_offset,icon_pixmap_);
	painter.drawText(rect().x()+text_x_offset,rect().y()+text_y_offset,title_);
}
void WIconButton::initPainterPath()
{
	const int radius   = 10;
	const int diameter = radius *2;
	const int x0       = rect().x();
	const int x1       = rect().x()+radius;
	const int x2       = rect().right()-radius;
	const int x3       = rect().right();
	const int y0       = rect().y();
	const int y1       = rect().y()+radius;
	const int y2       = rect().bottom()-radius;
	const int y3       = rect().bottom();
	const int y4       = y0+rect().height() *0.6;
	const int y5       = y0+rect().height() *0.4;

	background_painter_path_       =   QPainterPath();
	background_top_painter_path_   =   QPainterPath();

	background_painter_path_.moveTo(x0,y1);
	background_painter_path_.lineTo(x0,y2);
	background_painter_path_.arcTo(x0,y2-radius,diameter,diameter,180,90);
	background_painter_path_.lineTo(x2,y3);
	background_painter_path_.arcTo(x2-radius,y2-radius,diameter,diameter,270,90);
	background_painter_path_.lineTo(x3,y1);
	background_painter_path_.arcTo(x2-radius,y0,diameter,diameter,0,90);
	background_painter_path_.lineTo(x1,y0);
	background_painter_path_.arcTo(x0,y0,diameter,diameter,90,90);
	background_painter_path_.closeSubpath();

	background_top_painter_path_.moveTo(x0,y4);
	background_top_painter_path_.lineTo(x3,y5);
	background_top_painter_path_.lineTo(x3,y1);
	background_top_painter_path_.arcTo(x2-radius,y0,diameter,diameter,0,90);
	background_top_painter_path_.lineTo(x1,y0);
	background_top_painter_path_.arcTo(x0,y0,diameter,diameter,90,90);
	background_top_painter_path_.closeSubpath();
}
void WIconButton::resizeEvent(QResizeEvent* /*event*/)
{
	initPainterPath();
}
/*================================================================================*/
WToolButton::WToolButton(const QStringList& icon_list,unsigned id,const QSize& size)
:WAbstractButton(id)
{
	setIconSize(size);
	setIconList(icon_list);
}
bool WToolButton::setIconSize(const QSize& size)
{
	icon_size_ = size;
	setFixedSize(icon_size_.width()+2*margins_,icon_size_.height()+2*margins_);
	return true;
}
bool WToolButton::setIconList(const QStringList& icon_list)
{
	Q_ASSERT_X(icon_list.size()>0&&icon_list.size()<=3,__func__,"Error icon list size");
	normal_pix_ = QPixmap(icon_list.at(0));
	Q_ASSERT_X(!normal_pix_.isNull(),__func__,"ERROR icon");
	if(normal_pix_.size() != icon_size_) 
		normal_pix_ = normal_pix_.scaled(icon_size_,Qt::KeepAspectRatio,Qt::SmoothTransformation);
	if(icon_list.size()>=2) {
		pressed_pix_ = QPixmap(icon_list.at(1));
		Q_ASSERT_X(!pressed_pix_.isNull(),__func__,"ERROR icon");
		if(pressed_pix_.size() != icon_size_) 
			pressed_pix_= pressed_pix_.scaled(icon_size_,Qt::KeepAspectRatio,Qt::SmoothTransformation);
	} else {
		pressed_pix_ = normal_pix_;
		QPainter painter;
		painter.begin(&pressed_pix_);
		painter.fillRect(0,0,pressed_pix_.width(),pressed_pix_.height(),QColor(130,130,130));
		painter.drawPixmap(0,0,normal_pix_);
		painter.end();
	}
	if(icon_list.size()>=3) {
		hover_pix_ = QPixmap(icon_list.at(2));
		Q_ASSERT_X(!pressed_pix_.isNull(),__func__,"ERROR icon");
		if(hover_pix_.size() != icon_size_) 
			hover_pix_= hover_pix_.scaled(icon_size_,Qt::KeepAspectRatio,Qt::SmoothTransformation);
	} else {
		QPainter painter;
		hover_pix_ = normal_pix_;
		painter.begin(&hover_pix_);
		painter.fillRect(0,0,hover_pix_.width(),hover_pix_.height(),QColor(200,200,255));
		painter.drawPixmap(0,0,normal_pix_);
		painter.end();
	}
	return true;
}
void WToolButton::paintEvent(QPaintEvent* /*event*/)
{
	QPainter painter(this);
	if(state_&PRESSED) {
		painter.drawPixmap(margins_,margins_,pressed_pix_);
	} else if((state_&HOVER) || isChecked()) {
		painter.drawPixmap(margins_,margins_,hover_pix_);
	} else {
		painter.drawPixmap(margins_,margins_,normal_pix_);
	}
}
/*-------------------------------------------------------------*/
WButtonGroup::WButtonGroup()
{
}
WAbstractButton* WButtonGroup::button(unsigned index)
{
	if(index >= button_array_.size()) {
		ERR("Error index");
		return nullptr;
	}
	return button_array_[index];
}
void WButtonGroup::addButton(WAbstractButton* button)
{
	button_array_.push_back(button);
	if(button->isChecked())slotButtonClicked(button->id(),button->isChecked());
	connect(button,&WAbstractButton::sigClicked,this,&WButtonGroup::slotButtonClicked);
}
void WButtonGroup::slotButtonClicked(unsigned id,bool ischecked)
{
	if(!ischecked) return;
	for(auto it=button_array_.begin(); it!=button_array_.end(); ++it) {
		if((*it)->id() == id) continue;
		if((*it)->isChecked())(*it)->setChecked(false);
	}
}
void WButtonGroup::setChecked(WAbstractButton* btn)
{
	if(nullptr == btn) return;
	btn->setChecked(true);
	for(auto b:button_array_) {
		if(btn != b)b->setChecked(false);
	}
}
/*================================================================================*/
WStateButton::WStateButton(const QStringList& state_icon_list,const QSize& icon_size)
:icon_size_(40,40)
,state_(0)
{
	init(state_icon_list,icon_size);
}
void WStateButton::init(const QStringList& state_icon_list,const QSize& icon_size)
{
	if(!icon_size.isValid()) {
		const float ratio = 1.2f;
		icon_size_ = QSize(fontMetrics().height()*ratio,fontMetrics().height()*ratio);
	}
	pix_list_.clear();
	for(auto i=0; i<state_icon_list.size(); ++i) {
		pix_list_<<QPixmap(state_icon_list.at(i)).scaled(icon_size_, Qt::KeepAspectRatio,Qt::SmoothTransformation);
	}
	setFixedSize(icon_size_+QSize(2,2));
}
bool WStateButton::setState(unsigned state)
{
	if(state >= pix_list_.size()) return false;
	state_ = state;
	update();
	return true;
}
void WStateButton::paintEvent(QPaintEvent* /*e*/)
{
	QPainter painter(this);
	if(state_ >= pix_list_.size()) return;
	const QRect r = rect();
	painter.drawPixmap((width()-r.width())/2,(height()-r.height())/2,pix_list_.at(state_));
}
/******************************************************************************/
WSwitch::WSwitch(unsigned id)
:id_(id)
,icon_size_(40,40)
,state_(S_ON)
,read_only_(false)
{
	const float ratio = 1.8f;
	icon_size_ = QSize(fontMetrics().height()*ratio,fontMetrics().height()*ratio);
	btn_pix_ = QPixmap(imgP("switch_btn")).scaled(icon_size_, Qt::KeepAspectRatio,Qt::SmoothTransformation);
	setFixedSize(icon_size_.width()*2,icon_size_.height()+4);
}
void WSwitch::paintEvent(QPaintEvent* /*e*/)
{
	int i;
	QPainter painter(this);
	QRect r = rect();
	QLinearGradient gradient(rect().x(),rect().y(),rect().x()+rect().width()*0.8,rect().bottom());

	painter.setPen(Qt::NoPen);
	painter.setRenderHint(QPainter::Antialiasing,true);

	if(S_ON == state_) {
		GradientNode node_array[] = {
			{0      ,   QColor(0,   255   ,   0)}   ,   
			{0.41   ,   QColor(0,   214   ,   0)}    ,   
			{0.73   ,   QColor(0,   216   ,   0)}    ,   
			{1      ,   QColor(0,   231   ,   0)}   ,   
		};
		for(i=0; i<ARRAY_SIZE(node_array); ++i) {
			gradient.setColorAt(node_array[i].position,node_array[i].color);
		}
		painter.setBrush(gradient);
		painter.drawRoundedRect(r,r.height()/2,r.height()/2);
		painter.drawPixmap(QPoint(r.width()-btn_pix_.width()-2,(r.height()-btn_pix_.height())/2),btn_pix_);
	} else {
		GradientNode node_array[] = {
			{0      ,   QColor(200,   200   ,   200)}   ,   
			{0.41   ,   QColor(192,   192,   198)}    ,   
			{0.73   ,   QColor(175,   175,   180)}    ,   
			{1      ,   QColor(150,   150,   160)}   ,   
		};
		for(i=0; i<ARRAY_SIZE(node_array); ++i) {
			gradient.setColorAt(node_array[i].position,node_array[i].color);
		}
		painter.setBrush(gradient);

		painter.drawRoundedRect(r,r.height()/2,r.height()/2);
		painter.drawPixmap(QPoint(2,(r.height()-btn_pix_.height())/2),btn_pix_);
	}
	painter.setRenderHint(QPainter::Antialiasing,false);
}
void WSwitch::mousePressEvent(QMouseEvent* e)
{
	return QWidget::mousePressEvent(e);
}
void WSwitch::mouseReleaseEvent(QMouseEvent* e)
{
	if(read_only_) return;
	if(Qt::LeftButton != e->button()) {
		return QWidget::mouseReleaseEvent(e);
	}
	if(S_OFF == state_) {
		if(e->x() <= btn_pix_.width()) 
			return QWidget::mouseReleaseEvent(e);
		state_ = S_ON;
	} else {
		if(e->x() > width()-btn_pix_.width()) 
			return QWidget::mouseReleaseEvent(e);
		state_ = S_OFF;
	}
	emit sigClicked(id_,state_);
	update();
}
/******************************************************************************/
WLabelButton::WLabelButton(const QString& title,unsigned id)
:WAbstractButton(id)
,checked_color_(120,153,255)
{
	auto pl = palette();
	unchecked_color_ = pl.color(QPalette::Background);
	setText(title);
	setCheckable(true);
	connect(this,&WLabelButton::clicked,this,&WLabelButton::slotClicked);
	setAutoFillBackground(true);
	setMinimumSize(10+fontMetrics().width(title+"√")*1.1,fontMetrics().height()*1.2+10);
}
void WLabelButton::slotClicked(bool checked)
{
	checked = isChecked();
	update();
}
void WLabelButton::paintEvent(QPaintEvent* /*event*/)
{
	QPainter  painter(this);
	auto     &pl            = palette();

	painter.setRenderHint(QPainter::Antialiasing);
	if(isChecked()) {
		QPainterPath path;
		path.addRoundedRect(rect().adjusted(2,2,-2,-2),4,4);
		painter.setBrush(checked_color_);
		painter.setPen(Qt::NoPen);
		painter.drawPath(path);
		painter.setPen(Qt::white);
		painter.drawText(rect(),Qt::AlignCenter,QString("√")+text());
	} else {
		painter.fillRect(rect(),QBrush(pl.color(QPalette::Background)));
		painter.setPen(Qt::black);
		painter.drawText(rect(),Qt::AlignCenter,text());
		painter.setPen(Qt::gray);
		painter.setPen(Qt::DotLine);
		painter.drawRoundedRect(rect().adjusted(2,2,-2,-2),4,4);
	}
}
void WLabelButton::setChecked(bool checked)
{
	auto pl = palette();
	WAbstractButton::setChecked(checked);
	update();
}
