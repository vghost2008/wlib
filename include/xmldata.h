/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

_Pragma("once")
#include <QtCore>
#include <vector>
#include <memory>
#include "xmlgroup.h"

class WXmlData;
class WXmlGroup;
class WXmlItem;
/*
 * xml文件数据
 */
class WXmlData
{
	public:
		WXmlData();
		void dump()const;
	public:
		bool empty()const;
		inline void setId(unsigned id) { id_ = id;}
		inline unsigned id()const { return id_; }
		inline void setFilePath(const QString& path) { file_path_ = path; }
		inline QString filePath()const { return file_path_; }
		inline void setName(const QString& name) { name_= name;}
		inline const QString& name()const { return name_; }
		inline std::bitset<32>& flag() { return flag_; }
		inline const std::bitset<32>& flag()const { return flag_; }
		std::vector<std::shared_ptr<WXmlGroup>>& groups() {
			return groups_;
		}
		const std::vector<std::shared_ptr<WXmlGroup>>& groups()const {
			return groups_;
		}
		unsigned version()const { return version_; }
		void setVersion(unsigned v) { version_ = v; }
		/*
		 * 仅赋值，其它辅助数据不变
		 */
		void assign(const WXmlData& v);
	public:
		/*
		 * 获取一项的组id,无论当前项是item还是group都可以正确获取
		 */
		static unsigned getGid(unsigned pid) {
			return ((pid&(0xFFF<<12))>>12);
		}
		static unsigned getIid(unsigned pid) {
			return (pid&(0xFFF));
		}
		static unsigned isgroup(unsigned pid) {
			return pid&0x80000000;
		}
		/*
		 * 为一个item项产生一个id
		 */
		static unsigned mkposid(unsigned gid,unsigned iid) {
			iid &= 0xFFF;
			gid &= 0xFFF;
			return (gid<<12)|iid;
		}
		/*
		 * 为一个组项产生一个pid
		 */
		static unsigned mkposid(unsigned gid) {
			gid &= 0xFFF;
			return (gid<<12)|0x80000000;
		}
		WXmlItem* getItem(unsigned pid); //pid为内部使用的id
		WXmlGroup* getGroup(unsigned pid); //pid为内部使用的id
		WXmlGroup* groupAt(unsigned pos)const; //pos为内部使用的标记
		WXmlGroup* addGroup();
		bool addGroup(std::shared_ptr<WXmlGroup> v);
		WXmlItem* backItem();
		WXmlGroup* backGroup();
		void setToDefaultValue();
		bool eraseGroup(int i);
		WXmlGroup* findChildByID(unsigned gid);
		WXmlItem* findItemByID(unsigned gid,unsigned iid);
		WXmlGroup& findChildRefByID(unsigned gid)noexcept(false);
		WXmlItem& findItemRefByID(unsigned gid,unsigned iid)noexcept(false);
		const WXmlGroup& findChildRefByID(unsigned gid)const noexcept(false);
		const WXmlItem& findItemRefByID(unsigned gid,unsigned iid)const noexcept(false);
		inline size_t size()const { return groups_.size(); }
		QString userData()const { return user_data_; }
		void setUserData(const QString& v) { user_data_ = v; }
	public:
		class iterator:public boost::iterator_facade<iterator,WXmlItem,boost::forward_traversal_tag> 
	{
		public:
			bool equal(const iterator& v) const;
			void increment();
			inline WXmlItem& dereference()const { return *current_item_->get(); }
		private:
			std::vector<std::shared_ptr<WXmlGroup> >::iterator current_group_;
			std::vector<std::shared_ptr<WXmlItem>>::iterator current_item_;
			WXmlData* data_; //相识
		private:
			iterator(WXmlData* d):data_(d){}
			friend class boost::iterator_core_access;
			friend class WXmlData;
	};
		iterator begin();
		iterator end();
	private:
		unsigned        id_        = 0;
		unsigned        version_   = 1;
		QString         name_;
		std::bitset<32> flag_        {0};
		QString         file_path_;
		QString         user_data_;
		std::vector<std::shared_ptr<WXmlGroup> > groups_;
		friend CalibrationTest;
};
