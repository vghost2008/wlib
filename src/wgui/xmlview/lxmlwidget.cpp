/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <lxmlwidget.h>
#include <limits>
#include <thread>
#include <mutex>
#include <wgui/wprogressdlg.h>
#include <xmlstream.h>

using namespace std;
using namespace WSpace;

LXmlWidget::LXmlWidget(const QStringList& file_list,QWidget* parent,int column_count,function<void(vector<WXmlData>*)> func)
:BaseXmlWidget(file_list,parent,column_count,func)
{
	initGUI();
	init(file_list);
}
LXmlWidget::LXmlWidget(const QString& dir_path,QWidget* parent,int column_count,function<void(vector<WXmlData>*)> func)
:BaseXmlWidget(dir_path,parent,column_count,func)
{
	initGUI();
	auto file_list = file_list_;
	init(file_list);
}
LXmlWidget::LXmlWidget(QWidget* parent,int column_count,function<void(vector<WXmlData>*)> func)
:BaseXmlWidget(parent,column_count,func)
{
	initGUI();
}
void LXmlWidget::initGUI()
{
	auto widget  = new QWidget;

	list_widget_    =  new QListWidget;
	widget_layout_  =  new QVBoxLayout(widget);

	list_widget_->setMaximumWidth(165);
	list_widget_->setMinimumWidth(80);
	list_widget_->setAlternatingRowColors(true);


	main_layout_->addWidget(list_widget_);
	main_layout_->addWidget(widget);

	connect(list_widget_,&QListWidget::currentRowChanged,this,&LXmlWidget::slotCurrentFileChanged);
}
bool LXmlWidget::init(const QStringList& file_list)
{
	return BaseXmlWidget::init(file_list);
}
bool LXmlWidget::init()
{
	ProgressHelper      helper("打开进度");
	constexpr auto kMaxFileSize = 4096;
	constexpr auto kMaxReadSize = 16;

	data_list_.clear();
	data_list_.reserve(file_list_.size());

	std::thread t([this,&helper]() {
			try {
			int  i     = 0;
			auto total = file_list_.size();
			/*
			 * getFilePathByIndex为虚函数，可能total==0但getFilePathByIndex可以获取多个文件路径
			 */
			while((i<kMaxFileSize) && !helper.isCanceled()) {
			auto file_path = getFilePathByIndex(i);
			++i;
			helper.setProgressAndText(i*100/(total+1),(QString("正在打开(%1/%2):%3").arg(i).arg(total).arg(QFileInfo(file_path).fileName())).toUtf8().data());
			data_list_.emplace_back();
			if(i>kMaxReadSize) continue;
			if(!getXmlData(file_path,&data_list_.back())) {
				data_list_.pop_back();
				break;
			}
			} //while

			} catch(...) {
			}
			helper.finish();
			});

	if(file_list_.size()>=1000) {
		WProgressDlg dlg(helper);
		dlg.exec();
	}
	t.join();

	data_process_(&data_list_);
	processBeforeDisplay();

	auto name_list = getNameList();
	auto count     = min<int>(name_list.size(),data_list_.size());
	auto m         = list_widget_->model();

	m->removeRows(0,m->rowCount());

	for(int i=0; i<count; ++i){
		list_widget_->addItem(name_list[i]);
	}
	setCurrentDataIndex(0);
	flagChanged();
	return true;
}
void LXmlWidget::setCurrentDataIndex(unsigned i)
{
	if(nullptr != current_widget_) {
		widget_layout_->removeWidget(current_widget_.get());
	}
	if(data_list_.empty()) {
		current_widget_.reset();
		return;
	}

	if(i>=data_list_.size()) return;
	auto& data = data_list_[i];
	if(data.empty()) {
		try {
			auto file_path = getFilePathByIndex(i);
			if(!getXmlData(file_path,&data)) {
				return;
			}
		} catch(...) {
			return;
		}
	}
	current_widget_.reset(new XMLWidgetView(data,column_count()));
	connect(current_widget_.get(),&XMLWidgetView::sigValueChanged,this,&BaseXmlWidget::sigValueChanged);
	widget_layout_->addWidget(current_widget_.get());
}
void LXmlWidget::setModelData()
{
	if(nullptr != current_widget_)
		current_widget_->setModelData();
}
void LXmlWidget::setModelData(int index)
{
	if(currentIndex() != index) return;
	if(nullptr != current_widget_)
		current_widget_->setModelData();
}
void LXmlWidget::setModelData(int index,int gid)
{
	if(index<0 || unsigned(index)>=dataCount()) 
		throw std::runtime_error("error index.");
	if(currentIndex() != index) return;
	if(nullptr != current_widget_)
		current_widget_->setModelData(gid);
}
void LXmlWidget::setModelData(int index,int gid,int iid)
{
	if(index<0 || unsigned(index)>=dataCount()) 
		throw std::runtime_error("error index.");
	if(currentIndex() != index) return;
	if(nullptr != current_widget_)
		current_widget_->setModelData(gid,iid);
}
void LXmlWidget::setViewData()
{
	auto name_list = getNameList();
	auto count     = min<size_t>(name_list.size(),dataCount());
	auto m         = list_widget_->model();

	for(int i=0; i<count; ++i) {
		m->setData(m->index(i,0),name_list[i]);
	}
	if(nullptr != current_widget_)
		current_widget_->setViewData();
}
void LXmlWidget::setViewData(int index)
{
	auto name_list = getNameList();
	auto count     = min<size_t>(name_list.size(),dataCount());
	auto m         = list_widget_->model();

	if((index<0) || (index>=count)) return;

	m->setData(m->index(index,0),name_list[index]);

	if(currentIndex() != index) return;

	if(nullptr != current_widget_)
		current_widget_->setViewData();
}
void LXmlWidget::setViewData(int index,int gid)
{
	auto name_list = getNameList();
	auto count     = min<size_t>(name_list.size(),dataCount());
	auto m         = list_widget_->model();

	if((index<0) || (index>=count)) return;

	m->setData(m->index(index,0),name_list[index]);

	if(currentIndex() != index) return;

	if(nullptr != current_widget_)
		current_widget_->setViewData(gid);
}
void LXmlWidget::setViewData(int index,int gid,int iid)
{
	auto name_list = getNameList();
	auto count     = min<size_t>(name_list.size(),dataCount());
	auto m         = list_widget_->model();

	if((index<0) || (index>=count)) return;

	m->setData(m->index(index,0),name_list[index]);

	if(currentIndex() != index) return;

	if(nullptr != current_widget_)
		current_widget_->setViewData(gid,iid);
}
void LXmlWidget::slotCurrentFileChanged(int index)
{
	setCurrentDataIndex(index);
}
int LXmlWidget::currentIndex()const 
{
	auto res = list_widget_->currentRow();

	if((res<0 || res>=data_list_.size())
			&& data_list_.size()>0) 
		res = 0;
	return res;
}
