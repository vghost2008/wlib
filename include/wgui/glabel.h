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
class WGLabel:public QGraphicsItem
{
	public:
		WGLabel(const QString& text,WOrientation orientation=Horizontal,QGraphicsItem* parent=nullptr);
		using QGraphicsItem::QGraphicsItem;
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
		void setBackgroundColor(const QColor& color);
		void setTextColor(const QColor& color);
		virtual QRectF boundingRect()const override;
		void setText(const QString& text);
	private:
		QString      text_;
		QRectF       rect_;
		QColor       background_color_ = QColor(200,200,215);
		QColor       text_color_       = QColor(0,0,0);
		WOrientation orientation_      = Horizontal;
		const int    round_radius_     = 5;
		const float  margins_          = 10;
		const float  spacing_          = 5;
	private:
		virtual void   paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget)override;
		void init();
};
