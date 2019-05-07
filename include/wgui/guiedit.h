/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <type_traits>
#include <boost/lexical_cast.hpp>
#include <toolkit.h>
#include <limits>

#ifdef ENABLE_GIS
enum GPSEditType{
	GE_LONG,
	GE_LAT,
};
class GPSEdit:public QLineEdit
{
	public:
		GPSEdit(int d=0,int f=0, float s=0,GPSEditType type=GE_LONG);
		GPSEdit(double v,GPSEditType type= GE_LONG);
		GPSEdit(const QString& text,GPSEditType type=GE_LONG);
		bool setValue(double v);
		double value()const;
};
#endif //ENABLE_GIS

template <typename NumType>
class WNumValidator:public QValidator
{
	public:
		typedef NumType data_type;
		static_assert(std::is_integral<NumType>::value,"Error type");
		WNumValidator(QObject* parent=nullptr):QValidator(parent){}
		WNumValidator(data_type bottom,data_type top,QObject* parent=nullptr):QValidator(parent),bottom_(bottom),top_(top){}
	private:
		virtual State validate(QString& input,int& pos)const override {
			if(input.isEmpty()) return Intermediate;
			if(input == "-") {
				if(std::is_signed<data_type>::value) {
					if(bottom_<0)
						return Intermediate;
					else
						return Invalid;
				} else {
					input.clear();
					return Invalid;
				}
			}
			try {
				auto v = boost::lexical_cast<data_type>(input.toUtf8().data());
				if(v<bottom_) return Intermediate;
				if(v>top_) return Invalid;
			} catch(...) {
				return Invalid;
			}
			return Acceptable;
		}
		virtual void fixup(QString& input)const override {
			if(input.isEmpty() || input == "-") {
				input = QString::number(bottom_);
				return;
			}
			try {
				auto v = boost::lexical_cast<data_type>(input.toUtf8().data());
				if(v<bottom_) 
					input = QString::number(bottom_);
				else if(v>top_) 
					input = QString::number(top_);
			} catch(...) {
				input = QString::number(bottom_);
			}
		}
	private:
		data_type bottom_ = std::numeric_limits<data_type>::lowest();
		data_type top_    = std::numeric_limits<data_type>::max();
};
extern template class WNumValidator<unsigned>;
using UIntValidator=WNumValidator<unsigned>;

class WHexValidator:public QValidator
{
	public:
		using data_type=uint32_t;
		WHexValidator(QObject* parent=nullptr);
		WHexValidator(data_type bottom,data_type top,QObject* parent=nullptr);
	private:
		virtual State validate(QString& input,int& pos)const override;
		virtual void fixup(QString& input)const override;
	private:
		data_type bottom_ = std::numeric_limits<data_type>::lowest();
		data_type top_    = std::numeric_limits<data_type>::max();
};
template<typename T>
class WNumLineEdit:public QLineEdit
{
	public:
		using value_type = T;
	public:
		WNumLineEdit(T v) {
			setText(QString::number(v));
		}
		using QLineEdit::QLineEdit;
		T value()const {
			if(std::is_integral<T>::value) {
				if(std::is_unsigned<T>::value) {
					return text().toUInt();
				} else {
					return text().toInt();
				}
			} else {
				return text().toDouble();
			}
		}
		void setValue(T v) {
			setText(QString::number(v));
		}
};
using WIntLineEdit = WNumLineEdit<int>;
using WUIntLineEdit = WNumLineEdit<unsigned>;
using WDoubleLineEdit = WNumLineEdit<double>;
class WFileEdit:public QWidget
{
	public:
		WFileEdit(const QString& file_path=QString(),bool open=true);
		inline QString filePath()const { return line_editor_.text(); }
		inline void setFilePath(const QString& path) {
			line_editor_.setText(path);
		}
	private:
		QLineEdit line_editor_;
		bool      is_open_     = true;
	private:
		void setFilePath();
};
class WDirEdit:public QWidget
{
	public:
		WDirEdit(const QString& file_path=QString());
		inline QString filePath()const { return line_editor_.text(); }
		inline void setFilePath(const QString& path) {
			line_editor_.setText(path);
		}
	private:
		QLineEdit line_editor_;
	private:
		void setFilePath();
};
class WColorEdit:public QWidget
{
	public:
		WColorEdit(const QColor& color=Qt::white);
		const QColor& color()const { return color_; }
		void setColor(const QColor& color);
		inline bool isReadOnly()const { return read_only_; }
		inline void setReadOnly(bool ro=true) { read_only_ = ro; }
	private:
		QColor color_;
		bool   read_only_ = false;
	private:
		void paintEvent(QPaintEvent* event)override;
		void mouseDoubleClickEvent(QMouseEvent* event)override;
};
template<typename ValueType,class ValidatorType>
class WNumSpinBox:public QAbstractSpinBox
{
	public:
		using value_type = ValueType;
		using Validator  = ValidatorType;
	public:
		WNumSpinBox(QWidget* parent=nullptr):QAbstractSpinBox(parent){
			setRange(min_,max_);
			connect(this,&QAbstractSpinBox::editingFinished,this,&WNumSpinBox<value_type,Validator>::slotEditingFinished);
		}
		void setMinimum(value_type min) {
			min_ = min;
			setRange(min_,max_);
		}
		void setMaximum(value_type max) {
			max_ = max;
			setRange(min_,max_);
		}
		void setRange(value_type min, value_type max) {
			min_ = min;
			max_ = max;
			lineEdit()->setValidator(new Validator(min_,max_,lineEdit()));
		}
		void setSingleStep(value_type step) {
			step_ = step;
		}
		value_type value()const { return value_; }
		void setValue(value_type v) {
			WSpace::checkRange(&v,min_,max_);
			value_ = v;
			lineEdit()->setText(_valueToText(value()));
		}
		void setSuffix(const QString& str) { suffix_ = str;}
		void setPrefix(const QString& str) { prefix_ = str;}
		inline QString suffix()const { return suffix_; }
		inline QString prefix()const { return prefix_; }
		virtual QString valueToText(value_type value)=0;
		virtual value_type valueFromText(const QString& str,bool* bok)=0;
	private:
		QString    prefix_;
		QString    suffix_;
		value_type min_    = std::numeric_limits<value_type>::lowest();
		value_type max_    = std::numeric_limits<value_type>::max();
		value_type step_   = 1u;
		value_type value_  = std::numeric_limits<value_type>::lowest();
	private:
		void slotEditingFinished() {
			bool bok;
			value_type v = _valueFromText(lineEdit()->text(),&bok);
			if(!bok) return;
			WSpace::checkRange(&v,min_,max_);
			value_ = v;
		}
		virtual void stepBy(int steps)override {
			value_type v = value() + steps*step_;
			WSpace::checkRange(&v,min_,max_);
			value_ = v;
			lineEdit()->setText(_valueToText(value()));
		}
		virtual StepEnabled stepEnabled()const override {
			StepEnabled res;
			if(value_ > min_)
				res |= StepDownEnabled;
			if(value_ < max_)
				res |= StepUpEnabled;
			return res;
		}
		QString _valueToText(value_type value) {
			auto res = valueToText(value);
			return prefix()+res+suffix();
		}
		value_type _valueFromText(const QString& _str,bool* bok=nullptr) {
			auto str = _str.toLower();
			if(str.startsWith(prefix())) {
				str = str.right(str.size()-prefix().size());
			}
			if(str.endsWith(suffix())) {
				str = str.left(str.size()-suffix().size());
			}
			return valueFromText(str,bok);
		}
};
class WUIntSpinBox:public WNumSpinBox<uint32_t,WNumValidator<uint32_t>>
{
	public:
		using WNumSpinBox<value_type,Validator>::WNumSpinBox;
	private:
		virtual QString valueToText(value_type value);
		virtual value_type valueFromText(const QString& str,bool* bok);
};
class WHexSpinBox:public WNumSpinBox<uint32_t,WHexValidator>
{
	public:
		using WNumSpinBox<value_type,Validator>::WNumSpinBox;
	private:
		virtual QString valueToText(value_type value);
		virtual value_type valueFromText(const QString& str,bool* bok);
};
/*
 * 用户输入key,输出value
 * key使用Qt::UserRole储存
 * value使用Qt::EditRole储存
 */
class KVCompleter:public QCompleter
{
	public:
		template<typename T>
		KVCompleter(const T& data,bool value_as_key=true,QObject* parent=nullptr)
		:KVCompleter(parent)
		{
			initData(data,value_as_key);
		}
		KVCompleter(QObject* parent=nullptr);
		virtual QString pathFromIndex(const QModelIndex& index)const override;
		/*
		 * T为map<key,value>
		 * value_as_key:表明value本身也作为key
		 */
		template<class T>
			void initData(const T& data,bool value_as_key=true) {
				const auto row_count = value_as_key?data.size() *2:data.size();
				auto       m         = new QStandardItemModel(row_count,1,this);
				auto       row       = 0;

				/*
				 * map.key作为键，map.value作为值
				 */
				for(auto it = data.begin(); it!=data.end(); ++row,++it) {
					m->setData(m->index(row,0),it->first,Qt::UserRole);
					m->setData(m->index(row,0),it->second,Qt::EditRole);
				}
				/*
				 * map.value作为键，map.value作为值
				 */
				if(value_as_key) {
					for(auto it = data.begin(); it!=data.end();++it) {
						if(it->first.toLower()== it->second.toLower()) continue;
						m->setData(m->index(row,0),it->second,Qt::UserRole);
						m->setData(m->index(row,0),it->second,Qt::EditRole);
						++row;
					}
					m->removeRows(row,row_count-row);
				}

				setModel(m);
			}
};
