/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <boost/mpl/vector.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/pop_back.hpp>
#include <boost/mpl/iterator_range.hpp>
#include <boost/type_index.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/sizeof.hpp>
#include <stdexcept>
#include "toolkit.h"
#include "cmacros.h"
#include "wmpl_toolkit.h"

using seq_int_t = int32_t;
template<typename T>
struct is_seq_data:public std::false_type { };

template<typename T,bool is_seq_data>
struct seq_data_element_nr{
	static inline constexpr size_t apply() {
		return WSpace::array_size(*(const T*)0);
	}
};

template<typename ResT,typename DT,bool,bool>
struct wseq_get { };
template<typename T,typename DT,bool/*是否为wseq_data*/,bool/*是否为数组*/>
struct wseq_set { };

/*
 * 处理DT=wseq_data情况
 */
template<typename ResT,typename DT>
struct wseq_get<ResT,DT,true,false>
{
	static inline ResT apply(const DT* v,unsigned i) {
		return v->data.template get<ResT>(i);
	}
};
template<typename T,typename DT>
struct wseq_set<T,DT,true,false>
{
	static inline void apply(DT* v,unsigned i,T d) {
		return v->data.set(i,d);
	}
};
/*
 * 处理DT=内置数据类型的情况
 */
template<typename ResT, typename DT>
struct wseq_get<ResT,DT,false,false> {
	static inline ResT apply(const DT* v,unsigned ) {
		return v->data;
	}
};
template<typename T,typename DT>
struct wseq_set<T,DT,false,false>
{
	static inline void apply(DT* v,unsigned ,T d) {
		v->data = d;
	}
};
/*
 * 处理数组,数组成员不允许为wseq_data
 */
template<typename ResT, typename DT,bool IsSeq>
struct wseq_get<ResT,DT,IsSeq,true>
{
	static inline ResT apply(const DT* v,unsigned i) {
		return v->data[i];
	}
};
template<typename T,typename DT,bool IsSeq>
struct wseq_set<T,DT,IsSeq,true>
{
	static inline void apply(DT* v,unsigned i,T d) {
		v->data[i] = d;
	}
};
/*
 * 将不同类型的数据打包，Types的数量不能超过20个
 * 计算数量时数组按展开后的个数计算
 */
template<typename ...Types>
struct wseq_tuple
{
	static constexpr auto kMaxSize = 20;

	using _types       =  typename WSpace::unpack_array<Types...>::type;
	using types        =  typename WSpace::mpl_resize<_types,char,kMaxSize>::type;
	using type_sizes   =  typename boost::mpl::transform<_types,boost::mpl::sizeof_<boost::mpl::_1>>::type;
	using size_accumu  =  typename WSpace::seq_accumulate_with_init<type_sizes,boost::mpl::int_<0>>::type;
	using size_type    =  typename boost::mpl::back<size_accumu>::type;
	using _indexs      =  typename boost::mpl::pop_back<size_accumu>::type;
	using indexs       =  typename WSpace::mpl_resize<_indexs,boost::mpl::int_<0>,kMaxSize>::type;
	static constexpr size_t byte_size = size_type::value;
	static constexpr size_t data_nr   = boost::mpl::size<_types>::value;

	static_assert(data_nr<=20,"error wseq_tuple data nr.");
	constexpr static size_t size(){ return data_nr; }
	uint8_t data[byte_size];
	template<typename T=seq_int_t>
		inline T get(unsigned i)const 
		{
			if(i>=data_nr) {
				throw std::out_of_range("wseq_tuple.get out of range");
			}
#define GET_CASE_(x) case x: \
			{ \
			using DT = typename boost::mpl::at_c<types,x>::type; \
			using index_type = typename boost::mpl::at_c<indexs,x>::type; \
			auto& src = *reinterpret_cast<const DT*>(data+index_type::value); \
			return T(src);\
			} break

			switch(i) {
				GET_CASE_(0);GET_CASE_(1);GET_CASE_(2);GET_CASE_(3);
				GET_CASE_(4);GET_CASE_(5);GET_CASE_(6);GET_CASE_(7);
				GET_CASE_(8);GET_CASE_(9);GET_CASE_(10);GET_CASE_(11);
				GET_CASE_(12);GET_CASE_(13);GET_CASE_(14);GET_CASE_(15);
				GET_CASE_(16);GET_CASE_(17);GET_CASE_(18);GET_CASE_(19);
			};
			return 0;
#undef GET_CASE_
		}
	template<typename T>
		inline void set(unsigned i,T v){
			if(i>=data_nr) {
				throw std::out_of_range("wseq_tuple.set out of range");
			}
#define SET_CASE_(x) case x: \
			{ \
			using DT = typename boost::mpl::at_c<types,x>::type; \
			using index_type = typename boost::mpl::at_c<indexs,x>::type; \
			auto& dst = *reinterpret_cast<DT*>(data+index_type::value); \
			dst = v; \
			} break

			switch(i) {
				SET_CASE_(0);SET_CASE_(1);SET_CASE_(2);SET_CASE_(3);
				SET_CASE_(4);SET_CASE_(5);SET_CASE_(6);SET_CASE_(7);
				SET_CASE_(8);SET_CASE_(9);SET_CASE_(10);SET_CASE_(11);
				SET_CASE_(12);SET_CASE_(13);SET_CASE_(14);SET_CASE_(15);
				SET_CASE_(16);SET_CASE_(17);SET_CASE_(18);SET_CASE_(19);
			};
#undef SET_CASE_
		}
};
template<unsigned Nr,typename Types>
struct __attribute__((packed)) wseq_data_imp
{
	using self_type    =  wseq_data_imp<Nr,Types>;
	using array_type   =  typename boost::mpl::front<Types>::type;
	using value_type   =  std::remove_extent_t<array_type>;
	using first        =  typename boost::mpl::next<typename boost::mpl::begin<Types>::type>::type;
	using last         =  typename boost::mpl::end<Types>::type;
	using Types1       =  boost::mpl::iterator_range<first,last>;
	using value_type1  =  wseq_data_imp<Nr-1,Types1>;

	array_type  data;
	value_type1 data1;

	static constexpr auto data_nr = seq_data_element_nr<array_type,is_seq_data<array_type>::value>::apply();

	template<typename T>
		inline T get(unsigned i)const 
		{
			using geter        =  wseq_get<T,self_type,is_seq_data<value_type>::value,std::is_array<array_type>::value>;
			if(wunlikely(i<data_nr))
				return geter::apply(this,i);
			return data1.template get<T>(i-data_nr);
		}
	template<typename T>
		inline void set(unsigned i,T v){
			using seter        =  wseq_set<T,self_type,is_seq_data<value_type>::value,std::is_array<array_type>::value>;
			if(wunlikely(i<data_nr))
				seter::apply(this,i,v);
			else
				data1.set(i-data_nr,v);
		}
	template<class Visitor,typename T>
		inline void apply_visitor(Visitor& visitor,unsigned& index)const {
			using geter        =  wseq_get<T,self_type,is_seq_data<value_type>::value,std::is_array<array_type>::value>;
			for(auto i=0; i<data_nr; ++i) {
				visitor(index++,geter::apply(this,i));
			}
			data1.template apply_visitor<Visitor,T>(visitor,index);
		}
	template<class Seter,typename T>
		inline void apply_seter(Seter& set,unsigned& index) {
			using seter        =  wseq_set<T,self_type,is_seq_data<value_type>::value,std::is_array<array_type>::value>;
			for(auto i=0; i<data_nr; ++i) {
				seter::apply(this,i,set(index++));
			}
			data1.template apply_seter<Seter,T>(set,index);
		}

	static inline constexpr size_t size() { return data_nr+value_type1::size();}
};
template<typename Type>
struct __attribute__((packed)) wseq_data_imp<1,Type>
{
	using self_type   =  wseq_data_imp<1,Type>;
	using array_type  =  typename boost::mpl::front<Type>::type;
	using value_type  =  std::remove_extent_t<array_type>;

	array_type data;

	static constexpr auto data_nr = seq_data_element_nr<array_type,is_seq_data<array_type>::value>::apply();

	template<typename T>
		inline T get(unsigned i)const 
		{
			using geter        =  wseq_get<T,self_type,is_seq_data<value_type>::value,std::is_array<array_type>::value>;
			if(wlikely(i<data_nr))
				return geter::apply(this,i);
#ifndef NDEBUG
			throw std::out_of_range("wseq_data_imp.get out of range");
#endif
		}
	template<typename T>
		inline void set(unsigned i,T v){
			using seter        =  wseq_set<T,self_type,is_seq_data<value_type>::value,std::is_array<array_type>::value>;
			if(wlikely(i<data_nr))
				seter::apply(this,i,v);
#ifndef NDEBUG
			else
				throw std::out_of_range("wseq_data_imp.get out of range");
#endif
		}

	template<class Visitor,typename T>
		inline void apply_visitor(Visitor& visitor,unsigned& index)const {
			using geter        =  wseq_get<T,self_type,is_seq_data<value_type>::value,std::is_array<array_type>::value>;
			for(auto i=0; i<data_nr; ++i) {
				visitor(index++,geter::apply(this,i));
			}
		}
	template<class Seter,typename T>
		inline void apply_seter(Seter& set,unsigned& index) {
			using seter        =  wseq_set<T,self_type,is_seq_data<value_type>::value,std::is_array<array_type>::value>;
			for(auto i=0; i<data_nr; ++i) {
				seter::apply(this,i,set(index++));
			}
		}

	static inline constexpr size_t size() { return data_nr;}
};
/*
 * 通过索引访问不同类型组成的数据，Args的数量不能超过20个，但可以迭代使用
 * 如wseq_data<wseq_data<<wseq_data...>,,,>>
 * Args的类型一般使用wseq_tuple打包以加快访问速度
 * 大的数组不打包速度更快
 * 如:wseq_data<wseq_tuple<int,char,float[3],double[2]>,
 * int[100],char[200],
 * wseq_data<int,char,float>>;
 *
 * 对于中等规模的数据(100个左右)
 * 访问最后一个元素大约为直接使用C++的1/17, 访问靠前的元素会更快
 * 批量访问使用wseq_apply_visitor/wseq_apply_seter平均约为直接使用C++的1/5
 */
template<typename ...Args>
struct __attribute__((packed)) wseq_data
{
	using Types       =  boost::mpl::vector<Args...>;
	using value_type  =  wseq_data_imp<boost::mpl::size<Types>::value,Types>;

	value_type data;
	template<typename DT>
		class _wseq_data_element_ref
		{
			public:
				_wseq_data_element_ref(DT& v,int index):v_(v),index_(index){}
				template<typename T>
					operator T()const
					{
						return v_.get<T>(index_);
					}
				template<typename T>
					_wseq_data_element_ref& operator=(T v) {
						v_.set(index_,v);
						return *this;
					}
				inline DT& v() { return v_; }
				inline const DT& v()const { return v_; }
				template<typename T>
					bool operator==(const T rhv)const {
						return v_.get<T>(index_)==rhv;
					}
			private:
				DT& v_;
				int index_;
		};
	using wseq_data_element_ref        =  _wseq_data_element_ref<wseq_data>;
	using wseq_data_element_ref_const  =  _wseq_data_element_ref<const wseq_data>;

	template<typename T=seq_int_t>
		inline T get(int i)const {
			return data.template get<T>(i);
		}
	template<typename T>
		inline void set(int i,T v){
			data.set(i,v);
		}
	wseq_data_element_ref_const operator[](int i)const {
		return wseq_data_element_ref_const(*this,i);
	}
	wseq_data_element_ref operator[](int i){
		return wseq_data_element_ref(*this,i);
	}
	template<typename T=seq_int_t>
		inline T at(int i)const {
			return get(i);
		}

	static inline constexpr size_t size() { return value_type::size();}

	template<class Visitor,typename T=seq_int_t>
		inline void apply_visitor(Visitor& visitor)const {
			unsigned index = 0;
			data.template apply_visitor<Visitor,T>(visitor,index);
		}
	template<class Seter,typename T=seq_int_t>
		inline void apply_seter(Seter& seter) {
			unsigned index = 0;
			data.template apply_seter<Seter,T>(seter,index);
		}
};
template<typename ...Args>
struct is_seq_data<wseq_data<Args...>>:public std::true_type { };
template<typename ...Args>
struct is_seq_data<wseq_tuple<Args...>>:public std::true_type { };
template<typename T>
struct seq_data_element_nr<T,true> {
	static inline constexpr size_t apply() {
		return T::size();
	}
};
/*
 * Visitor:void fun(int i,T v);
 */
	template<class SeqData,class Visitor,typename T=seq_int_t>
void wseq_apply_visitor(const SeqData& data,Visitor visitor)
{
	data.template apply_visitor<Visitor,T>(visitor);
}
/*
 * Seter:T func(int i)
 */
	template<class SeqData,class Seter,typename T=seq_int_t>
void wseq_apply_seter(SeqData& data,Seter seter)
{
	data.template apply_seter<Seter,T>(seter);
}
