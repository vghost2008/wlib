/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "xmlviewdelegate.h"
#include "wkeyboard.h"
#include "wmacros.h"
#include "xmlview.h"
#include <wgui.h>
#include <limits>
#include <xmlviewdelegateimpsk.h>
#include <xmlviewdelegateimp.h>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>

using namespace boost;
using namespace std;

XmlBaseViewDelegate::XmlBaseViewDelegate(WXmlView* view)
:view_(view)
,use_soft_keyboard_(false)
{
}
WXmlItem* XmlBaseViewDelegate::getItem(const QModelIndex& index)const {
	const unsigned tindex = view_->hideData(index.row(),0).toUInt();
	return view_->data()->getItem(tindex);
}
WXmlGroup* XmlBaseViewDelegate::getGroup(const QModelIndex& index)const {
	const unsigned tindex = view_->hideData(index.row(),0).toUInt();
	return view_->data()->getGroup(tindex);
}
/*================================================================================*/
typedef mpl::vector<void,
WNormalItemDelegate::StringItemDelegate,
WNormalItemDelegate::IntItemDelegate,
WNormalItemDelegate::UIntItemDelegate,
WNormalItemDelegate::FloatItemDelegate,
WNormalItemDelegate::ListItemDelegate,
WNormalItemDelegate::DListItemDelegate,
WNormalItemDelegate::HexItemDelegate,
WNormalItemDelegate::TimeItemDelegate,
WNormalItemDelegate::IPItemDelegate,
WNormalItemDelegate::FileItemDelegate,
WNormalItemDelegate::DirItemDelegate,
WNormalItemDelegate::ColorItemDelegate,
WNormalItemDelegate::BoolItemDelegate,
WNormalItemDelegate::UIntItemDelegate,
WNormalItemDelegate::EnumItemDelegate
> normal_types;
typedef mpl::vector<void,
WSoftKeyItemDelegate::StringItemDelegate,
WSoftKeyItemDelegate::IntItemDelegate,
WSoftKeyItemDelegate::UIntItemDelegate,
WSoftKeyItemDelegate::FloatItemDelegate,
WSoftKeyItemDelegate::ListItemDelegate,
WSoftKeyItemDelegate::DListItemDelegate,
WSoftKeyItemDelegate::HexItemDelegate,
WSoftKeyItemDelegate::TimeItemDelegate,
WSoftKeyItemDelegate::IPItemDelegate,
WSoftKeyItemDelegate::FileItemDelegate,
WSoftKeyItemDelegate::DirItemDelegate,
WSoftKeyItemDelegate::ColorItemDelegate,
WSoftKeyItemDelegate::BoolItemDelegate,
WSoftKeyItemDelegate::UIntItemDelegate,
WNormalItemDelegate::EnumItemDelegate
> softkey_types;
template<class Types,int index>
struct InitDelegates
{
	void operator()(unique_ptr<WNormalItemDelegate::AbstractItemDelegate>* delegates,XmlBaseViewDelegate* parent) {
		assert(delegates[index]==nullptr);
		if(std::is_same<typename mpl::at_c<Types,index>::type,void>::value) {
			delegates[index].reset();
		} else {
			delegates[index].reset( new typename mpl::at_c<Types,index>::type(parent));
		}
		if(index>1) InitDelegates<Types,index-1>()(delegates,parent);
	}
};
template<class Types>
struct InitDelegates<Types,0>
{
	void operator()(unique_ptr<WNormalItemDelegate::AbstractItemDelegate>* delegates,XmlBaseViewDelegate* parent)
	{
	}
};

NormalXmlViewDelegate::NormalXmlViewDelegate(WXmlView* view)
:XmlBaseViewDelegate(view)
{
	using namespace mpl;

	static_assert(mpl::size<normal_types>::type::value==ItemType::IT_NR,"error size");
	static_assert(mpl::size<softkey_types>::type::value==ItemType::IT_NR,"error size");

	if(!use_soft_keyboard_) {
		InitDelegates<normal_types,ItemType::IT_NR-1>()(item_delegates_,this);
	} else {
		InitDelegates<softkey_types,ItemType::IT_NR-1>()(item_delegates_,this);
	}
}
NormalXmlViewDelegate::~NormalXmlViewDelegate()
{
}
QWidget* NormalXmlViewDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	if(index.column() == 0) 
			return nullptr;
	WXmlItem* item = getItem(index);
	if(nullptr == item) return nullptr;
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return nullptr;
	}
	if(item->type() == ItemType::IT_ERROR
		|| item->type() >= ItemType::IT_NR) {
		ERR("Error");
	} else {
		return item_delegates_[item->type()]->createEditor(item,parent,option,index);
	}
	return nullptr;
}
void NormalXmlViewDelegate::setEditorData (QWidget * editor, const QModelIndex & index ) const
{
	if(index.column() == 0) return;

	WXmlItem* item = getItem(index);
	if(nullptr == item) return ;

	if(item->type() == ItemType::IT_ERROR
		|| item->type() >= ItemType::IT_NR) {
			ERR("Error");
	} else {
		return item_delegates_[item->type()]->setEditorData(item,editor,index);
	}
}
void NormalXmlViewDelegate::setModelData (QWidget * editor, QAbstractItemModel * model,const QModelIndex & index )const
{
	if(index.column() == 0) return;
	WXmlItem* item = getItem(index);
	if(nullptr == item) return;
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	if(item->type() == ItemType::IT_ERROR
		|| item->type() >= ItemType::IT_NR) {
			ERR("Error");
	} else {
		return item_delegates_[item->type()]->setModelData(item,editor,model,index);
	}
}
void NormalXmlViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if(index.column() == 0) {
		QItemDelegate::paint(painter,option,index);
		return;
	}
	auto item = getItem(index);
	if(nullptr == item) {
		QItemDelegate::paint(painter,option,index);
		return;
	}
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		QItemDelegate::paint(painter,option,index);
		return ;
	}
	if(item->type() == ItemType::IT_ERROR
			|| item->type() >= ItemType::IT_NR) {
		ERR("Error");
	} else {
		if(!item_delegates_[item->type()]->paint(item,painter,option,index))
			QItemDelegate::paint(painter,option,index);
	}
}
