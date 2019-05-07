/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <iomonitor.h>
#include <toolkit.h>
#include <toolkit_qt.h>
#include <mutex>
#include <chrono>
#include <boost/algorithm/string/trim.hpp>

#define IOMONITOR_TIMER_INTERVAL 400
using namespace std;

WIOMonitor* WIOMonitor::s_instance = nullptr;
list<WIOMonitor::IOMonitorItem> WIOMonitor::s_item_list_;
QMutex  WIOMonitor::s_mutex_;
WIOMonitor::WIOMonitor(QWidget* parent)
:QWidget(parent)
,context_view_(nullptr)
,monitor_item_layout_(nullptr)
,signal_mapper_(new QSignalMapper(this))
,max_view_size_(32768)
,timer_id_(-1)
,paused_(false)
,max_cache_num_(64)
,unit_nr_for_hex_(8)
,unit_nr_for_bin_(2)
,column_for_hex_(3)
,column_for_bin_(4)
,data_trans_thread_(this)
{
	initGUI();
	connect(signal_mapper_,(void (QSignalMapper::*)(int))&QSignalMapper::mapped,this,&WIOMonitor::slotUIEvent);
	setWindowTitle("通信监视器");
	setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint|Qt::Tool);
	context_view_->setWordWrapMode(QTextOption::QTextOption::WordWrap);

	unsigned char data[unit_nr_for_hex_+1];
	for(unsigned char& c:data)
		c = 'M';
	QString str = boost::algorithm::trim_copy(WSpace::toHexString(data,unit_nr_for_hex_,unit_nr_for_hex_,1)+"MMMM").c_str();
	resize(context_view_->fontMetrics().width(str)*4+100,600);
}
void WIOMonitor::initGUI()
{
	QHBoxLayout *layout    = new QHBoxLayout(this);
	QVBoxLayout *vlayout0  = new QVBoxLayout;
	QVBoxLayout *vlayout1  = new QVBoxLayout;
	QPushButton *btn_close = nullptr;
	QPushButton *btn_save  = new QPushButton("保存");
	QPushButton *btn_clear = new QPushButton("清空");

	context_view_         =  new QTextEdit(this);
	monitor_item_layout_  =  new QGridLayout;
	btn_pause_            =  new QPushButton("暂停");
	side_widget_          =  new QWidget;
	btn_hide_side_        =  new QPushButton("》");
	btn_select_           =  new QPushButton("全选");
	if(parent() == nullptr)
		btn_close = new QPushButton("关闭");

	btn_hide_side_->setMaximumWidth(fontMetrics().width("》")+2);
	btn_hide_side_->setFlat(true);
	side_widget_->setLayout(vlayout1);

	vlayout1->addLayout(monitor_item_layout_);
	vlayout1->addStretch();
	vlayout1->addWidget(btn_pause_);
	vlayout1->addWidget(btn_select_);
	vlayout1->addWidget(btn_clear);
	vlayout1->addWidget(btn_save);
	if(nullptr != btn_close)
		vlayout1->addWidget(btn_close);
	vlayout0->addWidget(btn_hide_side_);
	layout->addWidget(context_view_);
	layout->addLayout(vlayout0);
	layout->addWidget(side_widget_);

	vlayout0->setContentsMargins(0,0,0,0);
	vlayout0->setSpacing(0);
	layout->setSpacing(0);
	layout->setContentsMargins(5,5,0,5);
	vlayout1->setContentsMargins(0,5,5,5);

	QPalette pte = context_view_->palette();
	pte.setColor(QPalette::Background,QColor(0,0,0));
	pte.setColor(QPalette::Base,QColor(0,0,0));
	pte.setColor(QPalette::Text,QColor(255,255,255));
	context_view_->setPalette(pte);
	context_view_->setAutoFillBackground(true);

	if(nullptr != btn_close)
		connect(btn_close,&QPushButton::clicked,this,&WIOMonitor::close);
	connect(btn_clear,&QPushButton::clicked,this,&WIOMonitor::slotClear);
	connect(btn_save,&QPushButton::clicked,this,&WIOMonitor::slotSave);
	connect(btn_pause_,&QPushButton::clicked,this,&WIOMonitor::slotPause);
	connect(btn_hide_side_,&QPushButton::clicked,this,&WIOMonitor::slotHideSide);
	connect(btn_select_,&QPushButton::clicked,this,&WIOMonitor::slotSelect);
	context_view_->setReadOnly(true);
#ifdef Q_OS_LINUX
	context_view_->setFont(QFont("文泉驿等宽正黑",12));
#else
	context_view_->setFont(QFont("新宋体",12));
#endif
}
int WIOMonitor::minit()
{
	return updateMonitors();
}
bool WIOMonitor::insertMonitor(const QString& title,unsigned id)
{
	s_item_list_.push_back(IOMonitorItem({title,id}));
	return true;
}
int WIOMonitor::updateMonitors()
{
	clearMonitorM();
	std::lock_guard<Mutex> guard{s_mutex_};

	for(auto it=s_item_list_.begin(); it!=s_item_list_.end(); ++it)
		insertMonitorM(it->title,it->id);
	return 0;
}
void WIOMonitor::clearMonitor()
{
	s_item_list_.clear();
}
bool WIOMonitor::insertMonitorM(const QString& title,unsigned id)
{
	if(monitor_config_.find(id) != monitor_config_.end()) 
		return false;

	WIOMonitorConfig config  =  {false,0xFF,DVM_HEX,title,0u,0u};
	WIOMonitorUISet uiset    =  {id,new QCheckBox(title),new QComboBox,new QComboBox};

	monitor_config_[id] = config;

	monitor_item_layout_->addWidget(uiset.cb_is_show,index_to_ui_.size(),0);
	monitor_item_layout_->addWidget(uiset.comb_view_mode,index_to_ui_.size(),1);
	monitor_item_layout_->addWidget(uiset.comb_monitor_type,index_to_ui_.size(),2);

	uiset.comb_view_mode->addItems(QStringList()<<"ASSIC"<<"HEX"<<"二进制");
	uiset.comb_view_mode->setCurrentIndex(1);
	uiset.comb_monitor_type->addItems(QStringList()<<"读写"<<"读"<<"写");
	uiset.comb_monitor_type->setCurrentIndex(0);

	connect(uiset.cb_is_show,&QCheckBox::clicked,signal_mapper_,(void (QSignalMapper::*)(void))&QSignalMapper::map);
	connect(uiset.comb_view_mode,(void (QComboBox::*)(int))&QComboBox::currentIndexChanged,signal_mapper_,(void (QSignalMapper::*)(void))&QSignalMapper::map);
	connect(uiset.comb_monitor_type,(void (QComboBox::*)(int))&QComboBox::currentIndexChanged,signal_mapper_,(void (QSignalMapper::*)(void))&QSignalMapper::map);

	signal_mapper_->setMapping(uiset.cb_is_show,index_to_ui_.size()|UM_CHECKBOX);
	signal_mapper_->setMapping(uiset.comb_view_mode,index_to_ui_.size()|UM_VIEW_MODE);
	signal_mapper_->setMapping(uiset.comb_monitor_type,index_to_ui_.size()|UM_MONITOR_TYPE);

	index_to_ui_.push_back(uiset);
	if(timer_id_ <= 0) {
		timer_id_ = startTimer(IOMONITOR_TIMER_INTERVAL);
	}
	if(!data_trans_thread_.isRunning())data_trans_thread_.start(QThread::LowPriority);
	return true;
}
void WIOMonitor::clearMonitorM()
{
	lock_guard<Mutex> guard{s_mutex_};
	if(!monitor_config_.empty()) {
		monitor_config_.clear();
	}
	if(!index_to_ui_.empty()) {
		for(auto it=index_to_ui_.begin(); it!=index_to_ui_.end(); ++it) {
			signal_mapper_->removeMappings(it->cb_is_show);
			signal_mapper_->removeMappings(it->comb_view_mode);
			signal_mapper_->removeMappings(it->comb_monitor_type);

			connect(it->cb_is_show,&QCheckBox::clicked,signal_mapper_,(void (QSignalMapper::*)(void))&QSignalMapper::map);
			connect(it->comb_view_mode,(void (QComboBox::*)(int))&QComboBox::currentIndexChanged,signal_mapper_,(void (QSignalMapper::*)(void))&QSignalMapper::map);
			connect(it->comb_monitor_type,(void (QComboBox::*)(int))&QComboBox::currentIndexChanged,signal_mapper_,(void (QSignalMapper::*)(void))&QSignalMapper::map);

			it->cb_is_show->deleteLater();
			it->comb_view_mode->deleteLater();
			it->comb_monitor_type->deleteLater();
		}
		index_to_ui_.clear();
	}
	if(timer_id_ > 0) {
		killTimer(timer_id_);
		timer_id_ = -1;
	}
}
void WIOMonitor::monitor(const void* data,size_t length,WIODeviceDirection direction,unsigned id,void* /*private_data*/)
{
	try {
		if(isInited())
			instance().monitor_imp(data,length,direction,id);
	} catch(...) {
	}
}
void WIOMonitor::monitor_imp(const void* data,unsigned length,WIODeviceDirection direction,unsigned id)
{
	lock_guard<Mutex> guard{s_mutex_};
	auto it = monitor_config_.find(id);
	if(it == monitor_config_.end()) {
		LOG(LL_INFO,"查找id=%x失败",id);
		return ;
	}
	WIOMonitorConfig& c = it->second;
	if(!c.show) goto out0;
	if((direction==WIODD_READ && !(c.monitor_type&MT_READ))
		||(direction==WIODD_WRITE&&!(c.monitor_type&MT_WRITE)))
		goto out0;
	if(data_trans_thread_.data_fifo_.size()>max_cache_num_) {
		ERR("Busy,NR+%d",data_trans_thread_.data_fifo_.size());
	} else {
		WBuffer buffer(data,length,length,id);
		buffer.setAttr(direction);
		data_trans_thread_.data_fifo_.enqueue(std::move(buffer));
	}
out0:
	return ;
}
void WIOMonitor::getMonitorData()
{
	QString            old_text;
	QString            text;
	std::list<QString> temp_data;

    if(data_fifo_.getAllData(&temp_data) <= 0) return;

	old_text = context_view_->toPlainText();
	for(auto jt=temp_data.begin(); jt!=temp_data.end(); ++jt) {
		text += *jt;
	}
	if(paused_) return;

	if(old_text.size()>= max_view_size_) {
		old_text = old_text.right(max_view_size_/2);
	}
	context_view_->setText(old_text+text);

	QTextCursor cursor = context_view_->textCursor();
	cursor.movePosition(QTextCursor::End);
	context_view_->setTextCursor(cursor);
}
QString WIOMonitor::transData(const WBuffer& buffer)
{
	unsigned                  id        = buffer.id();
	const WIODeviceDirection  direction = static_cast<WIODeviceDirection>(buffer.attr());
	const unsigned char      *data      = reinterpret_cast<const unsigned char *>(buffer.data());
	unsigned                  length    = buffer.size();
	unsigned                  total     = 0;
	auto                      it        = monitor_config_.find(id);

	if(it == monitor_config_.end()) {
		ERR("查找id=%x失败",id);
		return QString();
	}

	WIOMonitorConfig& c = it->second;
	if(!c.show) return QString();
	switch(direction) {
		case WIODD_READ:
			total = c.read_size += length;
			break;
		case WIODD_WRITE:
			total = c.write_size += length;
			break;
		default:
			LOG(LL_ERROR,"Unexpected");
			break;
	}
	if(paused_) return QString();
	QString text = QString("\n")+WIODevice::directionToString(direction).c_str()+" "+c.title+QString(", Len=%1, Total=%2").arg(length).arg(total)+":\n";
	switch(c.view_mode) {
		case DVM_BIN:
			text += WSpace::toBinString(data,length,unit_nr_for_bin_,column_for_bin_);
			break;
		case DVM_HEX:
			text += WSpace::toHexString(data,length,unit_nr_for_hex_,column_for_hex_).c_str();
			break;
		case DVM_ASSIC:
			text += QByteArray(reinterpret_cast<const char*>(data),length);
			break;
		default:
			LOG(LL_ERROR,"Unexpected");
			return QString();
			break;
	}
	return text;
}
void WIOMonitor::closeEvent(QCloseEvent* /*e*/)
{
	if(isInited()) {
		s_instance = nullptr;
		data_trans_thread_.stop();
		setVisible(false);
		if(timer_id_ > 0) { killTimer(timer_id_); timer_id_ = 0; }
		deleteLater();
	}
}
WIOMonitor::~WIOMonitor()
{
	qDebug()<<__func__;
}
void WIOMonitor::slotUIEvent(int index)
{
	UIMask mask;

	if(index&UM_CHECKBOX) {
		mask = UM_CHECKBOX;
	} else if(index&UM_VIEW_MODE) {
		mask = UM_VIEW_MODE;
	} else if(index&UM_MONITOR_TYPE) {
		mask = UM_MONITOR_TYPE;
	} else {
		LOG(LL_ERROR,"Error index=%x",index);
		return;
	}

	index &= UM_MASK;

	if(index >= index_to_ui_.size()||index<0) {
		LOG(LL_ERROR,"Error index=%d",index);
		return ;
	}

	unsigned id = index_to_ui_[index].id;
	auto     it = monitor_config_.find(id);

	if(it == monitor_config_.end()) {
		LOG(LL_ERROR,"查找id=%x失败",id);
		return;
	}

	switch(mask) {
		case UM_VIEW_MODE:
			it->second.view_mode = static_cast<WIODataViewMode>(index_to_ui_[index].comb_view_mode->currentIndex());
			break;
		case UM_CHECKBOX:
			it->second.show = index_to_ui_[index].cb_is_show->isChecked();
			break;
		case UM_MONITOR_TYPE:
			switch(index_to_ui_[index].comb_monitor_type->currentIndex()) {
				case 0:
					it->second.monitor_type = MT_ALL;
					break;
				case 1:
					it->second.monitor_type = MT_READ;
					break;
				case 2:
					it->second.monitor_type = MT_WRITE;
					break;
				default:
					it->second.monitor_type = MT_NONE;
					break;
			}
			break;
		default:
			LOG(LL_ERROR,"Unexpected");
			return;
			break;
	}
	updateUI();
}
void WIOMonitor::slotClear()
{
	context_view_->setText("");
	for(auto it=monitor_config_.begin(); it!=monitor_config_.end(); ++it) {
		it->second.read_size = it->second.write_size = 0;
	}
}
void WIOMonitor::slotSave()
{
	QString file_path = QFileDialog::getSaveFileName(this,"保存文件",QDir::home().absolutePath(),"txt (*.txt);;any file(*)");

	if(file_path.isEmpty()) return;

	if(file_path.right(3) != "txt") file_path += ".txt";

	QFile file(file_path);

	if(!file.open(QIODevice::WriteOnly)) {
		QALL_LOG(LL_ERROR,QString("打开文件%1失败").arg(file_path));
		return ;
	}

	QTextStream stream(&file);
	stream<<context_view_->toPlainText();
	file.close();
}
void WIOMonitor::timerEvent(QTimerEvent* /*e*/)
{
	if(data_fifo_.size() == 0) return;
	qApp->processEvents();
	getMonitorData();
}
void WIOMonitor::resizeEvent(QResizeEvent* /*event*/)
{
	contextViewSizeChanged();
}
void WIOMonitor::slotHideSide()
{
	if(side_widget_->isVisible()) {
		btn_hide_side_->setText("《");
		side_widget_->setVisible(false);
	} else {
		btn_hide_side_->setText("》");
		side_widget_->setVisible(true);
	}
	contextViewSizeChanged();
}
void WIOMonitor::contextViewSizeChanged()
{
	qApp->processEvents();
	column_for_hex_ = WSpace::getToHexStringColumnNr(context_view_->width(),context_view_->fontMetrics(),unit_nr_for_hex_);
	column_for_bin_ = WSpace::getToBinStringColumnNr(context_view_->width(),context_view_->fontMetrics(),unit_nr_for_bin_);
}
void WIOMonitor::slotPause()
{
	if(paused_) {
		data_trans_thread_.data_fifo_.clear();
		paused_ = false;
		btn_pause_->setText("暂停");
	} else {
		paused_ = true;
		btn_pause_->setText("继续");
	}
}
/*================================================================================*/
IOMDataTransThread::IOMDataTransThread(WIOMonitor* iomonitor)
:io_monitor_(iomonitor)
,run_(false)
{
}
void IOMDataTransThread::stop()
{
	run_ = false;
	data_fifo_.stop();
	wait(30000);
}
IOMDataTransThread::~IOMDataTransThread()
{
	stop();
}
void IOMDataTransThread::run()
{
	WBuffer       buffer;
	QString       str;
	list<WBuffer> temp_buffer_list;
	const int     size_limit       = 10;

	run_ = true;

	while(run_) {
		if(io_monitor_->data_fifo_.size()>=size_limit) {
			msleep(50);
			continue;
		}
		if(data_fifo_.empty()) {
            if(false == data_fifo_.dequeue(chrono::milliseconds(50),&buffer)) {
				msleep(50);
				continue;
			}
			str = io_monitor_->transData(buffer);
			if(!str.isEmpty()) {
				io_monitor_->data_fifo_.enqueue(str);
			}
		} else {
			if(data_fifo_.getAllData(&temp_buffer_list)==0) {
				msleep(50);
				continue;
			}
			for(auto it=temp_buffer_list.begin(); it!=temp_buffer_list.end(); ++it) {
				str = io_monitor_->transData(*it);
				if(!str.isEmpty()) {
					io_monitor_->data_fifo_.enqueue(str);
				}
			}
		}
	}
	INFO("Stop...");
}
void WIOMonitor::updateUI()
{
	int checked_num = 0;
	for(auto& iu:index_to_ui_) {
		if(iu.cb_is_show->isChecked())
			++checked_num;
	}
	if(checked_num < index_to_ui_.size()) {
		btn_select_->setText("全选");
	} else {
		btn_select_->setText("全不选");
	}
}
void WIOMonitor::slotSelect()
{
	if(btn_select_->text() == "全不选") {
		for(auto& iu:index_to_ui_) {
			iu.cb_is_show->setChecked(false);
		}
		for(auto& v:monitor_config_) {
			v.second.show = false;
		}
		btn_select_->setText("全选");
	} else {
		for(auto& iu:index_to_ui_) {
			iu.cb_is_show->setChecked(true);
		}
		for(auto& v:monitor_config_) {
			v.second.show = true;
		}
		btn_select_->setText("全不选");
	}
}
