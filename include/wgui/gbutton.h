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
#include "wgui_fwd.h"
class WGButtonGroup;
class WGAbstractButton:public QObject,public QGraphicsItem
{
	Q_OBJECT
	Q_PROPERTY(QPointF pos READ pos WRITE setPos)
	public:
		WGAbstractButton(unsigned id=0,QGraphicsItem* parent=nullptr);
		virtual ~WGAbstractButton();
		inline void setWidth(float width) {
			rect_.setWidth(width);
			update();
		}
		inline void setHeight(float height) {
			rect_.setHeight(height);
			update();
		}
		inline float width()const {
			return boundingRect().width();
		}
		inline float height()const {
			return boundingRect().height();
		}
		virtual QRectF boundingRect()const override;
		QPointF leftCenterPoint()const;
		QPointF rightCenterPoint()const;
	public:
		inline void setCheckable(bool checkable) {
			checkable_   =   checkable;
			flag_ &= (~F_CHECKED);
		}
		inline void setChecked(bool checked) {
			if(!checkable_) return ;
			if(checked) {
				flag_ |= F_CHECKED;
			} else {
				flag_ &= (~F_CHECKED);
			}
			update();
		}
		inline bool isChecked()const {
			if(!checkable_) return false;
			return flag_&F_CHECKED;
		}
		inline void setId(unsigned id) {
			id_ = id;
		}
		inline unsigned id()const { return id_; }
	protected:
		enum Flag
		{
			F_HOVER          =   0x00000001,
			F_CHECKED        =   0x00000002,
			F_LBUTTON_DOWN   =   0x00000004,
		};
	protected:
		unsigned    flag_         = 0;
		bool        checkable_    = false;
		unsigned    id_           = 0;
		QRectF      rect_;
		const float margins_      = 10;
		const float spacing_      = 5;
		friend      WGButtonGroup;
	private:
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event)override;
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event)override;
		void mousePressEvent(QGraphicsSceneMouseEvent* event)override;
		void mouseReleaseEvent(QGraphicsSceneMouseEvent* event)override;
signals:
		void sigClicked(unsigned id,bool ischecked);
		void sigHovered(unsigned id,bool isin);
};
class WGButtonGroup:public QObject
{
	public:
		WGButtonGroup();
		WGAbstractButton* button(unsigned index);
		void addButton(WGAbstractButton* button);
		void clear() { button_array_.clear(); }
		size_t size()const { return button_array_.size(); }
		unsigned checkedID()const;
	private:
		std::vector<WGAbstractButton*> button_array_;
	private:
		void slotButtonClicked(unsigned id,bool ischecked);
};
class WGButton:public WGAbstractButton
{
	public:
		WGButton(const QString& title,unsigned id,QGraphicsItem* parent=nullptr);
	private:
		QString   title_;
		const int round_radius_ = 10;
	private:
		virtual void paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget)override;
};
/******************************************************************************/
class WGIconButton:public WGAbstractButton
{
	public:
		WGIconButton(const QString& title,const QString& icon_path,GraphicsPlace icon_pos,unsigned id=0,QGraphicsItem* parent=nullptr);
	private:
		virtual void paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget)override;
		void paintIconLeft(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget);
		void paintIconTop(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget);
		inline void setIconSize(const QSize& size) {
			icon_size_ = size;
			rect_ = QRectF();
		}
	private:
		QPixmap       pix_;
		QString       title_;
		QSize         icon_size_;
		const int     round_radius_ = 10;
		GraphicsPlace icon_pos_     = G_TOP;
};
/*================================================================================*/
class WGStateButton:public QGraphicsItem
{
	public:
		WGStateButton(const QStringList& state_icon_list,QGraphicsItem* parent=nullptr);
		bool setState(unsigned state);
		inline unsigned state()const { return state_;}
	private:
		unsigned       state_;
		QRectF         rect_;
		QList<QPixmap> pix_list_;
		QStringList    state_icon_list_;
		const int      margins_         = 10;
		const int      spacing_         = 5;
	private:
		virtual void paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget)override;
		virtual QRectF boundingRect()const override;
};
/******************************************************************************/
class WGTextButton:public WGAbstractButton
{
	public:
		WGTextButton(const QString& text,unsigned id,WOrientation orientation=Horizontal,QGraphicsItem* parent=nullptr);
		void setBackgroundColor(const QColor& color);
		void setTextColor(const QColor& color);
		virtual QRectF boundingRect()const override;
		void setText(const QString& text);
	private:
		QString      text_;
		QColor       background_color_;
		QColor       text_color_;
		WOrientation orientation_;
		const int    round_radius_     = 5;
		const float  margins_          = 10;
		const float  spacing_          = 5;
	private:
		virtual void   paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget)override;
		void init();
};
