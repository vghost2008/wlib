/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include "wmacros.h"
#include <toolkit.h>
#include <limits>
#include "wbuffer.h"
#include "abstractioprotocol.h"
#include <list>
class WIODevice;
class WAbstractPacketModule:public WAbstractIOProtocol
{
	public:
		typedef WBuffer IOBuffer;
		typedef int (WAbstractPacketModule::*ReadFuncType)(WBuffer*,WIODevice*);
		enum UnpackFlag
		{
			UF_GOOD_STATE               =   0x00,
			UF_UNPACK_WITH_NEW_DATA     =   0x01,
			UF_UNPACK_PURE_WITH_CACHE   =   0x02,
		};
	public:
		/*
		 * 将数据分为数据流及数据报
		 */
		struct DataGram { };
		struct DataStream { };
		static DataGram data_gram;
		static DataStream data_stream;
	public:
		WAbstractPacketModule();
		virtual int write(WBuffer* data,WIODevice* io)override;
		virtual int read(WBuffer* data,WIODevice* io)override;
		virtual bool pack(IOBuffer* io_buffer);
		/*
		 * 如果还不足以完成解包或完成解包后有剩余数据，应当将数据暂存在temp_cache_中
		 * 子类也需要这要处理
		 * 进入之前需要检查temp_cache_是否为空，如果不为空，应当对temp_cache_一同解包
		 */
		virtual bool unpack(IOBuffer* data);
		/*
		 * 如果data为空，使用缓存中的数据解包,同样使用data输出
		 * 如果data不空，使用缓存+data中的数据解包
		 */
		bool smartUnpack(IOBuffer* data);
		inline void clearCache(){ temp_cache_.clear();}
		inline bool clearCacheIfNeed() {
			if(temp_cache_.size() > max_data_cache_capability_) {
				ERR("Temp cache over flow, force to clear temp cache, cache size=%d",max_data_cache_capability_);
				clearCache();
				return true;
			}
			return false;
		}
		inline void setMaxDataCacheCapability(int cap) {
			if(cap <= 8) return;
			max_data_cache_capability_ = cap;
		}

		virtual ~WAbstractPacketModule(){}
		static unsigned short getSum(const unsigned char* data,int length);
		static unsigned short getCRC(const unsigned char* data, int length);
		inline static unsigned short getCRC(const WBuffer& data) {
			return getCRC((unsigned char*)data.data(),data.size());
		}
		/*
		 * 在data中搜索指定的数据，返回最早匹配的索引,如果没有匹配则返回-1
		 * v:数据容器
		 */
		template<typename CT>
		int searchHeads(const WBuffer& data,const CT& v,int* pos=nullptr) {
			using DataType = typename CT::value_type;
			static_assert(std::is_standard_layout<DataType>::value,"error type");
			static_assert(!std::is_pointer<DataType>::value,"error type");

			if(data.size() < sizeof(DataType) || v.empty()) return -1;

			auto target = *data.cast<const DataType*>();
			for(int i=0; i<v.size(); ++i) {
				if(v[i] == target) {
					if(nullptr != pos)*pos = 0;
					return i;
				}
			}
			auto min_index     = -1;
			auto min_index_pos = std::numeric_limits<int>::max();

			for(int i=0; i<v.size(); ++i) {
				auto p = data.search(&v[i],sizeof(v[i]));
				if(-1 == p || p>min_index_pos)continue;

				min_index_pos  =  p;
				min_index      =  i;
			}
			if(nullptr != pos)*pos = min_index_pos;
			return min_index;
		}
	public:
		/*
		 * 数据流处理中
		 * 子类不需要处理下列函数，主要供收发策略使用
		 * 数据报不需要使用以下函数
		 */
		/*
		 * 如果使用缓冲区的数据进行解包比较可能成功则返回0，否则返回-1
		 * 如果缓存中有数据，将缓存中的数据放入data中，并清空缓存
		 */
		bool tryToUnpackOnOldData(WBuffer* data);
		/*
		 * 设置使用cache解包标签，
		 * 如果成功data获取temp_cache_的数据
		 */
		bool toCacheUnpackModel(IOBuffer* data);
		/*
		 * 设置使用新数据进行解包标签，如果成功data为temp_cache_+data
		 */
		bool toNewDataUnpackModel(IOBuffer* data);
		inline void set_unpack_flag(UnpackFlag flag) { unpack_flag_ = flag; }
	public:
		/*
		 * 数据报的读取处理函数
		 * 一次解包只与当次读取的数据有关
		 */
		int dataGramRead(WBuffer* data,WIODevice* io);
		/*
		 * 数据流的读取处理函数
		 * 一次解包可能前面几次及后面几次读取的数据有关
		 */
		int dataStreamRead(WBuffer* data,WIODevice* io);
		inline void setDataType(DataGram ) {
			temp_cache_.clear();
			read_func_ = &WAbstractPacketModule::dataGramRead;
		}
		inline void setDataType(DataStream) {
			temp_cache_.clear();
			read_func_ = &WAbstractPacketModule::dataStreamRead;
		}
	protected:
		/*
		 * 数据缓存
		 * 用于数据流方式
		 */
		IOBuffer        temp_cache_;
		/*
		 * 解包需要的最小长度(用于数据流方式)
		 */
		int            min_data_need_for_unpack_;
		/*
		 * 如果threshold_for_unpack_大于0，则表示如果数据长度大于等于threshold_for_unpack_
		 * 那么在这个数据里解包成功的概率很大，可以先偿试解包，再决定是否要读取新的数据
		 * 用于数据流方式
		 */
		int            threshold_for_unpack_;
		/*
		 * 缓存的最大长度
		 * 用于数据流方式
		 */
		int            max_data_cache_capability_;
		int            max_try_times_;
		ReadFuncType   read_func_ = &WAbstractPacketModule::dataStreamRead;
		friend class   WIODevice;
	private:
		UnpackFlag unpack_flag_;
};
