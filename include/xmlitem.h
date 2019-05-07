/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <memory>
#include <stdexcept>
#ifndef Q_MOC_RUN
#include <boost/iterator/iterator_facade.hpp>
#endif
#include <QtAlgorithms>
#include <bitset>
#include <toolkit.h>
#include "wmacros.h"
#include "basexmldata.h"
#include <QTime>
/*
 * xml项数据
 */
class MCSParamCreator;
class DataSimDialog;
class CalibrationTest;
class WXmlItem
{
	public:
		using int_type   =  WIntXmlItemData::value_type;
		using uint_type  =  WUIntXmlItemData::value_type;
		WXmlItem();
		WXmlItem(const WXmlItem&)=delete;
		~WXmlItem();
	public:
		std::unique_ptr<WAbstractXmlItemData>& data(){ return data_; }
		const std::unique_ptr<WAbstractXmlItemData>& data()const { return data_; }
		inline unsigned id()const { return id_; }
		inline void setId(unsigned id) { id_ = id; }
		inline void setDataSize(int ds) { data_size_ = ds; }
		inline int dataSize()const { return data_size_; }
		bool setId(const QString& id);
		inline const QString& name()const { return name_; }
		inline void setName(const QString& name) { name_ = name; }
		inline QString desc()const { return desc_ ; }
		inline void setDesc(const QString& desc) { desc_ = desc; }
		const std::map<QString,QString>&  userData()const { return user_data_; }
		std::map<QString,QString>&  userData(){ return user_data_; }
		inline std::vector<QVariant>&  userDataOld(){ return user_data_old_; }
		const QString userDataAt(const QString& key)const { return user_data_.at(key); }
		int width()const { return width_; }
		void setWidth(int width) { width_ = width; }
		int margin()const { return margin_; }
		void setMargin(int margin ) { margin_ = margin; }
		QString unit()const;
		bool setUnit(const QString& unit);
		const std::bitset<32>& flag()const noexcept(false) {
			if(nullptr == data_) throw std::runtime_error("null core data");
			return data_->flag();
		}
		std::bitset<32>& flag() noexcept(false) {
			if(nullptr == data_) throw std::runtime_error("null core data");
			return data_->flag();
		}
		inline bool isReadOnly()const { return flag().test(XIF_READ_ONLY); }
		inline void setReadOnly(bool on){
			if(on)
				flag().set(XIF_READ_ONLY);
			else
				flag().reset(XIF_READ_ONLY);
		}
	public:
		bool setTransParam(double a,double b);
		inline bool isUseTrans()const { return flag().test(XIF_USE_TRANS); }
		inline double transA()const { return a_; }
		inline double transB()const { return b_; }
		void cleanTransParam();
	public:
		bool init(const QString& type,unsigned id,const QString& name,const QString& value=QString());
		bool init(ItemType type,unsigned id,const QString& name,const QString& value=QString());
		/*
		 * 仅赋值，其它辅助数据不变
		 */
		void assign(const WXmlItem& item);
	public:
		inline ItemType type()const { 
			if(nullptr == data_) return ItemType::IT_ERROR;
			return data_->type();
		}
		inline QString typeName()const {
			if(nullptr == data_) return "nullptr Type";
			return data_->typeName();
		}
		template<typename ItemType>
			inline typename ItemType::value_type getStep()const {
				if(nullptr == data_) {
					ERR("Error, data_==nullptr");
					return typename ItemType::value_type();
				}
				ItemType* data = dynamic_cast<ItemType*>(data_.get());
				if(nullptr == data) {
					ERR("Error,type=%d",int(type()));
					return typename ItemType::value_type();
				}
				return data->step();
			}
		template<typename ValueType>
			inline ValueType step()const {
				if(nullptr == data_) return 0;
				switch(type()) {
					case IT_INT     :  
						return getStep<WIntXmlItemData>();
					case IT_UINT    :  
						return getStep<WUIntXmlItemData>();
					case IT_DOUBLE  :  
						return getStep<WFloatXmlItemData>();
					case IT_LIST    :  
					case IT_DLIST   :  
					case IT_ENUM    :
						return 0;
					case IT_HEX     :  
						return getStep<WHexXmlItemData>();
					default:
						return 0;
				}
			}
		QString stepStr()const;
		template<class ValueType>
			inline bool setStep(const ValueType step) {
				if(nullptr == data_) return false;
				switch(type()) {
					case IT_DOUBLE  :  
						{
							WFloatXmlItemData* data = nullptr;
							if((data = dynamic_cast<WFloatXmlItemData*>(data_.get())) == nullptr) {
								ERR("dynamic_cast faild,type=%d",int(type()));
								return false;
							}
							data->setStep(step);
							return true;
						}
					case IT_INT     :  
						{
							WIntXmlItemData* data = nullptr;
							if((data = dynamic_cast<WIntXmlItemData*>(data_.get())) == nullptr) {
								ERR("dynamic_cast faild,type=%d",int(type()));
								return false;
							}
							data->setStep(step);
							return true;
						}
					case IT_UINT    :  
						{
							WUIntXmlItemData* data = nullptr;
							if((data = dynamic_cast<WUIntXmlItemData*>(data_.get())) == nullptr) {
								ERR("dynamic_cast faild,type=%d",int(type()));
								return false;
							}
							data->setStep(step);
							return true;
						}
					default:
						return false;
						break;
				}
			}
		template<class ItemType>
			inline bool setStepByString(const QString& str) {
				ItemType* data = nullptr;
				if((data = dynamic_cast<ItemType*>(data_.get())) == nullptr) {
					ERR("dynamic_cast faild,type=%d",int(type()));
					return false;
				}
				return data->setStepByString(str);
			}
		template<class ItemType>
			inline bool stepToString()const {
				const ItemType* data = nullptr;
				if((data = dynamic_cast<const ItemType*>(data_.get())) == nullptr) {
					ERR("dynamic_cast faild,type=%d",int(type()));
					return false;
				}
				return data->stepToString();
			}
		template<typename ItemType,typename=typename ItemType::value_type>
			inline typename ItemType::value_type getValue()const {
				const ItemType* data = dynamic_cast<const ItemType*>(data_.get());
				if(nullptr == data) {
					ERR("Error,type=%d",int(type()));
					return typename ItemType::value_type();
				}
				return data->value();
			}
		template<typename DataType,typename=std::enable_if_t<std::is_arithmetic<DataType>::value>>
			inline DataType getValue()const noexcept(false) {
				if(nullptr == data_) throw std::runtime_error("null data");
				switch(data_->type()) {
					case ItemType::IT_INT:
						return toInt();
					case ItemType::IT_HEX:
					case ItemType::IT_UINT:
					case ItemType::IT_LIST:
					case ItemType::IT_STATUS:
					case ItemType::IT_ENUM:
						return toUInt();
					case ItemType::IT_DOUBLE:
						return toDouble();
					case ItemType::IT_BOOL:
						return toBool();
					default:
						throw std::runtime_error("error type");
						break;
				}
			}
		template<class ItemType>
			inline bool setItemValue(const typename ItemType::value_type& v) {
				ItemType* data = nullptr;
				if((data = dynamic_cast<ItemType*>(data_.get())) == nullptr) {
					ERR("dynamic_cast faild,type=%d",int(type()));
					return false;
				}
				return 0==data->setValue(v);
			}
		template<typename DataType>
			inline bool setValue(const DataType& v){
				if(nullptr == data_) return false;
				switch(data_->type()) {
					case ItemType::IT_INT:
						return setItemValue<WIntXmlItemData>(v);
						break;
					case ItemType::IT_HEX:
					case ItemType::IT_UINT:
					case ItemType::IT_LIST:
					case ItemType::IT_ENUM:
						return setItemValue<WUIntXmlItemData>(v);
						break;
					case ItemType::IT_DOUBLE:
						return setItemValue<WFloatXmlItemData>(v);
						break;
					case ItemType::IT_BOOL:
						return setItemValue<WBoolXmlItemData>(v);
						break;
					case ItemType::IT_STATUS:
						return setItemValue<WStatusXmlItemData>(v);
						break;
					default:
						ERR("Error");
						break;
				}
				return false;
			}
		bool haveMinAndMaxValue()const;
		bool setValue(const QString& v);
		template<class ItemType>
			inline ItemType* getCoreData() { return dynamic_cast<ItemType*>(data_.get()); }
		template<class ItemType>
			inline ItemType* getCoreData()const { return dynamic_cast<ItemType*>(data_.get()); }
		template<class ItemType>
			inline ItemType& getCoreDataRef() { 
				auto res = dynamic_cast<ItemType*>(data_.get()); 
				if(nullptr == res)throw std::runtime_error("null data or error type");
				return *res;
			}
		template<class ItemType>
			inline ItemType& getCoreDataRef()const { 
				auto res = dynamic_cast<ItemType*>(data_.get()); 
				if(nullptr == res)throw std::runtime_error("null data or error type");
				return *res;
			}
		template<typename ItemType>
			inline const typename ItemType::value_type* getLimit()const {
				if(nullptr == data_) return 0;
				ItemType* data = dynamic_cast<ItemType*>(data_.get());
				if(nullptr == data) return 0;
				return data->limit();
			}
		int setValueByString(const QString& v);
		QString toString()const;
		std::string toCString()const;
		int_type toInt()const;
		uint_type toUInt()const;
		bool toBool()const;
		double toFloat()const;
		inline double toDouble()const { return toFloat(); }
		int decimals()const;
		void setDecimals(int decimals);
		inline QString toIP()const { return getValue<WIPXmlItemData>(); }
		inline QTime toTime()const { return getValue<WTimeXmlItemData>(); }
		inline const int_type* ilimit()const { return getLimit<WIntXmlItemData>(); }
		inline const double* flimit()const { return getLimit<WFloatXmlItemData>(); }
		inline const uint_type* ulimit()const { return getLimit<WUIntXmlItemData>();}
		QStringList getValueList()const;
		void setToDefaultValue();
		bool initCoreDataByTypeName(const QString& name);
		bool initCoreDataByType(ItemType type);
	public:
		bool is_integral()const;
		bool is_floating_point()const;
		bool is_arithmetic()const;
		inline bool is_signed()const {
			return !is_unsigned();
		}
		bool is_unsigned()const;
	public:
		/*
		 * 获取value()*a_+b_
		 */
		double transedValue()const;
		/*
		 * 使得value() = (v-b_)/a_
		 */
		void setByTransedValue(double v);
	public:
		WXmlItem& operator=(const WXmlItem& v);
	public:
		void dump()const;
	private:
		QString                               name_;
		unsigned                              id_;
		QString                               desc_;
		std::unique_ptr<WAbstractXmlItemData> data_;
		int                                   data_size_;
		int                                   margin_    = 0; //显示时，下一项与自己的距离
		int                                   width_     = 1; //显示时所占用的宽度
		/*
		 * 数据内部值储存为x,外部为y; y=x*a_+b_; a_!=0:通过y = WXmlItem::transedValue(),WXmlItem::setByTransedValue(y)使用 
		 * 如GPS经纬度单位为度，设备上报精度为0.0000001度，则设置a_=1E7,b_=0,trans(1E7),收到数据后通过setByTransedValue(y)使用
		 */
		double                                a_ = 1.0;
		double                                b_ = 0;
		std::map<QString,QString> user_data_;
		std::vector<QVariant>         user_data_old_;
		friend MCSParamCreator;
		friend DataSimDialog;
		friend CalibrationTest;
};
