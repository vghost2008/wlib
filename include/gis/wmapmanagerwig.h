/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QtGui>
#include "wmapdata_fwd.h"
#include <wgui/wgui.h>
enum 
{
	MAV_FILE_NAME         ,  
	MAV_POLYGON_TYPE      ,  
	MAV_FILL_COLOR        ,  
	MAV_LINE_COLOR        ,  
	MAV_LINE_WIDTH        ,  
	MAV_POINT_SIZE        ,  
	MAV_POINT_TYPE        ,  
	MAV_TEXT_COLOR        ,  
	MAV_TEXT_SIZE         ,  
	MAV_TEXT_OFFSET       ,  
	MAV_LEVEL             ,  
	MAV_SHOW_LABEL        ,  
};
const int kMavNr = MAV_SHOW_LABEL+1;
class WMapAttrViewDelegate:public QItemDelegate
{
	public:
		virtual void paint(QPainter* painter,const QStyleOptionViewItem& option,const QModelIndex& index)const override;
		virtual QWidget *	createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const override;
		virtual void	setEditorData ( QWidget * editor, const QModelIndex & index ) const override;
		virtual void	setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const override;
		virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
class WMapLayoutAttrWig:public QWidget
{
	public:
		WMapLayoutAttrWig();
		bool getLayout(WMapLayout* layout);
		bool setLayout(const WMapLayout& layout);
	private:
		void initGUI();
	private:
		QLineEdit       *file_name_   = nullptr;
		WColorWidget    *fill_color_  = nullptr;
		WColorWidget    *text_color_  = nullptr;
		WColorWidget    *line_color_  = nullptr;
		WDoubleLineEdit *line_width_  = nullptr;
		WDoubleLineEdit *point_size_  = nullptr;
		WIntLineEdit    *text_size_   = nullptr;
		QCheckBox       *show_text_   = nullptr;
		QCheckBox       *show_layout_ = nullptr;
};
class WMapAttrWig:public QWidget
{
	Q_OBJECT
	public:
		WMapAttrWig();
		bool getLayouts(std::vector<WMapLayout>* layouts);
		bool setLayouts(const std::vector<WMapLayout>& layouts);
	private:
		void initGUI();
		virtual void contextMenuEvent(QContextMenuEvent* event)override;
		int selectedRow()const;
	private:
		QTableWidget *table_view_ = nullptr;
		QMenu        *menu_       = nullptr;
	private slots:
		void slotMenuAction();
};
