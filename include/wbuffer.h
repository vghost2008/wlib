/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <stddef.h>
#include <cassert>
#include <type_traits>
#include <memory>
#include <string.h>
#include <new>
#include <stdarg.h>
#include <cmacros.h>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <boost/type_index.hpp>
#ifdef max
#undef max
#endif
#define MAX_BUFFER_SIZE 0x4FFFFFFF
#define MAX_BSTRING_LEN 4096
const size_t kMinWbufferCapacity = 4096;
#define EMPTY_BUFFER EmptyBuffer()
struct EmptyBuffer{};
class WBuffer
{
	public:
		/*
		 * 构造
		 */
		explicit WBuffer(unsigned cap=1024)noexcept;
		explicit WBuffer(const char* data)noexcept;
		explicit WBuffer(const void* d,size_t s,size_t cap=0,unsigned id=0)noexcept;
		template<typename DataType>
			WBuffer(std::initializer_list<DataType> vl)noexcept
			:capacity_(0)
			 ,size_(0)
			 ,id_(0)
			 ,data_(nullptr)
			 ,attr_(0)
			 ,head_room_(s_init_head_room_)
	{
		static_assert(!std::is_pointer<DataType>::value,"Error type");
		static_assert(std::is_standard_layout<DataType>::value,"Error Memory layout");
		setCapacity(head_room_+(2+vl.size())*sizeof(DataType));
		for(auto&v : vl) {
			append(&v,sizeof(v));
		}
	}
		WBuffer(WBuffer&& buffer)noexcept;
		WBuffer(const WBuffer& buffer)noexcept;
		WBuffer(const EmptyBuffer&)noexcept;
		~WBuffer();
	public:
		/*
		 * 访问
		 */
		/*
		 * 有效数据尾与内存首地址的偏移
		 */
		inline size_t offset()const { return size_+head_room_; }
		/*
		 * 有效数据长度
		 */
		inline size_t size()const { return size_; }
		inline bool empty()const { return 0 == size_; }
		/*
		 * 在不覆盖现有有效数据的前提下可存放有效数据的空间
		 */
		inline size_t free_size()const { 
			if(capacity_ < offset()) return 0u;
			return capacity_ - offset(); 
		}
		/*
		 * 在覆盖现有的有效数据的前提下，可存放的有效数据长度
		 */
		inline size_t dataCapacity()const {
			if(capacity()<=head_room_) return 0;
			return capacity()-head_room_;
		}
		/*
		 * 所申请的内在大小，包括head_room_
		 */
		inline size_t capacity()const { return capacity_;}
		inline size_t head_room()const {
			return head_room_;
		}
		inline char* data() { return data_+head_room_; }
		inline const char* data()const { return data_+head_room_; }
		inline char* begin(){return data();}
		inline const char* begin()const {return data();}
		inline char* end() { return begin()+size_;}
		inline const char* end()const { return begin()+size_;}
		inline bool isNull()const { return 0==size_;}
		inline void fill(unsigned char v) {
			std::fill(begin(),end(),v);
		}
		/*
		 * 自己仅保留右边s个数据
		 */
		bool right(size_t s);
		/*
		 * 获取buffer右边的s个数据
		 */
		bool getRight(const WBuffer& buffer,size_t s);
		/*
		 * 将数据右边s个数据存入out中，自己保留剩下的size()-s个数据
		 */
		bool moveRightTo(WBuffer* out,size_t s);
		/*
		 * 自己仅保留左边s个数据
		 */
		inline bool left(size_t s) {
			if(s >= size()) 
				return false;
			set_size(s);
			return true;
		}
		/*
		 * 获取buffer左边的s个数据
		 */
		inline bool getLeft(const WBuffer& buffer, size_t s) {
			return setData(buffer.data(),s);
		}
		bool moveLeftTo(WBuffer* out,size_t s);
		bool mid(unsigned offset,size_t s);
		inline unsigned id()const { return id_; }
		inline void setId(unsigned id) { id_ = id;}
		inline void setAttr(long attr) { attr_ = attr; }
		inline long attr()const { return attr_; }
		/*
		 * 查找data,长度为size,如果找到返回位置，否则返回-1
		 */
		int search(const void* data,size_t size, unsigned beg_pos = 0)const;
		inline int search(const WBuffer& v, unsigned beg_pos = 0)const {
			return search(v.data(),v.size(),beg_pos);
		}
		template<typename DataType,typename=typename std::enable_if<!
			std::is_same<WBuffer,typename std::decay<DataType>::type>::value
		>::type
		,typename=typename std::enable_if<!std::is_pointer<DataType>::value>::type
		>
		inline int search(const DataType& d,unsigned beg_pos=0) {
			return search(&d,sizeof(d),beg_pos);
		}
		/*
		 * 保持capacity不变小
		 */
		void swap(WBuffer& buffer)noexcept;
	public:
		inline const char* src_info()const { return src_info_; }
		inline void setSrcInfo(const void* data, size_t size) {
			size = std::min(size,sizeof(src_info_));
			memcpy(src_info_,data,size);
		}
		template<typename T>
		inline void setSrcInfo(const T& v) {
			static_assert(std::is_standard_layout<T>::value,"Error type");
			setSrcInfo(&v,sizeof(v));
		}
	public:
		/*
		 * 设置值
		 */
		bool setData(const void* data,size_t size);
		bool setDataByString(const char* fmt,...);
	public:
		WBuffer& append(const WBuffer& buffer);
		WBuffer& append(WBuffer&& buffer);
		WBuffer& append(const void* data,size_t s);
		WBuffer& append(const char* data);
		template<typename DataType>
			WBuffer& append(const DataType& v) {
				static_assert(!std::is_pointer<DataType>::value,"Error type");
				static_assert(std::is_standard_layout<DataType>::value,"Error Memory layout");
				return append(&v,sizeof(v));
			}
		inline WBuffer& appendLeft(const WBuffer& buffer, size_t size) {
			assert(buffer.size()>=size);
			return append(buffer.data(),size);
		}
		inline WBuffer& appendRight(const WBuffer& buffer, size_t size) {
			assert(buffer.size()>=size);
			return append(buffer.data()+(buffer.size()-size),size);
		}
		WBuffer& prepend(const WBuffer& buffer);
		WBuffer& prepend(const char* data);
		WBuffer& prepend(const void* data,size_t s);
		WBuffer& __prepend(const void* data,size_t s);
		WBuffer& prepend(WBuffer&& buffer);
		template<typename DataType>
			WBuffer& prepend(const DataType& v) {
				static_assert(std::is_standard_layout<DataType>::value,"Error Memory layout");
				static_assert(!std::is_pointer<DataType>::value,"Error type");
				return prepend(&v,sizeof(v));
			}
		inline WBuffer& prependLeft(const WBuffer& buffer, size_t size) {
			assert(buffer.size()>=size);
			return prepend(buffer.data(),size);
		}
		inline WBuffer& prependRight(const WBuffer& buffer, size_t size) {
			assert(buffer.size()>=size);
			return prepend(buffer.data()+(buffer.size()-size),size);
		}
	public:
		template<typename DataType>
			DataType cast(int offset=0) {
				static_assert(std::is_pointer<DataType>::value,"Error type");
				return reinterpret_cast<DataType>(data()+offset);
			}
		template<typename DataType>
			const DataType cast(int offset=0)const {
				static_assert(std::is_pointer<DataType>::value,"Error type");
				return reinterpret_cast<const DataType>(data()+offset);
			}
		template<typename DataType>
			DataType& castRef(int offset=0)noexcept(false) {
				static_assert(!std::is_pointer<DataType>::value,"Error type");
				if(size()-offset<sizeof(DataType)) throw std::runtime_error("data size error.");
				return *reinterpret_cast<DataType*>(data()+offset);
			}
		template<typename DataType>
			const DataType& castRef(int offset=0)const noexcept(false){
				static_assert(!std::is_pointer<DataType>::value,"Error type");
				if(size()-offset<sizeof(DataType)) throw std::runtime_error("data size error.");
				return *reinterpret_cast<const DataType*>(data()+offset);
			}
		inline void clear() { 
			head_room_ = s_init_head_room_;
			size_ = 0;
		}
		inline void clear_head_room(size_t new_head_room=WBuffer::s_init_head_room_) {
			if(new_head_room>=head_room_)
				return;
			if(new_head_room>8192)
				new_head_room = 8192;

			auto pi = data();
			auto po = data_+new_head_room;
			for(auto i=0; i<size(); ++i)
				po[i] = pi[i];
			head_room_ = new_head_room;
		}
		/*
		 * 设置数据长度，不对数据内容作修改
		 */
		inline bool resize(int s){ 
			if(s<=0) {
				clear();
				return true;
			}
			if(capacity_<s+head_room_
					&&!setCapacity(s+head_room_))
				return false;
			return set_size(s);
		}
		bool setCapacity(int cap);
	public:
		/*
		 * 调试
		 */
		void dump()const;
		void assicDump()const;
		void dump(bool isassic)const;
	public:
		/*
		 * 重载
		 */
		WBuffer& operator=(const WBuffer& buffer)noexcept;
		WBuffer& operator=(WBuffer&& buffer)noexcept;
		WBuffer& operator=(const char* data)noexcept;
		inline char& operator[](int i) { return data()[i];}
		inline char operator[](int i)const { return data()[i];}
		template<class DataType>
			WBuffer& operator=(const DataType& v)noexcept 
			{
				static_assert(std::is_standard_layout<DataType>::value,"Error Memory layout");
				static_assert(!std::is_pointer<DataType>::value,"Error type");
				setData(&v,sizeof(v));
				return *this;
			}
		bool operator==(const WBuffer& buffer)noexcept;
		inline bool add_size(int s) {
			if(s<0||s+size_+head_room_>capacity_) {
				ERR("ERROR SIZE");
				return false;
			}
			size_ += s;
			return true;
		}
	private:
		size_t         size_             = 0u; //有效数据长度
		size_t         capacity_         = 0u;               //所申请的内在空间，包括head_room_
		size_t         head_room_        = s_init_head_room_; //有效数据前的留白
		char          *data_             = nullptr;
		unsigned       id_               = 0;
		long           attr_             = 0;
		char           src_info_[32]     = {'\0'};            //数据源信息
		static size_t  s_init_head_room_;
	private:
		inline bool set_size(int s) {
			if(s+head_room_>capacity_||s<0){
				ERR("ERROR SIZE");
				return false;
			}
			size_ = s;
			return true;
		}
};
namespace std
{
	inline void swap(WBuffer& lhv,WBuffer&rhv)noexcept {
		lhv.swap(rhv);
	}
}
