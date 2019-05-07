/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "oldxmlstream.h"
#include <boost/property_tree/xml_parser.hpp>
#include "wmacros.h"

using namespace std;
using namespace boost::property_tree;
void OldDefaultErrorHandler(const QString& tag_name,const QString& value,WXmlItem* item)
{
	WARNING_LOG("Error tag: tag_name=%s, value=%s, id=%d",tag_name.toUtf8().data(),value.toUtf8().data(),item->id());
}
OldXmlStream::OldXmlStream(WXmlData* data)
:data_(data)
,error_handler_(OldDefaultErrorHandler)
,user_data_handler_(nullptr)
{
}
void OldXmlStream::setData(WXmlData* data)
{
	data_ = data;
}
bool OldXmlStream::readFile(const QString& file_path)
{
	XMLTree pt;

	if(!QFile::exists(file_path)) {
		TRY_LOG(LL_ERROR,"文件\"%s\"不存在",file_path.toUtf8().data());
		return false;
	}
	data_->setFilePath(file_path);
	try {
		read_xml(file_path.toUtf8().data(),pt);
	} catch(...) {
		TRY_LOG(LL_ERROR,"Read xml\"%s\"faild!",file_path.toUtf8().data());
		return false;
	}
	try {
		XMLTree& root = pt.get_child(OLD_ROOT_TAGNAME);
		parseSystemConfigure(root);
	} catch (ptree_bad_path& e) {
		TRY_LOG(LL_ERROR,"Root Tag 不存在: %s",e.what());
		return false;
	} catch(...) {
		TRY_LOG(LL_ERROR,"ERROR");
		return false;
	}
	INFO("Read File finish.");
	return true;
}
bool OldXmlStream::parseSystemConfigure(const XMLTree& tree)
{
	QString  tag_name;

	for(const auto& child: tree) {
		tag_name = child.first.c_str();
		if(tag_name == OLD_CONFIGURE_ID_TAGNAME) {
			data_->setId(xmlvalue_text(child).toUInt());
		} else if(tag_name == OLD_CONFIGURE_NAME_TAGNAME) {
			data_->setName(xmlvalue_text(child));
		} else if(tag_name == OLD_GROUP_TAGNAME) {
			parseGroup(child.second);
		} else if(tag_name == OLD_DATAFLAG_TAGNAME) {
#if 0
			const QString& text = xmlvalue_text(child);
#endif
		} else {
			TRY_LOG(LL_ERROR,"Error element, TagName=%s",tag_name.toUtf8().data());
		}
	}
	return true;
}
bool OldXmlStream::parseGroup(const XMLTree& tree)
{
	QString    tag_name;
	WXmlGroup *group    = data_->addGroup();

	if(nullptr == group) return false;

	for(const auto& child: tree) {
		tag_name = child.first.c_str();
		if(tag_name == OLD_GROUPID_TAGNAME) {
			group->setId(xmlvalue_text(child));
		} else if(tag_name == OLD_GROUPNAME_TAGNAME) {
			group->setName(xmlvalue_text(child));
		} else  if(tag_name == OLD_ITEM_TAGNAME) {
			parseItem(child.second);
		} else if(tag_name == OLD_GROUPFLAG_TAGNAME) {
			const QString text = xmlvalue_text(child);
			TRY_LOG(LL_ERROR,"Error group flag:%s",text.toUtf8().data());
		} else {
			TRY_LOG(LL_ERROR,"Error tag name in group:%s",tag_name.toUtf8().data());
		}
	}
	return true;
}
bool OldXmlStream::parseItem(const XMLTree& tree)
{
	QString    temp;
	QString    tag_name;
	WXmlGroup *group    = data_->backGroup();

	if(nullptr == group) { TRY_LOG(LL_ERROR,"ERROR"); return false;}
	auto item = group->addItem();
	if(nullptr == item) { TRY_LOG(LL_ERROR,"ERROR"); return false; }

	for(const auto& child: tree) {
		tag_name = child.first.c_str();
		if(tag_name == OLD_ITEMID_TAGNAME) {
			item->setId(xmlvalue_text(child));
		} else if(tag_name == OLD_ITEMNAME_TAGNAME) {
			temp = xmlvalue_text(child);
			item->setName(temp);
			item->setDesc(temp);
		} else if(tag_name == OLD_ITEMDESC_TAGNAME) {
			item->setDesc(xmlvalue_text(child));
		} else if(tag_name == OLD_ITEMTYPE_TAGNAME) {
			const QString type = xmlvalue_text(child);
			if(item->initCoreDataByTypeName(type)) {
				switch(item->type()) {
					case ItemType::IT_STRING:
					case ItemType::IT_DIR:
					case ItemType::IT_FILE:
					case ItemType::IT_DLIST:
						parseItemString(tree);
						break;
					case ItemType::IT_INT:
						parseNumberItem<WIntXmlItemData>(tree);
						break;
					case ItemType::IT_UINT:
						parseNumberItem<WUIntXmlItemData>(tree);
						break;
					case ItemType::IT_HEX:
					case ItemType::IT_COLOR:
						parseNumberItem<WHexXmlItemData>(tree);
						break;
					case ItemType::IT_DOUBLE:
						parseNumberItem<WFloatXmlItemData>(tree);
						break;
					case ItemType::IT_LIST:
						parseItemList(tree);
						break;
					case ItemType::IT_TIME:
						parseItemTime(tree);
						break;
					case ItemType::IT_IP:
						parseItemIP(tree);
						break;
					default:
						TRY_LOG(LL_ERROR,"Error type!");
						item->dump();
						return false;
						break;
				}
			}
		} else if(tag_name == OLD_DATASIZE_TAGNAME) {
			const QString size = xmlvalue_text(child);
			item->setDataSize(size.toInt());
		} else if(tag_name == OLD_CURRENTVALUE_TAGNAME) {
			if(nullptr != item->data()) {
				item->data()->setValueByString(xmlvalue_text(child));
			} else {
				TRY_LOG(LL_ERROR,"ERROR");
				item->dump();
			}
		} else if(tag_name == OLD_DEFAULT_VALUE) {
			if(nullptr != item->data()) {
				item->data()->setDefaultValueByString(xmlvalue_text(child));
			} else {
				TRY_LOG(LL_ERROR,"ERROR");
				item->dump();
			}
		} else if(tag_name == OLD_DATAFLAG_TAGNAME) {
			if(nullptr != item->data()) {
				const QString temp = xmlvalue_text(child);
				if(temp == "hide") {
					item->data()->flag().set(XIF_HIDE_ITEM);
				}
			} else {
				TRY_LOG(LL_ERROR,"ERROR");
				item->dump();
			}

		} else {
			if(tag_name == OLD_LISTVALUES_TAGNAME || tag_name == OLD_LISTVALUE_TAGNAME || tag_name == OLD_DATASIZE_TAGNAME || tag_name == OLD_MINVALUE_TAGNAME 
					|| tag_name == OLD_MAXVALUE_TAGNAME || tag_name == OLD_DECIMALS_TAGNAME || tag_name == OLD_STEP_TAGNAME) continue;

			error_handler_(tag_name,xmlvalue_text(child),item.get());
		}
	}
	return true;
}
bool OldXmlStream::parseItemString(const XMLTree& /*tree*/)
{
	return true;
}
bool OldXmlStream::parseItemIP(const XMLTree& /*tree*/)
{
	return true;
}
bool OldXmlStream::parseItemTime(const XMLTree& /*tree*/)
{
	return true;
}
bool OldXmlStream::parseItemList(const XMLTree& tree)
{
	bool     ret_val  = false;
	QString  tag_name;
	WXmlItem *item     = data_->backItem();

	if(nullptr == item) { ERROR_LOG("Error"); return false; }
	WListXmlItemData* data = item->getCoreData<WListXmlItemData>();
	if(nullptr == data) { ERROR_LOG("ERROR"); return false; }
	try {
		ret_val = parseItemListValues(tree.get_child(OLD_LISTVALUES_TAGNAME));
	} catch(ptree_bad_path& e) {
		TRY_LOG(LL_ERROR,"Error Path: %s",e.what());
	} catch(...) {
		TRY_LOG(LL_ERROR,"ERROR");
	}
	return ret_val;
}
bool OldXmlStream::parseItemListValues(const XMLTree& tree)
{
	WXmlItem* item = data_->backItem();

	if(nullptr == item) { ERROR_LOG("Error"); return false; }
	WListXmlItemData* data = item->getCoreData<WListXmlItemData>();
	if(nullptr == data) { ERROR_LOG("ERROR"); return false; }

	QStringList lists;
	for(const auto& child: tree) {
		const QString tag_name = child.first.c_str();
		if(tag_name != OLD_LISTVALUE_TAGNAME) continue;
			lists<<xmlvalue_text(child);
	}

	data->setValueList(lists);
	return true;
}
