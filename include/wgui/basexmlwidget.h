_Pragma("once")
#include <QtCore>
#include <QtGui>
#include <bitset>
#include <stdexcept>
#include <vector>
#include "xmlwidgetview.h"
#include "xmlwidget_fwd.h"
extern void BaseXmlWidgetNullProcess(std::vector<WXmlData>*);
class BaseXmlWidget:public QWidget
{
	Q_OBJECT
	public:
		BaseXmlWidget(const QStringList& file_list,QWidget* parent=nullptr,int column_count=3,std::function<void(std::vector<WXmlData>*)> func=BaseXmlWidgetNullProcess);
		BaseXmlWidget(const QString& dir_path,QWidget* parent=nullptr,int column_count=3,std::function<void(std::vector<WXmlData>*)> func=BaseXmlWidgetNullProcess);
		BaseXmlWidget(QWidget* parent=nullptr,int column_count=3,std::function<void(std::vector<WXmlData>*)> func=BaseXmlWidgetNullProcess);
	public:
		QHBoxLayout* hlayout0() { return btn_layout0_; }
		QHBoxLayout* hlayout1() { return btn_layout_; }
	public:
		virtual bool init(const QStringList& file_list);
		/*
		 * 数据量发生改变时调用
		 */
		virtual bool init()=0;
		/*
		 * 当数据内容改变，但是数据数量没有改变时使用
		 * 否则调用init
		 */
		virtual void setViewData();
		/*
		 * 第index个数据内容改变
		 */
		virtual void setViewData(int index);
		virtual void setViewData(int index,int gid);
		virtual void setViewData(int index,int gid,int iid)=0;
		/*
		 * 将界面显示的值同步到xml_data数据里
		 */
		virtual void setModelData();
		virtual void setModelData(int index);
		virtual void setModelData(int index,int gid);
		virtual void setModelData(int index,int gid,int iid)=0;
		/*
		 * 返回数据数量
		 */
		virtual size_t dataCount()const;
		WXmlData* xmlDataAt(int i) {
			if(i<0 || i>= data_list_.size()) return nullptr;
			auto res = &data_list_[i];
			if(res->empty()) {
				getXmlData(getFilePathByIndex(i),res);
			}
			return res;
		}
		inline WXmlData& xmlDataRefAt(int i)noexcept(false) {
			if(i<0 || i>= data_list_.size()) throw std::runtime_error("null data");
			return *xmlDataAt(i);
		}
		inline const WXmlData& xmlDataRefAt(int i)const noexcept(false){
			if(i<0 || i>= data_list_.size()) throw std::runtime_error("null data");
			return data_list_[i];
		}
		inline const std::bitset<32>& flag()const { return flag_; }
		/*
		 * f为标志的第i个标记 [0,32)
		 */
		void setFlag(unsigned f);

		virtual WXmlData& getDataByIndex(int index)noexcept(false);
		virtual QStringList getNameList()const;
		virtual QString getFilePathByIndex(int index)const noexcept(false);
		virtual QString currentFile()const;
		virtual QString currentName()const;
		WXmlData& currentData()noexcept(false);
		const WXmlData& currentData()const noexcept(false);
		virtual int currentIndex()const=0;
		/*
		 * 当文件保存成功后调用，与sigXmlFileSaved作用相同
		 * 先调用xmlFileSaved再发送信号sigXmlFileSaved
		 * @file_path:文件绝对路径
		 */
		virtual void xmlFileSaved(const QString& file_path);
		virtual void processBeforeDisplay();
		/*
		 * slotSave的实现函数
		 */
		virtual void doSave();
		bool saveFileAt(int index);
		inline int column_count()const { return column_count_; }
		inline QStringList file_list()const { return file_list_; }
		inline const std::vector<WXmlData>& data_list ()const { return data_list_; }
		inline std::vector<WXmlData>& data_list (){ return data_list_; }
		inline QPushButton* btn_default() { return btn_default_; }
		inline QPushButton* btn_save() { return btn_save_; }
	protected:
		bool getXmlData(const QString& file_path,WXmlData* data);
		void flagChanged();
	protected:
		QHBoxLayout           *main_layout_  = nullptr;
		QHBoxLayout           *btn_layout0_  = nullptr;
		QHBoxLayout           *btn_layout_   = nullptr;
		QStringList            file_list_;
		std::vector<WXmlData>  data_list_;
		QPushButton           *btn_default_  = nullptr;
		QPushButton           *btn_save_     = nullptr;
		std::bitset<32>        flag_         = 0;
		int                    column_count_ = 3;
		std::function<void (std::vector<WXmlData>*)> data_process_;
	private:
		void initGUI();
	public:
		void slotSave();
		void slotDefaultValue();
		void slotCurrentFileChanged(int index);
	signals:
		/*
		 * 配置文件保存成功后后发送信号sigXmlFileSaved
		 * @file_path:文件绝对路径
		 */
		void sigXmlFileSaved(const QString& file_path);
		void sigValueChanged(int gid, int iid);
};
