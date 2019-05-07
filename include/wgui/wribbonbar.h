/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QtGui>
#include "wgui.h"
class WRibbonWidget;
class WTabBar;
class WRibbonBar:public QWidget
{
	Q_OBJECT
	public:
		WRibbonBar();
	protected:
		bool insertTab(const QString& title,WRibbonWidget* widget);
	private:
		WTabBar        *tab_bar_        = nullptr;
		QStackedWidget *stacked_widget_ = nullptr;
		WToolButton    *btn_pick_up_    = nullptr;
	private:
		void initConnect();
		void initGUI();
	private :
		void slotTabChanged(int index);
		void slotPickUp();
	signals:
		void explain();
		void sigButtonClicked(unsigned id,bool ischecked);
};
class WRibbonWidget:public QWidget
{
	Q_OBJECT
	public:
		WRibbonWidget(WRibbonBar* parent);
		void addWidget(QWidget* widget,const QString& title);
		void addLayout(QLayout* widget,const QString& title);
		void addControl(QWidget* ctrl);
		void addStretch();
	protected:
		WRibbonBar     *parent_ = nullptr;
	private:
		QHBoxLayout *layout_ = nullptr;
};
class WTabBar:public QTabBar
{
	Q_OBJECT
	public:
		WTabBar();
		QSize tabSizeHint(int index)const override;
		void mouseDoubleClickEvent(QMouseEvent* event)override;
	signals:
		void sigDoubleClicked();
};
