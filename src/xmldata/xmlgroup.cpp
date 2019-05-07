/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <xmlgroup.h>
#include "wmacros.h"
#include <algorithm>

using namespace std;

WXmlGroup::WXmlGroup()
:flag_(0)
,id_(0)
{
}
void WXmlGroup::setToDefaultValue()
{
	for(auto& p:items_) {
		p->setToDefaultValue();
	}
}
WXmlItem& WXmlGroup::findChildRefByID(unsigned id)noexcept(false)
{
	auto res = findChildByID(id);
	if(nullptr == res) throw runtime_error("error id");
	return *res;
}
const WXmlItem& WXmlGroup::findChildRefByID(unsigned id)const noexcept(false)
{
	auto res = const_cast<WXmlGroup*>(this)->findChildByID(id);
	if(nullptr == res) throw runtime_error("error id");
	return *res;
}
WXmlItem* WXmlGroup::findChildByID(unsigned id)
{
	auto it = find_if(items_.begin(),items_.end(),[id](const shared_ptr<WXmlItem>& item) {
			return item->id() ==id;
			});
	if(it != items_.end()) return it->get();
	return nullptr;
}
void WXmlGroup::dump()const
{
	qDebug()<<"\n-->Dump xml group info, {";
	qDebug()<<"Name:"<<name_;
	qDebug()<<"Id:"<<id_;
	qDebug()<<"Flag:"<<QString("0x%1").arg(flag().to_ulong(),0,16);
	qDebug()<<"Item count:"<<items_.size();
	for(auto it=items_.begin(); it!=items_.end(); ++it) {
		it->get()->dump();
	}
	qDebug()<<"}, xml group dump end<--";
}
shared_ptr<WXmlItem> WXmlGroup::addItem() 
{
	items_.emplace_back(make_shared<WXmlItem>());
	return items_.back();
}
bool WXmlGroup::addItem(shared_ptr<WXmlItem> v)
{
	if(nullptr == v) return false;
	items_.push_back(std::move(v));
	return true;
}
void WXmlGroup::assign(const WXmlGroup& g)
{
	auto min_size = std::min(items_.size(),g.items_.size());

	for(auto i=0; i<min_size; ++i) {
		if(nullptr == g.items_[i]) continue;
		items_[i]->assign(*g.items_[i]);
	}
}
