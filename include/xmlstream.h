/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <string>
#include <iostream>
#include "xmlstream_fwd.h"
class WXmlStream
{
	public:
		WXmlStream(WXmlData* data=nullptr);
		bool readFile(const QString& file_path);
		bool saveFile(const QString& file_path);
		bool read(std::istream& stream)noexcept(false);
		bool read(const void* data,size_t size)noexcept(false);
		bool save(std::ostream& ostream)noexcept(false);
		void setData(WXmlData* data);
		/*
		 * 读取文件时，当出现未识别的标签时(与ATTR_TAGNAME同一级)
		 * 使用error_handler_处理
		 */
		inline void setErrorHandler(std::function<ErrorHandler> handler) {
			error_handler_ = handler;
		}
	private:
		WXmlData* data_ = nullptr;//相识
		unsigned default_item_id_ = 0;
		std::function<void (const QString,const QString,WXmlItem* item)>  error_handler_;
	private:
		bool parseConfigL(const XMLTree& tree);
		std::shared_ptr<WXmlGroup> parseGroup(const XMLTree& element);
		std::vector<std::shared_ptr<WXmlItem>> parseGroupRepeat(const XMLTree& root);
		std::vector<std::shared_ptr<WXmlItem>> parseItem(const XMLTree& element);
		bool parseItemString(const XMLTree& root,std::shared_ptr<WXmlItem>& item);
		bool parseItemIP(const XMLTree& root,std::shared_ptr<WXmlItem>& item);
		bool parseItemTime(const XMLTree& root,std::shared_ptr<WXmlItem>& item);
		bool parseItemEnum(const XMLTree& root,std::shared_ptr<WXmlItem>& item);
		bool parseItemAttrs(const QStringList& attrs,std::shared_ptr<WXmlItem>& item);
		bool parseItemUserData(const XMLTree& root,std::shared_ptr<WXmlItem>& item);
		template<class DataType>
			bool parseNumberItem(const XMLTree& tree,std::shared_ptr<WXmlItem>& item) {
				if(nullptr == item) { ERR("Error"); return false; }
				DataType* data = item->getCoreData<DataType>();
				if(nullptr == data) { ERR("ERROR"); return false; }
				try {
					data->setMinByString(tree.get<std::string>(XML_ATTR "." MINVALUE_TAGNAME).c_str());
					data->setMaxByString(tree.get<std::string>(XML_ATTR "." MAXVALUE_TAGNAME).c_str());
					data->flag().set(XIF_SET_MIN);
					data->flag().set(XIF_SET_MAX);
				} catch(boost::property_tree::ptree_bad_path& e) {
				} catch(...) {
					ERR("ERROR");
				}
				return true;
			}
		bool parseItemList(const XMLTree& root,std::shared_ptr<WXmlItem>& item);
		QString xmlvalue_text(const XMLTree::value_type& v) {
			return v.second.data().c_str();
		}
	private:
		QString getRepeatItemName(const QString& pattern,int index,bool* bok);
		QString getRepeatItemName(const QString& pattern,int index);
		std::vector<std::shared_ptr<WXmlItem>> processRepeat(std::shared_ptr<WXmlItem>& item,int repeat_nr,const XMLTree& tree);
		std::vector<std::shared_ptr<WXmlItem>> processGroupRepeat(std::vector<std::shared_ptr<WXmlItem>>& item,int repeat_nr);
		bool buildXMLTree(const WXmlData* data,XMLTree* tree);
		bool fillXMLTreeWithGroupData(const std::shared_ptr<WXmlGroup>& data,XMLTree* tree);
		bool fillXMLTreeWithItemData(const std::shared_ptr<WXmlItem>& data,XMLTree* tree);
		bool fillXMLTreeWithStringItem(const std::shared_ptr<WXmlItem>& data,XMLTree* tree);
		bool fillXMLTreeWithListItem(const std::shared_ptr<WXmlItem>& data,XMLTree* tree);
		bool fillXMLTreeWithEnumItem(const std::shared_ptr<WXmlItem>& data,XMLTree* tree);
		bool fillXMLTreeWithUserData(const std::shared_ptr<WXmlItem>& data,XMLTree* tree);
		QString getXmlItemAttrs(const std::shared_ptr<WXmlItem>& data);
		template<class DataType>
			bool fillXMLTreeWithNumberItem(const std::shared_ptr<WXmlItem>& item,XMLTree* xml_tree)
			{
				auto data = item->getCoreData<DataType>();

				if(nullptr == data) { ERR("ERROR"); return false; }

				if(item->haveMinAndMaxValue()) {
					xml_tree->put(XML_ATTR "." MINVALUE_TAGNAME,data->valueToString(data->limit()[0]).toUtf8().data());
					xml_tree->put(XML_ATTR "." MAXVALUE_TAGNAME,data->valueToString(data->limit()[1]).toUtf8().data());
				}
				return true;
			}
		DISABLE_COPY_AND_ASSIGN(WXmlStream);
};
