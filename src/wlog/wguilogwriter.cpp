/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <wguilogwriter.h>
#include <QMenu>
#include <sstream>
#include <wlogwriter.h>
#include "wlog.h"

using namespace std;

WGUILogWriter* WGUILogWriter::s_instance = nullptr;

WGUILogWriter::WGUILogWriter(WLog* log)
:WAbstractLogWriter(log)
,writer_widget_(nullptr)
{
	mask_ = LWM_GUI;
	qRegisterMetaType<WLogData>("WLogData");
}
void WGUILogWriter::setLogWriterWidget(WGUILogWriterWidget* widget) 
{
	writer_widget_            =   widget;
	writer_widget_->writer_   =   this;
	connect(this,&WGUILogWriter::sigLogMessage,writer_widget_,&WGUILogWriterWidget::slotLogMessage);
	connect(writer_widget_,&WGUILogWriterWidget::destroyed,this,&WGUILogWriter::slotResetWriterWidget);
}
void WGUILogWriter::slotResetWriterWidget()
{
	writer_widget_ = nullptr;
}
bool WGUILogWriter::write(const WLogData& ld)
{
	if(!isCompatible(ld)) return false;
	log_data_queue_.enqueue(ld);
	return true;
}
bool WGUILogWriter::sync()
{
	if(log_data_queue_.empty()) return false;
	emit sigLogMessage();
	return true;
}
string WGUILogWriter::level(const WLogData& ld)const
{
	const static string level_name_array[] = {"未知","错误","警告","信息","调试"};
	if(ld.level<0 || ld.level>=ARRAY_SIZE(level_name_array)) {
		ERR("Error level: %d",ld.level);
		return string();
	}
	return level_name_array[ld.level];
}
string WGUILogWriter::time(const WLogData& ld)const 
{
	ostringstream ss;
	auto          time = ld.time.time_of_day();

	ss<<time.hours()<<':'<<time.minutes()<<':'<<time.seconds();
	return ss.str();
}
/*================================================================================*/
WGUILogWriterWidget::WGUILogWriterWidget()
:writer_(nullptr)
{
	auto m = model();

	menu_ = new QMenu;

	menu_->addAction("清空",this,&WGUILogWriterWidget::slotClear);
	menu_->addAction("更多",this,&WGUILogWriterWidget::slotMore);

	m->insertColumns(0,3);
	setHorizontalHeaderLabels(QStringList()<<"时间"<<"级别"<<"内容");
	horizontalHeader()->setStretchLastSection(true);
	setColumnWidth(0,44+fontMetrics().width("2014-03-01 24:00:00  "));
	setColumnWidth(1,fontMetrics().width("WARNING")+40);
	verticalHeader()->setVisible(false);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setItemDelegate(new WGUILogViewDelegate);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);
}
WGUILogWriterWidget::~WGUILogWriterWidget()
{
	if(nullptr != menu_) { menu_->deleteLater(); menu_ = nullptr; }
}
bool WGUILogWriterWidget::setHeader(const QStringList& header)
{
	if(header.size() != 3) return false;
	setHorizontalHeaderLabels(header);
	return true;
}
void WGUILogWriterWidget::slotClear()
{
	const auto row_count      = model()->rowCount();
	model()->removeRows(0,row_count);
}
void WGUILogWriterWidget::slotMore()
{
	try {
		auto file_path = WLog::instance().getWriter<WFileLogWriter>().filePath();
		QFile file(file_path);
		file.open(QIODevice::ReadOnly);
		auto ba = file.readAll();
		file.close();
		auto edit = new QTextEdit;
		edit->setPlainText(ba);
		edit->resize(800,600);
		edit->setReadOnly(true);
		edit->setAttribute(Qt::WA_DeleteOnClose);
		edit->show();
		QTextCursor cursor = edit->textCursor();
		cursor.movePosition(QTextCursor::End);
		edit->setTextCursor(cursor);
	} catch(...) {
		QMessageBox::warning(this,"Warning","没有找到日志文件");
	}
}
void WGUILogWriterWidget::contextMenuEvent(QContextMenuEvent* event)
{
	menu_->exec(event->globalPos());
}
void WGUILogWriterWidget::slotLogMessage()
{
	if(nullptr == writer_) return;

	std::vector<WLogData>  log_data_list;

	writer_->log_data_queue_.getAllData(&log_data_list);
	if(log_data_list.empty()) return;
	for(auto log_data:log_data_list) {
		insertLogData(log_data);
	}
}
bool WGUILogWriterWidget::insertLogData(const WLogData& ld)
{
	const int           max_row_count  = 10000;
	const int           reduce_row_num = max_row_count/2;
	QAbstractItemModel *m              = model();
	int                 row_count      = m->rowCount();
	QColor background_color(0,0,0);

	static_assert(max_row_count>reduce_row_num,"Error row num");
	if(nullptr == writer_) return false;
	if(row_count>=max_row_count) {
		m->removeRows(0,reduce_row_num);
		row_count = m->rowCount();
	}

	if(!m->insertRow(row_count)) {
		ERR("Insert row faild");
		return false;
	}
	switch(ld.level) {
		case LL_DEBUG:
			background_color = QColor(150,200,255);
			break;
		case LL_INFO:
			background_color = QColor(120,153,255);
			break;
		case LL_WARNING:
			background_color = QColor(255,216,87);
			break;
		case LL_ERROR:
			background_color = QColor(200,0,0);
			break;
		default:
			ERR("Error level=%d",ld.level);
			return false;
	}
	m->setData(m->index(row_count,0),writer_->time(ld).c_str());
	m->setData(m->index(row_count,1),writer_->level(ld).c_str());
	m->setData(m->index(row_count,2),writer_->message(ld).c_str());
	m->setData(m->index(row_count,2),
			QString("文件:%1\n行号:%2\n函数:%3\n").arg(writer_->codeFileName(ld).c_str()).arg(writer_->codeLine(ld).c_str()).arg(writer_->codeFuncName(ld).c_str()),
			Qt::ToolTipRole);

	m->setData(m->index(row_count,0),background_color,Qt::BackgroundColorRole);
	m->setData(m->index(row_count,1),background_color,Qt::BackgroundColorRole);
	m->setData(m->index(row_count,2),background_color,Qt::BackgroundColorRole);
	m->setData(m->index(row_count,0),ld.level,Qt::UserRole);

	scrollToBottom();
	return true;
}
/*================================================================================*/
void WGUILogViewDelegate::paint(QPainter * painter,const QStyleOptionViewItem& option, const QModelIndex& index )const
{
	if(index.column() == 0) {
		QPixmap pix;
		switch(index.data(Qt::UserRole).toInt()) {
			case LL_ERROR:
				pix= cache("error");
				break;
			case LL_WARNING:
				pix= cache("warning");
				break;
			case LL_INFO:
				pix= cache("info");
				break;
			case LL_DEBUG:
				pix= cache("debug");
				break;
			default:
				ERR("Error level");
				return;
				break;
		}
		if(pix.isNull()) {
			ERR("Error pix");
			return;
		}
		painter->drawPixmap(option.rect.left()+2,option.rect.y()+(option.rect.height()-pix.size().height())/2,pix);
		if(!(option.state&QStyle::State_Selected)) {
			painter->setBrush(QColor(index.data(Qt::BackgroundColorRole).value<QColor>()));
			painter->setPen(Qt::NoPen);
			painter->drawRect(option.rect.adjusted(4+pix.width(),1,-1,-1));
		} 
		painter->setPen(QColor(0,0,15));
		painter->drawText(option.rect.left()+4+pix.width(),option.rect.y()+(option.rect.height()+option.fontMetrics.height())/2-option.fontMetrics.descent(),index.data().toString());
	} else {
		return BaseItemDelegate::paint(painter,option,index);
	}
}
QPixmap WGUILogViewDelegate::cache(const QString& file_name)
{
	QPixmap pix;
	if(!QPixmapCache::find(file_name,&pix)) {
		if(!pix.load(QString(":/images/")+file_name+".png")) {
			ERR("Error pix");
			qDebug()<<file_name;
			return QPixmap();
		}
		pix = pix.scaled(40,40,Qt::KeepAspectRatio,Qt::SmoothTransformation);
		QPixmapCache::insert(file_name,pix);
	}
	return pix;
}
