#include <wioanalysisset.h>
#include <iomonitor_fwd.h>
#include <wmacros.h>

using namespace std;

WIOAnalysisSet::WIOAnalysisSet(const std::map<unsigned,WIOAnalysisItem>& items)
:items_(items)
,signal_mapper_(this)
{
	initGUI();
	setWindowTitle("设置");
}
void WIOAnalysisSet::initGUI()
{
	auto layout     = new QVBoxLayout(this);
	auto glayout0   = new QGridLayout;
	auto hlayout0   = new QHBoxLayout;
	auto btn_accept = new QPushButton("确定");
	auto btn_cancel = new QPushButton("取消");
	int  row        = 0;

	for(auto& pair:items_) {
		auto             &item  = pair.second;
		WIOAnalysisUISet  uiset = {pair.first,new QCheckBox(item.title),new QComboBox,new QComboBox};

		glayout0->addWidget(uiset.cb_is_show,row,0);
		glayout0->addWidget(uiset.comb_view_mode,row,1);
		glayout0->addWidget(uiset.comb_monitor_type,row,2);

		uiset.comb_view_mode->addItems(QStringList()<<"ASSIC"<<"HEX"<<"二进制");
		uiset.comb_view_mode->setCurrentIndex(item.view_mode);
		uiset.comb_monitor_type->addItems(QStringList()<<"读写"<<"读"<<"写");
		switch(item.monitor_type) {
			case MT_READ:
				uiset.comb_monitor_type->setCurrentIndex(1);
				break;
			case MT_WRITE:
				uiset.comb_monitor_type->setCurrentIndex(2);
				break;
			case MT_ALL:
			default:
				uiset.comb_monitor_type->setCurrentIndex(0);
				break;
		}
		uiset.cb_is_show->setChecked(item.show);

		connect(uiset.cb_is_show,&QCheckBox::clicked,&signal_mapper_,(void (QSignalMapper::*)(void))&QSignalMapper::map);
		connect(uiset.comb_view_mode,(void (QComboBox::*)(int))&QComboBox::currentIndexChanged,&signal_mapper_,(void (QSignalMapper::*)(void))&QSignalMapper::map);
		connect(uiset.comb_monitor_type,(void (QComboBox::*)(int))&QComboBox::currentIndexChanged,&signal_mapper_,(void (QSignalMapper::*)(void))&QSignalMapper::map);

		signal_mapper_.setMapping(uiset.cb_is_show,row|UM_CHECKBOX);
		signal_mapper_.setMapping(uiset.comb_view_mode,row|UM_VIEW_MODE);
		signal_mapper_.setMapping(uiset.comb_monitor_type,row|UM_MONITOR_TYPE);
		index_to_ui_.insert(make_pair(row,uiset));
		++row;
	}
	layout->addLayout(glayout0);
	hlayout0->addStretch(0);
	hlayout0->addWidget(btn_cancel);
	hlayout0->addWidget(btn_accept);
	layout->addLayout(hlayout0);

	connect(&signal_mapper_,(void (QSignalMapper::*)(int))&QSignalMapper::mapped,this,&WIOAnalysisSet::slotUIEvent);
	connect(btn_accept,&QAbstractButton::clicked,this,&QDialog::accept);
	connect(btn_cancel,&QAbstractButton::clicked,this,&QDialog::reject);
}
void WIOAnalysisSet::updateItems(map<unsigned,WIOAnalysisItem>* items)
{
	for(auto& pair:items_) {
		auto id = pair.first;
		auto it = items->find(id);
		if(it == items->end()) continue;
		it->second = pair.second;
	}
}
void WIOAnalysisSet::slotUIEvent(int index)
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
	auto iu = index_to_ui_.find(index);

	if(iu == index_to_ui_.end()) {
		LOG(LL_ERROR,"Error index=%d",index);
		return ;
	}
	auto     &uiset = iu->second;
	unsigned  id    = uiset.id;
	auto      it    = items_.find(id);

	if(it == items_.end()) {
		LOG(LL_ERROR,"查找id=%x失败",id);
		return;
	}
	auto& item = it->second;

	switch(mask) {
		case UM_VIEW_MODE:
			item.view_mode = static_cast<WIODataViewMode>(uiset.comb_view_mode->currentIndex());
			break;
		case UM_CHECKBOX:
			item.show = uiset.cb_is_show->isChecked();
			break;
		case UM_MONITOR_TYPE:
			switch(uiset.comb_monitor_type->currentIndex()) {
				case 0:
					item.monitor_type = MT_ALL;
					break;
				case 1:
					item.monitor_type = MT_READ;
					break;
				case 2:
					item.monitor_type = MT_WRITE;
					break;
				default:
					item.monitor_type = MT_NONE;
					break;
			}
			break;
		default:
			LOG(LL_ERROR,"Unexpected");
			return;
			break;
	}
}
