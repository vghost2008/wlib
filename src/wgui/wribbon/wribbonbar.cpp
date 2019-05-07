/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "wribbonbar.h"
#include "wmacros.h"
#include "toolkit.h"
#include "wribbonstyle.h"
#include <QtGlobal>
WRibbonBar::WRibbonBar()
:tab_bar_(nullptr)
,stacked_widget_(nullptr)
{
	initGUI();

    setStyle(&WRibbonStyle::instance_i());
	initConnect();
}
void WRibbonBar::initGUI()
{
	QVBoxLayout *layout  = new QVBoxLayout(this);
	QHBoxLayout *hlayout = new QHBoxLayout;

	btn_pick_up_      =   new WToolButton(QStringList()<<imgP("pick_up"),0);
	tab_bar_          =   new WTabBar;
	stacked_widget_   =   new QStackedWidget;
	hlayout->addWidget(tab_bar_);
	hlayout->addWidget(btn_pick_up_);
	layout->addLayout(hlayout);
	layout->addWidget(stacked_widget_);

	QLinearGradient gradient(0,0,100,height());

	GradientNode node_array[] = {
		{0      ,   QColor(219   ,   230   ,   244)}   ,   
		{0.21   ,   QColor(200   ,   217   ,   237)}   ,   
		{0.28   ,   QColor(200   ,   216   ,   237)}   ,   
		{1      ,   QColor(231   ,   242   ,   255)}   ,   
	};
	for(int i=0; i<ARRAY_SIZE(node_array); ++i) {
		gradient.setColorAt(node_array[i].position,node_array[i].color);
	}
	QPalette palette = stacked_widget_->palette();
	palette.setBrush(QPalette::Background,QBrush(gradient));
	setPalette(palette);
	setAutoFillBackground(true);

	hlayout->setSpacing(0);
	hlayout->setContentsMargins(0,0,0,0);
	layout->setSpacing(0);
	layout->setContentsMargins(0,0,0,0);
	stacked_widget_->layout()->setSpacing(0);
	stacked_widget_->layout()->setContentsMargins(0,0,0,0);
	tab_bar_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	//tab_bar_->setFixedHeight(fontMetrics().height()*2.4f);
	const int mh = WIconButton::minimumSize(Vertical).height()+10.0f+2.2f*fontMetrics().height();
	stacked_widget_->setMaximumHeight(mh);
}
void WRibbonBar::initConnect()
{
	connect(tab_bar_,&WTabBar::currentChanged,this,&WRibbonBar::slotTabChanged);
	connect(tab_bar_,&WTabBar::sigDoubleClicked,this,&WRibbonBar::slotPickUp);
	connect(btn_pick_up_,&WToolButton::clicked,this,&WRibbonBar::slotPickUp);
}
bool WRibbonBar::insertTab(const QString& title,WRibbonWidget* widget)
{
	Q_ASSERT_X(nullptr != widget,__func__,"Error widget");
	stacked_widget_->addWidget(widget);
	tab_bar_->addTab(title);
	return true;
}
void WRibbonBar::slotTabChanged(int index)
{
	if(index>=stacked_widget_->count()) return;
	stacked_widget_->setCurrentIndex(index);
}
void WRibbonBar::slotPickUp()
{
	if(stacked_widget_->isVisible()) {
		stacked_widget_->setVisible(false);
		btn_pick_up_->setIconList(QStringList()<<imgP("pick_down"));
	} else {
		stacked_widget_->setVisible(true);
		btn_pick_up_->setIconList(QStringList()<<imgP("pick_up"));
	}
}
/*--------------------------------------------------------*/
WRibbonWidget::WRibbonWidget(WRibbonBar* parent)
:parent_(parent)
,layout_(nullptr)
{
	layout_ = new QHBoxLayout(this);
	layout_->setContentsMargins(5,0,5,5);
	layout_->setSpacing(3);
}
void WRibbonWidget::addWidget(QWidget* widget,const QString& title)
{
	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(widget);
	addLayout(layout,title);
}
void WRibbonWidget::addLayout(QLayout* layout,const QString& title)
{
	QGroupBox* gbox = new QGroupBox(title);
	gbox->setLayout(layout);
	layout_->addWidget(gbox);
	layout_->addWidget(new WGuiSeparator(WOrientation::Vertical));
}
void WRibbonWidget::addControl(QWidget* ctrl)
{
	layout_->addWidget(ctrl);
}
void WRibbonWidget::addStretch()
{
	layout_->addStretch();
}
/*================================================================================*/
WTabBar::WTabBar()
{
	setExpanding(false);
	setDocumentMode(false);
}
void WTabBar::mouseDoubleClickEvent(QMouseEvent* event)
{
	if(Qt::LeftButton != event->button()) return;
	emit sigDoubleClicked();
}
QSize WTabBar::tabSizeHint(int index)const 
{
#ifdef Q_OS_WIN32
	return QSize(fontMetrics().width(tabText(index))+50,fontMetrics().height()*2.4);
#else
	return QSize(fontMetrics().width(tabText(index))+50,fontMetrics().height()*2);
#endif
}
