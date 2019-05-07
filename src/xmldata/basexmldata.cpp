/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <mutex>
#include "basexmldata.h"
#include "toolkit.h"
#include <wgui/wgui_fwd.h>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/size.hpp>
#include <QDebug>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace boost;
using namespace WSpace;

typedef mpl::vector<void  ,  
WStringXmlItemData        ,  
WIntXmlItemData           ,  
WUIntXmlItemData          ,  
WFloatXmlItemData         ,  
WListXmlItemData          ,  
WDListXmlItemData         ,  
WHexXmlItemData           ,  
WTimeXmlItemData          ,  
WIPXmlItemData            ,  
WFileXmlItemData          ,  
WDirXmlItemData           ,  
WColorXmlItemData         ,  
WBoolXmlItemData          ,  
WStatusXmlItemData        ,  
WEnumXmlItemData          
		> ItemTypes;
static_assert(mpl::size<ItemTypes>::type::value==IT_NR,"error type size");

template<int index>
struct InitMakeFunc
{
	void operator()(std::array<std::function<std::unique_ptr<WAbstractXmlItemData>()>,IT_NR>* funs) {
		if(index>1) {
			(*funs)[index] = [](){ return make_unique<typename mpl::at_c<ItemTypes,index>::type>();};
			InitMakeFunc<index-1>()(funs);
		}  else if(index==1) {
			(*funs)[index] = [](){ return make_unique<typename mpl::at_c<ItemTypes,index>::type>();};
		}
	}
};
template<>
struct InitMakeFunc<0>
{
	void operator()(std::array<std::function<unique_ptr<WAbstractXmlItemData>()>,IT_NR>* funs) {
		(*funs)[0] = nullptr;
	}
};
template<int index>
struct InitTypeNames
{
	void operator()(std::array<QString,IT_NR>* names) {
		if(index>1) {
			(*names)[index] = mpl::at_c<ItemTypes,index>::type::name;
			InitTypeNames<index-1>()(names);
		} else if(index==1) {
			(*names)[index] = mpl::at_c<ItemTypes,index>::type::name;
		}
	}
};
template<>
struct InitTypeNames<0>
{
	void operator()(std::array<QString,IT_NR>* names) {
		(*names)[0] = "error type";
	}
};

std::array<QString,IT_NR> WAbstractXmlItemData::s_type_name_list;
static once_flag type_name_init_falg;
void init_type_names() {
	InitTypeNames<IT_NR-1>()(&WAbstractXmlItemData::s_type_name_list);
}

WAbstractXmlItemData::WAbstractXmlItemData()
:type_(ItemType::IT_ERROR)
{
	flag_.set(XIF_HAVENT_DEFAULT_VALUE);
}
unique_ptr<WAbstractXmlItemData> WAbstractXmlItemData::createDataByTypeName(const QString& name)
{
	const auto index = typeNameToType(name);
	if(IT_ERROR == index) {
		ERR("ERROR TYPE");
		qDebug()<<name;
		return nullptr;
	}
	return createDataByType(index);
}
unique_ptr<WAbstractXmlItemData> WAbstractXmlItemData::createDataByType(ItemType type)
{
	static once_flag init_flag;
	static std::array<std::function<unique_ptr<WAbstractXmlItemData>()>,IT_NR>  make_funs;

	call_once(init_flag,[](std::array<std::function<unique_ptr<WAbstractXmlItemData>()>,IT_NR>* funs){ InitMakeFunc<IT_NR-1>()(funs);},&make_funs);

	if(wunlikely(type<1 || type>=IT_NR)) {
		LOG(LL_ERROR,"Error Type");
		return unique_ptr<WAbstractXmlItemData>();
	} 
	return make_funs[type]();
}
ItemType WAbstractXmlItemData::typeNameToType(const QString& name)
{
	call_once(type_name_init_falg,init_type_names);
	auto it =  find(s_type_name_list.begin(),s_type_name_list.end(),name);
	if(s_type_name_list.end() == it) return IT_ERROR;
	return static_cast<ItemType>(distance(s_type_name_list.begin(),it));
}
QString WAbstractXmlItemData::typeToTypeName(ItemType type)
{
	call_once(type_name_init_falg,init_type_names);

	if(ItemType::IT_ERROR == type
		|| size_t(type) >= s_type_name_list.size()) {
		ERR("Error type");
		return s_type_name_list.at(0);
	}
	return s_type_name_list.at(unsigned(type));
}
QString WAbstractXmlItemData::typeName()const
{
	return typeToTypeName(type_);
}
void WAbstractXmlItemData::assign(const WAbstractXmlItemData& v)
{
	type_  =  v.type_;
	flag_  =  v.flag_;
}
/******************************************************************************/
void WArithmeticXmlItemData::assign(const WAbstractXmlItemData& v)
{
	WAbstractXmlItemData::assign(v);
	try {
		auto& d = dynamic_cast<const WArithmeticXmlItemData&>(v);

		unit_  =  d.unit_;
	} catch(...) {
	}
}
/******************************************************************************/
int WIntXmlItemData::stringToValue(const QString& str,bool* bok)
{
	return str.toInt(bok);
}
QString WIntXmlItemData::valueToString(const int& v)const
{
	return QString::number(v);
}
void WIntXmlItemData::assign(const WAbstractXmlItemData& v)
{
	try {
		doAssign(dynamic_cast<const WIntXmlItemData&>(v));
	} catch(...) {
	}
}
/******************************************************************************/
unsigned int WUIntXmlItemData::stringToValue(const QString& str,bool* bok)
{
	return str.toUInt(bok);
}
QString WUIntXmlItemData::valueToString(const unsigned int& v)const
{
	return QString::number(v);
}
void WUIntXmlItemData::assign(const WAbstractXmlItemData& v)
{
	try {
		doAssign(dynamic_cast<const WUIntXmlItemData&>(v));
	} catch(...) {
	}
}
/******************************************************************************/
QStringList WStatusXmlItemData::status_pix_file_paths_={imgP("red"),imgP("green"),imgP("black")};
void WStatusXmlItemData::setStatusPixFilePath(const QStringList& paths)
{
	status_pix_file_paths_ = paths;
}
/******************************************************************************/
double WFloatXmlItemData::stringToValue(const QString& str,bool* bok)
{
	return str.toDouble(bok);
}
QString WFloatXmlItemData::valueToString(const double& v)const
{
	return QString("%1").arg(v,0,'f',decimals());
}
void WFloatXmlItemData::assign(const WAbstractXmlItemData& v)
{
	try {
		auto& d = dynamic_cast<const WFloatXmlItemData&>(v);

		doAssign(d);
		decimals_ = d.decimals_;
	} catch(...) {
	}
}
/******************************************************************************/
WBoolXmlItemData::WBoolXmlItemData()
{
	type_ = ItemType::IT_BOOL;
}
bool WBoolXmlItemData::stringToValue(const QString& str,bool* bok)
{
	const auto vstr = str.trimmed().toLower();
	if(nullptr != bok) *bok = true;
	if(vstr == "true") return true;
	if(vstr == "false") return false;
	if(nullptr != bok) *bok = false;
	return false;
}
QString WBoolXmlItemData::valueToString(const bool& v)const
{
	return v?"true":"false";
}
/******************************************************************************/
unsigned int WHexXmlItemData::stringToValue(const QString& str,bool*bok)
{
	QString tstr = str.toLower();
	if(tstr.size()>=2&&tstr.left(2) == "0x") {
		tstr = tstr.right(tstr.size()-2);
	}
	return tstr.toUInt(bok,16);
}
QString WHexXmlItemData::valueToString(const unsigned int& v)const
{
	stringstream ss;
	ss<<"0x"<<hex<<setfill('0')<<setw(8)<<v;
	return ss.str().c_str();
}
/*================================================================================*/
void WDListXmlItemData::setValueList(const QStringList& vl)
{
	value_list = vl;
}
void WDListXmlItemData::assign(const WAbstractXmlItemData& v)
{
	WAbstractXmlItemData::assign(v);
	try {
		setValueList(dynamic_cast<const WDListXmlItemData&>(v).valuelist());
	} catch(...) {} 
}
/*================================================================================*/
void WListXmlItemData::setValueList(const QStringList& vl)
{
	value_list = vl;
	setMin(0);
	setMax(vl.size()-1);
}
WListXmlItemData::number_type WListXmlItemData::stringToValue(const QString& str,bool* bok)
{
	int index = value_list.indexOf(str);
	if(index < 0) {
		index  = str.toInt(bok);
		if(*bok && 0==checkRange(index)) {
			return index;
		}
		*bok = false;
		return 0;
	}
	*bok = true;
	checkRange(index);
	return index;
}
QString WListXmlItemData::valueToString(const WListXmlItemData::number_type& v)const
{
	if(0 != checkRange(v)) {
		ERR("Check range faild,%d",v);
		return QString();
	}
	return value_list.at(v);
}
void WListXmlItemData::assign(const WAbstractXmlItemData& v)
{
	WAbstractXmlItemData::assign(v);
	try {
		setValueList(dynamic_cast<const WListXmlItemData&>(v).valuelist());
	} catch(...){}
}
/******************************************************************************/
void WEnumXmlItemData::setEnumData(const QString& data)
{
	auto value_list = data.split(";;",QString::SkipEmptyParts);
	vector<pair<int,QString>> enum_data;
	bool bok;

	for(auto& d:value_list) {
		auto v_pair = d.split(":",QString::SkipEmptyParts);
		if(v_pair.size()<2) continue;
		auto v = v_pair[0].toInt(&bok);
		if(!bok) {
			v = v_pair[0].toInt(&bok,16);
			if(!bok) continue;
		}
		enum_data.emplace_back(v,v_pair[1]);
	}

	setEnumData(enum_data);
}
void WEnumXmlItemData::setEnumData(const std::vector<pair<int,QString>>& data)
{
	v_to_string_.clear();
	string_to_v_.clear();
	string_list_.clear();

	for(auto& v:data) {
		v_to_string_[v.first] = v.second;
		string_to_v_[v.second] = v.first;
		string_list_<<v.second;
	}
}
WEnumXmlItemData::number_type WEnumXmlItemData::stringToValue(const QString& str,bool* bok)
{
	auto it = string_to_v_.find(str);

	if(it == string_to_v_.end()) {
		*bok = false;
		return 0;
	}
	*bok = true;
	return it->second;
}
QString WEnumXmlItemData::valueToString(const WEnumXmlItemData::number_type& v)const
{
	auto it = v_to_string_.find(v);
	if(it == v_to_string_.end()) {
		return QString();
	}
	return it->second;
}
vector<pair<int,QString>> WEnumXmlItemData::enumData()const
{
	vector<pair<int,QString>> res;

	for(int i=0; i<string_list_.size(); ++i) {
		auto& str = string_list_[i];
		res.emplace_back(string_to_v_.at(str),str);
	}

	return res;
}
void WEnumXmlItemData::assign(const WAbstractXmlItemData& v)
{
	WAbstractXmlItemData::assign(v);
	try {
		auto& d = dynamic_cast<const WEnumXmlItemData&>(v);
		setEnumData(d.enumData());
	}catch(...) {}
}
/*================================================================================*/
QString WStringXmlItemData::defaultValueToString()const
{
	return default_value();
}
void WStringXmlItemData::setToDefaultValue()
{
	value_ = default_value();
}
int WStringXmlItemData::setValueByString(const QString& v) 
{
	return setValue(v);
}
/*================================================================================*/
QString WIPXmlItemData::defaultValueToString()const
{
	return default_value();
}
void WIPXmlItemData::setToDefaultValue()
{
	value_ = default_value();
}
int WIPXmlItemData::setValueByString(const QString& v) 
{
	return setValue(v);
}
bool WIPXmlItemData::isIPAddr(const QString& ip)
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
bool WIPXmlItemData::isLikeIPAddr(const QString& ip)
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
/******************************************************************************/
WFileXmlItemData::WFileXmlItemData()
{
	type_ = ItemType::IT_FILE;
}
/******************************************************************************/
WDirXmlItemData::WDirXmlItemData()
{
	type_ = ItemType::IT_DIR;
}
/*================================================================================*/
const QString WTimeXmlItemData::s_format = "hh:mm:ss";
QString WTimeXmlItemData::defaultValueToString()const
{
	return default_value().toString(s_format);
}
void WTimeXmlItemData::setToDefaultValue()
{
	value_ = default_value();
}
int WTimeXmlItemData::setValueByString(const QString& v) 
{
	return setValue(QTime::fromString(v,s_format));
}
