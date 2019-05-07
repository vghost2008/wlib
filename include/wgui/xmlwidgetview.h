/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <memory>
#include <functional>
#include <array>
#include "xmlwidgetvdimp.h"

class XMLWidgetView:public QScrollArea
{
	Q_OBJECT
	public:
		using MakeEditorFunc = std::function<std::unique_ptr<AbstractXMLWidgetVDImp> (std::shared_ptr<WXmlItem>& ,int gid)>;
	public:
		XMLWidgetView(WXmlData& data,int column_count=3);
		bool setModelData();
		bool setModelData(int gid);
		bool setModelData(int gid,int iid);
		bool setViewData();
		bool setViewData(int gid);
		bool setViewData(int gid,int iid);
		inline int column_count()const { return column_count_; }
	private:
		void initGUI();
		static void initMakeEditorFuncs();
		std::unique_ptr<AbstractXMLWidgetVDImp> createEditor(std::shared_ptr<WXmlItem>& item,int gid);
		static int localWidth(std::shared_ptr<WXmlItem>& item);
	private:
		WXmlData &data_;
		int       column_count_ = 3;
		std::vector<std::unique_ptr<AbstractXMLWidgetVDImp>> imps_;
		static std::array<MakeEditorFunc,IT_NR> make_editor_funcs_;
		static std::array<MakeEditorFunc,IT_NR> make_readonly_editor_funcs_;
signals:
		void explain();
		void sigValueChanged(int gid,int iid);
};
