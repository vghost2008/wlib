/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include "xmlitem.h"
/*
 * xml组数据
 */
class WXmlGroup
{
	public:
		WXmlGroup();
		inline void setName(const QString& name) {
			name_ = name;
		}
		inline unsigned id()const { return id_; }
		inline void setId(const unsigned& id) {
			id_ = id;
		}
		inline bool setId(const QString& id) {
			bool bok = false;
			unsigned temp = id.toUInt(&bok);
			if(bok)id_ = temp;
			return bok;
		}
		inline std::vector<std::shared_ptr<WXmlItem>>& items() {
			return items_;
		}
		inline const std::vector<std::shared_ptr<WXmlItem>>& items()const {
			return items_;
		}
		std::shared_ptr<WXmlItem> addItem();
		bool addItem(std::shared_ptr<WXmlItem> data);
		template<typename ItemsType>
		bool addItems(ItemsType&& datas) {
			for(auto& data:datas) {
				if(!addItem(data)) return false;
			}
			return true;
		}
		inline const QString& name()const { return name_; }
		inline std::bitset<32>& flag() { return flag_; }
		inline const std::bitset<32>& flag()const { return flag_; }
		inline size_t size()const { return items_.size(); }
		QString userData()const { return user_data_; }
		void setUserData(const QString& v) { user_data_ = v; }
	public:
		/*
		 * 仅赋值，其它辅助数据不变
		 */
		void assign(const WXmlGroup& g);
	public:
		template<typename OutType,typename GetDataFunc>
			bool getItemData(unsigned id,GetDataFunc func,OutType* v) {
				auto item = findChildByID(id);
				if(nullptr == item) {
					QSIMPLE_LOG(LL_ERROR,QString("Find item id=")+id+QString(" faild!"));
					return false;
				}
				try {
				*v = static_cast<OutType>((item->*func)());
				} catch(std::bad_cast& e) {
					QSIMPLE_LOG(LL_ERROR,QString("ERROR cast ")+e.what());
				} catch(...) {
					QSIMPLE_LOG(LL_ERROR,QString("ERROR TYPE"));
				}
				return true;
			}
		void dump()const;
	public:
		void setToDefaultValue();
		WXmlItem* findChildByID(unsigned id);
		WXmlItem& findChildRefByID(unsigned id)noexcept(false);
		const WXmlItem& findChildRefByID(unsigned id)const noexcept(false);
		inline std::vector<std::shared_ptr<WXmlItem>>::iterator begin() { return items_.begin(); }
		inline std::vector<std::shared_ptr<WXmlItem>>::iterator end() { return items_.end(); }
		inline std::vector<std::shared_ptr<WXmlItem>>::const_iterator begin()const { return items_.cbegin(); }
		inline std::vector<std::shared_ptr<WXmlItem>>::const_iterator end()const { return items_.cend(); }
		inline std::vector<std::shared_ptr<WXmlItem>>::const_iterator cbegin() { return items_.cbegin(); }
		inline std::vector<std::shared_ptr<WXmlItem>>::const_iterator cend() { return items_.cend(); }
	private:
		unsigned                               id_;
		QString                                name_;
		std::bitset<32>                        flag_;
		std::vector<std::shared_ptr<WXmlItem>> items_;
		QString                                user_data_;
		friend MCSParamCreator;
		friend DataSimDialog;
};
