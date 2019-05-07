/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "wgui_fwd.h"
class WAbstractButton:public QAbstractButton
{
	Q_OBJECT
	typedef QAbstractButton BaseButton;
	public:
		WAbstractButton(unsigned id);
		~WAbstractButton();
		inline void setID(int id) { id_ = id; }
		inline unsigned id()const { return id_; }
		bool setShortcut(const QString& shortcut);
		void setChecked(bool checked);
	protected:
		unsigned state_ = 0;
		unsigned id_    = 0;
	protected:
		void enterEvent(QEvent* event)override;
		void leaveEvent(QEvent* event)override;
		void mousePressEvent(QMouseEvent* event)override;
		void mouseReleaseEvent(QMouseEvent* event)override;
	private:
		QShortcut* shortcut_;
	private:
		void slotClicked();
		void slotShortcut();
	signals:
		void explain();
		void sigClicked(unsigned id,bool checked);
};
class WIconButton:public WAbstractButton
{
	public:
		WIconButton(const QString& title,const QString& icon_name,unsigned id,WOrientation orientation=Vertical);
		inline void setIcon(const QString& icon_name) {
			setIcon(icon_name,icon_size_);
			update();
		}
		inline void setIconAndTitle(const QString& icon_name, const QString& title) {
			title_ = title;
			setIcon(icon_name);
		}
		void setIcon(const QString& icon_name,const QSize& icon_size);
		static QSize minimumSize(WOrientation orientation=Vertical);
	private:
		static const int s_base_offset;
		static const int s_base_margin;
		QString          title_;
		QPixmap          icon_pixmap_;
		QSize            size_;
		QSize            icon_size_;
		QColor           background_color_;
		QColor           checked_background_color_;
		WOrientation     orientation_;
		QPainterPath     background_painter_path_;
		QPainterPath     background_top_painter_path_;
	private:
		void paintEvent(QPaintEvent* event)override;
		void initPainterPath();
		void resizeEvent(QResizeEvent* event)override;
};
class WToolButton:public WAbstractButton
{
	public:
		WToolButton(const QStringList& icon_list,unsigned id,const QSize& size=QSize(24,24));
		bool setIconList(const QStringList& icon_list);
		bool setIconSize(const QSize& size);
	private:
		QPixmap   normal_pix_;
		QPixmap   hover_pix_;
		QPixmap   pressed_pix_;
		QSize     icon_size_;
		const int margins_     = 1;
	private:
		void paintEvent(QPaintEvent* event)override;
};
/*
 * 一个Group中的按钮仅有一个选中
 * 每个按钮的id不能相同
 */
class WButtonGroup:public QObject
{
	public:
		WButtonGroup();
		WAbstractButton* button(unsigned index);
		void addButton(WAbstractButton* button);
		size_t size()const { return button_array_.size();}
		void setChecked(WAbstractButton* btn);
	private:
		std::vector<WAbstractButton*> button_array_;
	private:
		void slotButtonClicked(unsigned id,bool ischecked);
};
class WSwitch:public QWidget
{
	Q_OBJECT
	public:
		enum State {
			S_OFF   ,   
			S_ON    ,   
		};
	public:
		WSwitch(unsigned id);
		void setState(State s) { 
			state_ = s;
			update(); 
		}
		inline State state()const { return state_;}
		void setReadOnly(bool readonly) { read_only_ = readonly;}
	private:
		unsigned id_;
		State    state_;
		QPixmap  btn_pix_;
		QSize    icon_size_;
		bool     read_only_;
	private:
		void mousePressEvent(QMouseEvent* e)override;
		void mouseReleaseEvent(QMouseEvent* e)override;
		void paintEvent(QPaintEvent* e)override;
signals:
		void explain();
		void sigClicked(unsigned id,bool ischecked);
};
/*
 * 用于显示状态，每一个状态通过一个位图表示
 * 状态0显示第0个位图,状态1显示第一个位图
 */
class WStateButton:public QWidget
{  
	public:
		WStateButton(const QStringList& state_icon_list,const QSize& icon_size=QSize());
		void init(const QStringList& state_icon_list,const QSize& icon_size=QSize());
		bool setState(unsigned state);
		inline unsigned state()const { return state_;}
	private:
		unsigned       state_;
		QList<QPixmap> pix_list_;
		QSize          icon_size_;
	private:
		void paintEvent(QPaintEvent* e)override;
};
/*
 * 显示一个可以选中或不选中的按钮
 * 状态通过不同的颜色表示
 */
class WLabelButton:public WAbstractButton
{
	public:
		WLabelButton(const QString& title,unsigned id=0);
		void setCheckedColor(const QColor& color);
		void setChecked(bool checked);
	protected:
		void paintEvent(QPaintEvent* e)override;
	private:
		QColor checked_color_;
		QColor unchecked_color_;
	private:
		void slotClicked(bool checked);
};
