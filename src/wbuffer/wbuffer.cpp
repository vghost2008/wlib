/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "wbuffer.h"
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

size_t WBuffer::s_init_head_room_ = 128u;

WBuffer::WBuffer(unsigned cap)noexcept
:capacity_(cap)
,size_(0)
,id_(0)
,data_(nullptr)
,attr_(0)
,head_room_(s_init_head_room_)
{
	assert(capacity_<MAX_BUFFER_SIZE);
	setCapacity(std::max<unsigned>(cap,head_room_));
}
WBuffer::WBuffer(const WBuffer& buffer)noexcept
:capacity_(0)
,size_(0)
,id_(0)
,data_(nullptr)
,attr_(0)
,head_room_(s_init_head_room_)
{
	setCapacity(0);
	*this = buffer;
}
WBuffer::WBuffer(const void* d,size_t s,size_t cap,unsigned id)noexcept
:capacity_(cap) 
,id_(id)
,data_(nullptr)
,size_(0)
,head_room_(s_init_head_room_)
{
	assert(capacity_<MAX_BUFFER_SIZE);
	setCapacity(std::max<unsigned>(cap,s+head_room_));
	setData(d,s);
}
WBuffer::WBuffer(const char* data)noexcept
:capacity_(0) 
,id_(0)
,data_(nullptr)
,size_(0)
,head_room_(s_init_head_room_)
{
	const size_t len = strlen(data);
	assert(len<MAX_BSTRING_LEN);
	setCapacity(len+head_room_);
	setData(data,len);
}
WBuffer::WBuffer(WBuffer&& buffer) noexcept
:capacity_(0u)
,size_(0u)
,id_(0)
,data_(nullptr)
,attr_(0)
,head_room_(s_init_head_room_)
{
	swap(buffer);
	buffer.clear();
}
WBuffer::WBuffer(const EmptyBuffer& )noexcept
:capacity_(0)
,size_(0)
,id_(0)
,data_(nullptr)
,attr_(0)
,head_room_(s_init_head_room_)
{
	setCapacity(0);
}
WBuffer& WBuffer::append(const WBuffer& buffer) 
{
	return append(buffer.data(),buffer.size());
}
WBuffer& WBuffer::append(WBuffer&& buffer) 
{
	append(buffer.data(),buffer.size());
	buffer.clear();
	return *this;
}
WBuffer& WBuffer::append(const char* d)
{
	return append(d,strlen(d));
}
WBuffer& WBuffer::append(const void* d,size_t s)
{
	assert(s+offset()<MAX_BUFFER_SIZE);
	if(0u == s) return *this;
	if(free_size() < s 
		&& !setCapacity(s+offset())) {
		ERR("Append Error");
		return *this;
	}
	memcpy(data()+size(),d,s);
	add_size(s);
	return *this;
}
WBuffer& WBuffer::prepend(const WBuffer& buffer) 
{
	return prepend(buffer.data(),buffer.size());
}
WBuffer& WBuffer::prepend(const char* data)
{
	return prepend(data,strlen(data));
}
WBuffer& WBuffer::__prepend(const void* d,size_t s)
{
	assert(s+offset()<MAX_BUFFER_SIZE);
	if(0u == s) return *this;
	if(free_size()<s
		&& !setCapacity(s+offset())) {
		ERR("Prepend ERROR");
		return *this;
	}
	if(head_room_ >= s) {
		memcpy(begin()-s,d,s);
		head_room_ -= s;
		add_size(s);
	} else {
		std::copy(begin(),end(),begin()+s);
		memcpy(begin(),d,s);
		add_size(s);
	}
	return *this;
}
WBuffer& WBuffer::prepend(const void* d,size_t s)
{
	assert(s+size()<MAX_BUFFER_SIZE);
	if(0 == s) return *this;
	if(free_size() < s 
		&& !setCapacity(s+offset())) {
		ERR("Prepend Error");
		return *this;
	}
	if(s > head_room_) 
		return __prepend(d,s);
	memcpy(begin()-s,d,s);
	head_room_ -= s;
	add_size(s);
	return *this;
}
WBuffer& WBuffer::prepend(WBuffer&& buffer) 
{
	if(0 == buffer.size()) return *this;
	if(head_room_ >= buffer.size()) {
		prepend(buffer.data(),buffer.size());
	} else {
		buffer.append(*this);
		*this = std::move(buffer);
	}
	buffer.clear();
	return *this;
}
bool WBuffer::right(size_t s) 
{
	if(s == 0) { size_ = 0; return true; }
	if(s >= size_) {
		WARNING("Error size");
		return false;
	}
	head_room_ += (size_-s);
	set_size(s);
	return true;
}
bool WBuffer::mid(unsigned offset,size_t s) 
{
	if(offset >= size()) {
		clear();
		return false;
	}
	if(offset+s > size_) { s = size_ - offset; }
	if(s == 0) { 
		clear();
		WARNING("Error size"); 
		return true; 
	}
	set_size(offset+s);
	return right(s);
}
bool WBuffer::getRight(const WBuffer& buffer,size_t s) 
{
	if(s <= 0 || buffer.empty()) { clear(); return true; }
	if(s >= buffer.size()) {
		setData(buffer.data(),buffer.size());
		return true;
	}
	return setData(buffer.data()+buffer.size()-s,s);
}
bool WBuffer::moveRightTo(WBuffer* out,size_t s) 
{
	out->clear();
	if(s <= 0)  return true; 
	if(s >= size()) {
		s = size();
	}
	out->getRight(*this,s);
	left(size()-s);
	return true;
}
bool WBuffer::moveLeftTo(WBuffer* out,size_t s) 
{
	out->clear();
	if(s <= 0) return true;
	if(s >= size()) {
		s = size();
	}
	out->getLeft(*this,s);
	right(size()-s);
	return true;
}
WBuffer& WBuffer::operator=(const WBuffer& buffer) noexcept
{
	if(&buffer == this ) return *this;

	clear(); 
	if(buffer.isNull()) { 
		return *this; 
	}
	if(free_size()<buffer.size()
			&& !setCapacity(buffer.size()+offset())) {
		return *this;
	}

	memcpy(begin(),buffer.data(),buffer.size());
	set_size(buffer.size());
	id_      =   buffer.id_;
	setAttr(buffer.attr());
	return *this;
}
WBuffer& WBuffer::operator=(WBuffer&& buffer) noexcept
{
	if(capacity() > buffer.capacity()) {
		return *this = static_cast<const WBuffer&>(buffer);
	}

	swap(buffer);
	buffer.clear();
	return *this;
}
WBuffer& WBuffer::operator=(const char* data)noexcept
{
	const size_t len = strlen(data);
	assert(len<MAX_BSTRING_LEN);
	setData(data,len);
	return *this;
}
bool WBuffer::operator==(const WBuffer& buffer) noexcept
{
	if(size_ != buffer.size()) return false;
	return std::equal(begin(),end(),buffer.begin());
}
void WBuffer::swap(WBuffer& buffer)noexcept
{
	if(&buffer == this ) return ;

	std::swap(data_,buffer.data_);
	std::swap(size_,buffer.size_);
	std::swap(capacity_,buffer.capacity_);
	std::swap(head_room_,buffer.head_room_);
	std::swap(id_,buffer.id_);
	std::swap(attr_,buffer.attr_);

	return ;
}
bool WBuffer::setCapacity(int cap)
{
	if(cap < capacity_) 
		return true;
	cap = std::max<unsigned>({cap,cap+s_init_head_room_-head_room_,s_init_head_room_+size_,kMinWbufferCapacity});
	char* p = new char[cap];
	if(nullptr == p) {
		ERR("Alloc Memory Faild");
		return false;
	}
	if(nullptr != data_) {
		char* old = data_;
		data_ = p;
		if(size_ > 0) 
			memcpy(data_+s_init_head_room_,old+head_room_,size_);
		delete[] old;
		old=nullptr;
	} else {
		data_   =   p;
		size_   =   0u;
	}
	head_room_   =   s_init_head_room_;
	capacity_    =   cap;
	return true;
}
bool WBuffer::setData(const void* d,size_t s)
{
	clear(); //防止不必要的复制

	if(nullptr == d || s<=0) return true;

	assert(s<MAX_BUFFER_SIZE);
	if(s+s_init_head_room_>capacity()
			&& !setCapacity(s+s_init_head_room_)) {
		return false;
	}
	memcpy(data(),d,s);
	size_  =   s;
	return true;
}
int WBuffer::search(const void* _data,size_t size, unsigned beg_pos)const
{
	const char* data = (const char*)(_data);
	if(beg_pos>this->size()) return -1;
	auto it = std::search(begin()+beg_pos,end(),data,data+size);
	if(it == end()) return -1;
	return it-begin();
}
bool WBuffer::setDataByString(const char* fmt,...)
{
	va_list     ap;

	clear();
	if(nullptr == fmt) return true;
	if(capacity() < 256+s_init_head_room_
		&& !setCapacity(256+s_init_head_room_)) {
		return false;
	}
	head_room_ = s_init_head_room_;
	va_start(ap,fmt);
	vsnprintf(data(),capacity(),fmt,ap);
	//vsprintf(data(),fmt,ap);
	va_end(ap);
	resize(strlen(data()));
	return true;
}
void WBuffer::dump()const
{
	dump(false);
}
void WBuffer::assicDump()const
{
	dump(true);
}
void WBuffer::dump(bool isassic)const
{
	std::cout<<"Dump wbuffer"<<std::endl;
	std::cout<<"Id:"<<id_<<std::endl;
	std::cout<<"Attr:"<<attr_<<"("<<std::hex<<attr_<<")"<<std::endl;
	std::cout<<std::dec;
	std::cout<<"Size:"<<size()<<std::endl;
	std::cout<<"Capacity:"<<capacity()<<std::endl;
	std::cout<<"HeadRoom:"<<head_room_<<std::endl;
	std::cout<<"Data:"<<std::endl;
	if(isassic) {
		std::cout<<data();
	} else {
		//std::cout<<WSpace::toHexString(reinterpret_cast<unsigned char*>(data_),size_,8,4);
	}
	std::cout<<"Dump wbuffer end";
}
WBuffer::~WBuffer() 
{
	if(nullptr != data_) { 
		delete[] data_;
		data_=nullptr;
	}
	capacity_   =   0;
	size_       =   0;
}
