/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#include "packetmodule.h"
#include <wmacros.h>
#include <wlogmacros.h>
#include <toolkit.h>
#include <boost/crc.hpp>

using namespace std;

WAbstractPacketModule::DataGram WAbstractPacketModule::data_gram;
WAbstractPacketModule::DataStream WAbstractPacketModule::data_stream;

WAbstractPacketModule::WAbstractPacketModule()
:min_data_need_for_unpack_(2)
,threshold_for_unpack_(0)
,max_data_cache_capability_(8192)
,unpack_flag_(UF_GOOD_STATE)
,max_try_times_(4)
,read_func_(&WAbstractPacketModule::dataStreamRead)
{
	temp_cache_.setId(0);
}
int WAbstractPacketModule::read(WBuffer* data,WIODevice* io)
{
	return (this->*read_func_)(data,io);
}
int WAbstractPacketModule::dataGramRead(WBuffer* data,WIODevice* io)
{
	int  max_try   = max_try_times_;
	int  read_size = 0;

	read_size = next()->read(data,io);

	temp_cache_.clear();
	while(unpack(data) == false) {
		--max_try;
		if(0==max_try || (read_size=next()->read(data,io))<=0) {
			//ERR("Unpack faild!"); 
			goto out0;
		}
		temp_cache_.clear();
	}
	return data->size();
out0:
	return 0;
}
int WAbstractPacketModule::dataStreamRead(WBuffer* data,WIODevice* io)
{
	int  max_try   = max_try_times_;
	int  read_size = 0;

	if(true == tryToUnpackOnOldData(data)) {
		goto label0;
	} else {
		read_size = next()->read(data,io);
		if(read_size <= 0) {
			if(false == toCacheUnpackModel(data)) goto out0;
		} else {
			if(false == toNewDataUnpackModel(data)) goto out0;
		}
	}
label0:
	while(unpack(data) == false) {
		--max_try;
		if(0==max_try || (read_size=next()->read(data,io))<=0) {
			//ERR("Unpack faild!"); 
			goto out0;
		}
		if(false == toNewDataUnpackModel(data)) goto out0;
	}
	set_unpack_flag(WAbstractPacketModule::UF_GOOD_STATE);
	clearCacheIfNeed();
	return data->size();
out0:
	clearCacheIfNeed();
	return 0;
}
int WAbstractPacketModule::write(WBuffer* data,WIODevice* io)
{
	if(pack(data) == false) {
		return -1;
	}
	return next()->write(data,io);
}
bool WAbstractPacketModule::tryToUnpackOnOldData(WBuffer* data)
{
	data->clear();
	if(UF_GOOD_STATE == unpack_flag_
			&& temp_cache_.size() > threshold_for_unpack_ 
			&& true == toCacheUnpackModel(data)) {
		return true;
	}
	return false;
}
bool WAbstractPacketModule::toCacheUnpackModel(IOBuffer* data) 
{
	if(temp_cache_.size() < min_data_need_for_unpack_) return true;
	if(UF_UNPACK_PURE_WITH_CACHE == unpack_flag_) return true;
	unpack_flag_   =   UF_UNPACK_PURE_WITH_CACHE;
	*data          =   std::move(temp_cache_);
	temp_cache_.clear();
	return true;
}
bool WAbstractPacketModule::toNewDataUnpackModel(IOBuffer* data) 
{
	if(data->size()+temp_cache_.size()<min_data_need_for_unpack_) {
		temp_cache_.append(*data);
		data->clear();
		return false;
	}
	if(!temp_cache_.isNull()) {
		data->prepend(std::move(temp_cache_));
		temp_cache_.clear();
	}
	Q_ASSERT_X(data->size()>=min_data_need_for_unpack_,__FILE__,"min size");
	unpack_flag_ = UF_UNPACK_WITH_NEW_DATA;
	return true;
}
unsigned short WAbstractPacketModule::getSum(const unsigned char* data,int length)
{
	int      i       = 0;
	unsigned the_sum = 0;

	for(i=0; i<length; ++i) {
		the_sum += (unsigned short)data[i];
	}
	return the_sum&0xFFFF;
}
bool WAbstractPacketModule::unpack(IOBuffer* data)
{
	return true;
}
bool WAbstractPacketModule::smartUnpack(IOBuffer* data)
{
	bool res = true;

	if(!data->empty()) {
		if(true == toNewDataUnpackModel(data)) {
			res = unpack(data);
		} else {
			return false;
		}
	} else {
		if(true == tryToUnpackOnOldData(data)) {
			res = unpack(data);
		} else {
			return false;
		}
	}
	if(true == res)
		set_unpack_flag(WAbstractPacketModule::UF_GOOD_STATE);
	return res;
}
bool WAbstractPacketModule::pack(IOBuffer* buffer)
{
	return true;
}
unsigned short WAbstractPacketModule::getCRC(const unsigned char* data, int length)
{
	return boost::crc<16,0x8005,0x0000,0x0000,false,false>(data,length);
}
