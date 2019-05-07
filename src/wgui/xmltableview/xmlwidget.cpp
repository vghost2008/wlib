/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <xmlwidget.h>
#include <limits>
using namespace std;
WXmlWidget::WXmlWidget(const QStringList& file_list,QWidget* parent)
:QWidget(parent)
,list_widget_(nullptr)
,view_(nullptr)
,btn_layout_(nullptr)
,flag_(0)
{
	initGUI();
	init(file_list);
}
WXmlWidget::WXmlWidget(const QString& dir_path,QWidget* parent)
:QWidget(parent)
,list_widget_(nullptr)
,view_(nullptr)
,btn_layout_(nullptr)
,flag_(0)
{
	QDir dir(dir_path);
	QFileInfoList info_list = dir.entryInfoList(QStringList()<<"*.xml");
	QStringList file_list;
	for(int i=0; i<info_list.size(); ++i) 
		file_list<<info_list.at(i).absoluteFilePath();
	initGUI();
	init(file_list);
}
WXmlWidget::WXmlWidget(QWidget* parent)
:QWidget(parent)
,list_widget_(nullptr)
,view_(nullptr)
,btn_layout_(nullptr)
,flag_(0)
{
	initGUI();
}
void WXmlWidget::initGUI()
{
	QVBoxLayout *layout      = new QVBoxLayout(this);
	QHBoxLayout *hlayout0    = new QHBoxLayout;

	btn_default_   =   new QPushButton(tr("恢复默认值"));
	btn_save_      =   new QPushButton(tr("保存当前文件"));
	btn_close_     =   new QPushButton(tr("关闭"));
	btn_layout_    =   new QHBoxLayout;
	list_widget_   =   new QListWidget;
	view_          =   new WXmlView;

	hlayout0->addWidget(list_widget_);
	hlayout0->addWidget(view_);
	btn_layout_->addStretch();
	btn_layout_->addWidget(btn_default_);
	btn_layout_->addWidget(btn_save_);
	btn_layout_->addWidget(btn_close_);
	layout->addLayout(hlayout0);
	layout->addLayout(btn_layout_);


	list_widget_->setMaximumWidth(165);

	connect(btn_default_,&QPushButton::clicked,this,&WXmlWidget::slotDefaultValue);
	connect(btn_save_,&QPushButton::clicked,this,&WXmlWidget::slotSave);
	connect(btn_close_,&QPushButton::clicked,this,&WXmlWidget::sigClose);
	connect(list_widget_,&QListWidget::currentRowChanged,this,&WXmlWidget::slotConfigChanged);
}
bool WXmlWidget::init(const QStringList& file_list)
{
	if(file_list.isEmpty()) {
		QERROR_LOG("Init config failed!!!");
		return false;
	}
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
bool WXmlWidget::init()
{
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
	}catch(...) {
	}

	QAbstractItemModel* model = list_widget_->model();
	model->removeRows(0,model->rowCount());
	auto names = getNameList();
	for(auto& name:names) {
		list_widget_->addItem(name);
	}
	flagChanged();
	try {
		auto& xml_data = getDataByIndex(0);
		view_->setXmlData(&xml_data);
		list_widget_->selectionModel()->setCurrentIndex(model->index(0,0),QItemSelectionModel::Select);
	} catch(...) {
		LOG(LL_ERROR,"没有数据");
	}
	return true;
}
void WXmlWidget::slotConfigChanged(int index)
{
	if(index<0 || unsigned(index)>=data_list_.size()) return;
	view_->setXmlData(&data_list_[index]);
}
WXmlData& WXmlWidget::getDataByIndex(int index)noexcept(false)
{
	if(index<0 || unsigned(index)>=data_list_.size()) 
		throw std::runtime_error("error index.");
	return data_list_[index];
}
QStringList WXmlWidget::getNameList()
{
	QStringList name_list;
	for(vector<WXmlData>::iterator it=data_list_.begin(); it != data_list_.end(); ++it) {
		name_list<<it->name();
	}
	return name_list;
}
QString WXmlWidget::getFilePathByIndex(int index)const noexcept(false)
{
	if(index < 0 || index >= file_list_.size()) 
		throw std::runtime_error("error index.");
	return file_list_[index];
}
bool WXmlWidget::getXmlData(const QString& file_path,WXmlData* data)
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
void WXmlWidget::slotSave()
{
	if(data_list_.empty()) return;

	WXmlStream xml_stream;

	try {
		if(flag_.test(XWF_SAVE_ALL_FILE)) {
			for(int i=0; i<data_list_.size(); ++i) {
				auto file_path = getFilePathByIndex(i);
				xml_stream.setData(&getDataByIndex(i));
				if(!xml_stream.saveFile(file_path)) continue;
				xmlFileSaved(file_path);
				emit sigXmlFileSaved(file_path);
			}
		} else {
			int selected_index =list_widget_->selectionModel()->currentIndex().row();
			xml_stream.setData(&getDataByIndex(selected_index));
			auto file_path = getFilePathByIndex(selected_index);
			if(!xml_stream.saveFile(file_path)) return;
			xmlFileSaved(file_path);
			emit sigXmlFileSaved(file_path);
		}
	} catch(...) {
	}
}
void WXmlWidget::slotDefaultValue()
{
	if(data_list_.empty()) return;
	int selected_index =list_widget_->selectionModel()->currentIndex().row();
	try {
		getDataByIndex(selected_index).setToDefaultValue();
		slotConfigChanged(selected_index);
	} catch(...) {
	}
}
QString WXmlWidget::currentFile()const
{
	int selected_index =list_widget_->selectionModel()->currentIndex().row();
	QString res;
	try {
		res = getFilePathByIndex(selected_index);
	}catch(...) {
	}
	return res;
}
void WXmlWidget::setFlag(unsigned f)
{
	try {
		flag_.set(f);
		flagChanged();
	} catch(...) {
	}
}
void WXmlWidget::flagChanged()
{
	auto names = getNameList();
	if(names.size()<=1) {
		if(flag_.test(XWF_ALWAYS_SHOW_LIST)) {
			list_widget_->setVisible(true);
		} else {
			list_widget_->setVisible(false);
		}
	}
}
void WXmlWidget::xmlFileSaved(const QString& /*file_path*/)
{
}
