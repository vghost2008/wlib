/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include<toolkit.h>
#include<chrono>
#include<iostream>

/*
 * 用于表示一个有理数，储存count
 * 表示的值为count*num/den
 * Rep既可以是一般的int等内部类型也可以是WSpace::endian_buffer中定义的类型
 */
template<class Rep,int Num=1,int Denom=1>
class __attribute__((packed)) WRational
{
	public:
		using rep        =  Rep;
		using self_type  =  WRational<Rep,Num,Denom>;

		constexpr static auto den = Denom;
		constexpr static auto num = Num;
	public:
		WRational(Rep count=0):count_(count){}
		operator double ()const {
			return double(count())*Num/Denom;
		}
		template<typename T>
			WRational& operator=(T v) {
				count_ = double(v)*Denom/Num;
				return *this;
			}
		WRational& operator=(const self_type& v) {
			count_ = v.count_;
			return *this;
		}
		inline Rep count()const { return count_; }
	private:
		Rep count_;
};
static_assert(sizeof(WRational<int,1,2>)==sizeof(int),"error Rep size");
template<class Rep,int Num,int Denom>
std::ostream& operator<<(std::ostream& os,const WRational<Rep,Num,Denom>& v) {
	os<<double(v);
	return os;
}

/*
 * 带偏移的有理数，偏移大小为OffsetNum/OffsetDenom
 * T可以为int或 WRational
 */
template <class T,int OffsetNum,int OffsetDenom=1>
class ORational
{
	public:
		using value_type =  T;
		constexpr static auto den = OffsetDenom;
		constexpr static auto num = OffsetNum;
	public:
		operator double ()const {
			return v_+double(num)/den;
		}
		template<typename VT,typename=std::enable_if_t<std::is_floating_point<VT>::value,void>>
			ORational& operator=(VT v) {
				v_ = v-double(num)/den;
				return *this;
			}
	private:
		T v_;
};
static_assert(sizeof(ORational<int,1,2>)==sizeof(int),"error Rep size");
template<class T,int Num,int Denom>
std::ostream& operator<<(std::ostream& os,const ORational<T,Num,Denom>& v) {
	os<<double(v);
	return os;
}
