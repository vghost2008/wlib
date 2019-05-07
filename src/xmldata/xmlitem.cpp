/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#include <xmlitem.h>
#include <algorithm>
#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

WXmlItem::WXmlItem()
:data_(nullptr)
,data_size_(-1)
{
}
bool WXmlItem::init(const QString& type,unsigned id,const QString& name,const QString& value)
{
	auto t= WAbstractXmlItemData::typeNameToType(type);
	return init(t,id,name,value);
}
bool WXmlItem::init(ItemType type,unsigned id,const QString& name,const QString& value)
{
	setId(id);
	setName(name);

	if(false == initCoreDataByType(type))
		return false;
	if(!value.isEmpty())
		setValueByString(value);
	if((type == IT_FILE)
		|| (type == IT_DIR)) {
		width_ = 2;
	}
	return true;
}
bool WXmlItem::setValue(const QString& v)
{
	if(nullptr == data_) return false;
	return data_->setValueByString(v);
}
WXmlItem::~WXmlItem()
{
}
void WXmlItem::setToDefaultValue()
{
	if(nullptr == data_) return;
	if(flag().test(XIF_HAVENT_DEFAULT_VALUE)) return;

	data_->setToDefaultValue();
}
bool WXmlItem::initCoreDataByTypeName(const QString& name)
{
	data_ = WAbstractXmlItemData::createDataByTypeName(name);
	return nullptr != data_;
}
bool WXmlItem::initCoreDataByType(ItemType type)
{
	data_ = WAbstractXmlItemData::createDataByType(type);
	return nullptr != data_;
}
void WXmlItem::dump()const
{
	qDebug()<<"-->Dump xml item info, {";
	qDebug()<<"Name:"<<name_;
	qDebug()<<"Desc:"<<desc_;
	qDebug()<<"Id:"<<id_;
	qDebug()<<"Flag:"<<QString("0x%1").arg(flag().to_ulong(),0,16);
	qDebug()<<"Type:"<<typeName();
	qDebug()<<"Value:"<<toString();
	qDebug()<<"}, xml item dump end<--";
}
QStringList WXmlItem::getValueList()const 
{
	if(nullptr == data_) return QStringList();
	switch(type()) {
		case ItemType::IT_LIST:
			{
				WListXmlItemData* data = dynamic_cast<WListXmlItemData*>(data_.get());
				if(nullptr == data) return QStringList();
				return data->valuelist();
			}
			break;
		case ItemType::IT_DLIST:
			{
				WDListXmlItemData* data = dynamic_cast<WDListXmlItemData*>(data_.get());
				if(nullptr == data) return QStringList();
				return data->valuelist();
			}
			break;
		case ItemType::IT_ENUM:
			{
				auto  data = dynamic_cast<WEnumXmlItemData*>(data_.get());
				if(nullptr == data) return QStringList();
				return data->valuelist();
			}
			break;
		default:
			ERR("ERROR type=%d",int(type()));
			break;
	}
	return QStringList();
}
int WXmlItem::decimals()const
{
	switch(type()) {
		case ItemType::IT_DOUBLE:
			{
				auto data = getCoreData<WFloatXmlItemData>();
				if(nullptr != data) return data->decimals();
				return 0;
			}
			break;
		default:
			return 0;
			break;
	}
}
void WXmlItem::setDecimals(int decimals)
{
	if(type() == ItemType::IT_DOUBLE) {
		auto data = getCoreData<WFloatXmlItemData>();
		if(nullptr != data) data->setDecimals(decimals);
	}
}
QString WXmlItem::toString()const 
{
	if(nullptr == data_) return QString();
	return data_->toString();
}
int WXmlItem::setValueByString(const QString& str)
{
	if(nullptr == data_) return -1;
	return data_->setValueByString(str);
}
std::string WXmlItem::toCString()const 
{
	return toString().toUtf8().data();
}
WXmlItem::int_type WXmlItem::toInt()const 
{ 
	switch(type()) {
		case ItemType::IT_INT:
			return getValue<WIntXmlItemData>(); 
		case ItemType::IT_DOUBLE:
			return getValue<WFloatXmlItemData>(); 
		case ItemType::IT_BOOL:
			return getValue<WBoolXmlItemData>(); 
		case ItemType::IT_HEX:
		case ItemType::IT_UINT:
		case ItemType::IT_LIST:
		case ItemType::IT_ENUM:
		default:
			return toUInt();
	}
}
WXmlItem::uint_type WXmlItem::toUInt()const 
{ 
	switch(type()) {
		case ItemType::IT_INT:
			return toInt();
		case ItemType::IT_DOUBLE:
			return getValue<WFloatXmlItemData>(); 
		case ItemType::IT_BOOL:
			return getValue<WBoolXmlItemData>(); 
		case ItemType::IT_HEX:
		case ItemType::IT_UINT:
		case ItemType::IT_LIST:
		case ItemType::IT_ENUM:
		default:
			return getValue<WUIntXmlItemData>(); 
	}
}
bool WXmlItem::toBool()const
{
	switch(type()) {
		case ItemType::IT_INT:
			return toInt()!=0;
			break;
		case ItemType::IT_DOUBLE:
			return fabs(getValue<WFloatXmlItemData>())>0; 
			break;
		case ItemType::IT_BOOL:
			return getValue<WBoolXmlItemData>(); 
			break;
		default:
			return getValue<WUIntXmlItemData>()!=0; 
			break;
	}
}
double WXmlItem::toFloat()const 
{ 
	switch(type()) {
		case ItemType::IT_DOUBLE:
			return getValue<WFloatXmlItemData>(); 
		default:
			if(is_signed())
				return toInt();
			else
				return toUInt();
	}
}
bool WXmlItem::setId(const QString& id) 
{
	bool     bok  = false;
	unsigned temp = id.toUInt(&bok);
	if(bok) id_ = temp;
	return bok;
}
QString WXmlItem::unit()const
{
	auto d = getCoreData<WArithmeticXmlItemData>();
	if(nullptr == d) return QString();
	return d->unit();
}
bool WXmlItem::setUnit(const QString& unit)
{
	auto d = getCoreData<WArithmeticXmlItemData>();
	if(nullptr == d) return false;
	d->setUnit(unit);
	return true;
}
bool WXmlItem::is_integral()const 
{
	switch(type()) {
		case IT_INT     :  
		case IT_UINT    :  
		case IT_HEX     :  
		case IT_LIST    :  
		case IT_ENUM:
			return true;
		default:
		case IT_DLIST   :  
		case IT_DOUBLE  :  
		case IT_TIME    :  
		case IT_IP      :  
		case IT_FILE    :  
		case IT_STRING  :  
		case IT_DIR     :  
		case IT_COLOR   :  
			return false;
	}
}
bool WXmlItem::is_floating_point()const 
{
	switch(type()) {
		case IT_DOUBLE  :  
			return true;
		default:
		case IT_INT     :  
		case IT_UINT    :  
		case IT_LIST    :  
		case IT_ENUM:  
		case IT_DLIST   :  
		case IT_HEX     :  
		case IT_TIME    :  
		case IT_IP      :  
		case IT_FILE    :  
		case IT_STRING  :  
		case IT_DIR     :  
		case IT_COLOR   :  
			return false;
	}
}
bool WXmlItem::is_arithmetic()const 
{
	switch(type()) {
		case IT_DOUBLE  :  
		case IT_INT     :  
		case IT_UINT    :  
		case IT_HEX     :  
		case IT_LIST    :  
		case IT_ENUM    :
			return true;
		default:
		case IT_DLIST   :  
		case IT_TIME    :  
		case IT_IP      :  
		case IT_FILE    :  
		case IT_STRING  :  
		case IT_DIR     :  
		case IT_COLOR   :  
			return false;
	}
}
bool WXmlItem::is_unsigned()const 
{
	switch(type()) {
		case IT_UINT    :  
		case IT_HEX     :  
		case IT_LIST    :  
		case IT_ENUM:
			return true;
		case IT_DOUBLE  :  
		case IT_INT     :  
			return false;
		default:
		case IT_TIME    :  
		case IT_IP      :  
		case IT_FILE    :  
		case IT_STRING  :  
		case IT_DIR     :  
		case IT_COLOR   :  
		case IT_DLIST   :  
			return true;
	}
	return true;
}
bool WXmlItem::haveMinAndMaxValue()const 
{
	return (data_->flag().test(XIF_SET_MIN))
		&&(data_->flag().test(XIF_SET_MAX));
}
QString WXmlItem::stepStr()const 
{
	switch(type()) {
		case IT_INT     :  
			return QString::number(step<int32_t>());
			break;
		case IT_UINT    :  
		case IT_LIST    :  
		case IT_ENUM:  
		case IT_DLIST   :  
		case IT_HEX     :  
			return QString::number(step<uint32_t>());
		case IT_DOUBLE  :  
			return QString::number(step<double>());
		default:
			return QString();
	}
}
void WXmlItem::assign(const WXmlItem& item)
{
	user_data_  =  item.user_data_;
	setValueByString(item.toString());
}
double WXmlItem::transedValue()const
{
	if(nullptr == data_) return 0.0;
	if(!isUseTrans()) return toDouble();
	return toFloat()*transA()+transB();
}
void WXmlItem::setByTransedValue(double v)
{
	constexpr auto kDelta = 1E-12;

	if(nullptr == data_) return ;

	if(!isUseTrans() || (fabs(transA())<kDelta))
		setValue(v);
	else
		setValue((v-transB())/transA());
}
WXmlItem& WXmlItem::operator=(const WXmlItem& v)
{
	setId(v.id());
	setName(v.name());
	initCoreDataByType(v.type());
	flag() = v.flag();

	setReadOnly(v.isReadOnly());
	setUnit(v.unit());
	setMargin(v.margin());
	setWidth(v.width());
	setDesc(v.desc());
	data_size_  =  v.data_size_;
	user_data_  =  v.user_data_;
	margin_     =  v.margin_;
	width_      =  v.width_;
	a_          =  v.a_;
	b_          =  v.b_;

	data_->assign(*v.data_);

	return *this;
}
bool WXmlItem::setTransParam(double a,double b)
{
	const auto delta = 1E-10;

	if(fabs(a)<delta) {
		ERR("Trans param a is too small %lf.",a);
		return false;
	}

	a_ = a;
	b_ = b;
	flag().set(XIF_USE_TRANS);

	return true;
}
void WXmlItem::cleanTransParam()
{
	flag().reset(XIF_USE_TRANS);
}
