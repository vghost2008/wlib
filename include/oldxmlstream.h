/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <string>
#include "xmlstream_fwd.h"
/*
 * xml读取及保存模块
 */
class OldXmlStream
{
	public:
		OldXmlStream(WXmlData* data=nullptr);
		bool readFile(const QString& file_path);
		void setData(WXmlData* data);
		inline void setErrorHandler(std::function<ErrorHandler> handler) {
			error_handler_ = handler;
		}
		inline void setUserDataHandler(std::function<ProcessUserDataHandler> handler) {
			user_data_handler_ = handler;
		}
		bool parseSystemConfigure(const XMLTree& tree);
	private:
		WXmlData* data_ ;//相识
		std::function<void (const QString,const QString,WXmlItem* item)>  error_handler_;
		std::function<void (const WXmlItem& item,XMLTree* tree)>  user_data_handler_;
	private:
		bool parseGroup(const XMLTree& element);
		bool parseItem(const XMLTree& element);
		bool parseItemString(const XMLTree& root);
		bool parseItemIP(const XMLTree& root);
		bool parseItemTime(const XMLTree& root);
		template<class DataType>
			bool parseNumberItem(const XMLTree& tree) {
				WXmlItem *item     = data_->backItem();
				using std::string;
				if(nullptr == item) { ERR("Error"); return false; }
				DataType* data = item->getCoreData<DataType>();
				if(nullptr == data) { ERR("ERROR"); return false; }
				try {
					auto decimals = tree.get<int>(DECIMALS_TAGNAME);
					item->setDecimals(decimals);
				} catch(boost::property_tree::ptree_bad_path& e) {
				} catch(...) {
				}
				try {
					data->setMinByString(tree.get<string>(OLD_MINVALUE_TAGNAME).c_str());
					data->setMaxByString(tree.get<string>(OLD_MAXVALUE_TAGNAME).c_str());
					data->flag().set(XIF_SET_MIN);
					data->flag().set(XIF_SET_MAX);
				} catch(boost::property_tree::ptree_bad_path& e) {
				} catch(...) {
					ERR("ERROR");
				}
				try {
					data->setStepByString(tree.get<string>(OLD_STEP_TAGNAME).c_str());
				} catch(...) {
				}
				return true;
			}
		bool parseItemList(const XMLTree& root);
		bool parseItemListValues(const XMLTree& element);
		QString xmlvalue_text(const XMLTree::value_type& v) {
			return v.second.data().c_str();
		}
		DISABLE_COPY_AND_ASSIGN(OldXmlStream);
};
