/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <xmlviewdelegateimpsk.h>
#include <wkeyboard.h>
#include <xmldata.h>
#include <xmlviewdelegate.h>
#include <wgui.h>
#include <xmlview.h>
namespace WSoftKeyItemDelegate{
QWidget* IntItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	WKeyBoard* keyboard = new WKeyBoard(KBT_INT,QString(),parent);
	keyboard->setMinAndMax(item->ilimit()[0],item->ilimit()[1]);
	return keyboard;
}
void IntItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	auto       v    = item->toInt();
	WKeyBoard *edit = dynamic_cast<WKeyBoard*>(editor);
	edit->setValue(QString::number(v));
}
void IntItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}

	bool       bok;
	QString    str;
	WKeyBoard *edit = dynamic_cast<WKeyBoard*>(editor);

	str = edit->getValueString();
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
	WKeyBoard* keyboard = new WKeyBoard(KBT_INT,QString(),parent);
	keyboard->setMinAndMax(item->ilimit()[0],item->ilimit()[1]);
	return keyboard;
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
	WKeyBoard* keyboard = new WKeyBoard(KBT_FLOAT,QString(),parent);
	if(item->haveMinAndMaxValue())
		keyboard->setMinAndMax(item->flimit()[0],item->flimit()[1]);
	return keyboard;
}
void FloatItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	auto  v    = item->toFloat();
	auto *edit = dynamic_cast<WKeyBoard *>(editor);
	edit->setValue(QString::number(v));
}
void FloatItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	QString str;
	bool    bok;
	auto    edit = dynamic_cast<WKeyBoard *>(editor);
	str = edit->getValueString();
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
	WKeyBoard* keyboard = new WKeyBoard(KBT_IP,QString(),parent);
	return keyboard;
}
void IPItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	auto ip   = item->toIP();
	auto edit = dynamic_cast<WKeyBoard *>(editor);
	edit->setValue(ip);
}
void IPItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	auto edit = dynamic_cast<WKeyBoard *>(editor);
	auto ip   = edit->getValueString();
	item->setItemValue<WIPXmlItemData>(ip);
	model->setData(index,item->toString());
}
/********************************************************************************/
QWidget* HexItemDelegate::createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	auto keyboard = new WKeyBoard(KBT_HEX,QString(),parent);
	keyboard->setMinAndMax(item->ilimit()[0],item->ilimit()[1]);
	return keyboard;
}
void HexItemDelegate::setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	auto          v    = item->toUInt();
	const QString str  = QString("0x%1").arg(v,0,16);
	auto          edit = dynamic_cast<WKeyBoard *>(editor);
	edit->setValue(str);
}
void HexItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	bool    bok;
	QString str;
	auto    edit = dynamic_cast<WKeyBoard *>(editor);

	str = edit->getValueString();

	auto v = str.toUInt(&bok,16);
	if(bok) {
		item->setValue(v);
		model->setData(index,item->toString());
	}
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
	auto v    = item->toString();
	auto edit = dynamic_cast<WKeyBoard *>(editor);
	edit->setValue(v);
}
void StringItemDelegate::setModelData(WXmlItem* item,QWidget* editor,QAbstractItemModel* model,const QModelIndex& index)const
{
	if(index.column() != 1
			|| (item->flag().test(XIF_HAVENT_VALUE))) {
		return ;
	}
	auto edit = dynamic_cast<WKeyBoard *>(editor);
	auto v    = edit->getValueString();
	model->setData(index,v);
	item->setValue(v);
}
} //end namespace
