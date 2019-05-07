/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <basexmlwidget.h>
#include <limits>
#include <thread>
#include <mutex>
#include <wgui/wprogressdlg.h>
#include <xmlstream.h>

using namespace std;
using namespace WSpace;

void BaseXmlWidgetNullProcess(std::vector<WXmlData>*)
{
}
BaseXmlWidget::BaseXmlWidget(const QStringList& file_list,QWidget* parent,int column_count,function<void(vector<WXmlData>*)> func)
:QWidget(parent)
,btn_layout_(nullptr)
,flag_(0)
,data_process_(func)
,column_count_(column_count)
{
	initGUI();
}
BaseXmlWidget::BaseXmlWidget(const QString& dir_path,QWidget* parent,int column_count,function<void(vector<WXmlData>*)> func)
:QWidget(parent)
,btn_layout_(nullptr)
,flag_(0)
,data_process_(func)
,column_count_(column_count)
{
	QDir dir(dir_path);
	QFileInfoList info_list = dir.entryInfoList(QStringList()<<"*.xml");
	QStringList file_list;
	for(int i=0; i<info_list.size(); ++i) 
		file_list<<info_list.at(i).absoluteFilePath();
	file_list_ = file_list;
	initGUI();
}
BaseXmlWidget::BaseXmlWidget(QWidget* parent,int column_count,function<void(vector<WXmlData>*)> func)
:QWidget(parent)
,btn_layout_(nullptr)
,flag_(0)
,data_process_(func)
,column_count_(column_count)
{
	initGUI();
}
void BaseXmlWidget::initGUI()
{
	auto layout  = new QVBoxLayout(this);

	main_layout_    =  new QHBoxLayout;
	btn_default_    =  new QPushButton(tr("恢复默认值"));
	btn_save_       =  new QPushButton(tr("保存当前文件"));
	btn_layout_     =  new QHBoxLayout;
	btn_layout0_    =  new QHBoxLayout;


	btn_layout_->addStretch();
	btn_layout_->addLayout(btn_layout0_);
	btn_layout_->addWidget(btn_default_);
	btn_layout_->addWidget(btn_save_);

	layout->addLayout(main_layout_);
	layout->addLayout(btn_layout_);

	connect(btn_default_,&QPushButton::clicked,this,&BaseXmlWidget::slotDefaultValue);
	connect(btn_save_,&QPushButton::clicked,this,&BaseXmlWidget::slotSave);
	resize(1000,800);
}
bool BaseXmlWidget::init(const QStringList& file_list)
{
	file_list_.clear();
	for(auto& file_path:file_list) {
		if(!QFile::exists(file_path)) {
			ERROR_LOG("文件\"%s\"不存在.",file_path.toUtf8().data());
			continue;
		}
		file_list_<<file_path;
	}
	return init();
}
void BaseXmlWidget::processBeforeDisplay()
{
}
WXmlData& BaseXmlWidget::getDataByIndex(int index)noexcept(false)
{
	if(index<0 || unsigned(index)>=data_list_.size()) 
		throw std::runtime_error("error index.");
	setModelData(index);
	return data_list_[index];
}
QStringList BaseXmlWidget::getNameList()const
{
	QStringList name_list;
	for(auto it=data_list_.begin(); it != data_list_.end(); ++it) {
		name_list<<it->name();
	}
	return name_list;
}
QString BaseXmlWidget::getFilePathByIndex(int index)const noexcept(false)
{
	if(index < 0 || index >= file_list_.size()) 
		throw std::runtime_error("error index.");
	return file_list_[index];
}
bool BaseXmlWidget::getXmlData(const QString& file_path,WXmlData* data)
{
	WXmlStream xml_stream;
	QFileInfo file_info(file_path);

	if(!file_info.exists()) {
		QERROR_LOG(QString("配置文件不存在:")+file_path);
		return false;
	}
	xml_stream.setData(data);
	if(!xml_stream.readFile(file_path)) {
		QERROR_LOG("Read data failed!");
		return false;
	} 
	return true;
}
void BaseXmlWidget::slotSave()
{
	doSave();
}
void BaseXmlWidget::doSave()
{
	auto index = currentIndex();
	setModelData(index);
	saveFileAt(currentIndex());
}
bool BaseXmlWidget::saveFileAt(int index)
{
	if(data_list_.empty()) return false;

	WXmlStream xml_stream;

	try {
		auto& xml_data = xmlDataRefAt(index);
		xml_stream.setData(&xml_data);
		auto file_path = getFilePathByIndex(index);
		if(!xml_stream.saveFile(file_path)) return false;
		xmlFileSaved(file_path);
		emit sigXmlFileSaved(file_path);
	} catch(...) {
		return false;
	}
	return true;
}
void BaseXmlWidget::slotDefaultValue()
{
	if(data_list_.empty()) return;
	int selected_index = currentIndex();
	try {
		getDataByIndex(selected_index).setToDefaultValue();
		setViewData(selected_index);
	} catch(...) {
	}
}
void BaseXmlWidget::setModelData()
{
	auto count     = dataCount();

	for(int i=0; i<count; ++i) {
		setModelData(i);
	}
}
void BaseXmlWidget::setModelData(int index)
{
	try {
		auto &xml_data = xmlDataRefAt(index);
		auto &groups   = xml_data.groups();

		for(auto& g:groups) {
			setModelData(index,g->id());
		}
	} catch(...) {
	}
}
void BaseXmlWidget::setModelData(int index,int gid)
{
	try {
		auto &xml_data = xmlDataRefAt(index);
		auto &group    = xml_data.findChildRefByID(gid);
		auto &items    = group.items();

		for(auto& item:items) {
			setModelData(index,gid,item->id());
		}
	} catch(...) {
	}
}
void BaseXmlWidget::setViewData()
{
	auto count     = dataCount();
	for(auto i=0; i<count; ++i)
		setViewData(i);
}
void BaseXmlWidget::setViewData(int index)
{
	try {
		auto &xml_data = xmlDataRefAt(index);
		auto &groups   = xml_data.groups();

		for(auto& g:groups) {
			setViewData(index,g->id());
		}
	} catch(...) {
	}
}
void BaseXmlWidget::setViewData(int index,int gid)
{
	try {
		auto &xml_data = xmlDataRefAt(index);
		auto &group    = xml_data.findChildRefByID(gid);
		auto &items    = group.items();

		for(auto& item:items) {
			setViewData(index,gid,item->id());
		}
	} catch(...) {
	}
}
size_t BaseXmlWidget::dataCount()const
{
	return data_list_.size();
}
WXmlData& BaseXmlWidget::currentData() noexcept(false)
{
	return const_cast<WXmlData&>(static_cast<const BaseXmlWidget&>(*this).currentData());
}
const WXmlData& BaseXmlWidget::currentData()const
{
	int selected_index = currentIndex();

	if(selected_index<0 || selected_index>=data_list_.size()) throw std::runtime_error("error data");
	return data_list_[selected_index];
}
QString BaseXmlWidget::currentName()const noexcept(false)
{
	auto name_list      = getNameList();
	auto selected_index = currentIndex();

	if(selected_index<0 || selected_index>=name_list.size()) return QString();
	return name_list[selected_index];
}
QString BaseXmlWidget::currentFile()const
{
	int     selected_index = currentIndex();
	QString res;

	try {
		res = getFilePathByIndex(selected_index);
	}catch(...) {
	}
	return res;
}
void BaseXmlWidget::setFlag(unsigned f)
{
	try {
		flag_.set(f);
		flagChanged();
	} catch(...) {
	}
}
void BaseXmlWidget::flagChanged()
{
}
void BaseXmlWidget::xmlFileSaved(const QString& /*file_path*/)
{
}
