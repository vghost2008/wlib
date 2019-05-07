/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")

#include <QtCore>
#include <QtWidgets>
#include <mutex>
#include "wsingleton.h"
#include "wbuffer.h"
#include "wthreadtools.h"
#include "wioanalysis_fwd.h"
#include <boost/circular_buffer.hpp>
#include <vector>
#include <memory>

class WIOFilter;
class WIOAnalysis:public QWidget,public WSingleton<WIOAnalysis>
{
	public:
	private:
		enum UIMask {
			UM_CHECKBOX       =   0x01000000,
			UM_VIEW_MODE      =   0x02000000,
			UM_MONITOR_TYPE   =   0x04000000,
			UM_MASK           =   0x00FFFFFF, //相应的位用于位置索引
		};
		struct IOAnalysisItem
		{
			QString  title;
			unsigned id;
		};
	public:
		~WIOAnalysis();
		static void monitor(const void* data,size_t length,WIODeviceDirection direction,unsigned id,void* private_data);
		/*
		 * 线程安全
		 */
		void monitor_imp(const void* data,unsigned length,WIODeviceDirection direction,unsigned id)noexcept;
		static bool insertMonitor(const QString& title,unsigned id);
		static void clearMonitor();
		void setMaxCacheNum(unsigned num);
		static void setAlias(const std::vector<std::string>& alias);
	private:
		WIOAnalysis();
		void initGUI();
		void initView(const std::lock_guard<std::mutex>& guard0,const std::lock_guard<std::shared_mutex>& guard);
		void initView();
		void updateAnalysis();
		void slotCurrentDataChanged(); 
		void slotPauseStart();
		void slotConfig();
		void slotUseFilter();
		void resizeEvent(QResizeEvent* event)override;
		void showEvent(QShowEvent* event)override;
		void closeEvent(QCloseEvent* e)override;
		void contextViewSizeChanged();
		void editFilter(const QString& text);
		QStringList historyFilters()const;
		void saveFilter(const QString& filter)const;
		void clear();
		void insertItemToTable(const WIOADataItem& item);
		inline void checkTableData(const std::shared_lock<std::shared_mutex>& ) {
			return __checkTableData();
		}
		inline void checkTableData(const std::lock_guard<std::shared_mutex>& ) {
			return __checkTableData();
		}
		void __checkTableData();
		virtual void contextMenuEvent(QContextMenuEvent* e)override;
		void showCurrentData(const std::shared_lock<std::shared_mutex>& guard0,const std::lock_guard<std::mutex>& guard1);
		void selectionChanged();
	private:
		QComboBox     *cb_filter_       = nullptr;
		QPushButton   *btn_pause_start_ = nullptr;
		QPushButton   *btn_use_filter_  = nullptr;
		QTableWidget  *data_table_      = nullptr;
		QTextEdit     *data_content_    = nullptr;
		QMenu          menu_;
		volatile bool  in_update_view_  = false;   //表明是否在更新表格图，更新表格时不能触发选择消息，否则会死锁
	private:
		static std::vector<std::pair<std::string,std::string>> alias_;
		std::unique_ptr<WIOFilter> filter_;
	private:
		boost::circular_buffer<std::shared_ptr<WIOADataItem>>  data_buffer_;              //接收到的数据，没有经过filter_处理
		boost::circular_buffer<std::shared_ptr<WIOADataItem>>  view_data_buffer_;         //用于在列表中显示的数据
		std::vector<std::shared_ptr<WIOADataItem>>             tmp_data_buffer_;          //还没有在列表中显示
		std::shared_ptr<WIOADataItem>                          current_data_;             //当前显示的数据
		WIOAnalysisItem                                    current_item_;             //与current_data_相对应的Item
		std::mutex                                         mtx_;                      //保护data_buffer_,tmp_data_buffer_
		std::shared_mutex                                  vd_mtx_;                   //保护view_data_buffer_,current_data_
		static std::mutex                                  item_mtx_;                 //保护analysis_items_,alias_,加锁顺序mtx_,vd_mtx_,item_mtx_
		QTimer                                             timer_;
		unsigned                                           unit_nr_for_bin_  = 2;
		unsigned                                           unit_nr_for_hex_  = 8;
		unsigned                                           column_for_hex_   = 4;
		unsigned                                           column_for_bin_   = 4;
		volatile bool                                      need_update_      = false;
		volatile bool                                      is_running_       = true;
		volatile bool                                      use_filter_       = false;
		static WIOAnalysis                                *s_instance;
		static std::map<unsigned,WIOAnalysisItem>          analysis_items_; //id->items
		friend WSingleton<WIOAnalysis>;
};
