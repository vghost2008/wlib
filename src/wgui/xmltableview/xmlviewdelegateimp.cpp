/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <xmlviewdelegateimp.h>
#include <xmldata.h>
#include <xmlviewdelegate.h>
#include <wgui.h>
#include <xmlview.h>
namespace WNormalItemDelegate {
WXmlView* AbstractItemDelegate::view()const
{
	return view_delegate_->view_;
}
AbstractItemDelegate::~AbstractItemDelegate()
{
}
bool AbstractItemDelegate::paint(WXmlItem* item,QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return false;
}
/********************************************************************************/
QWidget* IntItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	QLineEdit* edit = new QLineEdit(parent);
	if(item->data()->flag().test(XIF_SET_MAX) && item->data()->flag().test(XIF_SET_MIN))
		edit->setValidator(new QIntValidator(item->ilimit()[0],item->ilimit()[1],edit));
	else
		edit->setValidator(new QIntValidator(edit));
	return edit;
}
void IntItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	auto v = item->toInt();
	QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
	edit->setText(QString::number(v));
}
void IntItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	bool bok;
	QString str;
	QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
	str = edit->text();
	auto v = str.toInt(&bok);
	if(bok) {
		WIntXmlItemData* data = item->getCoreData<WIntXmlItemData>();
		data->setValue(v);
		model->setData(index,item->toString());
	}
}
/********************************************************************************/
QWidget* UIntItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	QLineEdit* edit = new QLineEdit(parent);
	if(item->data()->flag().test(XIF_SET_MAX) && item->data()->flag().test(XIF_SET_MIN))
		edit->setValidator(new UIntValidator(item->ulimit()[0],item->ulimit()[1],edit));
	else
		edit->setValidator(new UIntValidator(edit));
	return edit;
}
void UIntItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	auto v = item->toUInt();
	QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
	edit->setText(QString::number(v));
}
void UIntItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	bool       bok;
	QString    str;
	QLineEdit *edit = qobject_cast<QLineEdit*>(editor);

	str = edit->text();
	auto v = str.toUInt(&bok);
	if(bok) {
		WUIntXmlItemData* data = item->getCoreData<WUIntXmlItemData>();
		data->setValue(v);
		model->setData(index,item->toString());
	}
}
/********************************************************************************/
QWidget* FloatItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	QLineEdit* edit = new QLineEdit(parent);
	if(item->data()->flag().test(XIF_SET_MAX) && item->data()->flag().test(XIF_SET_MIN))
		edit->setValidator(new QDoubleValidator(item->flimit()[0],item->flimit()[1],item->decimals(),edit));
	return edit;
}
void FloatItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
	edit->setText(item->toString());
}
void FloatItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	QString    str;
	bool       bok;
	QLineEdit *edit = qobject_cast<QLineEdit*>(editor);

	str = edit->text();

	auto v = str.toDouble(&bok);

	if(bok) {
		WFloatXmlItemData* data = item->getCoreData<WFloatXmlItemData>();
		if(nullptr==data) return;
		data->setValue(v);
		model->setData(index,item->toString());
	}
}
/********************************************************************************/
QWidget* IPItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	QRegExp rx("[0-2]{0,1}\\d{0,2}(\\.[0-2]{0,1}\\d{1,2}){3}");
	QLineEdit* edit = new QLineEdit(parent);
	edit->setValidator(new QRegExpValidator(rx,edit));
	return edit;
}
void IPItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	auto       ip   = item->toIP();
	QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
	edit->setText(ip);
}
void IPItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	QString    ip;
	QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
	ip = edit->text();
	item->setItemValue<WIPXmlItemData>(ip);
	model->setData(index,item->toString());
}
/********************************************************************************/
QWidget* HexItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	QLineEdit* edit = new QLineEdit(parent);
	QRegExp rx("^(0[xX])?[0-9A-Fa-f]+$");
	edit->setValidator(new QRegExpValidator(rx,edit));
	return edit;
}
void HexItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	auto           v    = item->toUInt();
	const QString &str  = QString("0x%1").arg(v,0,16);
	QLineEdit     *edit = qobject_cast<QLineEdit*>(editor);
	edit->setText(str);
}
void HexItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	bool       bok;
	QString    str;
	QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
	str = edit->text();
	auto v = str.toUInt(&bok,16);
	if(bok) {
		item->setValue(v);
		model->setData(index,item->toString());
	}
}
/********************************************************************************/
QWidget* ListItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	QComboBox* edit = new QComboBox(parent);
	return edit;
}
void ListItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}

	auto       v          = item->toInt();
	auto       value_list = item->getValueList();
	QComboBox *edit       = qobject_cast<QComboBox*>(editor);

	edit->addItems(value_list);
	edit->setCurrentIndex(v);
}
void ListItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	QComboBox* edit = qobject_cast<QComboBox*>(editor);
	auto v = edit->currentIndex();
	item->setValue(v);
	model->setData(index,item->toString());
}
/********************************************************************************/
QWidget* EnumItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	QComboBox* edit = new QComboBox(parent);
	return edit;
}
void EnumItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}

	auto       v          = item->toInt();
	auto       value_list = item->getValueList();
	QComboBox *edit       = qobject_cast<QComboBox*>(editor);

	edit->addItems(value_list);
	edit->setCurrentIndex(v);
}
void EnumItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	QComboBox* edit = qobject_cast<QComboBox*>(editor);
	auto v = edit->currentIndex();
	item->setValue(v);
	model->setData(index,item->toString());
}
/********************************************************************************/
QWidget* BoolItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	auto edit = new QCheckBox(parent);
	return edit;
}
void BoolItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}

	auto v    = item->toBool();
	auto edit = qobject_cast<QCheckBox *>(editor);

	edit->setChecked(v);
}
void BoolItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}

	auto edit  =  qobject_cast<QCheckBox*>(editor);
	auto v     =  edit->isChecked();
	item->setValue(v);
	model->setData(index,item->toString());
}
/********************************************************************************/
QWidget* DListItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	QComboBox* edit = new QComboBox(parent);
	return edit;
}
void DListItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	auto       v          = item->toString();
	auto       value_list = item->getValueList();
	QComboBox *edit       = qobject_cast<QComboBox*>(editor);

	if(!value_list.isEmpty()) {
		edit->addItems(value_list);
	}
	edit->setCurrentIndex(edit->findText(v));
}
void DListItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	QComboBox* edit = qobject_cast<QComboBox*>(editor);
	auto v = edit->currentText();
	item->setValue(v);
	model->setData(index,v);
}
/********************************************************************************/
QWidget* DirItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	static QString old_dir_path     = QDir::homePath();
	const QString  default_dir_path = QFile::exists(item->toString())?item->toString():old_dir_path;
	const QString  dir              = QFileDialog::getExistingDirectory(parent,"请选择文件夹",default_dir_path);

	if(dir.isEmpty()) return nullptr;
	view()->model()->setData(index,dir);
	item->setValue(dir);
	old_dir_path = dir;
	return nullptr;
}
void DirItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
}
void DirItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
}
/******************************************************************************/
QWidget* ColorItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	auto  data = item->getCoreData<WColorXmlItemData>();
	if(nullptr == data) return nullptr;
	auto color = QColorDialog::getColor(data->color());
	data->setColor(color);
	return nullptr;
}
void ColorItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
}
void ColorItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
}
bool ColorItemDelegate::paint(WXmlItem* item,QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	auto  data = item->getCoreData<WColorXmlItemData>();
	if(nullptr == data) return nullptr;
	auto color = data->color();
	painter->save();
	painter->setBrush(color);
	painter->setPen(color);
	painter->drawRect(option.rect);
	painter->restore();
	return true;
}
/********************************************************************************/
QWidget* StringItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	QLineEdit* editor = new QLineEdit(parent);
	return editor;
}
void StringItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	auto       v    = item->toString();
	QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
	edit->setText(v);
}
void StringItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	QString v;
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
	qDebug()<<edit->text();
	v = edit->text();
	model->setData(index,v);
	item->setValue(v);
}
/********************************************************************************/
QWidget* FileItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	static QString old_file_name     = QDir::homePath();
	const QString  default_file_name = QFile::exists(item->toString())?item->toString():old_file_name;
	const QString  file              = QFileDialog::getOpenFileName(parent,"请选择文件",default_file_name);

	if(file.isEmpty()) return nullptr;
	view()->model()->setData(index,file);
	item->setValue(file);
	old_file_name = file;
	return nullptr;
}
void FileItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
}
void FileItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
}
/********************************************************************************/
QWidget* TimeItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	QTimeEdit * editor = new QTimeEdit(parent);
	return editor;
}
void TimeItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	auto time = item->toTime();
	QTimeEdit* edit = qobject_cast<QTimeEdit*>(editor);
	edit->setDisplayFormat(WTimeXmlItemData::s_format);
	edit->setTime(time);
}
void TimeItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	QTimeEdit* edit = qobject_cast<QTimeEdit*>(editor);
	auto time = edit->time();
	item->setItemValue<WTimeXmlItemData>(time);
	model->setData(index,item->toString());
}
} //end namespace
