/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "xmlstream.h"
#define SETITEM(fname,index) bool setItem##fname(const QString& name) \
{ \
	if(model()->rowCount() == 0) { return false; } \
	setData(model()->rowCount()-1,index,name); \
	return true; \
}
/*
 * 配置视图
 */
class NormalXmlViewDelegate;
class SetFloatMinAndMaxViewDelegate;
class WXmlView:public QTableWidget
{
	public:
		WXmlView(QWidget* parent=nullptr);
		bool setXmlData(WXmlData* data);
		QVariant hideData(int row,int column)const {
			return model()->data(model()->index(row,column),Qt::UserRole);
		}
		bool setViewData();
		bool isSelectedGroup()const;
		bool forceDeleteCurrentGroup();
		bool haveSelected()const;
		/*
		 * 获取当前组，如果选择的为item,则返回item所在的组号
		 * 如果没有选择已经存在的组(如Add项）则返回负数
		 */
		int getCurrentGroup()const;
		QString getCurrentGroupName()const;
		inline WXmlData* data() { return data_; }
		inline const WXmlData* data()const { return data_; }
	private:
		NormalXmlViewDelegate         *normal_view_delegate_;
		SetFloatMinAndMaxViewDelegate *setfloat_min_and_max_view_delegate_;
		WXmlData                      *data_;                                 //相识
	private:
		void initGUI();
		bool newGroup(const QString& name);
		bool newItem();
		SETITEM(Name,0);
		SETITEM(Value,1);
		SETITEM(Desc,2);
		inline bool setItemData(const QString& name,int column) {
			if(model()->rowCount() == 0) { return false; } 
			setData(model()->rowCount()-1,column,name); 
			return true; 
		}
		void setData(int row,int column,const QVariant& value) {
			model()->setData(model()->index(row,column),value);
		}
		void setHideData(int row,int column,const QVariant& value) {
			model()->setData(model()->index(row,column),value,Qt::UserRole);
		}
		DISABLE_COPY_AND_ASSIGN(WXmlView);
};
