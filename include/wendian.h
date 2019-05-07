/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <stdint.h>
#include <toolkit.h>
#include <iostream>
#include <boost/endian/buffers.hpp>
#include <boost/endian/conversion.hpp>

namespace WSpace
{
	template<boost::endian::order Order,class T,std::size_t Nbits,boost::endian::align Align=boost::endian::align::no>
		class __attribute__((packed)) endian_buffer:public boost::endian::endian_buffer<Order,T,Nbits,Align>
	{
		private:
			using base_class=boost::endian::endian_buffer<Order,T,Nbits,Align>;
		public:
			endian_buffer(T v):boost::endian::endian_buffer<Order,T,Nbits,Align>(v){}
			explicit endian_buffer():boost::endian::endian_buffer<Order,T,Nbits,Align>(0){}
			inline operator T ()const {
				return this->value();
			}
			using base_class::operator=;
			template<typename IT,typename=std::enable_if_t<std::is_integral<IT>::value>>
				inline IT operator+(IT rhv)const
				{
					return this->value()+rhv;
				}
			template<typename IT,typename=std::enable_if_t<std::is_integral<IT>::value>>
				inline IT operator-(IT rhv)const
				{
					return this->value()-rhv;
				}
			template<typename IT,typename=std::enable_if_t<std::is_integral<IT>::value>>
				inline IT operator*(IT rhv)const
				{
					return this->value()*rhv;
				}
			template<typename IT,typename=std::enable_if_t<std::is_integral<IT>::value>>
				inline IT operator/(IT rhv)const
				{
					return this->value()/rhv;
				}
			template<typename IT,typename=std::enable_if_t<std::is_integral<IT>::value>>
				inline IT operator+=(IT rhv)
				{
					*this = this->value()+rhv;
					return this->value();

				}
			template<typename IT,typename=std::enable_if_t<std::is_integral<IT>::value>>
				inline IT operator-=(IT rhv)
				{
					*this = this->value()-rhv;
					return this->value();

				}
			template<typename IT,typename=std::enable_if_t<std::is_integral<IT>::value>>
				inline IT operator*=(IT rhv)
				{
					*this = this->value()*rhv;
					return this->value();

				}
			template<typename IT,typename=std::enable_if_t<std::is_integral<IT>::value>>
				inline IT operator/=(IT rhv)
				{
					*this = this->value()/rhv;
					return this->value();

				}
			inline endian_buffer& operator++() 
			{
				*this = this->value()+1;
				return *this;
			}
			inline endian_buffer& operator--() 
			{
				*this = this->value()+1;
				return *this;
			}
			template<typename VT>
				inline endian_buffer& operator&(VT v) 
				{
					*this = this->value()&v;
					return *this;
				}
			template<typename VT>
				inline endian_buffer& operator|(VT v) 
				{
					*this = this->value()|v;
					return *this;
				}
			template<typename VT>
				inline endian_buffer& operator^(VT v) 
				{
					*this = this->value()^v;
					return *this;
				}
	};
	template<boost::endian::order Order,class T>
		class __attribute__((packed)) float_endian_buffer
		{
			static_assert(std::is_floating_point<T>::value,"error type");
			T v_;
		};
	/*
	 * 用于表示一个大端的浮点数
	 * T可以为float或double
	 */
	template<class T>
		class __attribute__((packed)) float_endian_buffer<boost::endian::order::big,T>
		{
			public:
				using InnerType   =  std::conditional_t<std::is_same<T,float>::value,uint32_t,uint64_t>;
				using value_type  =  T;
				static_assert(std::is_floating_point<value_type>::value,"error type");
				template<typename CT,typename=std::enable_if_t<std::is_floating_point<CT>::value,void>>
					operator CT()const {
						const auto iv = reinterpret_cast<const InnerType*>(v_);
						const auto v   = boost::endian::endian_reverse(*iv);

						return *reinterpret_cast<const value_type *>(&v);
					}
				template<typename VT>
					float_endian_buffer& operator=(VT _v) {
						value_type v = _v;
						auto iv = *reinterpret_cast<InnerType*>(&v);
						iv   = boost::endian::endian_reverse(iv);
						memcpy(v_,&iv,sizeof(v_));
						return *this;
					}
			private:
				char v_[sizeof(T)];
		};
	/*
	 * 生成一个mask [begin,end]之间的bits全为1其它为0
	 */
	template<typename T>
		constexpr T make_bits_mask(T begin,T end)
		{
			return begin==end?(1<<begin):((1<<begin)|make_bits_mask(begin+1,end));
		}
	/*
	 * 用于反转字节序
	 */
	template<typename T,int size>
		struct __wendian_reverse{
		};
	/*
	 * 一个字节不用什么操作
	 */
	template<typename T>
		struct __wendian_reverse<T,1>{
			static constexpr T apply(T v) {
				return v&0xFF;
			}
		};
	/*
	 * 反转两个字节
	 */
	template<typename T>
		struct __wendian_reverse<T,2>{
			static constexpr T apply(T v) {
				return (0x00FF&(v>>8))|((v<<8)&0xFF00);
			}
		};
	/*
	 * 反转两个字节
	 */
	template<typename T>
		struct __wendian_reverse<T,3>{
			static constexpr T apply(T v) {
				return (0x0000FF&(v>>16))|((v<<16)&0xFF0000)|(v&0x00FF00);
			}
		};
	/*
	 * 反转四个字节
	 */
	template<typename T>
		struct __wendian_reverse<T,4>{
			static constexpr T apply(T v) {
				return __wendian_reverse<T,2>::apply(v>>16)|(__wendian_reverse<T,2>::apply(v&0xFFFF)<<16);
			}
		};
	template<typename T>
		constexpr T wendian_reverse(T v) {
			return __wendian_reverse<T,sizeof(T)>::apply(v);
		}
	template<boost::endian::order Order,typename T,int begin,int end>
		struct frag_endian_buffer {
		};
	/*
	 * 大端时，begin,end指的是数据反转后在小端模式下的字节位置
	 * 因为大端字节位置可能不连续
	 * 如在大端cpu中定义结构
	 * struct T
	 * {
	 * uint16_t version:2;
	 * uint16_t id:8;
	 * uint16_t channel:6;
	 * };
	 * 那么第一字节的[6:7]为version, [0:5]为id高6位
	 * 第二字节的[6,7]为id低2位　[0:5]为channel
	 * 在小端CPU通过以下union表示上述结构
	 * union {
	 * big_uint16_frag_buffer_t<0,5>  channel;
	 * big_uint16_frag_buffer_t<6,13>  id;
	 * big_uint16_frag_buffer_t<14,15>  version;
	 * };
	 */
	template<typename T,int begin,int end>
		struct __attribute__((packed)) frag_endian_buffer<boost::endian::order::big,T,begin,end> {
			constexpr static int kBegin        = begin;
			constexpr static int kEnd          = end;
			constexpr static T   kMask         = make_bits_mask(kBegin,kEnd);
			constexpr static T   kReversedMask = wendian_reverse(kMask);
			using value_type=T;
			using self_type = frag_endian_buffer<boost::endian::order::big,T,begin,end>;

			template<typename TT>
				operator TT()const {
					return (boost::endian::endian_reverse(v_)&kMask)>>kBegin;
				}
			template<typename TT>
				self_type& operator=(TT _v) {
					T v = _v;
					v = kMask&(v<<kBegin);
					auto rv = boost::endian::endian_reverse(v);
					v_ = (v_&(~kReversedMask))|rv;
					return *this;
				}
			T v_;
		};
	template<typename T,int begin,int end>
		struct __attribute__((packed)) frag_endian_buffer<boost::endian::order::little,T,begin,end> {
			constexpr static int kBegin = begin;
			constexpr static int kEnd   = end;
			constexpr static T   kMask  = make_bits_mask(kBegin,kEnd);
			using value_type=T;
			using self_type = frag_endian_buffer<boost::endian::order::little,T,begin,end>;

			template<typename TT>
				operator TT()const {
					return (v_&kMask)>>kBegin;
				}
			template<typename TT>
				self_type& operator=(TT v) {
					v = (v<<kBegin)&kMask;
					v_ = (v_&(~kMask))|v;
					return *this;
				}
			T v_;
		};

	using order=boost::endian::order;
	using align=boost::endian::align;
	// unaligned big endian signed integer buffers
	typedef endian_buffer<order::big, int_least8_t, 8>        big_int8_buf_t;
	typedef endian_buffer<order::big, int_least16_t, 16>      big_int16_buf_t;
	typedef endian_buffer<order::big, int_least32_t, 24>      big_int24_buf_t;
	typedef endian_buffer<order::big, int_least32_t, 32>      big_int32_buf_t;
	typedef endian_buffer<order::big, int_least64_t, 40>      big_int40_buf_t;
	typedef endian_buffer<order::big, int_least64_t, 48>      big_int48_buf_t;
	typedef endian_buffer<order::big, int_least64_t, 56>      big_int56_buf_t;
	typedef endian_buffer<order::big, int_least64_t, 64>      big_int64_buf_t;

	// unaligned big endian unsigned integer buffers
	typedef endian_buffer<order::big, uint_least8_t, 8>       big_uint8_buf_t;
	typedef endian_buffer<order::big, uint_least16_t, 16>     big_uint16_buf_t;
	typedef endian_buffer<order::big, uint_least32_t, 24>     big_uint24_buf_t;
	typedef endian_buffer<order::big, uint_least32_t, 32>     big_uint32_buf_t;
	typedef endian_buffer<order::big, uint_least64_t, 40>     big_uint40_buf_t;
	typedef endian_buffer<order::big, uint_least64_t, 48>     big_uint48_buf_t;
	typedef endian_buffer<order::big, uint_least64_t, 56>     big_uint56_buf_t;
	typedef endian_buffer<order::big, uint_least64_t, 64>     big_uint64_buf_t;

	// unaligned little endian signed integer buffers
	typedef endian_buffer<order::little, int_least8_t, 8>     little_int8_buf_t;
	typedef endian_buffer<order::little, int_least16_t, 16>   little_int16_buf_t;
	typedef endian_buffer<order::little, int_least32_t, 24>   little_int24_buf_t;
	typedef endian_buffer<order::little, int_least32_t, 32>   little_int32_buf_t;
	typedef endian_buffer<order::little, int_least64_t, 40>   little_int40_buf_t;
	typedef endian_buffer<order::little, int_least64_t, 48>   little_int48_buf_t;
	typedef endian_buffer<order::little, int_least64_t, 56>   little_int56_buf_t;
	typedef endian_buffer<order::little, int_least64_t, 64>   little_int64_buf_t;

	// unaligned little endian unsigned integer buffers
	typedef endian_buffer<order::little, uint_least8_t, 8>    little_uint8_buf_t;
	typedef endian_buffer<order::little, uint_least16_t, 16>  little_uint16_buf_t;
	typedef endian_buffer<order::little, uint_least32_t, 24>  little_uint24_buf_t;
	typedef endian_buffer<order::little, uint_least32_t, 32>  little_uint32_buf_t;
	typedef endian_buffer<order::little, uint_least64_t, 40>  little_uint40_buf_t;
	typedef endian_buffer<order::little, uint_least64_t, 48>  little_uint48_buf_t;
	typedef endian_buffer<order::little, uint_least64_t, 56>  little_uint56_buf_t;
	typedef endian_buffer<order::little, uint_least64_t, 64>  little_uint64_buf_t;

	// aligned big endian signed integer buffers
	typedef endian_buffer<order::big, int8_t, 8, align::yes>       big_int8_buf_at;
	typedef endian_buffer<order::big, int16_t, 16, align::yes>     big_int16_buf_at;
	typedef endian_buffer<order::big, int32_t, 32, align::yes>     big_int32_buf_at;
	typedef endian_buffer<order::big, int64_t, 64, align::yes>     big_int64_buf_at;

	// aligned big endian unsigned integer buffers
	typedef endian_buffer<order::big, uint8_t, 8, align::yes>      big_uint8_buf_at;
	typedef endian_buffer<order::big, uint16_t, 16, align::yes>    big_uint16_buf_at;
	typedef endian_buffer<order::big, uint32_t, 32, align::yes>    big_uint32_buf_at;
	typedef endian_buffer<order::big, uint64_t, 64, align::yes>    big_uint64_buf_at;

	// aligned little endian signed integer buffers
	typedef endian_buffer<order::little, int8_t, 8, align::yes>    little_int8_buf_at;
	typedef endian_buffer<order::little, int16_t, 16, align::yes>  little_int16_buf_at;
	typedef endian_buffer<order::little, int32_t, 32, align::yes>  little_int32_buf_at;
	typedef endian_buffer<order::little, int64_t, 64, align::yes>  little_int64_buf_at;

	// aligned little endian unsigned integer buffers
	typedef endian_buffer<order::little, uint8_t, 8, align::yes>   little_uint8_buf_at;
	typedef endian_buffer<order::little, uint16_t, 16, align::yes> little_uint16_buf_at;
	typedef endian_buffer<order::little, uint32_t, 32, align::yes> little_uint32_buf_at;
	typedef endian_buffer<order::little, uint64_t, 64, align::yes> little_uint64_buf_at;

	typedef float_endian_buffer<order::big, float>     big_float_buf_t;
	typedef float_endian_buffer<order::big, double>     big_double_buf_t;

	static_assert(sizeof(little_int24_buf_t)==3,"error size");
	static_assert(sizeof(big_int24_buf_t)==3,"error size");
	static_assert(sizeof(big_float_buf_t)==4,"error size");
	static_assert(sizeof(big_double_buf_t)==8,"error size");

	extern template class float_endian_buffer<order::big,float>;
	extern template class float_endian_buffer<order::big,double>;
	extern template class endian_buffer<order::big, uint16_t, 16, align::no>;
	extern template class endian_buffer<order::big, uint32_t, 32, align::no>;
	extern template class endian_buffer<order::big, uint64_t, 64, align::no>;
	extern template class endian_buffer<order::big, int16_t, 16, align::no>;
	extern template class endian_buffer<order::big, int32_t, 32, align::no>;
	extern template class endian_buffer<order::big, int64_t, 64, align::no>;

	template<int b,int e>
		using big_uint16_frag_buffer_t=frag_endian_buffer<boost::endian::order::big,uint16_t,b,e>;
	template<int b,int e>
		using big_uint24_frag_buffer_t=frag_endian_buffer<boost::endian::order::big,little_uint24_buf_t,b,e>;
	template<int b,int e>
		using big_uint32_frag_buffer_t=frag_endian_buffer<boost::endian::order::big,uint32_t,b,e>;
	template<int b,int e>
		using little_uint8_frag_buffer_t=frag_endian_buffer<boost::endian::order::little,uint8_t,b,e>;
	template<int b,int e>
		using little_uint16_frag_buffer_t=frag_endian_buffer<boost::endian::order::little,uint16_t,b,e>;
	template<int b,int e>
		using little_uint24_frag_buffer_t=frag_endian_buffer<boost::endian::order::little,little_uint24_buf_t,b,e>;
	template<int b,int e>
		using little_uint32_frag_buffer_t=frag_endian_buffer<boost::endian::order::little,uint32_t,b,e>;
	template<int b,int e>
		using little_uint64_frag_buffer_t=frag_endian_buffer<boost::endian::order::little,uint64_t,b,e>;
}
extern std::ostream& operator<<(std::ostream& os,const WSpace::big_float_buf_t& v);
extern std::ostream& operator<<(std::ostream& os,const WSpace::big_double_buf_t& v);
template<boost::endian::order Order,typename T,int b,int e>
std::ostream& operator<<(std::ostream& os,const WSpace::frag_endian_buffer<Order,T,b,e>& v){
	os<<T(v);
	return os;
}
