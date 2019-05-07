/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#include "wkeyboard.h"
#include "toolkit.h"
using namespace WSpace;
WKeyBoard::WKeyBoard(KeyBoardType type,const QString& v,QWidget* parent)
:QWidget(parent)
,limit_{0.0f,0.0f}
,limit_is_set_(false)
,type_(type)
,display_(v)
,first_click_(true)
{
	initGUI();
	setFocusPolicy(Qt::StrongFocus);
	setWindowTitle("输入");
	setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint|Qt::Tool);
	move(0,0);
}
void WKeyBoard::initGUI()
{
	int          i;
	int          current_index      = 0;
	int          key_counter        = 0;
	bool         is_enable[WKEY_NR];
	QFont        tfont              = font();
	QVBoxLayout *layout             = new QVBoxLayout(this);
	QGridLayout *glayout            = new QGridLayout;
	QString btn_text[] = {
		"0"   ,   "1"   ,   "2"       ,   "3"       ,   
		"4"   ,   "5"   ,   "6"       ,   "7"       ,   
		"8"   ,   "9"   ,   "A"       ,   "B"       ,   
		"C"   ,   "D"   ,   "E"       ,   "F"       ,   
		"."   ,   "-"   ,   "Clear"   ,   "Enter"   ,   
	};
	tfont.setPointSize(32);

	static_assert(ARRAY_SIZE(btn_text)==WKEY_NR,"Error button text array size");

	glayout->setSpacing(0);
	for(i=0; i<WKEY_NR; ++i) {
		btn_array_[i].setMinimumSize(72,72);
		btn_array_[i].setText(btn_text[i]);
		btn_array_[i].setFont(tfont);
		btn_array_[i].setFocusPolicy(Qt::NoFocus);
		is_enable[i] = true;
	}
	if(KBT_IP == type_)tfont.setPointSize(24);
	display_.setFont(tfont);
	
	for(i=0; i<WKEY_NR-2; ++i) {
		connect(btn_array_+i,&QPushButton::clicked,this,&WKeyBoard::slotBtnClicked);
		btn_array_[i].setStyleSheet("background-color:rgb(230,230,239)");
	}
	btn_array_[WKEY_OK].setStyleSheet("background-color:rgb(216,176,56)");
	btn_array_[WKEY_CLEAR].setStyleSheet("background-color:rgb(216,176,56)");
	connect(btn_array_+WKEY_CLEAR,&QPushButton::clicked,this,&WKeyBoard::slotClear);
	connect(btn_array_+WKEY_OK,&QPushButton::clicked,this,&WKeyBoard::slotOK);

	switch(type_) {
		case KBT_INT:
			for(i=WKEY_A; i<=WKEY_DOT; ++i) 
				is_enable[i] = false;
			break;
		case KBT_FLOAT:
			for(i=WKEY_A; i<=WKEY_F; ++i) 
				is_enable[i] = false;
			break;
		case KBT_IP:
			for(i=WKEY_A; i<=WKEY_F; ++i) 
				is_enable[i] = false;
			is_enable[WKEY_DEC] = false;
			break;
		case KBT_HEX:
			is_enable[WKEY_DOT] = false;
			is_enable[WKEY_DEC] = false;
			break;
	}

	while(current_index<WKEY_NR-2) {
		if(is_enable[current_index]) {
			glayout->addWidget(btn_array_+current_index,key_counter/4,key_counter%4);
			++key_counter;
		}
		++current_index;
	}

	glayout->addWidget(btn_array_+WKEY_CLEAR,key_counter/4+1,0,1,2);
	glayout->addWidget(btn_array_+WKEY_OK,key_counter/4+1,2,1,2);
	layout->addWidget(&display_);
	layout->addLayout(glayout);
}
void WKeyBoard::slotBtnClicked()
{
	QPushButton *btn       = qobject_cast<QPushButton*>(sender());
	QString      old_value = first_click_?QString():display_.text();

	first_click_ = false;
	if(btn->text() == "-") {
		if(!old_value.isEmpty()) {
			if(old_value.left(1) == "-") {
				old_value = old_value.right(old_value.size()-1);
			} else {
				old_value.prepend("-");
			}
			display_.setText(old_value);
		} else {
			display_.setText("-");
		}
	} else {
		display_.setText(old_value+btn->text());
	}
	switch(type_) {
		case KBT_IP:
			if(!isWillBeIPAddr(display_.text())) {
				display_.setText(old_value);
			}
			break;
		case KBT_FLOAT:
			if(display_.text().count(".") > 1) {
				display_.setText(old_value);
			}
			break;
		default:
			break;
	}

	if(!limit_is_set_) return;
	switch(type_) {
		case KBT_INT:
		{
			int v = display_.text().toInt();
			if(!isInRange<float>(v,limit_)) {
				display_.setStyleSheet("color:red");
			} else {
				display_.setStyleSheet("color:black");
			}
		}
		break;
		case KBT_FLOAT:
		{
			float v = display_.text().toFloat();
			if(!isInRange<float>(v,limit_)) {
				display_.setStyleSheet("color:red");
			} else {
				display_.setStyleSheet("color:black");
			}
		}
		break;
		case KBT_HEX:
		{
			int v = display_.text().toInt(nullptr,16);
			if(!isInRange<float>(v,limit_)) {
				display_.setStyleSheet("color:red");
			} else {
				display_.setStyleSheet("color:black");
			}
		}
		break;
		case KBT_IP:
		return;
		break;
	}
}
void WKeyBoard::slotClear()
{
	display_.setText(QString(""));
}
void WKeyBoard::slotOK()
{
	setVisible(false);
	clearFocus();
}
bool WKeyBoard::isWillBeIPAddr(const QString& ip)
{
	int               i    = 0;
	int               v    = 0;
	bool              bok  = false;
	const QStringList list = ip.split(".");

	if(list.size() > 4) return false;
	for(i=0; i<list.size(); ++i) {
		if(list.at(i).isEmpty())  {
			if(i != list.size()-1) 
				return false;
			else
				return true;
		}
		v = list.at(i).toInt(&bok);
		if(!bok) return false;
		if(v<0||v>255) return false;
	}
	return true;
}
bool WKeyBoard::isIPAddr(const QString& ip)
{
	int               i    = 0;
	int               v    = 0;
	bool              bok  = false;
	const QStringList list = ip.split(".");

	if(list.size() != 4) return false;
	for(i=0; i<list.size(); ++i) {
		if(list.at(i).isEmpty())  return false;
		v = list.at(i).toInt(&bok);
		if(!bok) return false;
		if(v<0||v>255) return false;
	}
	return true;
}
WKeyBoard::~WKeyBoard()
{
}
