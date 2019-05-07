/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <string.h>
#include <algorithm>
#include <wmacros.h>
#include <QMutex>
#include <atomic>
#include "toolkit.h"
#include "wglobal.h"
/*
 * data:当前行压入的数据
 * size:当前行数据的大小
 */
typedef void (*PushLineCallBack)(const char* data,int size,void* private_data);
enum FillBufferMode:bool
{
	FBM_RESTORE_OLD_POS        ,   
	FBM_DONT_RESTORE_OLD_POS   ,   
};
class WTwoDBufferWithSampleRate;
class WTwoDBufferWithRandomPusher;
/*
 * NA=4
 * -----------------------------------------------------------
 *  刚开始
 * 0 :<- current_pos_ = 0,line_count_ = 0 
 * 1 :
 * 2 :
 * 3 :
 * NA:
 * 状态:isFull() == false, isNull()==true, push_action_count_==0
 * -----------------------------------------------------------
 * 开始一小段时间后的状态
 *
 * 0 : 
 * 1 :
 * 2 :
 * 3 :<- current_pos_ = 3,line_count_ = 3
 * NA:
 *
 * 状态:isFull() == false, isNull()==false, push_action_count_==3
 *
 * [0-2]:getFirstPart
 * nullptr :getSecondpart
 *
 * -----------------------------------------------------------
 * 一段时间过后
 * 
 * 5 : 
 * 6 :
 * 7 :
 * 3 :<- current_pos_ = 2, line_count_ = 5(line_count_ <= height())
 * 4 :
 *
 * 状态:isFull()==true, isNull()==false, push_action_count_==7
 *
 * [3-4]:getFirstPart
 * [5,7]:getSecondpart
 *
 * 注意：当当前Buffer是由另一个Buffer填充时push_action_count_可能大于height()而line_count_小于height()
 * -----------------------------------------------------------
 */
class WBaseTwoDBuffer
{
	public:
		virtual ~WBaseTwoDBuffer();
		WBaseTwoDBuffer();
		WBaseTwoDBuffer(unsigned width,unsigned height,unsigned pixel_size,size_t capacity=0);
		virtual bool resize(unsigned width,unsigned height,unsigned pixel_size,size_t capacity=0,WSpace::MutexState state=WSpace::MutexState::MS_UNLOCKED);
		virtual bool pushLineData(const char* data,size_t size,WSpace::MutexState state=WSpace::MutexState::MS_UNLOCKED);
		virtual void linePushed();
		void setCurrentPos(int current_pos);
		/*
		 * 返回连续的旧的数据
		 */
		char* getFirstPart(int* height);
		/*
		 * 返回连续的新的数据
		 */
		char* getSecondpart(int* height);
		bool getTwoPartsHeight(int* first,int* second)const;
		inline unsigned height()const { return height_;}
		inline unsigned width()const { return width_;}
		inline unsigned capacity()const { return capacity_;}
		inline unsigned lineCount()const { return line_count_; }
		inline size_t widthToSize(unsigned width)const { return width*pixelSize();}
		inline size_t widthSize()const { return widthToSize(width());}
		inline bool isFull()const { return line_count_>=height_;}
		inline bool empty()const { return 0 == line_count_;}
		inline bool isNull()const { return nullptr == data_; }
		inline size_t pixelSize()const { return pixel_size_;}
		/*
		 * i = [0,height()-1]
		 * 0 为最老数据
		 * height()-1 为最新数据
		 */
		const char* pushOrderDataAt(unsigned i)const;
		const char* pixelAt(int x,int y)const;
		void setPushLineCallBack(PushLineCallBack call_back,void* private_data=nullptr) {
			push_line_callback_                =   call_back;
			push_line_callback_private_data_   =   private_data;
		}
		inline char* currentData() {
			return dataAt(current_pos_);
		}
		inline char* dataAt(unsigned row) {
			return data_+widthSize()*row;
		}
		inline const char* dataAt(unsigned row)const {
			return data_+widthSize()*row;
		}
		inline char* data() { return data_;}
		inline bool isNew()const { return is_new_; }
		inline void setDataToOld() { is_new_ = false; }
		inline int current_pos()const { return current_pos_;}
		virtual void clear(WSpace::MutexState state=WSpace::MutexState::MS_UNLOCKED);
		virtual void clearMemory(WSpace::MutexState state=WSpace::MutexState::MS_UNLOCKED);
		DISABLE_COPY_AND_ASSIGN(WBaseTwoDBuffer);
		/*
		 * 将当前有效的数据备份到buffer
		 */
		bool backupData(WBaseTwoDBuffer* buffer);
		/*
		 * 用当前有效的数据填充buffer,
		 */
		bool fillBuffer(WBaseTwoDBuffer* buffer,FillBufferMode mode);
		bool fillBuffer(WTwoDBufferWithSampleRate* buffer,FillBufferMode mode);
		/*
		 * 返回新压入的数据范围
		 * 如果没有新压入的数据返回0
		 * 否则返回新数据范围的数量[1-2]
		 * index[0]:第一个新数据范围的开始行号
		 * index[1]:第一个新数据范围的结束行号
		 * index[2]:第二个新数据范围的开始行号
		 * index[3]:第二个新数据范围的结束行号
		 */
		int getNewRange(int* index);
	public:
		inline bool lock(WSpace::MutexState state=WSpace::MutexState::MS_UNLOCKED)const {
			using namespace WSpace;
			if(MutexState::MS_LOCKED == state) return true;
			if(mutex_.tryLock()) return true;
			mutex_.lock();
			return true;
		}
		inline void unlock(WSpace::MutexState state=WSpace::MutexState::MS_UNLOCKED)const {
			using namespace WSpace;
			if(MutexState::MS_LOCKED == state) return ;
			mutex_.unlock();
		}
	protected:
		size_t            capacity_;
		unsigned          line_count_;
		char             *data_;
		/*
		 * 宽度：单位为像素点数据
		 * 一行的数据大小为 width_ * pixelSize()
		 */
		unsigned          width_;
		unsigned          height_;
		int               current_pos_;                       //必须使用有符号数,用于指示即将插入数据的位置
		bool              is_new_;                            //当前数据中是否有新发数据
		bool             *is_raw_new_;                        //指示具体的一行是否为新数据
		PushLineCallBack  push_line_callback_;
		void             *push_line_callback_private_data_;
		/*
		 * 像素大小
		 */
		size_t            pixel_size_;
		unsigned          push_action_count_;
		mutable QMutex    mutex_;
};
/*================================================================================*/
class WTwoDBufferWithRandomPusher:public WBaseTwoDBuffer
{
	public:
		WTwoDBufferWithRandomPusher();
		WTwoDBufferWithRandomPusher(unsigned width,unsigned height,unsigned pixel_size,size_t capacity=0);
		virtual ~WTwoDBufferWithRandomPusher();
		virtual bool resize(unsigned width,unsigned height,unsigned pixel_size,size_t capacity=0,WSpace::MutexState state=WSpace::MutexState::MS_UNLOCKED)override;
		virtual bool pushLineData(const char* data,size_t size,WSpace::MutexState state=WSpace::MutexState::MS_UNLOCKED)override;
		virtual void clear(WSpace::MutexState state=WSpace::MutexState::MS_UNLOCKED)override;
		virtual void clearMemory(WSpace::MutexState state=WSpace::MutexState::MS_UNLOCKED)override;
	private:
		char   *line_buffer_;
		/*
		 * 当前行的容纳能力==widthSize()
		 */
		size_t  line_buffer_capacity_;
		/*
		 * 当前行中已有数据的大小
		 */
		size_t  line_buffer_size_;
};
/*================================================================================*/
class WTwoDBufferWithSampleRate:public WBaseTwoDBuffer
{
	public:
		WTwoDBufferWithSampleRate();
		WTwoDBufferWithSampleRate(unsigned width,unsigned height,unsigned pixel_size,size_t capacity);
		/*
		 * resize设置原始宽度与高度，真实的宽度与高度由原始大小与抽样比共同决定
		 * width()与height()返回的是真实的宽度与高度
		 * org_width()与org_height()返回原始的宽度与高度
		 */
		virtual bool resize(unsigned width,unsigned height,unsigned pixel_size,size_t capacity=0,WSpace::MutexState state=WSpace::MutexState::MS_UNLOCKED)override;
		virtual bool pushLineData(const char* data,size_t size,WSpace::MutexState state=WSpace::MutexState::MS_UNLOCKED)override;
		inline bool setSampleRate(unsigned width_rate,unsigned height_rate,WSpace::MutexState state=WSpace::MutexState::MS_UNLOCKED) {
			if(width_rate == width_sample_rate_ && height_rate == height_sample_rate_) return false;
			return forceSetSampleRate(width_rate,height_rate,state);
		}
		inline bool forceSetSampleRate(unsigned width_rate,unsigned height_rate,WSpace::MutexState state=WSpace::MutexState::MS_UNLOCKED) {
			if(!lock(state)) return false;
			if(width_rate == 0 || height_rate == 0) return false;
			width_sample_rate_    =   width_rate;
			height_sample_rate_   =   height_rate;
			width_                =   orgWidthToWidth(org_width_);
			height_               =   org_height_;
			unlock(state);
			return true;
		}
		inline int orgWidthToWidth(int org_width) {
			return  WSpace::ceilingAlignTo<int>(org_width-1,width_sample_rate_)/width_sample_rate_+1;
		}
		inline int widthToOrgWidth(int width)const {
			return width_sample_rate_*(width-1);
		}
		inline int orgHeightToHeight(int org_height) {
			return  WSpace::ceilingAlignTo<int>(org_height-1,height_sample_rate_)/height_sample_rate_+1;
		}
		inline int heightToOrgHeight(int height)const {
			return height_sample_rate_*(height-1)+1;
		}
		inline void setPushActionCount(unsigned count) {
			push_action_count_ = count;
		}
		inline unsigned org_width()const { return org_width_;}
		inline unsigned org_height()const { return org_height_;}
		friend class WTwoDBufferWithRandomPusher;
		friend class WBaseTwoDBuffer;
	private:
		unsigned width_sample_rate_;
		unsigned height_sample_rate_;
		/*
		 * 当width_sample_rate_==1时的width_
		 */
		unsigned org_width_;
		/*
		 * 当height_sample_rate_==1时的height_
		 */
		unsigned org_height_;
};
/*================================================================================*/
class QImage;
bool twoDBufferToImage(const WBaseTwoDBuffer& buffer,QImage** image);
