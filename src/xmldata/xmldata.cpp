/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "xmldata.h"
#include "wmacros.h"
#include <algorithm>
#include <boost/foreach.hpp>
using namespace boost;
using namespace std;
WXmlData::WXmlData()
:flag_(0)
{
}
void WXmlData::setToDefaultValue()
{
	using std::shared_ptr;
	for(auto & p:groups_) {
		p->setToDefaultValue();
	}
}
WXmlGroup* WXmlData::findChildByID(unsigned gid)
{
	for(unsigned i=0; i<groups_.size(); ++i) 
		if(gid == groups_.at(i)->id()) return groups_[i].get();
	return nullptr;
}
bool WXmlData::empty()const 
{
	using std::shared_ptr;
	if(groups_.empty()) return true;
	if(!groups_.front()->items().empty()) return false;
	BOOST_FOREACH(const shared_ptr<WXmlGroup>& p,groups()) {
		if(!p->items().empty()) return false;
	}
	return true;
}
WXmlData::iterator WXmlData::begin()
{
	if(empty()) return end();
	iterator it(this);
	it.current_group_ = groups_.begin();
	while(it.current_group_!=groups().end()
			&& (*(it.current_group_))->items().empty())
		++it.current_group_;
	if(it.current_group_ != groups_.end())
		it.current_item_ = (*(it.current_group_))->items().begin();
	return it;
}
WXmlData::iterator WXmlData::end()
{
	iterator it(this);
	it.current_group_ = groups_.end();
	return it;
}
void WXmlData::dump()const
{
	qDebug()<<"-->Dump xml data info, {";
	qDebug()<<"Name:"<<name_;
	qDebug()<<"Id:"<<id_;
	qDebug()<<"FilePath:"<<file_path_;
	qDebug()<<"Flag:"<<QString("0x%1").arg(flag().to_ulong(),0,16);
	qDebug()<<"Group count:"<<groups_.size();
	for(auto it=groups_.begin(); it!=groups_.end(); ++it) {
		it->get()->dump();
	}
	qDebug()<<"}, xml data dump end<--";
}
bool WXmlData::iterator::equal(const iterator& v) const
{
	if(current_group_ == data_->groups_.end()) {
		if(v.current_group_ == data_->groups_.end()) return true;
		return false;
	}
	return current_group_==v.current_group_ && current_item_==v.current_item_;
}
void WXmlData::iterator::increment()
{
	++current_item_;
	if(current_item_ == (*current_group_)->items().end()) {
		++current_group_;
		while(current_group_ != data_->groups_.end()
				&& (*current_group_)->items().empty())
			++current_group_;
		if(data_->groups_.end() != current_group_) 
			current_item_ = (*current_group_)->items().begin();
	}
}
WXmlItem* WXmlData::getItem(unsigned pid) 
{
	const unsigned gid = getGid(pid);
	const unsigned iid = getIid(pid);
	if(gid >= groups().size()) return nullptr;
	if(iid >= groups()[gid]->items().size()) return nullptr;
	if(isgroup(pid)) return nullptr;
	return groups_[gid]->items()[iid].get();
}
WXmlGroup* WXmlData::getGroup(unsigned pid) 
{
	const unsigned gid = getGid(pid);
	if(!isgroup(pid)) return nullptr;
	if(gid>=groups().size()) return nullptr;
	return groups()[gid].get();
}
WXmlGroup* WXmlData::groupAt(unsigned pos)const 
{
	if(pos >= groups().size()) return nullptr;
	return groups()[pos].get();
}
WXmlGroup* WXmlData::addGroup() 
{
	groups_.emplace_back(std::make_shared<WXmlGroup>());
	return groups().back().get();
}
bool WXmlData::addGroup(std::shared_ptr<WXmlGroup> v)
{
	if(v == nullptr) return false;
	groups().push_back(std::move(v));
	return true;
}
WXmlItem* WXmlData::backItem()
{
	if(groups().empty()) return nullptr;
	if(groups().back()->items().empty()) return nullptr;
	return groups().back()->items().back().get();
}
WXmlGroup* WXmlData::backGroup() 
{
	if(groups().empty()) return nullptr;
	return groups().back().get();
}
bool WXmlData::eraseGroup(int i) 
{
	if(i<0 || i>=groups().size()) return false;
	groups().erase(groups().begin()+i);
	return true;
}
WXmlItem* WXmlData::findItemByID(unsigned gid,unsigned iid) 
{
	WXmlGroup* g = findChildByID(gid);
	if(nullptr == g) return nullptr;
	return g->findChildByID(iid);
}
WXmlGroup& WXmlData::findChildRefByID(unsigned gid)noexcept(false)
{
	auto res = findChildByID(gid);
	if(nullptr == res)throw runtime_error("error gid");
	return *res;
}
WXmlItem& WXmlData::findItemRefByID(unsigned gid,unsigned iid)noexcept(false)
{
	auto res = findItemByID(gid,iid);
	if(nullptr == res)throw runtime_error("error gid or iid");
	return *res;
}
const WXmlGroup& WXmlData::findChildRefByID(unsigned gid)const noexcept(false)
{
	auto res = const_cast<WXmlData*>(this)->findChildByID(gid);
	if(nullptr == res)throw runtime_error("error gid");
	return *res;
}
const WXmlItem& WXmlData::findItemRefByID(unsigned gid,unsigned iid)const noexcept(false)
{
	auto res = const_cast<WXmlData*>(this)->findItemByID(gid,iid);
	if(nullptr == res)throw runtime_error("error gid or iid");
	return *res;
}
void WXmlData::assign(const WXmlData& d)
{
	auto min_size = std::min(groups_.size(),d.groups_.size());

	for(auto i=0; i<min_size; ++i) {
		if(nullptr == d.groups_[i]) continue;
		groups_[i]->assign(*d.groups_[i]);
	}
}
