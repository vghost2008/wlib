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
/*
 * 与NXmlWidget的区别为
 * 1,使用列表显示不同的文件
 * 2,所有文件共享一个视图，可以有效的管理大量的文件
 */
class LXmlWidget:public BaseXmlWidget
{
	Q_OBJECT
	struct XMLWidgetViewDeleter{
		inline void operator()(XMLWidgetView* ptr){ ptr->deleteLater();}
	};
	public:
		LXmlWidget(const QStringList& file_list,QWidget* parent=nullptr,int column_count=3,std::function<void(std::vector<WXmlData>*)> func=BaseXmlWidgetNullProcess);
		LXmlWidget(const QString& dir_path,QWidget* parent=nullptr,int column_count=3,std::function<void(std::vector<WXmlData>*)> func=BaseXmlWidgetNullProcess);
		LXmlWidget(QWidget* parent=nullptr,int column_count=3,std::function<void(std::vector<WXmlData>*)> func=BaseXmlWidgetNullProcess);
	public:
		/*
		 * 当数据内容改变，但是数据数量没有改变时使用
		 * 否则调用init
		 */
		virtual void setViewData()override;
		virtual void setViewData(int index)override;
		virtual void setViewData(int index,int gid)override;
		virtual void setViewData(int index,int gid,int iid)override;
		virtual void setModelData()override;
		virtual void setModelData(int index)override;
		virtual void setModelData(int index,int gid)override;
		virtual void setModelData(int index,int gid,int iid)override;
		virtual int currentIndex()const override;
		/*
		 * 数据量发生改变时调用
		 */
		virtual bool init()override;
		virtual bool init(const QStringList& file_list)override;
	protected:
		QListWidget *list_widget_   = nullptr;
		QVBoxLayout *widget_layout_ = nullptr;
		std::unique_ptr<XMLWidgetView,XMLWidgetViewDeleter> current_widget_;
	private:
		void initGUI();
		void setCurrentDataIndex(unsigned i);
	public:
		void slotCurrentFileChanged(int index);
};
