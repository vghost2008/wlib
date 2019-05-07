/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QtGui>
#include <vector>
#include <bitset>
#include <stdexcept>
#include "xmlview.h"
#include "xmlwidget_fwd.h"

class WXmlWidget:public QWidget
{
	Q_OBJECT
	public:
		QHBoxLayout *btn_layout_ = nullptr;
	public:
		WXmlWidget(const QStringList& file_list,QWidget* parent=nullptr);
		WXmlWidget(const QString& dir_path,QWidget* parent=nullptr);
		WXmlWidget(QWidget* parent=nullptr);
		WXmlView* view() { return view_; }
		const WXmlView* view()const { return view_; }
	public:
		bool init(const QStringList& file_list);
		QString currentFile()const;
		inline WXmlData* xmlData() {
			return view_->data();
		}
		inline WXmlData* xmlDataAt(int i) {
			if(i<0 || i>= data_list_.size()) return nullptr;
			return &data_list_[i];
		}
		inline WXmlData& xmlDataRefAt(int i)noexcept(false){
			if(i<0 || i>= data_list_.size()) throw std::runtime_error("null data");
			return data_list_[i];
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
		virtual QStringList getNameList();
		virtual QString getFilePathByIndex(int index)const noexcept(false);
		virtual bool init();
		/*
		 * 当文件保存成功后调用，与sigXmlFileSaved作用相同
		 * 先调用xmlFileSaved再发送信号sigXmlFileSaved
		 * @file_path:文件绝对路径
		 */
		virtual void xmlFileSaved(const QString& file_path);
	protected:
		QListWidget           *list_widget_ = nullptr; //聚合
		QStringList            file_list_;
		std::vector<WXmlData>  data_list_;
		QPushButton           *btn_default_ = nullptr;
		QPushButton           *btn_save_    = nullptr;
		QPushButton           *btn_close_   = nullptr;
		std::bitset<32>        flag_        = 0;
	private:
		WXmlView    *view_       = nullptr; //聚合
	private:
		void initGUI();
		bool getXmlData(const QString& file_path,WXmlData* data);
		void flagChanged();
	public:
		void slotSave();
		/*
		 * 选择的配置项改变
		 */
		void slotConfigChanged(int index);
		void slotDefaultValue();
	signals:
		/*
		 * 配置文件保存成功后后发送信号sigXmlFileSaved
		 * @file_path:文件绝对路径
		 */
		void sigXmlFileSaved(const QString& file_path);
		void sigClose();
};
