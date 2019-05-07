/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "twodbuffer.h"
#include <algorithm>
#include <new>
#include "wmacros.h"
#include <QtGlobal>
#include <QImage>
#include <iostream>
#include <QDebug>
using namespace std;
using namespace WSpace;
WBaseTwoDBuffer::WBaseTwoDBuffer()
:capacity_(0)
,data_(nullptr)
,width_(0)
,height_(0)
,current_pos_(0)
,line_count_(0)
,is_new_(false)
,push_line_callback_(nullptr)
,push_line_callback_private_data_(nullptr)
,pixel_size_(1)
,push_action_count_(0)
,is_raw_new_(nullptr)
{

}
WBaseTwoDBuffer::WBaseTwoDBuffer(unsigned width,unsigned height,unsigned pixel_size,size_t capacity)
:capacity_(capacity)
,data_(nullptr)
,width_(0)
,height_(0)
,current_pos_(0)
,line_count_(0)
,is_new_(false)
,push_action_count_(0)
{
	new(this) WBaseTwoDBuffer();
	resize(width,height,pixel_size,capacity);
}
bool WBaseTwoDBuffer::resize(unsigned width,unsigned height,unsigned pixel_size,size_t capacity,MutexState state)
{
	bool    ret_val       = false;
	char   *data          = nullptr;
	size_t  new_capacity  = 0;
	size_t  new_line_size = 0;
	if(0 == width||0==height) {
		ERROR_LOG("Error width or height");
		return false;
	}
#ifndef QT_NO_DEBUG
	if(0 != capacity_) {
		Q_ASSERT_X(nullptr!=data_,__func__,"Error state");
	} else {
		Q_ASSERT_X(nullptr==data_,__func__,"Error state");
	}
#endif

	if(width == width_ 
			&& height == height_ 
			&& pixel_size == pixel_size_
			&& capacity<= capacity_)
		return true;

	if(!lock(state)) goto out0;

	width_          =   width;
	height_         =   height;
	pixel_size_     =   pixel_size;
	new_line_size   =   widthSize();
	new_capacity    =   max({capacity,capacity_,new_line_size*height_});
	new_capacity    =   ceilingAlignTo<unsigned>(new_capacity,4u);
	if(capacity_ < new_capacity) {
		data = new(std::nothrow) char[new_capacity];
		if(nullptr == data) {
			SIMPLE_LOG(LL_ERROR,"Alloc memory faild,size=%d!",new_capacity);
			goto out1;
		} 
		if(nullptr != data_) {delete[] data_; data_=nullptr;}
		data_       =   data;
		capacity_   =   new_capacity;
		data        =   nullptr;
	}
	if(nullptr != is_raw_new_) {
		delete[] is_raw_new_;
		is_raw_new_ = nullptr;
	}
	is_raw_new_ = new bool[height_];
	memset(is_raw_new_,0,sizeof(bool)*height_);

	clear(MutexState::MS_LOCKED);
	DBG("width=%d, height=%d",width_,height_);
#if WDEBUG_MESSAGE_LEVEL >= WDBG_LEVEL
	memset(data_,0xF0,capacity_);
#endif
	ret_val = true;
out1:
	unlock(state);
	if(nullptr != data){delete[] data; data=nullptr;}
	return ret_val;
out0:
	return false;
}
void WBaseTwoDBuffer::linePushed() 
{
	if(nullptr != push_line_callback_)push_line_callback_(currentData(),widthSize(),push_line_callback_private_data_);
	is_raw_new_[current_pos_] = true;
	++current_pos_;
	Q_ASSERT_X(line_count_<=height_,__func__,"Error line count");
	if(line_count_<height()) ++line_count_;
	is_new_ = true;
	++push_action_count_;
	if(current_pos_ >= height_) current_pos_ = 0;
}
void WBaseTwoDBuffer::clear(MutexState state)
{
	if(!lock(state)) {
		ERROR_LOG("Lock faild!");
		return;
	}
	current_pos_         =   0;
	line_count_          =   0;
	push_action_count_   =   0;
	memset(is_raw_new_,0x00,sizeof(bool)*height_);
	unlock(state);
}
void WBaseTwoDBuffer::clearMemory(MutexState state)
{
	if(!lock(state)) {
		ERROR_LOG("Lock faild!");
		return;
	}
	width_           =   0;
	height_          =   0;
	capacity_        =   0;
	setDataToOld();
	clear(MutexState::MS_LOCKED);
	if(nullptr != data_){delete[] data_; data_ = nullptr;}
	if(nullptr != is_raw_new_){delete[] is_raw_new_; is_raw_new_=nullptr;}
	unlock(state);
}
bool WBaseTwoDBuffer::pushLineData(const char* data,size_t size,MutexState state)
{
	Q_ASSERT_X(nullptr != data,__func__,"Error data");
	Q_ASSERT_X(widthSize() != 0,__func__,"Error width");
	if(size%widthSize() != 0) {
		SIMPLE_LOG_TO(LL_DEBUG,LWM_GUI|LWM_FILE,"Error data size");
		return false;
	}
	const char *d   = data;
	const char *end = data+size;
	if(!lock(state)) return false;
	while(d<end) {
		memcpy(currentData(),d,widthSize());
		linePushed();
		d += widthSize();
	}
	unlock(state);
	return true;
}
char* WBaseTwoDBuffer::getFirstPart(int* height)
{
	if(empty()) return nullptr;
	if(!isFull()) {
		if(nullptr != height)*height = line_count_;
		return data_;
	}
	if(0 != current_pos_) {
		if(nullptr != height)*height = height_-current_pos_;
		return dataAt(current_pos_);
	}
	if(nullptr != height)*height = height_;
	return data_;
}
char* WBaseTwoDBuffer::getSecondpart(int* height)
{
	if(!isFull()) return nullptr;
	if(0 != current_pos_) {
		if(nullptr != height)*height = current_pos_;
		return data_;
	}
	return nullptr;
}
bool WBaseTwoDBuffer::getTwoPartsHeight(int* first,int*second)const
{
	if(empty()) return false;
	*first = *second = 0;
	if(!isFull()) {
		*first = line_count_;
		return true;
	}
	if(0 != current_pos_) {
		*first    =   height_-current_pos_;
		*second   =   current_pos_;
		return true;
	} else {
		*first = height_;
	}
	return true;
}
const char* WBaseTwoDBuffer::pushOrderDataAt(unsigned i)const
{
	if(i>=height_) return nullptr;
	if(!isFull()) {
		if(i >= line_count_) return nullptr;
		return dataAt(i);
	}
	if(i<=height_-1-current_pos_) {
		return dataAt(current_pos_+i);
	}
	return dataAt(i-(height_-current_pos_));
}
const char* WBaseTwoDBuffer::pixelAt(int x,int y)const
{
	const char* line_data = pushOrderDataAt(y);
	if(x<0 || x>= width()) return nullptr;
	if(nullptr == line_data) return nullptr;
	return line_data+widthToSize(x);
}
WBaseTwoDBuffer::~WBaseTwoDBuffer()
{
	clearMemory();
}
bool WBaseTwoDBuffer::backupData(WBaseTwoDBuffer* buffer)
{
	int         i;
	bool        ret_val = false;
	const char *data    = nullptr;

	if(!lock()) return false;
	buffer->resize(width(),lineCount(),pixelSize(),0,MutexState::MS_LOCKED);
	for(i=0; i<buffer->height(); ++i) {
		data = pushOrderDataAt(i);
		if(nullptr == data)continue;
		buffer->pushLineData(data,buffer->widthSize(),MutexState::MS_LOCKED);
	}
	unlock();
	return ret_val;
}
bool WBaseTwoDBuffer::fillBuffer(WBaseTwoDBuffer* buffer,FillBufferMode mode)
{
	if(!lock()) {
		ERROR_LOG("Lock faild");
		return false;
	}
	if(!buffer->lock()) {
		unlock();
		return false;
	}
	int            i;
	const unsigned useable_count         = lineCount();
	const int      line_count            = std::min(buffer->height(),useable_count);
	const int      begin_index_in_buffer = lineCount()-line_count;
	const int      current_pos_in_buffer = buffer->current_pos_-line_count;

	buffer->clear(MutexState::MS_LOCKED);
	if(FBM_RESTORE_OLD_POS==mode) buffer->setCurrentPos(current_pos_in_buffer);
	for(i=begin_index_in_buffer; i<lineCount(); ++i) {
		buffer->pushLineData(pushOrderDataAt(i),widthSize(),MutexState::MS_LOCKED);
	}
	buffer->unlock();
	unlock();
	return true;
}
bool WBaseTwoDBuffer::fillBuffer(WTwoDBufferWithSampleRate* buffer,FillBufferMode mode)
{
	if(!lock()) {
		ERROR_LOG("Lock faild");
		return false;
	}
	if(!buffer->lock()) {
		unlock();
		return false;
	}
	int       i;
	const int height_capcity          = buffer->heightToOrgHeight(buffer->org_height_);
	const int useable_line_count      = std::min<int>(height_capcity,lineCount());
	const int end_push_action_index   = floorAlignTo(push_action_count_-1,buffer->height_sample_rate_);
	const int begin_push_action_index = ceilingAlignTo<int>(end_push_action_index-useable_line_count+1,buffer->height_sample_rate_);
	const int line_count              = end_push_action_index-begin_push_action_index+1;
	const int begin_index_in_buffer   = lineCount()-(push_action_count_-1-end_push_action_index)-line_count;
	const int current_pos_in_buffer   = buffer->current_pos_-buffer->orgHeightToHeight(line_count);

	buffer->clear(MutexState::MS_LOCKED);
	buffer->setPushActionCount(begin_push_action_index);
	if(FBM_RESTORE_OLD_POS==mode && line_count>=height_capcity) buffer->setCurrentPos(current_pos_in_buffer);
	for(i=begin_index_in_buffer; i<lineCount(); ++i) {
		buffer->pushLineData(pushOrderDataAt(i),widthSize(),MutexState::MS_LOCKED);
	}
	buffer->unlock();
	unlock();
	return true;
}
void WBaseTwoDBuffer::setCurrentPos(int current_pos) 
{
	while(current_pos<0)current_pos+=height();
	while(current_pos>=height())current_pos-=height();
	current_pos_ = current_pos;
}
int WBaseTwoDBuffer::getNewRange(int* index)
{
	if(!is_new_) return 0;
	if(0 == current_pos_) {
		index[0] = index[1] = height_-1;
		while(index[0]>=0&&is_raw_new_[index[0]]) {
			is_raw_new_[index[0]] = false;
			--index[0];
		}

		if(index[0] != index[1]) {
			++index[0];
		} else {
			return 0;
		}

		return 1;
	} else {
		index[0] = index[1] = current_pos_-1;
		while(index[0]>=0&&is_raw_new_[index[0]]) {
			is_raw_new_[index[0]] = false;
			--index[0];
		}
		if(index[0] != index[1]) {
			++index[0];
		} else {
			return 0;
		}

		if(0 != index[0]) return 1;

		index[2] = index[3] = height_-1;
		while(index[2]>=0&&is_raw_new_[index[2]]) {
			is_raw_new_[index[2]] = false;
			--index[2];
		}

		if(index[2] != index[3]) {
			++index[2];
		} else {
			return 1;
		}

		return 2;
	}
	return 0;
}
/*================================================================================*/
WTwoDBufferWithRandomPusher::WTwoDBufferWithRandomPusher()
:WBaseTwoDBuffer()
,line_buffer_(nullptr)
,line_buffer_capacity_(0)
,line_buffer_size_(0)
{
}
WTwoDBufferWithRandomPusher::WTwoDBufferWithRandomPusher(unsigned width,unsigned height,unsigned pixel_size,size_t capacity)
{
	new(this) WTwoDBufferWithRandomPusher();
	resize(width,height,pixel_size,capacity);
}
bool WTwoDBufferWithRandomPusher::resize(unsigned width,unsigned height,unsigned pixel_size,size_t capacity,MutexState state)
{
	bool ret_val = false;
	if(!lock(state)) {
		return false;
	}
	if(!WBaseTwoDBuffer::resize(width,height,pixel_size,capacity,MutexState::MS_LOCKED)) {
		ERROR_LOG("Resize faild");
		goto out0;
	}
	if(nullptr != line_buffer_){delete[] line_buffer_; line_buffer_=nullptr;}

	line_buffer_capacity_   =   widthSize();
	line_buffer_size_       =   0;
	line_buffer_            =   new(std::nothrow) char[line_buffer_capacity_];
out0:	
	unlock(state);
	return ret_val;
}
bool WTwoDBufferWithRandomPusher::pushLineData(const char* data_i,size_t size,MutexState state)
{
	const char* data = data_i;
label0:
	if(0==line_buffer_size_
			&& line_buffer_capacity_ ==size) {
		return WBaseTwoDBuffer::pushLineData(data,size,state);
	} 
	Q_ASSERT_X(line_buffer_size_<=line_buffer_capacity_,__func__,"Error line buffer size");
	Q_ASSERT_X(widthSize()==line_buffer_capacity_,__func__,"Error line buffer size");

	if(0 != line_buffer_size_) {
		if(size < line_buffer_capacity_-line_buffer_size_) {
			memcpy(line_buffer_+line_buffer_size_,data,size);
			line_buffer_size_+= size;
			goto out1;
		} else {
			const int needed_size = line_buffer_capacity_-line_buffer_size_;
			memcpy(line_buffer_+line_buffer_size_,data,needed_size);
			WBaseTwoDBuffer::pushLineData(line_buffer_,line_buffer_capacity_,state);
			line_buffer_size_= 0;
			size -= needed_size;
			data = data+needed_size;
			if(0 != size) goto label0;
		}
	} else {
		if(size < line_buffer_capacity_) {
			memcpy(line_buffer_,data,size);
			line_buffer_size_ = size;
			goto out1;
		} else {
			WBaseTwoDBuffer::pushLineData(data,line_buffer_capacity_,state);
			size -= line_buffer_capacity_;
			data += line_buffer_capacity_;
			if(0 != size)goto label0;
		}
	}
out1:
	return isNew();
}
void WTwoDBufferWithRandomPusher::clear(MutexState state)
{
	if(!lock(state)) {
		ERROR_LOG("Lock faild");
		return;
	}
	line_buffer_size_ = 0;
	WBaseTwoDBuffer::clear(MutexState::MS_LOCKED);
	unlock(state);
}
void WTwoDBufferWithRandomPusher::clearMemory(MutexState state)
{
	if(!lock(state)) {
		ERROR_LOG("Lock faild");
		return;
	}
	line_buffer_size_ = 0;
	line_buffer_capacity_ = 0;
	if(nullptr != line_buffer_) { delete[] line_buffer_; line_buffer_=nullptr;}
	WBaseTwoDBuffer::clearMemory(MutexState::MS_LOCKED);
	unlock(state);
}
WTwoDBufferWithRandomPusher::~WTwoDBufferWithRandomPusher()
{
	clearMemory();
}
/*================================================================================*/
WTwoDBufferWithSampleRate::WTwoDBufferWithSampleRate()
:WBaseTwoDBuffer()
,width_sample_rate_(1)
,height_sample_rate_(1)
,org_width_(0)
,org_height_(0)
{
}
WTwoDBufferWithSampleRate::WTwoDBufferWithSampleRate(unsigned width,unsigned height,unsigned pixel_size,size_t capacity)
{
	new(this) WTwoDBufferWithSampleRate();
	resize(width,height,pixel_size,capacity);
}
bool WTwoDBufferWithSampleRate::resize(unsigned width,unsigned height,unsigned pixel_size,size_t capacity,WSpace::MutexState state)
{
	bool    ret_val       = false;
	if(!lock(state)) {
		ERROR_LOG("Lock faild");
		return false;
	}
	if(!WBaseTwoDBuffer::resize(width,height,pixel_size,capacity,MutexState::MS_LOCKED)) {
		goto out1;
	}

	org_width_     =   width;
	org_height_    =   height;
	forceSetSampleRate(width_sample_rate_,height_sample_rate_,MutexState::MS_LOCKED);
	clear(MutexState::MS_LOCKED);
	ret_val = true;
out1:
	unlock();
	return ret_val;
}
bool WTwoDBufferWithSampleRate::pushLineData(const char* data,size_t size,MutexState state)
{
	bool ret_val = false;

	if(!lock(state)) return false;

	char       *pout       = currentData();
	const char *pout_end   = currentData()+widthSize();
	const char *pin        = data;
	const char *pin_end    = data+size;
	const int   pin_offset = widthToSize(width_sample_rate_);

	if(1 != height_sample_rate_
			&& 0 != push_action_count_%height_sample_rate_) {
		++push_action_count_;
		goto out0;
	}
	while(pout<pout_end
			&& pin < pin_end) {
		memcpy(pout,pin,pixel_size_);
		pout += pixel_size_;
		pin += pin_offset;
	}
	while(pout<pout_end) {
		*pout = 0xFF;
		++pout;
	}
	linePushed();
out0:
	ret_val = true;
	unlock(state);
	return ret_val;
}
/*================================================================================*/
unsigned rgb(const void* data) {
	const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
	return unsigned(p[0])<<16|unsigned(p[1])<<8|p[2];
}
unsigned rgb(const char data) {
	unsigned char c = static_cast<unsigned char>(data);
	return unsigned(c)<<16|unsigned(c)<<8|c;
}

bool twoDBufferToImage(const WBaseTwoDBuffer& buffer,QImage** image)
{
	int         i;
	int         j;
	bool        ret_val = false;
	const char *data    = nullptr;

	if(!buffer.lock()) return false;
	if(nullptr != *image) { delete *image; *image=nullptr;}
	*image = new QImage(buffer.width(),buffer.lineCount(),QImage::Format_RGB888);
	QImage& img = **image;
	if(buffer.empty()) goto out0;
	if(1 == buffer.pixelSize()) {
		for(i=0; i<img.width(); ++i) {
			for(j=0; j<img.height(); ++j) {
				data = buffer.pixelAt(i,j);
				if(nullptr == data) {
					ERROR_LOG("Error");
					continue;
				}
				img.setPixel(i,img.height()-1-j,rgb(*data));
			}
		}
	} else if(3 == buffer.pixelSize()) {
		for(i=0; i<img.width(); ++i) {
			for(j=0; j<img.height(); ++j) {
				data = buffer.pixelAt(i,j);
				if(nullptr == data) {
					ERROR_LOG("Error");
					continue;
				}
				img.setPixel(i,img.height()-1-j,rgb(data));
			}
		}
	} else {
		SIMPLE_LOG(LL_ERROR,"Error pixel size=%d",buffer.pixelSize());
	}
	ret_val = true;
out0:
	buffer.unlock();
	return ret_val;
}
