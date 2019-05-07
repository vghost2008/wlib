/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

_Pragma("once")
#include <QString>
#include <QStringList>
#include <typeinfo>
#include <wmacros.h>
#include <QTime>
#include <bitset>
#include <QColor>
#include <stdint.h>
#include <memory>
#include <array>
#define     XML_DEFAULT_FLOAT_DECIMALS               4
/*
 * XML数据类型
 */
enum ItemType:unsigned char
{
	IT_ERROR   ,  
	IT_STRING  ,  
	IT_INT     ,  
	IT_UINT    ,  
	IT_DOUBLE  ,  
	IT_LIST    ,  
	IT_DLIST   ,  
	IT_HEX     ,  
	IT_TIME    ,  
	IT_IP      ,  
	IT_FILE    ,  
	IT_DIR     ,  
	IT_COLOR   ,  
	IT_BOOL    ,  
	IT_STATUS  ,  
	IT_ENUM    ,  
	IT_NR      ,  
};
/*
 * 数据标志
 */
enum {
      XIF_HAVENT_VALUE          ,  //没有设值
      XIF_SET_MIN               ,  //数据有最小值
      XIF_SET_MAX               ,  //数据有最大值
      XIF_HIDE_ITEM             ,  
      XIF_HAVENT_DEFAULT_VALUE  ,  //无默认值
      XIF_READ_ONLY             ,  //是否为只读
      XIF_USE_TRANS             ,  //对数据使用变换
};
enum {
	XI_MIN   ,   
	XI_MAX   ,   
};
/*
 * 错误码
 */
enum {
	XIE_SUCCESS            =   0,
	XIE_TO_VALUE_FAILD     =   0x00001,
	XIE_GREATER_THAN_MAX   =   0x0002,
	XIE_LITTLE_THAN_MIN    =   0x0004,
};
/*
 * XML基础数据
 */
class WAbstractXmlItemData
{
	public:
		WAbstractXmlItemData();
	public:
		virtual ~WAbstractXmlItemData(){}
	public:
		typedef WAbstractXmlItemData self;
		virtual QString toString()const=0;
		virtual QString defaultValueToString()const=0;
		virtual void setToDefaultValue()=0;
		virtual int setValueByString(const QString& v)=0;
		virtual int setDefaultValueByString(const QString& v)=0;
		inline ItemType type()const { return type_;}
		inline void setType(ItemType type) { type_ = type;}
		static std::unique_ptr<WAbstractXmlItemData> createDataByTypeName(const QString& name);
		static std::unique_ptr<WAbstractXmlItemData> createDataByType(ItemType type);
		QString typeName()const;
		static ItemType typeNameToType(const QString& name);
		static QString typeToTypeName(ItemType type);
		virtual void assign(const WAbstractXmlItemData& v);
		inline const std::bitset<32>& flag()const noexcept { return flag_; }
		inline std::bitset<32>& flag() noexcept { return flag_; }
	public:
		static std::array<QString,IT_NR> s_type_name_list;
	protected:
		ItemType        type_   = IT_ERROR;
		std::bitset<32> flag_   = 0;
		friend class WXmlItem;
		friend class WXmlStream;
		friend class WXmlData;
		friend class WXmlGroup;
	private:
		DISABLE_COPY_AND_ASSIGN(WAbstractXmlItemData);
};
/*
 * 数字类基类
 */
class WArithmeticXmlItemData:public WAbstractXmlItemData
{
	public:
		inline QString unit()const { return unit_; }
		inline void setUnit(const QString& v) { unit_ = v; }
		virtual void assign(const WAbstractXmlItemData& v);
	protected:
		QString         unit_;
};
/*
 * 数字数据
 */
class WXmlItem;
template<class DataType>
class WNumberXmlItemData:public WArithmeticXmlItemData
{
	public:
		typedef WNumberXmlItemData<DataType> self;
		typedef DataType                     value_type;
		typedef DataType                     number_type;
		friend WXmlItem;
	public:
		int setValue(value_type v) {
			checkRange(v);
			value_ = v;
			return 0;
		}
		int setDefaultValue(value_type dv) {
			checkRange(dv);
			default_value_ = dv;
			return 0;
		}
		void setMin(value_type v) {
			limit_[XI_MIN] = v;
			flag().set( XIF_SET_MIN);
		}
		void setMax(value_type v) {
			limit_[XI_MAX] = v;
			flag().set(XIF_SET_MAX);
		}
		virtual int setValueByString(const QString& v)override{
			bool bok = false;
			value_type temp_data = stringToValue(v,&bok);
			if(!bok) return XIE_TO_VALUE_FAILD;
			return setValue(temp_data);
		}
		virtual int setDefaultValueByString(const QString& v)override {
			bool bok = false;
			value_type temp_data = stringToValue(v,&bok);
			if(!bok) return XIE_TO_VALUE_FAILD;
			return setDefaultValue(temp_data);
		}
		int setMinByString(const QString& min) {
			bool bok = false;
			value_type temp_data = stringToValue(min,&bok);
			if(!bok) return XIE_TO_VALUE_FAILD;
			limit_[XI_MIN] = temp_data;
			flag().set(XIF_SET_MIN);
			return 0;
		}
		int setMaxByString(const QString& max) {
			bool bok = false;
			value_type temp_data = stringToValue(max,&bok);
			if(!bok) return XIE_TO_VALUE_FAILD;
			limit_[XI_MAX] = temp_data;
			flag().set(XIF_SET_MAX);
			return 0;
		}
	public:
		inline value_type step()const { return step_; }
		inline void setStep(value_type step) {
			step_ = step;
		}
		inline bool setStepByString(const QString& str) {
			bool bok;
			step_ = stringToValue(str,&bok);
			return bok;
		}
		inline QString stepToString()const {
			return valueToString(step());
		}
	public:
		virtual QString toString()const override{
			return valueToString(value());
		}
		virtual QString defaultValueToString()const override{
			return valueToString(default_value_);
		}
		virtual value_type stringToValue(const QString& str,bool* bok)=0;
		virtual QString valueToString(const value_type& v)const=0;
	public:
		const value_type* limit()const { return limit_;}
		inline value_type value()const { return value_; }
	public:
		virtual void setToDefaultValue()override {
			value_ = default_value_;
		}
		virtual value_type min()const { return limit_[XI_MIN]; }
		virtual value_type max()const { return limit_[XI_MAX]; }
	protected:
		virtual int checkRange(value_type& v)const {
			unsigned ret_val = 0;
			if(flag().test(XIF_SET_MIN)
					&& v < min()) {
				v = min();
				ret_val |= XIE_LITTLE_THAN_MIN;
			}
			if(flag().test(XIF_SET_MAX)
					&& v> max()) {
				v = max();
				ret_val |= XIE_GREATER_THAN_MAX;
			}
			return ret_val;
		}
		virtual int checkRange(const value_type& v)const {
			unsigned ret_val = 0;
			if(flag().test(XIF_SET_MIN)
					&& v < min()) {
				ret_val |= XIE_LITTLE_THAN_MIN;
			}
			if(flag().test(XIF_SET_MAX)
					&& v> max()) {
				ret_val |= XIE_GREATER_THAN_MAX;
			}
			return ret_val;
		}
		friend class WXmlStream;
		inline void doAssign(const WNumberXmlItemData<DataType>& v)
		{
			WArithmeticXmlItemData::assign(v);

			value_             =  v.value_;
			limit_[0]          =  v.limit_[0];
			limit_[1]          =  v.limit_[1];
			default_value_     =  v.default_value_;
			default_limit_[0]  =  v.default_limit_[0];
			default_limit_[1]  =  v.default_limit_[1];
			step_              =  v.step_;
		}
	protected:
		value_type      value_            = {0};
		value_type      limit_[2]         = {0};
		value_type      default_value_    = {0};
		value_type      default_limit_[2] = {0};
		value_type      step_             = {0}; //步进
};
class WIntXmlItemData:public WNumberXmlItemData<int32_t>
{
	public:
		typedef WIntXmlItemData self;
		static constexpr const char* name = "int";
		WIntXmlItemData(){type_ = ItemType::IT_INT;}
		virtual void assign(const WAbstractXmlItemData& data)override;
	private:
		friend class WXmlStream;
		virtual int stringToValue(const QString& str,bool* bok)override;
		virtual QString valueToString(const int& v)const override;
};
class WUIntXmlItemData:public WNumberXmlItemData<uint32_t>
{
	public:
		typedef WUIntXmlItemData self;
		static constexpr const char* name = "uint";
		WUIntXmlItemData(){type_ = ItemType::IT_UINT;}
		virtual void assign(const WAbstractXmlItemData& data)override;
	private:
		friend class WXmlStream;
		virtual unsigned stringToValue(const QString& str,bool* bok)override;
		virtual QString valueToString(const unsigned int& v)const override;
};
class WStatusXmlItemData:public WUIntXmlItemData
{
	public:
		typedef WStatusXmlItemData self;
		static constexpr const char* name = "status";
		WStatusXmlItemData(){type_ = ItemType::IT_STATUS;}
	public:
		static void setStatusPixFilePath(const QStringList& paths);
		inline static const QStringList& statusPaths(){ return status_pix_file_paths_; }
	private:
		static QStringList status_pix_file_paths_;
};
class WFloatXmlItemData:public WNumberXmlItemData<double>
{
	public:
		typedef WFloatXmlItemData self;
		static constexpr const char* name = "double";
		WFloatXmlItemData(){type_ = ItemType::IT_DOUBLE;}
		inline int decimals()const { return decimals_; }
		inline void setDecimals(int decimals) {
			decimals_ = decimals;
		}
		virtual void assign(const WAbstractXmlItemData& data)override;
	private:
		friend class WXmlStream;
		virtual double stringToValue(const QString& str,bool* bok)override;
		virtual QString valueToString(const double& v)const override;
	private:
		int decimals_ = XML_DEFAULT_FLOAT_DECIMALS;
};
class WBoolXmlItemData:public WNumberXmlItemData<bool>
{
	public:
		typedef WBoolXmlItemData self;
		static constexpr const char* name = "bool";
		WBoolXmlItemData();
	private:
		friend class WXmlStream;
		virtual bool stringToValue(const QString& str,bool* bok)override;
		virtual QString valueToString(const bool& v)const override;
};
class WHexXmlItemData:public WUIntXmlItemData
{
	public:
		typedef WHexXmlItemData self;
		static constexpr const char* name = "hex";
		WHexXmlItemData(){type_ = ItemType::IT_HEX;}
	private:
		friend class WXmlStream;
		virtual unsigned int stringToValue(const QString& str,bool* bok)override;
		virtual QString valueToString(const unsigned int& v)const override;
};
class WStringXmlItemData:public WAbstractXmlItemData
{
	public:
		typedef WStringXmlItemData self;
		static constexpr const char* name = "string";
		typedef QString value_type;
	public:
		WStringXmlItemData(){type_ = ItemType::IT_STRING;}
		inline QString value()const { return value_; }
		inline QString default_value()const { return default_value_; }
	public:
		virtual QString toString()const override{
			return value_;
		}
		virtual QString defaultValueToString()const override;
		virtual void setToDefaultValue()override;
		int setValue(const QString& v) {
			value_ = v;
			return 0;
		}
		virtual int setValueByString(const QString& v)override;
		virtual int setDefaultValueByString(const QString& v) override{
			default_value_ = v;
			return 0;
		}
	protected:
		QString value_;
		QString default_value_;
};
class WFileXmlItemData:public WStringXmlItemData
{
	public:
		static constexpr const char* name = "file";
		WFileXmlItemData();
};
class WDirXmlItemData:public WStringXmlItemData
{
	public:
		static constexpr const char* name = "dir";
		WDirXmlItemData();
};
class WColorXmlItemData:public WHexXmlItemData
{
	public:
		WColorXmlItemData(){type_ = ItemType::IT_COLOR; }
		static constexpr const char* name = "color";
		inline QColor value()const { return color(); }
		inline QColor color()const {
			return QColor(WHexXmlItemData::value()>>16,(WHexXmlItemData::value()>>8)&0xFF,WHexXmlItemData::value()&0xFF);
		}
		inline void setColor(const QColor& c) {
			WHexXmlItemData::value_ = (c.red()<<16)|(c.green()<<8)|(c.blue());
		}
};
class WIPXmlItemData:public WAbstractXmlItemData
{
	public:
		typedef WIPXmlItemData self;
		static constexpr const char* name = "ip";
		typedef QString value_type;
	public:
		WIPXmlItemData(){type_ = ItemType::IT_IP;}
		inline QString value()const { return value_; }
		inline QString default_value()const { return default_value_; }
	public:
		virtual QString toString()const override{
			return value_;
		}
		virtual QString defaultValueToString()const override;
		virtual void setToDefaultValue()override;
		int setValue(const value_type& v) {
			value_ = v;
			return 0;
		}
		virtual int setValueByString(const QString& v)override;
		virtual int setDefaultValueByString(const QString& v) override{
			default_value_ = v;
			return 0;
		}
		static bool isIPAddr(const QString& str);
		static bool isLikeIPAddr(const QString& str);
	protected:
		QString value_;
		QString default_value_;
};
class WTimeXmlItemData:public WAbstractXmlItemData
{
	public:
		typedef WTimeXmlItemData self;
		static constexpr const char* name = "time";
		typedef QTime value_type;
		QTime value()const { return value_; }
		QTime default_value()const { return default_value_; }
	public:
		WTimeXmlItemData(){type_ = ItemType::IT_TIME;}
		static const QString s_format;
	public:
		virtual QString toString()const override{
			return value_.toString(s_format);
		}
		virtual QString defaultValueToString()const override;
		virtual void setToDefaultValue()override;
		int setValue(const value_type& v) {
			value_ = v;
			return 0;
		}
		virtual int setValueByString(const QString& v)override;
		virtual int setDefaultValueByString(const QString& v) override{
			default_value_ = QTime::fromString(v,s_format);
			return 0;
		}
	protected:
		QTime value_;
		QTime default_value_;
};
class WListXmlItemData:public WUIntXmlItemData
{
	public:
		typedef WListXmlItemData self;
		static constexpr const char* name = "list";
		WListXmlItemData(){type_ = ItemType::IT_LIST;
			flag().set(XIF_SET_MIN); flag().set(XIF_SET_MAX);
		}
		void setValueList(const QStringList& vl);
		inline QStringList valuelist()const{ return value_list;}
		virtual number_type min()const override final{ return 0;}
		virtual number_type max()const override final{ return value_list.size()-1;}
		virtual void assign(const WAbstractXmlItemData& data)override;
	private:
		QStringList value_list;
	private:
		virtual number_type stringToValue(const QString& str,bool* bok)override;
		virtual QString valueToString(const number_type& v)const override;
};
class WEnumXmlItemData:public WUIntXmlItemData
{
	public:
		typedef WEnumXmlItemData self;
		static constexpr const char* name = "enum";
		WEnumXmlItemData(){type_ = ItemType::IT_ENUM;
		}
		void setEnumData(const QString& vl);
		void setEnumData(const std::vector<std::pair<int,QString>>& data);
		inline QStringList valuelist()const{ return string_list_;}
		virtual void assign(const WAbstractXmlItemData& data)override;
		std::vector<std::pair<int,QString>> enumData()const;
	private:
		std::map<int,QString> v_to_string_;
		std::map<QString,int> string_to_v_;
		QStringList string_list_;
	public:
		virtual number_type stringToValue(const QString& str,bool* bok)override;
		virtual QString valueToString(const number_type& v)const override;
};
class WDListXmlItemData:public WStringXmlItemData
{
	public:
		typedef WListXmlItemData self;
		static constexpr const char* name = "dlist";
		typedef QString value_type;
		WDListXmlItemData(){
			type_ = ItemType::IT_DLIST;
		}
		void setValueList(const QStringList& vl);
		inline QStringList valuelist()const{ return value_list;}
		virtual void assign(const WAbstractXmlItemData& data)override;
	private:
		QStringList value_list;
};
