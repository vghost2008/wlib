/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <guiedit.h>
#include <toolkit.h>
#include <limits>
#include <gis/gis.h>
#include <sstream>

using namespace std;
#ifdef ENABLE_GIS
GPSEdit::GPSEdit(int d, int f,float s,GPSEditType type)
:QLineEdit(QString("%1 %2 %3").arg(d).arg(f).arg(s))
{
	if(GE_LONG == type)
		setInputMask("#000°00'00.000\"");
	else
		setInputMask("#00°00'00.000\"");
}
GPSEdit::GPSEdit(double v, GPSEditType type)
{
	if(GE_LONG == type)
		setInputMask("#000°00'00.000\"");
	else
		setInputMask("#00°00'00.000\"");
	setValue(v);
}
GPSEdit::GPSEdit(const QString& text,GPSEditType type)
:QLineEdit(text)
{
	if(GE_LONG == type)
		setInputMask("#000°00'00.000\"");
	else
		setInputMask("#00°00'00.000\"");
}
bool GPSEdit::setValue(double v)
{
	setText(WSpace::toGPSString(v));
	return true;
}
double GPSEdit::value()const
{
	return WSpace::toGPSValue(text());
}
#endif //ENABLE_GIS
/********************************************************************************/
template class WNumValidator<unsigned>;
/********************************************************************************/
WHexValidator::WHexValidator(QObject* parent)
:QValidator(parent)
{
}
WHexValidator::WHexValidator(data_type bottom,data_type top,QObject* parent)
:QValidator(parent),bottom_(bottom),top_(top)
{
}
QValidator::State WHexValidator::validate(QString& input,int& pos)const  
{
	if(input.isEmpty() || input.toLower() == "0x") return Intermediate;
	bool bok;
	auto v = input.toULong(&bok,16);
	if(v<bottom_) return Intermediate;
	if(!bok || v>top_) {
		return Invalid;
	}
	return Acceptable;
}
void WHexValidator::fixup(QString& input)const
{
	if(input.isEmpty() || input.toLower() == "0x") {
		input = QString("0x%1").arg(bottom_,0,16).toUpper();
		return;
	}

	bool bok;
	auto v = input.toULong(&bok,16);

	if(v<bottom_ || !bok) {
		input = QString("0x%1").arg(bottom_,0,16).toUpper();
	} else if(v>top_) {
		input = QString("0x%1").arg(top_,0,16).toUpper();
	}
}
/********************************************************************************/
WFileEdit::WFileEdit(const QString& file_path,bool is_open)
:line_editor_(file_path)
,is_open_(is_open)
{
	auto layout = new QHBoxLayout(this);
	auto btn    = new QPushButton("...");

	layout->addWidget(&line_editor_);
	layout->addWidget(btn);
	line_editor_.setFocusPolicy(Qt::NoFocus);
	btn->setFixedWidth(fontMetrics().width(btn->text())+10);
	connect(btn,&QPushButton::clicked,this,(void (WFileEdit::*)())&WFileEdit::setFilePath);
}
void WFileEdit::setFilePath()
{
	QString file_path;
	if(is_open_) {
		file_path = QFileDialog::getOpenFileName(this,"选择文件路径",filePath());
	} else {
		file_path = QFileDialog::getSaveFileName(this,"选择文件路径",filePath());
	}
	if(file_path.isEmpty()) return;
	setFilePath(file_path);
}
/******************************************************************************/
WDirEdit::WDirEdit(const QString& file_path)
:line_editor_(file_path)
{
	auto layout = new QHBoxLayout(this);
	auto btn    = new QPushButton("...");

	layout->addWidget(&line_editor_);
	layout->addWidget(btn);
	line_editor_.setFocusPolicy(Qt::NoFocus);
	btn->setFixedWidth(fontMetrics().width(btn->text())+10);

	connect(btn,&QPushButton::clicked,this,(void (WDirEdit::*)())&WDirEdit::setFilePath);
}
void WDirEdit::setFilePath()
{
	auto file_path = QFileDialog::getExistingDirectory(this,"选择文件夹路径",filePath());
	if(file_path.isEmpty()) return;
	setFilePath(file_path);
}
/******************************************************************************/
WColorEdit::WColorEdit(const QColor& color)
:color_(color)
{
	setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
	setMaximumHeight(fontMetrics().height()*1.3);
}
void WColorEdit::paintEvent(QPaintEvent* )
{
	QPainter painter(this);

	painter.setPen(color());
	painter.setBrush(color());
	painter.drawRect(rect());
}
void WColorEdit::setColor(const QColor& color)
{
	color_ = color;
	update();
}
void WColorEdit::mouseDoubleClickEvent(QMouseEvent* event)
{
	if(isReadOnly() || (event->button() != Qt::LeftButton)) return;

	auto c = QColorDialog::getColor(color(),this,"选择颜色");

	if(!c.isValid()) return;
	setColor(c);
}
/******************************************************************************/
QString WUIntSpinBox::valueToText(value_type value)
{
	return QString::number(value);
}
WUIntSpinBox::value_type WUIntSpinBox::valueFromText(const QString& str,bool* bok)
{
	return str.toULong(bok);
}
/******************************************************************************/
QString WHexSpinBox::valueToText(value_type value)
{
	stringstream ss;
	ss<<setfill('0')<<hex<<setw(8)<<value;
	return ss.str().c_str();
}
WHexSpinBox::value_type WHexSpinBox::valueFromText(const QString& str,bool *bok)
{
	return str.toULong(bok,16);
}
/******************************************************************************/
KVCompleter::KVCompleter(QObject* parent)
:QCompleter(parent)
{
	setCompletionRole(Qt::UserRole);
	setCaseSensitivity(Qt::CaseInsensitive);
	setFilterMode(Qt::MatchContains);
}
QString KVCompleter::pathFromIndex(const QModelIndex& index)const
{
	return model()->data(index,Qt::EditRole).toString();
}
