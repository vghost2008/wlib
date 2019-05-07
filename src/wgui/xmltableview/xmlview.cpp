/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <xmlview.h>
#include <xmldata.h>

#include <wmacros.h>
#include <xmlviewdelegate.h>
#define COLOR0 QBrush(QColor(234,234,234))
#define COLOR2 QBrush(QColor(45,125,179))
#define COLOR3 QBrush(QColor(235,235,235))
#define COLOR4 QBrush(QColor(150,150,150))
using namespace std;
WXmlView::WXmlView(QWidget* parent)
:QTableWidget(parent)
,data_(nullptr)
,normal_view_delegate_(nullptr)
,setfloat_min_and_max_view_delegate_(nullptr)
{
	initGUI();
}
bool WXmlView::setXmlData(WXmlData* data)
{
	if(data->empty()) { WARNING_LOG("Empty data!");  }
	data_ = data;
	return setViewData();
}
bool WXmlView::setViewData()
{
	unsigned i;
	unsigned j;

	if(model()->rowCount() != 0) {
		model()->removeRows(0,model()->rowCount());
	}

	if(nullptr == data_) { ERROR_LOG("data is null"); return false; }

	setColumnCount(3);
	setHorizontalHeaderLabels(QStringList()<<tr("名称")<<tr("值")<<tr("描述"));
	for(i=0; i<data_->groups().size(); ++i) {
		const WXmlGroup* group= data_->groups()[i].get();
		newGroup(group->name());
		setHideData(model()->rowCount()-1,0,WXmlData::mkposid(i));
		for(j=0; j<group->items().size(); ++j) {
			const auto& item = group->items()[j];
			if(nullptr == item->data())continue;
			if(item->flag().test(XIF_HIDE_ITEM))continue;
			newItem();
			setItemName(item->name());
			setItemValue(item->toString());
			setItemDesc(item->desc());
			setHideData(model()->rowCount()-1,0,WXmlData::mkposid(i,j));
			if(j%2) {
				model()->setData(model()->index(model()->rowCount()-1,0),COLOR0,Qt::BackgroundRole);
				model()->setData(model()->index(model()->rowCount()-1,1),COLOR0,Qt::BackgroundRole);
				model()->setData(model()->index(model()->rowCount()-1,2),COLOR0,Qt::BackgroundRole);
			}
		}
	}
	if(nullptr == normal_view_delegate_)normal_view_delegate_ = new NormalXmlViewDelegate(this);
	setItemDelegate(normal_view_delegate_);
	return true;
}
void WXmlView::initGUI()
{
	setMinimumSize(600,400);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	horizontalHeader()->setStretchLastSection(true);
	verticalHeader()->hide();
}
bool WXmlView::newGroup(const QString& name)
{
	QFont tfont = font();

	model()->insertRow(model()->rowCount());
	setData(model()->rowCount()-1,0,name);
	setHideData(model()->rowCount()-1,0,-1);
	setSpan(model()->rowCount()-1,0,1,model()->columnCount());
	model()->setData(model()->index(model()->rowCount()-1,0),COLOR2,Qt::BackgroundRole);
	tfont.setBold(true);
	item(model()->rowCount()-1,0)->setFont(tfont);
	return true;
}
bool WXmlView::newItem()
{
	model()->insertRow(model()->rowCount());
	return true;
}
bool WXmlView::isSelectedGroup()const
{
	QItemSelectionModel *selection     = selectionModel();
	QModelIndexList      selected_list = selection->selectedIndexes();

	if(selected_list.isEmpty()) return false;
	const QModelIndex& index = selected_list.at(0);

	if(index.column() != 0) return false;
	if(index.row() == rowCount()-1) return false;

	const WXmlData  &data   = *data_;
	const unsigned  tindex = hideData(index.row(),0).toUInt();
	const unsigned  gid    = data.getGid(tindex);

	if(!data.isgroup(tindex)) return false;
	if(gid>=data.groups().size()) return false;
	return true;
}
bool WXmlView::forceDeleteCurrentGroup()
{
	const int gid    = getCurrentGroup();
	if(gid < 0) return false;
	WXmlGroup       *group  = data_->groupAt(gid);

	if(nullptr  == group) return false;
	data_->eraseGroup(gid);
	setViewData();
	return true;
}
QString WXmlView::getCurrentGroupName()const
{
	const int gid    = getCurrentGroup();
	if(gid < 0) return QString();
	WXmlGroup       *group  = data_->groupAt(gid);

	if(nullptr == group) return QString();
	return group->name();
}
bool WXmlView::haveSelected()const
{
	return getCurrentGroup()>=0;
}
int WXmlView::getCurrentGroup()const
{
	QItemSelectionModel *selection     = selectionModel();
	QModelIndexList      selected_list = selection->selectedIndexes();

	if(selected_list.isEmpty()) return -1;

	const unsigned  tindex = hideData(selected_list.at(0).row(),0).toUInt();
	const unsigned  gid    = data_->getGid(tindex);
	WXmlGroup       *group  = data_->groupAt(gid);

	if(nullptr == group) return -1;
	return gid;
}
