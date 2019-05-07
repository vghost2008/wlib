/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#include <nxmlwidget.h>
#include <limits>
#include <xmlstream.h>

using namespace std;

NXmlWidget::NXmlWidget(const QStringList& file_list,QWidget* parent,int column_count,function<void(vector<WXmlData>*)> func)
:BaseXmlWidget(file_list,parent,column_count,func)
{
	initGUI();
	init(file_list);
}
NXmlWidget::NXmlWidget(const QString& dir_path,QWidget* parent,int column_count,function<void(vector<WXmlData>*)> func)
:BaseXmlWidget(dir_path,parent,column_count,func)
{
	initGUI();
	init(file_list_);
}
NXmlWidget::NXmlWidget(QWidget* parent,int column_count,function<void(vector<WXmlData>*)> func)
:BaseXmlWidget(parent,column_count,func)
{
	initGUI();
}
void NXmlWidget::initGUI()
{

	tab_widget_   =  new QTabWidget;
	main_layout_->addWidget(tab_widget_);
}
void NXmlWidget::appendWidget(const QString& name,QWidget* widget)
{
	tab_widget_->addTab(widget,name);
}
bool NXmlWidget::init(const QStringList& file_list)
{
	return BaseXmlWidget::init(file_list);
}
bool NXmlWidget::init()
{
	for(auto i=tab_widget_->count()-1; i>=0; --i) {
		auto widget = tab_widget_->widget(i);
		tab_widget_->removeTab(i);
		widget->deleteLater();
	}
	data_list_.clear();
	try {
		int i=0;
		while(i<std::numeric_limits<decltype(i)>::max()) {
			WXmlData xml_data;
			auto file_path = getFilePathByIndex(i);
			++i;
			if(!getXmlData(file_path,&xml_data)) break;
			data_list_.push_back(xml_data);
		}
	} catch(...) {
	}
	data_process_(&data_list_);
	processBeforeDisplay();

	auto name_list = getNameList();
	auto count     = min<int>(name_list.size(),data_list_.size());

	for(int i=0; i<count; ++i){
		auto view = new XMLWidgetView(data_list_[i],column_count());
		connect(view,&XMLWidgetView::sigValueChanged,this,&BaseXmlWidget::sigValueChanged);
		tab_widget_->addTab(view,name_list[i]);
	}
	flagChanged();
	return true;
}
void NXmlWidget::setModelData(int index)
{
	if(index<0 || unsigned(index)>=dataCount()) 
		throw std::runtime_error("error index.");
	auto w = dynamic_cast<XMLWidgetView*>(tab_widget_->widget(index));
	if(nullptr != w) {
		w->setModelData();
	}
}
void NXmlWidget::setModelData(int index,int gid)
{
	if(index<0 || unsigned(index)>=data_list_.size()) 
		throw std::runtime_error("error index.");
	auto w = dynamic_cast<XMLWidgetView*>(tab_widget_->widget(index));
	if(nullptr != w) {
		w->setModelData(gid);
	}
}
void NXmlWidget::setModelData(int index,int gid,int iid)
{
	if(index<0 || unsigned(index)>=data_list_.size()) 
		throw std::runtime_error("error index.");
	auto w = dynamic_cast<XMLWidgetView*>(tab_widget_->widget(index));
	if(nullptr != w) {
		w->setModelData(gid,iid);
	}
}
void NXmlWidget::setViewData(int index)
{
	auto name_list = getNameList();
	auto count     = min<size_t>(name_list.size(),dataCount());

	if(index<0 || index>=count) return;

	tab_widget_->setTabText(index,name_list[index]);
	auto widget = dynamic_cast<XMLWidgetView*>(tab_widget_->widget(index));
	if(nullptr != widget)
		widget->setViewData();
}
void NXmlWidget::setViewData(int index,int gid)
{
	auto name_list = getNameList();
	auto count     = min<size_t>(name_list.size(),tab_widget_->count());

	if(index<0 || index>=count) return;

	tab_widget_->setTabText(index,name_list[index]);
	auto widget = dynamic_cast<XMLWidgetView*>(tab_widget_->widget(index));
	if(nullptr != widget)
		widget->setViewData(gid);
}
void NXmlWidget::setViewData(int index,int gid,int iid)
{
	auto name_list = getNameList();
	auto count     = min<size_t>(name_list.size(),tab_widget_->count());

	if(index<0 || index>=count) return;

	tab_widget_->setTabText(index,name_list[index]);
	auto widget = dynamic_cast<XMLWidgetView*>(tab_widget_->widget(index));
	if(nullptr != widget)
		widget->setViewData(gid,iid);
}
int NXmlWidget::currentIndex()const
{
	auto res = tab_widget_->currentIndex();
	if((res<0 || res>=data_list_.size())
			&& data_list_.size()>0) 
		res = 0;
	return res;
}
