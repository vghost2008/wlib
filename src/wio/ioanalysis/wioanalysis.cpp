#include <wioanalysis.h>
#include <wiodevice.h>
#include <wiofilter.h>
#include <wgui/wgui_fwd.h>
#include <wioanalysisset.h>
#include <toolkit.h>
#include <toolkit_qt.h>
#include <boost/algorithm/string/split.hpp>

using namespace std;

WIOAnalysis* WIOAnalysis::s_instance = nullptr;
map<unsigned,WIOAnalysisItem>  WIOAnalysis::analysis_items_;
std::vector<std::pair<std::string,std::string>> WIOAnalysis::alias_;
mutex      WIOAnalysis::item_mtx_;

WIOAnalysis::WIOAnalysis()
:data_buffer_(2048)
,view_data_buffer_(2048)
,filter_(make_unique<WIOFilter>())
{
	initGUI();

	connect(&timer_,&QTimer::timeout,this,&WIOAnalysis::updateAnalysis);
	timer_.start(500);
	setMinimumSize(800,600);
	resize(1000,600);
#ifdef Q_OS_LINUX
	data_content_->setFont(QFont("文泉驿等宽正黑",12));
#else
	data_content_->setFont(QFont("新宋体",12));
#endif
	setWindowTitle("IO数据分析");
	use_filter_ = false;
}
void WIOAnalysis::initGUI()
{
	auto        layout       = new QVBoxLayout(this);
	auto        hlayout0     = new QHBoxLayout;
	auto        btn_config   = new QPushButton(QIcon(imgP("set")),"");
	auto        splitter     = new QSplitter(Qt::Vertical);
	QStringList header_names{"时间","IO设备","长度","操作"};

	cb_filter_        =  new QComboBox;
	btn_pause_start_  =  new QPushButton(QIcon(imgP("pause")),"");
	btn_use_filter_   =  new QPushButton(QIcon(imgP("apply")),"");
	data_table_       =  new QTableWidget;
	data_content_     =  new QTextEdit;

	cb_filter_->setEditable(true);
	cb_filter_->addItems(historyFilters());
	cb_filter_->setCurrentIndex(-1);
	data_content_->setReadOnly(true);
	cb_filter_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

	hlayout0->addWidget(new QLabel("过滤器:"));
	hlayout0->addWidget(cb_filter_);
	hlayout0->addWidget(btn_use_filter_);
	hlayout0->addWidget(btn_pause_start_);
	hlayout0->addWidget(btn_config);

	btn_use_filter_->setToolTip("运行过滤器");
	btn_pause_start_->setToolTip("暂停/启动");
	btn_config->setToolTip("配置");

	splitter->addWidget(data_table_);
	splitter->addWidget(data_content_);
	splitter->setCollapsible(0,false);

	layout->addLayout(hlayout0);
	layout->addWidget(splitter);

	data_table_->model()->insertColumns(0,header_names.size());
	data_table_->setHorizontalHeaderLabels(header_names);
	data_table_->setSelectionBehavior(QAbstractItemView::SelectRows);
	data_table_->setSelectionMode(QAbstractItemView::SingleSelection);
	data_table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
	data_table_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	data_table_->setAlternatingRowColors(true);
	data_content_->setMinimumHeight(260);
	data_table_->setMinimumHeight(260);

	//data_table_->verticalHeader()->setVisible(false);
	data_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	menu_.addAction("清空",this,&WIOAnalysis::clear);

	connect(data_table_,&QTableWidget::itemSelectionChanged,this,&WIOAnalysis::slotCurrentDataChanged);
	connect(btn_pause_start_,&QAbstractButton::clicked,this,&WIOAnalysis::slotPauseStart);
	connect(btn_config,&QAbstractButton::clicked,this,&WIOAnalysis::slotConfig);
	connect(btn_use_filter_,&QAbstractButton::clicked,this,&WIOAnalysis::slotUseFilter);
	connect(cb_filter_,&QComboBox::editTextChanged,this,&WIOAnalysis::editFilter);
	connect(data_content_,&QTextEdit::selectionChanged,this,&WIOAnalysis::selectionChanged);
	if(cb_filter_->lineEdit() != nullptr) {
		connect(cb_filter_->lineEdit(),&QLineEdit::returnPressed,this,&WIOAnalysis::slotUseFilter);
	}
}
WIOAnalysis::~WIOAnalysis()
{
}
void WIOAnalysis::closeEvent(QCloseEvent* /*e*/)
{
	if(isInited()) {
		s_instance = nullptr;
		setVisible(false);
		deleteLater();
	}
}
bool WIOAnalysis::insertMonitor(const QString& title,unsigned id)
{
	lock_guard<mutex> guard{item_mtx_};

	if(analysis_items_.find(id) != analysis_items_.end()) return false;

	WIOAnalysisItem item{true,MT_ALL,DVM_HEX,title,0,0};
	analysis_items_.insert(make_pair(id,item));
	return true;
}
void WIOAnalysis::clearMonitor()
{
	lock_guard<mutex> guard{item_mtx_};

	analysis_items_.clear();
}
void WIOAnalysis::setMaxCacheNum(unsigned num) 
{
	if(num<16) 
		num = 16;
	else if(num > 1E4)
		num = 1E4;

	lock_guard<mutex>       guard0{mtx_};
	lock_guard<shared_mutex> guard1{vd_mtx_};

	data_buffer_.resize(num);
	view_data_buffer_.resize(num);
}
void WIOAnalysis::monitor(const void* data,size_t length,WIODeviceDirection direction,unsigned id,void* /*private_data*/)
{
	try {
		if(s_instance)
			s_instance->monitor_imp(data,length,direction,id);
	} catch(...) {
	}
}
void WIOAnalysis::monitor_imp(const void* data,unsigned length,WIODeviceDirection direction,unsigned id)noexcept
{
	const auto kMaxCacheSize = std::max<size_t>(8192,data_buffer_.capacity());
	lock_guard<mutex>  guard{mtx_};
	lock_guard<mutex>  guard0{item_mtx_};
	auto               it     = analysis_items_.find(id);
	auto              &item   = it->second;

	if(it==analysis_items_.end() || !item.show) return;
	if((direction==WIODD_READ && !(item.monitor_type&MT_READ))
			||(direction==WIODD_WRITE&&!(item.monitor_type&MT_WRITE)))
		return;

	auto data_item = make_shared<WIOADataItem>(QTime::currentTime(),id,direction,item.title,data,length);
	if(tmp_data_buffer_.size()>kMaxCacheSize) {
		ERROR_LOG("缓存的数据过多，删除部分数据");
		tmp_data_buffer_.erase(tmp_data_buffer_.begin(),next(tmp_data_buffer_.begin(),kMaxCacheSize/2));
	}

	tmp_data_buffer_.push_back(std::move(data_item));
	need_update_ = true;
}
void WIOAnalysis::updateAnalysis()
{
	if(!need_update_ || !is_running_) return;

	lock_guard<mutex> guard0{mtx_};
	lock_guard<shared_mutex> guard1{vd_mtx_};

	if(!need_update_ || !is_running_) return;
	need_update_ = false;
	initView(guard0,guard1);
}
void WIOAnalysis::initView()
{
	lock_guard<mutex> guard0{mtx_};
	lock_guard<shared_mutex> guard1{vd_mtx_};

	initView(guard0,guard1);
}
void WIOAnalysis::initView(const lock_guard<mutex>& /*guard0*/,const lock_guard<shared_mutex>& guard)
{
	in_update_view_ = true;

	auto model      =  data_table_->model();
	auto row_count  =  model->rowCount();

	for_each(tmp_data_buffer_.begin(),tmp_data_buffer_.end(),[this,model](shared_ptr<WIOADataItem>& item){
			if((!use_filter_) 
				|| (use_filter_&& filter_->eval(item))) {
			view_data_buffer_.push_back(item);
			insertItemToTable(*item);
			}
			data_buffer_.push_back(std::move(item));
			}
			);

	tmp_data_buffer_.clear();

	if(row_count != model->rowCount()) {
		checkTableData(guard);

		data_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
		data_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
		data_table_->scrollToBottom();
	}
	in_update_view_ = false;
}
void WIOAnalysis::insertItemToTable(const WIOADataItem&  item)
{
	auto model = data_table_->model();
	auto row   = model->rowCount();

	model->insertRows(model->rowCount(),1);
	model->setData(model->index(row,0),item.time.toString("hh:mm:ss.zzz"));
	model->setData(model->index(row,1),item.title);
	model->setData(model->index(row,2),QString::number(item.data.size()));
	model->setData(model->index(row,3),WIODevice::directionToString(static_cast<WIODeviceDirection>(item.direction)).c_str());
}
void WIOAnalysis::__checkTableData()
{
	auto model = data_table_->model();

	if(model->rowCount() > view_data_buffer_.size()) {
		auto remove_size = model->rowCount()-view_data_buffer_.size();
		model->removeRows(0,remove_size);
	}
}
void WIOAnalysis::slotCurrentDataChanged()
{
	if(in_update_view_) return;

	int row = data_table_->currentRow();

	if(row < 0) return ;

	shared_lock<shared_mutex> guard{vd_mtx_};
	lock_guard<mutex>         guard0{item_mtx_};

	if(row<0 || row>=view_data_buffer_.size()) return;

	current_data_ = view_data_buffer_[row];

	if(nullptr == current_data_) {
		current_item_.view_mode = DVM_UNKNOW;
		return;
	}

	auto  it   = analysis_items_.find(current_data_->id);
	if(it == analysis_items_.end()) {
		current_item_.view_mode = DVM_UNKNOW;
		LOG(LL_ERROR,"Unexpected");
		return;
	}
	current_item_ = it->second;
	showCurrentData(guard,guard0);
}
void WIOAnalysis::showCurrentData(const shared_lock<shared_mutex>& , const lock_guard<mutex>& )
{
	if(nullptr == current_data_) return ;

	auto    &item          = current_data_;
	auto    &analysis_item = current_item_;
	QString  text;

	text += analysis_item.title+"\n";
	text += QString("Time:")+item->time.toString("hh:mm:ss:zzz, ");
	text += "id="+QString::number(item->id)+", ";
	text += WIODevice::directionToString(static_cast<WIODeviceDirection>(item->direction)).c_str();
	switch(item->direction) {
		case WIODD_READ:
			analysis_item.read_size += item->data.size();
			text+= ":"+QString::number(item->data.size())+", Total:"+QString::number(analysis_item.read_size);
			break;
		case WIODD_WRITE:
			analysis_item.write_size += item->data.size();
			text+= ":"+QString::number(item->data.size())+", Total:"+QString::number(analysis_item.write_size);
			break;
		default:
			text+= "Unexpected";
			break;
	}
	text += "\n";
	switch(analysis_item.view_mode) {
		case DVM_BIN:
			text += WSpace::toBinString((const unsigned char*)item->data.data(),item->data.size(),unit_nr_for_bin_,column_for_bin_);
			break;
		case DVM_HEX:
			text += WSpace::toHexString((const unsigned char*)item->data.data(),item->data.size(),unit_nr_for_hex_,column_for_hex_).c_str();
			break;
		case DVM_ASSIC:
			text += QByteArray(reinterpret_cast<const char*>(item->data.data()),item->data.size());
			break;
		default:
			LOG(LL_ERROR,"Unexpected");
			break;
	}
	data_content_->setText(text);
}
void WIOAnalysis::slotPauseStart()
{
	is_running_ = !is_running_;
	if(is_running_) {
		btn_pause_start_->setIcon(QIcon(imgP("pause")));
		initView();
		data_content_->setText("");
	} else {
		btn_pause_start_->setIcon(QIcon(imgP("start")));
	}
}
void WIOAnalysis::resizeEvent(QResizeEvent* event)
{
	const auto kDelta = 4;

	contextViewSizeChanged();

	/*
	 * 高度变化不更新显示，宽度变化太小也不更新显示
	 */
	if(abs(event->oldSize().width()-event->size().width()) < kDelta) 
		return ; 
	/*
	 * 更新显示
	 */
	shared_lock<shared_mutex> guard   {vd_mtx_};
	lock_guard<mutex>              guard0  {item_mtx_};
	showCurrentData(guard,guard0);
}
void WIOAnalysis::showEvent(QShowEvent* /*event*/)
{
	contextViewSizeChanged();
}
void WIOAnalysis::contextViewSizeChanged()
{
	qApp->processEvents();
	column_for_hex_ = WSpace::getToHexStringColumnNr(data_content_->width(),data_content_->fontMetrics(),unit_nr_for_hex_);
	column_for_bin_ = WSpace::getToBinStringColumnNr(data_content_->width(),data_content_->fontMetrics(),unit_nr_for_bin_);
}
void WIOAnalysis::slotConfig()
{
	map<unsigned,WIOAnalysisItem> items;

	{
		lock_guard<mutex>              guard0  {item_mtx_};

		items = analysis_items_;
	}

	WIOAnalysisSet set(items);

	if(QDialog::Accepted != set.exec()) return; 

	{
		lock_guard<mutex>              guard0  {item_mtx_};

		set.updateItems(&analysis_items_);
	}
}
void WIOAnalysis::slotUseFilter()
{
	bool old_use_filter = use_filter_;
	auto old_rule       = filter_->get_rule();
	auto filter_text    = cb_filter_->currentText();

	for(auto& alias_item:alias_) {
		filter_text.replace(QRegExp(QString("\\b")+alias_item.first.c_str()+"\\b"),alias_item.second.c_str());
	}

	use_filter_ = false;

	if(!filter_->setRule(filter_text.toUtf8().data())) {
		if(filter_text.isEmpty()) 
			btn_use_filter_->setDisabled(true);
		goto out0;
	}
	INFO("Filter:%s",filter_text.toUtf8().data());

	saveFilter(cb_filter_->currentText());
	use_filter_ = true;
	btn_use_filter_->setDisabled(true);

out0:
	if( ((old_use_filter == false) && (use_filter_==false))
			|| (old_rule==filter_->get_rule())) 
		return;
	/*
	 * 对缓存的数据使用过滤器并显示
	 */

	lock_guard<mutex>       guard0{mtx_};
	lock_guard<shared_mutex> guard1{vd_mtx_};
	auto                    model  = data_table_->model();

	model->removeRows(0,model->rowCount());
	view_data_buffer_.clear();

	for_each(data_buffer_.begin(),data_buffer_.end(),[this,model](const shared_ptr<WIOADataItem>& item) {
			if(use_filter_&&(!filter_->eval(item))) return;
			insertItemToTable(*item);
			view_data_buffer_.push_back(item);
			});
	checkTableData(guard1);

	data_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	data_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	data_table_->scrollToBottom();
}
void WIOAnalysis::editFilter(const QString& /*text*/)
{
	btn_use_filter_->setDisabled(false);
}
QStringList WIOAnalysis::historyFilters()const
{
	QSettings set(QDir::home().absoluteFilePath(".ioanalysis_historyrc"));
	auto history = set.value("History").toStringList();

	if(history.size()<6) {
		history.push_back("(data[0]==1)&&(data[1]==2||dword[4]=0x1dfccf1a)");
		history.push_back("(word[0]==0xcf1a) && (word[1]==0x1dfc)");
		history.push_back("(id==1) || (data[0] == 2) || len==1060");
		history.push_back("(id<5) || (data[0] == 2)");
		history.push_back("(type==write) || (data[0] == 2)");
		history.push_back("(id!=5) || (type==read)");
	}
	return history;
}
void WIOAnalysis::saveFilter(const QString& filter)const
{
	const auto kMaxHistorySize = 10;
	auto       history         = historyFilters();
	QSettings  set(QDir::home().absoluteFilePath(".ioanalysis_historyrc"));

	auto it = std::find(history.begin(),history.end(),filter);

	if(it != history.end()) history.erase(it);
	history.push_front(filter);
	if(history.size() > kMaxHistorySize) 
		history.erase(history.begin()+kMaxHistorySize,history.end());
	set.setValue("History",history);

	auto model = cb_filter_->model();

	model->removeRows(0,model->rowCount());
	cb_filter_->addItems(history);
}
void WIOAnalysis::clear()
{
	lock_guard<mutex> guard0{mtx_};
	lock_guard<shared_mutex> guard1{vd_mtx_};
	auto model      =  data_table_->model();

	view_data_buffer_.clear();
	data_buffer_.clear();
	tmp_data_buffer_.clear();

	model->removeRows(0,model->rowCount());
}
void WIOAnalysis::contextMenuEvent(QContextMenuEvent* e)
{
	menu_.exec(e->globalPos());
}
void WIOAnalysis::selectionChanged()
{
	auto            text_cursor = data_content_->textCursor();
	auto            text        = text_cursor.selectedText();
	auto            text_list   = text.split(QRegExp("\\s+"),QString::SkipEmptyParts);
	bool            bok;
	vector<uint8_t> values;
	QString         dec_strs;

	if(text_list.size() > sizeof(uint64_t) || text_list.isEmpty()) return;

	switch(current_item_.view_mode) {
		case DVM_HEX:
			for(auto& text:text_list) {
				values.emplace_back(text.toUInt(&bok,16));
				if(!bok) return;
			}
			break;
		case DVM_BIN:
			for(auto& text:text_list) {
				values.emplace_back(text.toUInt(&bok,2));
				if(!bok) return;
			}
			break;
		default:
			return;
	}

	if(values.empty()) return;

	uint64_t  le  = 0;
	uint64_t  be  = 0;
	uint64_t  ile = 0;
	uint64_t  ibe = 0;
	auto     *ple = (uint8_t *) &le;
	auto     *pbe = ((uint8_t *) &be)+values.size()-1;

	for(auto& v:values) {
		dec_strs += QString(" %1").arg(uint(v),2,10,QChar('0'));
		*ple++ = v;
		*pbe-- = v;
	}
	ile = le;
	ibe = be;
	switch(values.size()) {
		case 1:
			ile = int8_t(le);
			ibe = int8_t(be);
			break;
		case 2:
			ile = int16_t(le);
			ibe = int16_t(be);
			break;
		case 4:
			ile = int32_t(le);
			ibe = int32_t(be);
			break;
		case 8:
			ile = int64_t(le);
			ibe = int64_t(be);
			break;
	}


	QToolTip::showText(QCursor::pos(),QString("十进制:%1\n十进制(ile):%2\n十进制(ule): %3\n十进制(ibe):%4\n十进制(ube): %5")
			.arg(dec_strs)
			.arg(qlonglong(ile))
			.arg(qulonglong(le))
			.arg(qlonglong(ibe))
			.arg(qulonglong(be)),
			this);
}
void WIOAnalysis::setAlias(const vector<string>& alias)
{
	lock_guard<mutex> guard{item_mtx_};

	alias_.clear();
	vector<string> split_vec;
	for(auto& str:alias) {
		boost::algorithm::split(split_vec,str,[](const char c) { return c=='#';},boost::algorithm::token_compress_on);
		if(split_vec.size()<2) continue;
		alias_.push_back(std::make_pair(split_vec[0],split_vec[1]));
	}
}
