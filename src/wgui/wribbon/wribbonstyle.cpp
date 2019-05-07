/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "wribbonstyle.h"
#include <wgui.h>
#include <toolkit.h>

WRibbonStyle* WRibbonStyle::s_instance = nullptr;
WRibbonStyle::WRibbonStyle()
:BaseRibbonStyle(QStyleFactory::create("windows"))
{
}
void WRibbonStyle::polish(QWidget* widget)
{
	QList<QWidget*> child_widget_list = widget->findChildren<QWidget*>();
	for(int i=0; i<child_widget_list.size(); ++i) {
		child_widget_list[i]->setStyle(this);
	}
}
void WRibbonStyle::drawPrimitive(PrimitiveElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget) const
{
	switch(element) {
		case PE_FrameTabBarBase:
			{
				const QStyleOptionTabBarBase *tab_option = qstyleoption_cast<const QStyleOptionTabBarBase*>(option);
				const int                     y          = tab_option->rect.bottom()-2;
				if(!(tab_option->state&QStyle::State_Enabled)) return;
				painter->save();
				painter->setPen(QColor(255,255,255));
				painter->drawLine(tab_option->rect.x(),y,tab_option->selectedTabRect.x(),y);
				painter->drawLine(tab_option->selectedTabRect.right(),y,tab_option->rect.right(),y);
				painter->restore();
				return;
			}
			break;
		case PE_FrameGroupBox:
			if(const QStyleOptionFrameV2* group 
					= qstyleoption_cast<const QStyleOptionFrameV2*>(option)) {
				//显示GroupBox基本部分
				const QRect &r       = group->rect;
				int          radius  = 14;
				int          radius2 = radius *2;
				QPainterPath painter_path;
				GradientNode node_array[] = {
					{0      ,   QColor(222   ,   232   ,   245)}   ,   
					{0.24   ,   QColor(199   ,   216   ,   237)}   ,   
					{1      ,   QColor(215   ,   231   ,   245)}   ,   
				};
				QLinearGradient gradient(r.topLeft(),r.bottomLeft());
				for(int i=0; i<ARRAY_SIZE(node_array); ++i) {
					gradient.setColorAt(node_array[i].position,node_array[i].color);
				}
				painter_path.moveTo(radius, 0);
				painter_path.arcTo(r.right() - radius2, 0, radius2, radius2, 90, -90);
				painter_path.lineTo(r.right(),r.bottom());
				painter_path.lineTo(r.left(),r.bottom());
				painter_path.arcTo(r.left(), r.top(), radius2, radius2, 180, -90);
				painter->save();
				painter->setRenderHint(QPainter::Antialiasing,true);
				painter->setBrush(QBrush(gradient));
				painter->setPen(QColor(50,50,50,100));
				painter->drawPath(painter_path);
				painter->restore();
			}
			break;
		case PE_FrameFocusRect:
			break;
		default:
			return BaseRibbonStyle::drawPrimitive(element,option,painter,widget);
			break;
	}
}
void WRibbonStyle::drawControl(ControlElement control, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
	switch(control) {
		case CE_TabBarTabShape:
			{
				const int              radius       = 10;
				const int              radius2      = radius *2;
				const QStyleOptionTab *tab_option   = qstyleoption_cast<const QStyleOptionTab *>(option);
				const QRect            r            = option->rect.adjusted(0,0,0,-3);
				QPainterPath           painter_path;

				if(nullptr == tab_option)  
					return BaseRibbonStyle::drawControl(control,option,painter,widget);
				painter->save();

				painter->setRenderHint(QPainter::Antialiasing,true);
				painter_path.moveTo(r.left(),r.bottom());
				painter_path.lineTo(r.left(),r.y()+radius);
				painter_path.arcTo(r.left(),r.y(),radius2,radius2,180,-90);
				painter_path.arcTo(r.right()-radius2,r.y(),radius2,radius2,90,-90);
				painter_path.lineTo(r.right(),r.bottom());
				if(tab_option->state&State_Selected) {
					QLinearGradient gradient(0,0,0,r.height());
					gradient.setSpread(QGradient::ReflectSpread);
					GradientNode node_array[] = {
						{0      ,   QColor(240   ,   246   ,   255)}   ,   
						{0.17   ,   QColor(246   ,   250   ,   255)}   ,   
						{0.8    ,   QColor(219   ,   230   ,   245     ,   100)}   ,   
						{1      ,   QColor(219   ,   230   ,   245     ,   0)}     ,   
					};
					for(int i=0; i<ARRAY_SIZE(node_array); ++i) {
						gradient.setColorAt(node_array[i].position,node_array[i].color);
					}
					painter->setBrush(QBrush(gradient));
					painter->setPen(QPen(QColor(50,50,50,100)));
					painter->drawPath(painter_path);
				} 
				painter->restore();
			}
			break;
		case CE_PushButtonBevel:
		case CE_PushButtonLabel:
			break;
		default:
			return BaseRibbonStyle::drawControl(control,option,painter,widget);
	}
}
void WRibbonStyle::drawComplexControl ( ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget ) const
{
	switch(control) {
		case CC_GroupBox:
			if(const QStyleOptionGroupBox* group
					= qstyleoption_cast<const QStyleOptionGroupBox*>(option)) {
				//显示GroupBox标题
				QStyleOptionGroupBox group_box_copy(*group);
				group_box_copy.subControls &=  ~SC_GroupBoxLabel;
				BaseRibbonStyle::drawComplexControl(control,&group_box_copy,painter,widget);
				if(group->subControls&SC_GroupBoxLabel) {
					QRect               title_rect = subControlRect(control,option,SC_GroupBoxLabel,widget);
					const QFontMetrics &mt         = option->fontMetrics;
					painter->save();
					painter->setRenderHint(QPainter::Antialiasing,true);
					painter->setBrush(QBrush(QColor(173,196,230)));
					painter->setPen(Qt::NoPen);
					painter->drawRect(title_rect);
					painter->setPen(QColor(0,0,0));
					painter->drawText(title_rect.center().x()-mt.width(group->text)/2,
							title_rect.center().y()+mt.height()/2-mt.descent(),group->text);
					painter->restore();
				}
			}
			break;
		case CC_TitleBar:
			//MdiSubwindow title bar
			if(const QStyleOptionTitleBar* title_bar 
					= qstyleoption_cast<const QStyleOptionTitleBar*>(option)) {
				QStyleOptionTitleBar title_bar_copy(*title_bar);
				title_bar_copy.subControls &= ~SC_TitleBarLabel;
				if(title_bar->subControls&SC_TitleBarLabel) {
					QRect           r            = title_bar->rect;

					painter->save();
					painter->setRenderHint(QPainter::Antialiasing,true);
					if(title_bar->state&State_Active) {
						QLinearGradient gradient(0,0,0,option->rect.height());
						gradient.setSpread(QGradient::ReflectSpread);
						GradientNode node_array[] = {
							{0      ,   QColor(120,   180,   255)}   ,   
							{0.17   ,   QColor(130,   200,   255)}   ,   
							{1      ,   QColor(100,   153,   255)}   ,   
						};
						for(int i=0; i<ARRAY_SIZE(node_array); ++i) {
							gradient.setColorAt(node_array[i].position,node_array[i].color);
						}
						painter->setBrush(QBrush(gradient));
					} else {
						painter->setBrush(QBrush(QColor(151,151,155)));
					}
					painter->setPen(Qt::NoPen);
					painter->drawRect(r);
					painter->setPen(QColor(0,0,0));
					painter->drawText(title_bar->rect,Qt::AlignCenter,title_bar->text);
					painter->restore();
				}
				BaseRibbonStyle::drawComplexControl(control,&title_bar_copy,painter,widget);
			}
			break;
		case CC_MdiControls:
			return BaseRibbonStyle::drawComplexControl(control,option,painter,widget);
			break;
		default:
			return BaseRibbonStyle::drawComplexControl(control,option,painter,widget);
			break;
	}
}
QRect WRibbonStyle::subControlRect ( ComplexControl control, const QStyleOptionComplex * option, SubControl subControl, const QWidget * widget ) const 
{
	QRect rect;
	switch(control) {
		case CC_GroupBox:
			if(const QStyleOptionGroupBox* group
					= qstyleoption_cast<const QStyleOptionGroupBox*>(option)) {
				switch(subControl) {
					case SC_GroupBoxContents:
						{
							rect = BaseRibbonStyle::subControlRect(control,option,subControl,widget);
							const int h =  BaseRibbonStyle::subControlRect(control,option,SC_GroupBoxLabel,widget).height();
							rect.setY(0);
							rect.adjust(0,0,0,-h);
						}
						break;
					case SC_GroupBoxFrame:
						rect = group->rect;
						break;
					case SC_GroupBoxLabel:
						{
							const int delta = 4;
							const int h =  BaseRibbonStyle::subControlRect(control,option,SC_GroupBoxLabel,widget).height()+2*delta;
							rect = QRect(group->rect.x()+delta,group->rect.bottom()-h,group->rect.width()-delta*2,h-delta);
						}
						break;
					default:
						rect = BaseRibbonStyle::subControlRect(control,option,subControl,widget);
						break;
				}
			}
			break;
		default:
			return BaseRibbonStyle::subControlRect(control,option,subControl,widget);
			break;
	}
	return rect;

}
QPixmap WRibbonStyle::cached(const QString &img)
{
	if (QPixmap *p = QPixmapCache::find(img)) return *p;

	QPixmap pm;
	pm = QPixmap::fromImage(QImage(img), Qt::OrderedDither | Qt::OrderedAlphaDither);
	if (pm.isNull())
		return QPixmap();

	QPixmapCache::insert(img, pm);
	return pm;
}
QRect WRibbonStyle::subElementRect(SubElement element, const QStyleOption * option, const QWidget * widget) const 
{
	switch(element) {
		case QStyle::SE_ProgressBarLabel:
			{
				const QStyleOptionProgressBarV2* progress = qstyleoption_cast<const QStyleOptionProgressBarV2*>(option);
				const int text_width = option->fontMetrics.width(progress->text);
				QRect r((option->rect.width()-text_width)/2,0,text_width,option->rect.height());
				return r;
			}
			break;
		case QStyle::SE_ProgressBarGroove:
			return option->rect.adjusted(2,2,-2,-2);
			break;
		case QStyle::SE_ProgressBarContents:
			{
				const QStyleOptionProgressBarV2 *progress       = qstyleoption_cast<const QStyleOptionProgressBarV2*>(option);
				QRect                            rect           = progress->rect.adjusted(5,5,-5,-5);
				const int                        progress_width = progress->maximum-progress->minimum;
				if(progress_width > 0) 
					rect.setWidth(rect.width()*(progress->progress-progress->minimum)/progress_width);
				return rect;
			}
			break;
		default:
			return	BaseRibbonStyle::subElementRect(element,option,widget);
			break;
	}
}
