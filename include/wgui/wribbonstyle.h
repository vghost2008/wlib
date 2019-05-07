/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtGui>
#include <QtCore>
#include <QtWidgets>
#include <wsingleton.h>
using BaseRibbonStyle=QProxyStyle;
class WRibbonStyle:public BaseRibbonStyle,public WSingleton<WRibbonStyle>
{
	private:
		friend WSingleton<WRibbonStyle>;
	private:
		WRibbonStyle();
		void polish(QWidget* widget)override;
		void drawPrimitive(PrimitiveElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = nullptr)const override;
		void drawControl(ControlElement control, const QStyleOption *option, QPainter *painter, const QWidget *widget)const override;
		QRect subControlRect(ComplexControl control, const QStyleOptionComplex * option, SubControl subControl, const QWidget * widget = nullptr)const override;
		void drawComplexControl(ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = nullptr)const override;
		QRect subElementRect ( SubElement element, const QStyleOption * option, const QWidget * widget = 0 ) const override;
		static QPixmap cached(const QString &img);
	private:
		static WRibbonStyle* s_instance;
};
