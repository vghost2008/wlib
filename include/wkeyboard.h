/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
/*
 * 键盘类型
 */
enum KeyBoardType
{
	KBT_INT     ,   
	KBT_FLOAT   ,   
	KBT_HEX     ,   
	KBT_IP      ,   
};
/*
 * 键值
 */
enum WKey
{
	WKEY_0       ,   
	WKEY_1       ,   
	WKEY_2       ,   
	WKEY_3       ,   
	WKEY_4       ,   
	WKEY_5       ,   
	WKEY_6       ,   
	WKEY_7       ,   
	WKEY_8       ,   
	WKEY_9       ,   
	WKEY_A       ,   
	WKEY_B       ,   
	WKEY_C       ,   
	WKEY_D       ,   
	WKEY_E       ,   
	WKEY_F       ,   
	WKEY_DOT     ,   
	WKEY_DEC     ,   
	WKEY_CLEAR   ,   
	WKEY_OK      ,   
	WKEY_NR      ,   
};
/*
 * 用于输入数字及ip地址的小键盘
 */
class WKeyBoard:public QWidget
{
	public:
		WKeyBoard(KeyBoardType type,const QString& v=QString(),QWidget* parent=nullptr);
		QString getValueString()const {
			if(display_.text().isEmpty()) return org_value_;
			if(KBT_IP == type_ 
					&& !isIPAddr(display_.text()))
				return org_value_;
			return display_.text();
		}
		void setValue(const QString& value) {
			org_value_ = value;
			display_.setText(value);
		}
		void setMinAndMax(float min,float max) {
			if(KBT_IP == type_ ) return;
			limit_[0]       =   min;
			limit_[1]       =   max;
			limit_is_set_   =   true;
			if(limit_[0]>=0) {
				btn_array_[WKEY_DEC].setVisible(false);

			}
		}
		~WKeyBoard();
	private:
		KeyBoardType type_;
		QPushButton  btn_array_[WKEY_NR];
		QLineEdit    display_;
		bool         first_click_;
		QString      org_value_;
		float        limit_[2];
		bool         limit_is_set_;
	private:
		void initGUI();
		/*
		 * 判断一个字符串是否是ip地址
		 */
		static bool isIPAddr(const QString& ip);
		/*
		 * 判断一个字符串是否可能成为ip地址
		 */
		static bool isWillBeIPAddr(const QString& ip);
	private:
		void slotBtnClicked();
		void slotClear();
		void slotOK();
};
