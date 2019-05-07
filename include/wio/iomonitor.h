/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <atomic>
#include <wsingleton.h>
#include <sfifo.h>
#include "iomonitor_fwd.h"
#include "wiodevice.h"
struct WIOMonitorConfig
{
	bool            show;
	unsigned        monitor_type;
	WIODataViewMode view_mode;
	QString         title;
	unsigned        read_size;
	unsigned        write_size;
};
struct WIOMonitorUISet
{
	unsigned   id;
	QCheckBox *cb_is_show;
	QComboBox *comb_view_mode;
	QComboBox *comb_monitor_type;
};
class WIOMonitor;
class IOMDataTransThread:public QThread
{
	public:
		IOMDataTransThread(WIOMonitor* iomonitor);
		void stop();
		~IOMDataTransThread();
	private:
		void run()override;
	private:
		friend WIOMonitor;
		WSFIFO<WBuffer>   data_fifo_;
		std::atomic_bool  run_;
		WIOMonitor       *io_monitor_ = nullptr;
};
class WIOMonitor:public QWidget,public WSingleton<WIOMonitor>
{
	private:
		typedef QMutex Mutex;
	public:
		enum UIMask {
			UM_CHECKBOX       =   0x01000000,
			UM_VIEW_MODE      =   0x02000000,
			UM_MONITOR_TYPE   =   0x04000000,
			UM_MASK           =   0x00FFFFFF, //相应的位用于位置索引
		};
		struct IOMonitorItem
		{
			QString  title;
			unsigned id;
		};
	public:
		~WIOMonitor();
		/*
		 * 线程安全
		 */
		static void monitor(const void* data,size_t length,WIODeviceDirection direction,unsigned id,void* private_data);
		/*
		 * 线程安全
		 */
		void monitor_imp(const void* data,unsigned length,WIODeviceDirection direction,unsigned id);
		void getMonitorData();
		void initGUI();
		static bool insertMonitor(const QString& title,unsigned id);
		static void clearMonitor();
		inline void setMaxCacheNum(unsigned num) {
			if(num<4) 
				num = 4;
			else if(num > 1E4)
				num = 1E4;
			max_cache_num_ = num;
		}
		unsigned maxCacheNum()const { return max_cache_num_; }
		QString transData(const WBuffer& data);
		int updateMonitors();
	private:
		static std::list<IOMonitorItem> s_item_list_;
		static Mutex                  s_mutex_;
		QTextEdit                    *context_view_;
		QGridLayout                  *monitor_item_layout_;
		QSignalMapper                *signal_mapper_;
		std::vector<WIOMonitorUISet>  index_to_ui_;
		unsigned                      max_view_size_;
		int                           timer_id_;
		WSFIFO<QString>               data_fifo_;
		IOMDataTransThread            data_trans_thread_;
		QPushButton                  *btn_pause_;
		QPushButton                  *btn_hide_side_;
		QPushButton                  *btn_select_;
		QWidget                      *side_widget_;
		std::atomic_bool              paused_;
        static WIOMonitor*            s_instance;
		unsigned                      max_cache_num_;          //最大缓冲数据包个数，如果data_fifo_中的数据包个数大于max_cache_num_将被强制清除
		/*
		 * id->WIOMonitorConfig
		 */
		std::map<unsigned,WIOMonitorConfig> monitor_config_;
	private:
		const int unit_nr_for_hex_;
		const int unit_nr_for_bin_;
		int       column_for_hex_;
		int       column_for_bin_;
	private:
		void closeEvent(QCloseEvent* e)override;
		void timerEvent(QTimerEvent* e);
		bool insertMonitorM(const QString& title,unsigned id);
		void clearMonitorM();
		void resizeEvent(QResizeEvent* event)override;
		void contextViewSizeChanged();
		void updateUI();
		virtual int minit()override;
		WIOMonitor(QWidget* parent=nullptr);
		friend IOMDataTransThread;
		friend WSingleton<WIOMonitor>;
		inline static WIOMonitor& instance() { return WSingleton<WIOMonitor>::instance(); }
	private:
		void slotUIEvent(int index);
		void slotClear();
		void slotPause();
		void slotHideSide();
		void slotSave();
		void slotSelect();
};
