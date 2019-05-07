/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "xmlstream.h"
#include "oldxmlstream.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>
#include <fstream>
#include <sstream>
#include "wmacros.h"

using namespace std;
using namespace boost::property_tree;

void DefaultErrorHandler(const QString& tag_name,const QString& value,WXmlItem* item)
{
	WARNING_LOG("Error tag: tag_name=%s, value=%s, id=%d",tag_name.toUtf8().data(),value.toUtf8().data(),item->id());
}
WXmlStream::WXmlStream(WXmlData* data)
:data_(data)
,error_handler_(DefaultErrorHandler)
{
}
void WXmlStream::setData(WXmlData* data)
{
	data_ = data;
}
bool WXmlStream::read(const void* data,size_t size)noexcept(false)
{
	string str((const char*)(data),size);
	istringstream ss(str);

	return read(ss);
}
bool WXmlStream::read(istream& stream)noexcept(false)
{
	XMLTree pt;
	try {
		read_xml(stream,pt);
	} catch(...) {
		throw runtime_error("Read xml faild.");
	}

	try {
		XMLTree& root = pt.get_child(ROOT_TAGNAME);
		parseConfigL(root);
	} catch (ptree_bad_path& e) {
		try {
			XMLTree& root = pt.get_child(OLD_ROOT_TAGNAME);
			OldXmlStream old_stream;
			old_stream.setErrorHandler(error_handler_);
			old_stream.setData(data_);
			old_stream.parseSystemConfigure(root);
			return true;
		} catch(...) {
		}
		auto info = QString("Root Tag 不存在: ")+e.what();
		throw runtime_error(info.toUtf8().data());
	} catch(...) {
		TRY_LOG(LL_ERROR,"ERROR");
		return false;
	}
	INFO("Read xml finish.");
	return true;
}
bool WXmlStream::readFile(const QString& file_path)
{

	if(!QFile::exists(file_path)) {
		TRY_LOG(LL_ERROR,"文件\"%s\"不存在",file_path.toUtf8().data());
		return false;
	}
	data_->setFilePath(file_path);
	try {
		QFile file(file_path);
		if(!file.open(QIODevice::ReadOnly)) {
			TRY_LOG(LL_ERROR,"Read xml \"%s\" faild",file_path.toUtf8().data());
			return false;
		}
		auto xml_data = file.readAll();
		file.close();
		string xml_str(xml_data.data(),xml_data.size());
		stringstream ss(xml_str);
		return read(ss);
	} catch(runtime_error& e) {
		TRY_LOG(LL_ERROR,"Read xml \"%s\" faild",file_path.toUtf8().data());
		return false;
	} catch(...) {
		TRY_LOG(LL_ERROR,"ERROR");
		return false;
	}
	return true;
}
bool WXmlStream::save(ostream& stream)noexcept(false)
{
	XMLTree root;
	boost::property_tree::xml_parser::xml_writer_settings<string> settings(' ',2);

	if(!buildXMLTree(data_,&root)) {
		throw runtime_error("Build XML Tree Faild!");
	}
	try {
		write_xml(stream,root,settings);
	} catch(...) {
		throw runtime_error("Save XML Faild!");
	}
	return true;
}
bool WXmlStream::saveFile(const QString& file_path)
{
	try {
		fstream stream(file_path.toUtf8().data(),ios_base::out);
		return save(stream);
	} catch(runtime_error& e) {
		TRY_LOG(LL_ERROR,"%s",e.what());
		return false;
	} catch(...) {
		TRY_LOG(LL_ERROR,"Save XML Faild!");
		return false;
	}
	return true;
}
bool WXmlStream::parseConfigL(const XMLTree& tree)
{
	QString  tag_name;

	for(const auto& child: tree) {
		tag_name = child.first.c_str();
		if(tag_name == XML_ATTR) {
			try {
				data_->setId(boost::lexical_cast<unsigned>(child.second.get<string>(CONFIGURE_ID_TAGNAME)));
				data_->setName(child.second.get<string>(CONFIGURE_NAME_TAGNAME).c_str());
				data_->setVersion(child.second.get<unsigned>(CONFIGURE_VERSION_TAGNAME));
			}catch(...) {
			}
			data_->setUserData(child.second.get<string>(USER_DATA_TAGNAME,string()).c_str());
		} else if(tag_name == GROUP_TAGNAME) {
			auto group = parseGroup(child.second);
			data_->addGroup(group);	
		} else {
			TRY_LOG(LL_ERROR,"Error element, TagName=%s",tag_name.toUtf8().data());
		}
	}
	return true;
}
shared_ptr<WXmlGroup> WXmlStream::parseGroup(const XMLTree& tree)
{
	QString tag_name;
	auto    group       = make_shared<WXmlGroup>();
	bool    is_readonly = false;

	if(tree.get<string>(string(XML_ATTR)+"."+REINIT_ID,"true") != "false")
		default_item_id_ = 0;

	for(const auto& child: tree) {
		tag_name = child.first.c_str();
		if(tag_name == XML_ATTR) {
			try {
				group->setName(child.second.get<string>(GROUPNAME_TAGNAME).c_str());
				group->setId(child.second.get<string>(GROUPID_TAGNAME).c_str());
			} catch(...) {
				group.reset();
				return group;
			}
			group->setUserData(child.second.get<string>(USER_DATA_TAGNAME,string()).c_str());
			try {
				if(child.second.get<string>(XA_READONLY) == "enable")
					is_readonly = true;
			} catch(...) {
			}
		} else  if(tag_name == ITEM_TAGNAME) {
			auto items = parseItem(child.second);
			group->addItems(items);
		} else if(tag_name == GROUP_REPEAT_TAGNAME) {
			auto items = parseGroupRepeat(child.second);
			group->addItems(items);
		} else if(tag_name == ATTR_TAGNAME) {
			const QString text = xmlvalue_text(child);
			TRY_LOG(LL_ERROR,"Error group attr:%s",text.toUtf8().data());
		} else {
			TRY_LOG(LL_ERROR,"Error tag name in group:%s",tag_name.toUtf8().data());
		}
	}
	if(is_readonly) {
		for(auto it=group->begin(); it!=group->end(); ++it) {
			(*it)->setReadOnly(true);
		}
	}
	return group;
}
vector<shared_ptr<WXmlItem>> WXmlStream::parseGroupRepeat(const XMLTree& tree)
{
	QString                      tag_name;
	unsigned                     repeat_nr = 0;
	vector<shared_ptr<WXmlItem>> res;

	for(const auto& child: tree) {
		tag_name = child.first.c_str();
		if(tag_name == XML_ATTR) {
			try {
				repeat_nr = child.second.get<unsigned>(CURRENTVALUE_TAGNAME);
				res.reserve(repeat_nr*2);
			} catch(...) {
				res.clear();
				return res;
			}
		} else  if(tag_name == ITEM_TAGNAME) {
			auto items = parseItem(child.second);
			res.insert(res.end(),items.begin(),items.end());
		} else if(tag_name == ATTR_TAGNAME) {
			const QString text = xmlvalue_text(child);
			TRY_LOG(LL_ERROR,"Error group repeat attr:%s",text.toUtf8().data());
		} else {
			TRY_LOG(LL_ERROR,"Error tag name in group repeat:%s",tag_name.toUtf8().data());
		}
	}

	return processGroupRepeat(res,repeat_nr);
}
vector<shared_ptr<WXmlItem>> WXmlStream::processGroupRepeat(vector<shared_ptr<WXmlItem>>& items,int repeat_nr)
{
	const int                    group_nr       = items.size();
	vector<shared_ptr<WXmlItem>> res;

	if(items.empty()) 
		return res;

	res = items;

	if(repeat_nr <= 1) 
		return res;

	try {
		vector<QString> patterns;

		for(auto i=0; i<group_nr; ++i) {
			auto ref_item = res[i];
			auto pattern  = ref_item->name();
			auto name     = getRepeatItemName(pattern,1);
			patterns.push_back(pattern);


			ref_item->setName(name);
		}

		for(int i=1; i<repeat_nr; ++i) {
			for(auto j=0; j<group_nr; ++j) {
				auto  ref_item = res[j];
				auto &pattern  = patterns[j];
				auto  item     = make_shared<WXmlItem>();

				*item = *ref_item;

				item->setId(default_item_id_++);
				auto name = getRepeatItemName(pattern,i+1);
				item->setName(name);
				res.push_back(item);
			}
		}
	} catch(...) {}

	return res;
}
vector<shared_ptr<WXmlItem>> WXmlStream::parseItem(const XMLTree& tree)
{
	QString temp;
	QString tag_name;
	int     repeat_nr = 1;
	vector<shared_ptr<WXmlItem>> res;
	auto    item      = make_shared<WXmlItem>();

	item->userData().clear();

	for(const auto& child: tree) {

		tag_name = child.first.c_str();
		if(tag_name == XML_ATTR) { 
			try {
				const auto id = child.second.get<unsigned>(ITEMID_TAGNAME,default_item_id_);
				default_item_id_ = id+1;
				item->setId(id);
				item->setName(child.second.get<string>(ITEMNAME_TAGNAME).c_str());

				repeat_nr = child.second.get<unsigned>(REPEAT_TAGNAME,1);

				const QString type = child.second.get<string>(ITEMTYPE_TAGNAME).c_str();
				if(item->initCoreDataByTypeName(type)) {
					item->flag().set(XIF_HAVENT_VALUE);
					item->flag().set(XIF_HAVENT_DEFAULT_VALUE);
					switch(item->type()) {
						case ItemType::IT_STRING:
						case ItemType::IT_DIR:
						case ItemType::IT_FILE:
						case ItemType::IT_DLIST:
							parseItemString(tree,item);
							break;
						case ItemType::IT_INT:
							parseNumberItem<WIntXmlItemData>(tree,item);
							break;
						case ItemType::IT_UINT:
							parseNumberItem<WUIntXmlItemData>(tree,item);
							break;
						case ItemType::IT_HEX:
						case ItemType::IT_COLOR:
							parseNumberItem<WHexXmlItemData>(tree,item);
							break;
						case ItemType::IT_DOUBLE:
							parseNumberItem<WFloatXmlItemData>(tree,item);
							break;
						case ItemType::IT_LIST:
							parseItemList(tree,item);
							break;
						case ItemType::IT_ENUM:
							parseItemEnum(tree,item);
							break;
						case ItemType::IT_TIME:
							parseItemTime(tree,item);
							break;
						case ItemType::IT_IP:
							parseItemIP(tree,item);
							break;
						case ItemType::IT_BOOL:
							break;
						case ItemType::IT_STATUS:
							break;
						default:
							TRY_LOG(LL_ERROR,"Error type!");
							item->dump();
							item.reset();
							return res;
							break;
					}
				}

				if(nullptr != item->data()) {
					auto value = child.second.get<string>(CURRENTVALUE_TAGNAME);
					item->flag().reset(XIF_HAVENT_VALUE);
					item->data()->setValueByString(value.c_str());
				} else {
					TRY_LOG(LL_ERROR,"ERROR");
					item->dump();
				}
			} catch(...) {
				ERR("Unexpected.");
				item.reset();
				return res;
			}
			try {
			} catch(...) {
			}
		} else if(tag_name == ITEMDESC_TAGNAME) {
			item->setDesc(xmlvalue_text(child));
		} else if(tag_name == DEFAULT_VALUE) {
			if(nullptr != item->data()) {
				item->data()->setDefaultValueByString(xmlvalue_text(child));
				item->flag().reset(XIF_HAVENT_DEFAULT_VALUE);
			} else {
				TRY_LOG(LL_ERROR,"ERROR");
				item->dump();
			}
		} else if(tag_name == ATTR_TAGNAME) {
			auto value = xmlvalue_text(child);
			value.simplified();
			value.remove(" ");
			value.remove("	");
			QStringList attrs = value.split(";",QString::SkipEmptyParts);
			parseItemAttrs(attrs,item);
		} else if(tag_name == USER_DATA_TAGNAME) {
			parseItemUserData(child.second,item);
		} else {
			if(tag_name == LISTVALUES_TAGNAME 
			  ) continue;
			error_handler_(tag_name,xmlvalue_text(child),item.get());
		}
	}
	if(1==repeat_nr) {
		res.push_back(item);
		return res;
	}
	return processRepeat(item,repeat_nr,tree);
}
vector<shared_ptr<WXmlItem>> WXmlStream::processRepeat(shared_ptr<WXmlItem>& ref_item,int repeat_nr,const XMLTree& tree)
{
	bool   user_define_id = false;
	bool   bok            = false;
	double a              = 1.0;
	double b              = 0.0;
	vector<shared_ptr<WXmlItem>> res;

	if(nullptr == ref_item) 
		return res;

	res.push_back(ref_item);

	if(repeat_nr <= 1) 
		return res;

	auto trans = QString(tree.get<string>(XML_ATTR "." REPEAT_ID_TAGNAME,string()).c_str());

	if(!trans.isEmpty()) {
		const auto trans_list = trans.split(",",QString::SkipEmptyParts);

		if(!trans_list.isEmpty()) {
			a = trans_list[0].toDouble(&bok);
			if(bok) user_define_id = true;
			if(trans_list.size()>1) {
				b = trans_list[1].toDouble(&bok);
				if(!bok) {
					user_define_id = false;
				}
			}
		}
	}

	try {
		auto  pattern  = ref_item->name();
		auto  name     = getRepeatItemName(pattern,1);

		if(user_define_id) {
			default_item_id_ = a+b;
			ref_item->setId(default_item_id_++);
		}

		ref_item->setName(name);

		for(int i=1; i<repeat_nr; ++i) {
			auto item = make_shared<WXmlItem>();

			*item = *ref_item;

			if(user_define_id) {
				default_item_id_ = (i+1)*a+b;
			} 
			item->setId(default_item_id_++);
			name = getRepeatItemName(pattern,i+1);
			item->setName(name);
			res.push_back(item);
		}
	} catch(...) {}
	return res;
}
bool WXmlStream::parseItemUserData(const XMLTree& root,shared_ptr<WXmlItem>& item)
{
	try {
		auto name = root.get<string>(string(XML_ATTR)+"."+USER_DATA_NAME_TAGNAME);
		item->userData()[name.c_str()] = root.data().c_str();
	} catch(...) {
		return false;
	}
	return true;
}
QString WXmlStream::getRepeatItemName(const QString& pattern,int index)
{
	bool bok = true;
	auto res = pattern;
	do {
		res = getRepeatItemName(res,index,&bok);
	} while(bok);

	return res;
}
QString WXmlStream::getRepeatItemName(const QString& pattern,int index,bool* bok)
{
	auto res = pattern;
	const auto begin = pattern.indexOf("{");
	if(-1 == begin) {
		if(bok) *bok = false;
		return pattern;
	}
	const auto end = pattern.indexOf("}",begin+1);
	if(-1 == end) {
		if(bok) *bok = false;
		return pattern;
	}

	if(bok) *bok = true;

	auto num_pattern = pattern.mid(begin+1,end-begin-1);

	if(num_pattern.isEmpty()) 
		return pattern.left(begin)+QString::number(index)+pattern.right(pattern.size()-end-1);

	double scale        = 1.0;
	double offset       = 0.0;
	auto   num_patterns = num_pattern.split(",",QString::SkipEmptyParts);
	bool   lbok         = false;

	if(num_patterns.isEmpty()) {
		return pattern.left(begin)+QString::number(index)+pattern.right(pattern.size()-end-1);
	}

	scale = num_patterns[0].toDouble(&lbok);
	if(!lbok) 
		return pattern.left(begin)+QString::number(index)+pattern.right(pattern.size()-end-1);

	if(num_patterns.size()>1) {
		offset = num_patterns[1].toDouble(&lbok);
		if(!lbok) 
			return pattern.left(begin)+QString::number(index)+pattern.right(pattern.size()-end-1);
	}

	return pattern.left(begin)+QString::number(int(index*scale+offset))+pattern.right(pattern.size()-end-1);
}
bool WXmlStream::parseItemAttrs(const QStringList& attrs,std::shared_ptr<WXmlItem>& item)
{
	for(int i=0; i<attrs.size(); ++i) {
		auto pair = attrs[i].split("=",QString::SkipEmptyParts);
		if(pair.isEmpty())continue;
		if(nullptr == item->data()) continue;
		if(pair.size() == 1) {
			if(pair[0] == XA_HIDE) {
				item->data()->flag().set(XIF_HIDE_ITEM);
			} else if(pair[0] == XA_READONLY) {
				item->setReadOnly(true);
			} else {
			}
		} else if(pair.size() >= 2) {
			bool bok;
			if(pair[0] == DATASIZE_TAGNAME) {
				auto size = pair[1].toInt(&bok);
				if(bok)item->setDataSize(size);
			} else if(pair[0] == MARGIN_TAGNAME) {
				auto margin = pair[1].toInt(&bok);
				if(bok)item->setMargin(margin);
			} else if(pair[0] == UNIT_TAGNAME) {
				item->setUnit(pair[1]);
			} else if(pair[0] == WIDTH_TAGNAME) {
				auto width = pair[1].toInt(&bok);
				if(bok)item->setWidth(width);
			} else if(pair[0] == DECIMALS_TAGNAME) {
					auto decimals = pair[1].toInt(&bok);
					if(bok)item->setDecimals(decimals);
			} else if(pair[0] == STEP_TAGNAME) {
				if(item->is_integral()) {
					auto step = pair[1].toInt(&bok);
					if(bok) item->setStep(step);
				} else if(item->is_floating_point()) {
					auto step = pair[1].toFloat(&bok);
					if(bok) item->setStep(step);
				}
			} else if(pair[0] == XA_TRANS) {
				if(nullptr == item->data())continue;
				//example trans=(1.1,2.3); trans=(1.1);
				pair[1] = pair[1].simplified();
				if(pair[1].size()<3)continue;
				pair[1] = pair[1].mid(1,pair[1].size()-2);
				auto temp_list = pair[1].split(',',QString::SkipEmptyParts);
				if(temp_list.isEmpty()) continue;

				double a   = 0;
				double b   = 0;
				bool   bok = false;

				a = temp_list[0].toDouble(&bok);
				if(!bok) continue;
				if(temp_list.size()>=2) {
					b = temp_list[1].toDouble(&bok);
					if(!bok) continue;
				}
				item->setTransParam(a,b);
			} else {
				TRY_LOG(LL_ERROR,"ERROR");
				item->dump();
			}
		}
	}
	return true;
}
QString WXmlStream::getXmlItemAttrs(const std::shared_ptr<WXmlItem>& item)
{
	QString data_attr;

	if(item->data()->flag().test(XIF_HIDE_ITEM)) {
		data_attr += QString(XA_HIDE)+";";
	}
	if(item->isReadOnly()) {
		data_attr += QString(XA_READONLY)+";";
	}
	if(item->dataSize() > 0
			&& item->dataSize() < 1024) {
		data_attr += QString("%1=%2;").arg(DATASIZE_TAGNAME).arg(item->dataSize());
	}
	if(item->is_floating_point() && item->decimals() != XML_DEFAULT_FLOAT_DECIMALS) {
		data_attr += QString("%1=%2;").arg(DECIMALS_TAGNAME).arg(item->decimals());
	}
	if(item->is_integral() && item->step<int>() > 0) {
		data_attr += QString("%1=%2;").arg(STEP_TAGNAME).arg(item->step<int>());
	}
	if(item->is_floating_point() && item->step<float>() > 1E8) {
		data_attr += QString("%1=%2;").arg(STEP_TAGNAME).arg(item->step<double>());
	}
	if(item->margin() != 0) {
		data_attr += QString("%1=%2;").arg(MARGIN_TAGNAME).arg(item->margin());
	}
	if(item->width() != 1) {
		data_attr += QString("%1=%2;").arg(WIDTH_TAGNAME).arg(item->width());
	}
	if(item->is_arithmetic() && !item->unit().isEmpty()) {
		data_attr += QString("%1=%2;").arg(UNIT_TAGNAME).arg(item->unit());
	}
	if(item->isUseTrans()) {
		data_attr += QString("%1=(%2,%3);").arg(XA_TRANS).arg(item->transA()).arg(item->transB());
	}
	return data_attr;
}
bool WXmlStream::parseItemString(const XMLTree& /*tree*/,shared_ptr<WXmlItem>& /*item*/)
{
	return true;
}
bool WXmlStream::parseItemIP(const XMLTree& /*tree*/,shared_ptr<WXmlItem>& /*item*/)
{
	return true;
}
bool WXmlStream::parseItemTime(const XMLTree& /*tree*/,shared_ptr<WXmlItem>& /*item*/)
{
	return true;
}
bool WXmlStream::parseItemList(const XMLTree& tree,shared_ptr<WXmlItem>& item)
{
	bool     ret_val  = false;
	QString  tag_name;

	if(nullptr == item) { ERROR_LOG("Error"); return false; }
	auto data = item->getCoreData<WListXmlItemData>();
	if(nullptr == data) { ERROR_LOG("ERROR"); return false; }
	try {
		QString values = tree.get<string>(LISTVALUES_TAGNAME).c_str();
		auto lists = values.split(";;",QString::SkipEmptyParts);
		data->setValueList(lists);
	} catch(ptree_bad_path& e) {
		TRY_LOG(LL_ERROR,"Error Path: %s",e.what());
	} catch(...) {
		TRY_LOG(LL_ERROR,"ERROR");
	}
	return ret_val;
}
bool WXmlStream::parseItemEnum(const XMLTree& tree,shared_ptr<WXmlItem>& item)
{
	bool     ret_val  = false;
	QString  tag_name;

	if(nullptr == item) { ERROR_LOG("Error"); return false; }
	auto  data = item->getCoreData<WEnumXmlItemData>();
	if(nullptr == data) { ERROR_LOG("ERROR"); return false; }
	try {
		QString values = tree.get<string>(LISTVALUES_TAGNAME).c_str();
		data->setEnumData(values);
	} catch(ptree_bad_path& e) {
		TRY_LOG(LL_ERROR,"Error Path: %s",e.what());
	} catch(...) {
		TRY_LOG(LL_ERROR,"ERROR");
	}
	return ret_val;
}
bool WXmlStream::buildXMLTree(const WXmlData* data,XMLTree* root)
{
	using std::shared_ptr;
	root->put(ROOT_TAGNAME "." XML_ATTR "." CONFIGURE_ID_TAGNAME,QByteArray::number(data->id()).data());
	root->put(ROOT_TAGNAME "." XML_ATTR "." CONFIGURE_NAME_TAGNAME,data->name().toUtf8().data());
	root->put(ROOT_TAGNAME "." XML_ATTR "." CONFIGURE_VERSION_TAGNAME,data->version());
	if(!data->userData().isEmpty()) {
		root->put(ROOT_TAGNAME "." XML_ATTR "." USER_DATA_TAGNAME,data->userData().toUtf8().data());
	}

	for(const auto& p: data->groups()) {
		XMLTree group_tree;
		fillXMLTreeWithGroupData(p,&group_tree);
		root->add_child(ROOT_TAGNAME "." GROUP_TAGNAME,group_tree);
	}
	return true;
}
bool WXmlStream::fillXMLTreeWithGroupData(const shared_ptr<WXmlGroup>& group,XMLTree* group_tree)
{
	group_tree->put(XML_ATTR "." GROUPID_TAGNAME,QByteArray::number(group->id()).data());
	group_tree->put(XML_ATTR "." GROUPNAME_TAGNAME,group->name().toUtf8().data());

	if(!group->userData().isEmpty()) {
		group_tree->put(XML_ATTR "." USER_DATA_TAGNAME,group->userData().toUtf8().data());
	}

	for(const auto& p: group->items()) {
		XMLTree item_tree;
		fillXMLTreeWithItemData(p,&item_tree);
		group_tree->add_child(ITEM_TAGNAME,item_tree);
	}
	return true;
}
bool WXmlStream::fillXMLTreeWithItemData(const shared_ptr<WXmlItem>& item,XMLTree* item_tree)
{
	QString data_attr;
	if(nullptr == item->data()) return false;

	item_tree->put(XML_ATTR "." ITEMID_TAGNAME,QByteArray::number(item->id()).data());
	item_tree->put(XML_ATTR "." ITEMNAME_TAGNAME,item->name().toUtf8().data());
	item_tree->put(XML_ATTR "." ITEMTYPE_TAGNAME,item->data()->typeName().toUtf8().data());
	if(!(item->flag().test(XIF_HAVENT_VALUE))) {
		item_tree->put(XML_ATTR "." CURRENTVALUE_TAGNAME,item->toString().toUtf8().data());
	} 
	if(item->name() != item->desc() && !item->desc().isEmpty())
		item_tree->put(ITEMDESC_TAGNAME,item->desc().toUtf8().data());

	/*>------------------------------------------------*/
	switch(item->type()) {
		case ItemType::IT_STRING:
		case ItemType::IT_FILE:
		case ItemType::IT_DIR:
		case ItemType::IT_DLIST:
			fillXMLTreeWithStringItem(item,item_tree);
			break;
		case ItemType::IT_INT:
			fillXMLTreeWithNumberItem<WIntXmlItemData>(item,item_tree);
			break;
		case ItemType::IT_UINT:
			fillXMLTreeWithNumberItem<WUIntXmlItemData>(item,item_tree);
			break;
		case ItemType::IT_HEX:
		case ItemType::IT_COLOR:
			fillXMLTreeWithNumberItem<WHexXmlItemData>(item,item_tree);
			break;
		case ItemType::IT_DOUBLE:
			fillXMLTreeWithNumberItem<WFloatXmlItemData>(item,item_tree);
			break;
		case ItemType::IT_LIST:
			fillXMLTreeWithListItem(item,item_tree);
			break;
		case ItemType::IT_ENUM:
			fillXMLTreeWithEnumItem(item,item_tree);
			break;
		default:
			break;
	}
	data_attr = getXmlItemAttrs(item);

	if(!data_attr.isEmpty())
		item_tree->put(ATTR_TAGNAME,data_attr.toUtf8().data());
	if(!item->flag().test(XIF_HAVENT_DEFAULT_VALUE))
		item_tree->put(DEFAULT_VALUE,item->data()->defaultValueToString().toUtf8().data());
	fillXMLTreeWithUserData(item,item_tree);
	return true;
}
bool WXmlStream::fillXMLTreeWithStringItem(const shared_ptr<WXmlItem>& /*item*/,XMLTree* /*stream*/)
{
	return true;
}
bool WXmlStream::fillXMLTreeWithListItem(const shared_ptr<WXmlItem>& item,XMLTree* xml_tree)
{
	WListXmlItemData *data   = item->getCoreData<WListXmlItemData>();
	QString           values;

	if(nullptr == data) { ERROR_LOG("ERROR"); return false; }

	for(const auto& str:data->valuelist()) {
		values += str+";;";
	}
	xml_tree->put(LISTVALUES_TAGNAME,values.toUtf8().data());
	return true;
}
bool WXmlStream::fillXMLTreeWithUserData(const shared_ptr<WXmlItem>& item,XMLTree* xml_tree)
{
	auto& user_data = item->userData();

	if(user_data.empty()) return true;

	for(auto& d:user_data) {
		XMLTree child;
		child.put(XML_ATTR "." USER_DATA_NAME_TAGNAME,d.first.toUtf8().data());
		child.data() = d.second.toUtf8().data();
		xml_tree->add_child(USER_DATA_TAGNAME,child);
	}
	return true;
}
bool WXmlStream::fillXMLTreeWithEnumItem(const shared_ptr<WXmlItem>& item,XMLTree* xml_tree)
{
	auto    data   = item->getCoreData<WEnumXmlItemData>();
	QString values;
	bool    bok;

	if(nullptr == data) { ERROR_LOG("ERROR"); return false; }

	for(const auto& str:data->valuelist()) {
		auto v = data->stringToValue(str,&bok);
		if(!bok) continue;
		values += QString::number(v)+":"+str+";;";
	}
	xml_tree->put(LISTVALUES_TAGNAME,values.toUtf8().data());
	return true;
}
