/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <xmlwidgetvdimp.h>
#include <wgui/guiedit.h>
#include <limits>

using namespace std;

AbstractXMLWidgetVDImp::AbstractXMLWidgetVDImp(std::shared_ptr<WXmlItem>& item,int gid)
:item_(item)
,gid_(gid)
{
}
AbstractXMLWidgetVDImp::~AbstractXMLWidgetVDImp()
{
}
void AbstractXMLWidgetVDImp::valueChanged()
{
	if(sig_send_) return;
	sig_send_ = true;
	emit sigValueChanged(gid(),iid());
	sig_send_ = false;
}
/******************************************************************************/
QWidget* StringXMLWidgetVDImp::createWidget(QWidget* parent) 
{
	if(item()->isReadOnly()) 
		editor_.setReadOnly(true);
	else
		connect(&editor_,&QLineEdit::textChanged, this,&AbstractXMLWidgetVDImp::valueChanged);
	return &editor_;
}
void StringXMLWidgetVDImp::setEditorData()
{
	editor_.setText(item()->toString());
}
void StringXMLWidgetVDImp::setModelData() 
{
	item()->setValueByString(editor_.text());
}
/******************************************************************************/
QWidget* IntXMLWidgetVDImp::createWidget(QWidget* parent)
{
	if(item()->data()->flag().test(XIF_SET_MAX )&& item()->data()->flag().test(XIF_SET_MIN))
		editor_.setRange(item()->ilimit()[0],item()->ilimit()[1]);
	else
		editor_.setRange(numeric_limits<int32_t>::lowest(),numeric_limits<int32_t>::max());
	if(item()->step<int>() > 0)
		editor_.setSingleStep(item()->step<int>());
	if(item()->isReadOnly()) 
		editor_.setReadOnly(true);
	else
		connect(&editor_,(void (QSpinBox::*)(int))&QSpinBox::valueChanged,this,&AbstractXMLWidgetVDImp::valueChanged);
	return &editor_;
}
void IntXMLWidgetVDImp::setEditorData() 
{
	editor_.setValue(item()->toInt());
}
void IntXMLWidgetVDImp::setModelData() 
{
	item()->setValue(editor_.value());
}
/******************************************************************************/
QWidget* UIntXMLWidgetVDImp::createWidget(QWidget* parent) 
{
	if(item()->data()->flag().test(XIF_SET_MAX )&& item()->data()->flag().test(XIF_SET_MIN))
		editor_.setRange(item()->ulimit()[0],item()->ulimit()[1]);
	else
		editor_.setRange(numeric_limits<uint32_t>::lowest(),numeric_limits<uint32_t>::max());
	if(item()->step<uint32_t>() > 0)
		editor_.setSingleStep(item()->step<uint32_t>());
	if(item()->isReadOnly()) 
		editor_.setReadOnly(true);
	return &editor_;
}
void UIntXMLWidgetVDImp::setEditorData() 
{
	editor_.setValue(item()->toUInt());
}
void UIntXMLWidgetVDImp::setModelData() 
{
	item()->setValue(editor_.value());
}
/******************************************************************************/
QWidget* HexXMLWidgetVDImp::createWidget(QWidget* parent) 
{
	if(item()->data()->flag().test(XIF_SET_MAX )&& item()->data()->flag().test(XIF_SET_MIN))
		editor_.setRange(item()->ulimit()[0],item()->ulimit()[1]);
	else
		editor_.setRange(numeric_limits<uint32_t>::lowest(),numeric_limits<uint32_t>::max());
	if(item()->step<uint32_t>() > 0)
		editor_.setSingleStep(item()->step<uint32_t>());
	editor_.setPrefix("0X");
	if(item()->isReadOnly()) editor_.setReadOnly(true);
	return &editor_;
}
void HexXMLWidgetVDImp::setEditorData() 
{
	editor_.setValue(item()->toUInt());
}
void HexXMLWidgetVDImp::setModelData() 
{
	item()->setValue(editor_.value());
}
/******************************************************************************/
QWidget* FloatXMLWidgetVDImp::createWidget(QWidget* parent) 
{
	if(item()->data()->flag().test(XIF_SET_MAX ) && item()->data()->flag().test(XIF_SET_MIN)) 
		editor_.setRange(item()->flimit()[0],item()->flimit()[1]);
	 else  
		editor_.setRange(numeric_limits<float>::lowest(),numeric_limits<float>::max());
	editor_.setDecimals(item()->decimals());
	if(item()->step<double>() > 1E-6) {
		editor_.setSingleStep(item()->step<double>());
	}
	if(item()->isReadOnly()) 
		editor_.setReadOnly(true);
	else
		connect(&editor_,(void (QDoubleSpinBox::*)(double))&QDoubleSpinBox::valueChanged, this,&AbstractXMLWidgetVDImp::valueChanged);
	return &editor_;
}
void FloatXMLWidgetVDImp::setEditorData() 
{
	editor_.setValue(item()->toFloat());
}
void FloatXMLWidgetVDImp::setModelData() 
{
	item()->setValue(editor_.value());
}
/******************************************************************************/
QWidget* IPXMLWidgetVDImp::createWidget(QWidget* parent) 
{
	QRegExp rx("[0-2]{0,1}\\d{0,2}(\\.[0-2]{0,1}\\d{1,2}){3}");
	editor_.setValidator(new QRegExpValidator(rx,&editor_));
	if(item()->isReadOnly()) editor_.setReadOnly(true);
	return &editor_;
}
void IPXMLWidgetVDImp::setEditorData() 
{
	editor_.setText(item()->toString());
}
void IPXMLWidgetVDImp::setModelData() 
{
	item()->setValueByString(editor_.text());
}
/******************************************************************************/
QWidget* EnumXMLWidgetVDImp::createWidget(QWidget* parent)
{
	if(!item()->isReadOnly())
		connect(&editor_,(void (QComboBox::*)(int))&QComboBox::currentIndexChanged,this,&AbstractXMLWidgetVDImp::valueChanged);
	return &editor_;
}
void EnumXMLWidgetVDImp::setEditorData()
{
	auto       value_list = item()->getValueList();

	editor_.clear();

	editor_.addItems(value_list);
	editor_.setCurrentText(item()->toString());
}
void EnumXMLWidgetVDImp::setModelData()
{
	item()->setValueByString(editor_.currentText());
}
/******************************************************************************/
QWidget* ListXMLWidgetVDImp::createWidget(QWidget* parent)
{
	if(!item()->isReadOnly())
		connect(&editor_,(void (QComboBox::*)(int))&QComboBox::currentIndexChanged,this,&AbstractXMLWidgetVDImp::valueChanged);
	return &editor_;
}
void ListXMLWidgetVDImp::setEditorData()
{
	auto       v          = item()->toInt();
	auto       value_list = item()->getValueList();

	editor_.clear();

	editor_.addItems(value_list);
	editor_.setCurrentIndex(v);
}
void ListXMLWidgetVDImp::setModelData()
{
	item()->setValue(editor_.currentIndex());
}
/******************************************************************************/
QWidget* DListXMLWidgetVDImp::createWidget(QWidget* parent)
{
	if(!item()->isReadOnly())
		connect(&editor_,(void (QComboBox::*)(int))&QComboBox::currentIndexChanged,this,&AbstractXMLWidgetVDImp::valueChanged);
	return &editor_;
}
void DListXMLWidgetVDImp::setEditorData() 
{
	auto       v          = item()->toString();
	auto       value_list = item()->getValueList();

	editor_.clear();

	if(!value_list.isEmpty()) {
		editor_.addItems(value_list);
	}
	editor_.setCurrentIndex(editor_.findText(v));
}
void DListXMLWidgetVDImp::setModelData() 
{
	item()->setValue(editor_.currentText());
}
/******************************************************************************/
QWidget* DirXMLWidgetVDImp::createWidget(QWidget* parent) 
{
	return &editor_;
}
void DirXMLWidgetVDImp::setEditorData() 
{
	editor_.setFilePath(item()->toString());
}
void DirXMLWidgetVDImp::setModelData() 
{
	item()->setValueByString(editor_.filePath());
}
/******************************************************************************/
QWidget* FileXMLWidgetVDImp::createWidget(QWidget* parent) 
{
	return &editor_;
}
void FileXMLWidgetVDImp::setEditorData() 
{
	editor_.setFilePath(item()->toString());
}
void FileXMLWidgetVDImp::setModelData() 
{
	item()->setValueByString(editor_.filePath());
}
/******************************************************************************/
QWidget* TimeXMLWidgetVDImp::createWidget(QWidget* parent) 
{
	editor_.setDisplayFormat(WTimeXmlItemData::s_format);
	if(item()->isReadOnly()) editor_.setReadOnly(true);
	return &editor_;
}
void TimeXMLWidgetVDImp::setEditorData() 
{
	editor_.setTime(item()->getCoreData<WTimeXmlItemData>()->value());
}
void TimeXMLWidgetVDImp::setModelData() 
{
	item()->getCoreData<WTimeXmlItemData>()->setValue(editor_.time());
}
/******************************************************************************/
QWidget* ColorXMLWidgetVDImp::createWidget(QWidget* parent) 
{
	if(item()->isReadOnly()) editor_.setReadOnly(true);

	return &editor_;
}
void ColorXMLWidgetVDImp::setEditorData() 
{
	editor_.setColor(item()->getCoreData<WColorXmlItemData>()->color());
}
void ColorXMLWidgetVDImp::setModelData() 
{
	item()->getCoreData<WColorXmlItemData>()->setColor(editor_.color());
}
/******************************************************************************/
QWidget* BoolXMLWidgetVDImp::createWidget(QWidget* parent) 
{
	if(!item()->isReadOnly())
		connect(&editor_,&QCheckBox::stateChanged,this,&AbstractXMLWidgetVDImp::valueChanged);
	return &editor_;
}
void BoolXMLWidgetVDImp::setEditorData() 
{
	editor_.setChecked(item()->getCoreData<WBoolXmlItemData>()->value());
}
void BoolXMLWidgetVDImp::setModelData() 
{
	item()->getCoreData<WBoolXmlItemData>()->setValue(editor_.isChecked());
}
/******************************************************************************/
StatusXMLWidgetVDImp::StatusXMLWidgetVDImp(shared_ptr<WXmlItem>& item,int gid)
:AbstractXMLWidgetVDImp(item,gid)
,editor_(WStatusXmlItemData::statusPaths())
{
}
QWidget* StatusXMLWidgetVDImp::createWidget(QWidget* parent) 
{
	return &editor_;
}
void StatusXMLWidgetVDImp::setEditorData() 
{
	editor_.setState(item()->getCoreData<WStatusXmlItemData>()->value());
}
void StatusXMLWidgetVDImp::setModelData() 
{
	item()->getCoreData<WStatusXmlItemData>()->setValue(editor_.state());
}
