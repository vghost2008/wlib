/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <boost/mpl/plus.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/max_element.hpp>
#include <boost/mpl/deref.hpp>
#include "toolkit.h"

namespace WSpace
{
	namespace bm=boost::mpl;
	template<int nr,int index,typename accmu,typename IntTypes>
		struct seq_accumulate_imp
		{
			using _accmu = typename bm::push_back<accmu,
				  typename bm::plus<typename bm::back<accmu>::type,typename bm::at_c<IntTypes,index>::type>::type
					  >::type;
			using type = typename seq_accumulate_imp<nr-1,index+1,_accmu,IntTypes>::type;
		};
	template<int index,typename accmu,typename IntTypes>
		struct seq_accumulate_imp<1,index,accmu,IntTypes>
		{
			using type = accmu;
		};
	/*
	 * 将IntTypes进行累加, 使得:
	 *  types[i] = sum_0,i-1(IntTypes)
	 */
	template<typename IntTypes>
		struct seq_accumulate
		{
			using accmu = bm::vector_c<int,bm::front<IntTypes>::type::value>;
			using type  = typename seq_accumulate_imp<bm::size<IntTypes>::value,1,accmu,IntTypes>::type;
		};
	template<typename IntTypes,typename InitType=bm::int_<0>>
		struct seq_accumulate_with_init
		{
			using accmu = typename bm::push_back<bm::vector<InitType>,typename bm::front<IntTypes>::type>::type;
			using type  = typename seq_accumulate_imp<bm::size<IntTypes>::value,1,accmu,IntTypes>::type;
		};




	template<typename Types,typename Type,int needed_size,bool is_ns_pos>
		struct mpl_resize_imp {
			using type = Types;
		};
	template<typename Types,typename Type,int needed_size>
		struct mpl_resize_imp<Types,Type,needed_size,true>
		{
			using _type  =  typename bm::push_back<Types,Type>::type;
			using type   =  typename mpl_resize_imp<_type,Type,needed_size-1,(needed_size-1>0)>::type;
		};
	template<typename Types,typename Type>
		struct mpl_resize_imp<Types,Type,1,true>
		{
			using type = typename bm::push_back<Types,Type>::type;
		};
	/*
	 * 将容器类型Types的数量变为size个，数个Types的数量小于size,在Types后面添加类型Type
	 */
	template<typename Types,typename Type,int size>
		struct mpl_resize
		{
			static constexpr auto needed_size = size-bm::size<Types>::value;
			using type = typename mpl_resize_imp<Types,Type,needed_size,(needed_size>0)>::type;
		};



	template<int nr,typename Init_types,typename Types>
		struct unpack_array_imp
		{
			using front_type = typename bm::front<Types>::type;
			using array_value_type = std::remove_extent_t<front_type>;
			using _type = std::conditional_t<std::is_array<front_type>::value,
				  typename mpl_resize_imp<Init_types,array_value_type,WSpace::array_size(*(const front_type*)0),true>::type,
				  typename bm::push_back<Init_types,front_type>::type>;
			using type = typename unpack_array_imp<nr-1,_type,typename bm::pop_front<Types>::type>::type;
		};
	template<typename Init_types,typename Types>
		struct unpack_array_imp<1,Init_types,Types>
		{
			using front_type = typename bm::front<Types>::type;
			using array_value_type = std::remove_extent_t<front_type>;
			using type = std::conditional_t<std::is_array<front_type>::value,
				  typename mpl_resize_imp<Init_types,array_value_type,WSpace::array_size(*(const front_type*)0),true>::type,
				  typename bm::push_back<Init_types,front_type>::type>;
		};
	template<typename Init_types,typename Types>
		struct unpack_array_imp<0,Init_types,Types>
		{
			using type = Init_types;
		};
	/*
	 * 将数组类型展开
	 * 如int,char[3],short展开为
	 * int,char,char,char,short
	 * 展开后的数组数量不能超过20
	 */
	template<typename ...Args>
		struct unpack_array
		{
			using init_types = bm::vector<>;
			using type = typename unpack_array_imp<sizeof...(Args),init_types,bm::vector<Args...>>::type;
		};
	/*
	 * 定义一个联合，使用类似于数组的方式访问联合，如
	 * using UnionType = array_like_union<
	 * little_uint32_frag_buffer_t<0,1>,
	 * little_uint16_frag_buffer_t<2,4>,
	 * little_uint16_frag_buffer_t<5,9>,
	 * little_uint32_frag_buffer_t<10,16>,
	 * little_uint32_frag_buffer_t<17,27>,
	 * little_uint32_frag_buffer_t<28,31>>;
	 * std::is_array<UnionType>::value == true
	 * WSpace::array_size(....) = 6
	 * UnionType data;
	 * data[0] =1;
	 * cout<<int(data[0])<<endl;
	 */
	template<typename ...Args>
		struct __attribute__((packed)) array_like_union
		{
			typedef bm::vector<Args...> _types;
			typedef typename bm::max_element<
				_types,bm::sizeof_<bm::_1> 
				>::type iter;
			using types = typename mpl_resize<_types,typename bm::at_c<_types,0>::type,20>::type;
			template<typename DT>
				class _array_like_union_element_ref
				{
					public:
						_array_like_union_element_ref(DT& v,int index):v_(v),index_(index){}
						template<typename T>
							operator T()
							{
#define CASE_(x) case x:\
								{\
									using _VT = typename boost::mpl::at_c<types,x>::type; \
									using VT = std::conditional_t<std::is_const<DT>::value,const _VT,_VT>; \
									return T(typename VT::value_type((*reinterpret_cast<VT*>(&v_)))); \
								} break

								switch(index_) {
									CASE_(0);CASE_(1);CASE_(2);CASE_(3);
									CASE_(4);CASE_(5);CASE_(6);CASE_(7);
									CASE_(8);CASE_(9);CASE_(10);CASE_(11);
									CASE_(12);CASE_(13);CASE_(14);CASE_(15);
									CASE_(16);CASE_(17);CASE_(18);CASE_(19);
								};
#undef CASE_
								return 0;
							}
						template<typename T>
							_array_like_union_element_ref& operator=(T v) {
#define CASE_(x) case x:\
								{\
									using VT = typename boost::mpl::at_c<types,x>::type; \
									*reinterpret_cast<VT*>(&v_) = typename VT::value_type(v); \
								} break

								switch(index_) {
									CASE_(0);CASE_(1);CASE_(2);CASE_(3);
									CASE_(4);CASE_(5);CASE_(6);CASE_(7);
									CASE_(8);CASE_(9);CASE_(10);CASE_(11);
									CASE_(12);CASE_(13);CASE_(14);CASE_(15);
									CASE_(16);CASE_(17);CASE_(18);CASE_(19);
								};
#undef CASE_
								return *this;
							}
					private:
						DT& v_;
						int index_;
				};
			using array_like_union_element_ref = _array_like_union_element_ref<array_like_union>;
			using array_like_union_element_ref_const = _array_like_union_element_ref<const array_like_union>;
			array_like_union_element_ref operator[](int i) {
				return array_like_union_element_ref(*this,i);
			}
			array_like_union_element_ref_const operator[](int i)const {
				return array_like_union_element_ref_const(*this,i);
			}
			private:
			uint8_t __data[sizeof(typename bm::deref<iter>::type)];
		};
	template<typename ...Args>
		inline constexpr size_t array_size(const array_like_union<Args...>& )noexcept { return sizeof...(Args); }
		template<int size>
			struct __attribute__((packed)) padding_data
			{
				template<typename T>
					operator T()const { return T(0); }
				uint8_t _[size];
			};
}
namespace std
{
	template<typename ...Args>
		struct is_array<WSpace::array_like_union<Args...>>:public true_type { };
}
