/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QtGui>
#include <bitset>
#include <stdexcept>
#include <vector>
#include "basexmlwidget.h"
class NXmlWidget:public BaseXmlWidget
{
	Q_OBJECT
	public:
		NXmlWidget(const QStringList& file_list,QWidget* parent=nullptr,int column_count=3,std::function<void(std::vector<WXmlData>*)> func=BaseXmlWidgetNullProcess);
		NXmlWidget(const QString& dir_path,QWidget* parent=nullptr,int column_count=3,std::function<void(std::vector<WXmlData>*)> func=BaseXmlWidgetNullProcess);
		NXmlWidget(QWidget* parent=nullptr,int column_count=3,std::function<void(std::vector<WXmlData>*)> func=BaseXmlWidgetNullProcess);
	public:
		virtual bool init(const QStringList& file_list)override;
		/*
		 * 数据量发生改变时调用
		 */
		virtual bool init()override;
		/*
		 * 当数据内容改变，但是数据数量没有改变时使用
		 * 否则调用init
		 */
		/*
		 * 第index个数据内容改变
		 */
		virtual void setViewData(int index)override;
		virtual void setViewData(int index,int gid)override;
		virtual void setViewData(int index,int gid,int iid)override;
		/*
		 * 将界面显示的值同步到数据里
		 */
		virtual void setModelData(int index)override;
		virtual void setModelData(int index,int gid)override;
		virtual void setModelData(int index,int gid,int iid)override;
		virtual int currentIndex()const override;
		/*
		 * 在tab_widget_后添加一个窗口
		 */
		void appendWidget(const QString& name,QWidget* wig);
	protected:
		QTabWidget            *tab_widget_   = nullptr;
	private:
		void initGUI();
};
