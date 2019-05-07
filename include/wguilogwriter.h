/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <wsingleton.h>
#include <QTableWidget>
#include <QtWidgets>
#include <QItemDelegate>
#include "sfifo.h"
#include "wabstractlogwriter.h"

class WGUILogViewDelegate:public QItemDelegate
{
	typedef QItemDelegate BaseItemDelegate;
	public:
		void paint(QPainter * painter,const QStyleOptionViewItem& option, const QModelIndex& index)const override;
	private:
		static QPixmap cache(const QString& file_name);
};
/*
 * GUI日志写入器widget
 */
class WGUILogWriter;
class WGUILogWriterWidget:public QTableWidget
{
	public:
		WGUILogWriterWidget();
		~WGUILogWriterWidget();
		bool setHeader(const QStringList& title);
	private:
		WGUILogWriter *writer_ = nullptr;
		QMenu         *menu_   = nullptr;
		friend class WGUILogWriter;
	private:
		bool insertLogData(const WLogData& data);
		void contextMenuEvent(QContextMenuEvent* event)override;
	public:
		void slotLogMessage();
		void slotClear();
		void slotMore();
};
/*
 * GUI日志写入器
 */
class WGUILogWriter:public QObject,public WAbstractLogWriter,public WSingleton<WGUILogWriter>
{
	Q_OBJECT
	public:
		virtual bool write(const WLogData& ld)override;
		void setLogWriterWidget(WGUILogWriterWidget* widget);
	private:
		WGUILogWriterWidget  *writer_widget_;
		WSFIFO<WLogData>      log_data_queue_;
		static WGUILogWriter *s_instance;
		friend class WGUILogWriterWidget;
        friend class WSingleton<WGUILogWriter>;
	private:
		WGUILogWriter(WLog* log);
		virtual bool sync()override;
		virtual std::string level(const WLogData& ld)const override;
		virtual std::string time(const WLogData& ld)const override;
	private:
		void slotResetWriterWidget();
	signals:
		void explain();
		void sigLogMessage();
};
