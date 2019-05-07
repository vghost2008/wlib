/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#include <generalpm.h>
#include <wmacros.h>
#include <stdexcept>

using namespace std;

GeneralPM::GeneralPM()
{
	current_function_ = unpack_functions_.begin();
}
bool GeneralPM::unpack(WBuffer* data)
{
	if(wunlikely(unpack_functions_.empty())) return true;


	if(!temp_cache_.empty()) 
		data->prepend(std::move(temp_cache_));
	temp_cache_.clear();

	if(wunlikely(data->empty())) return false;

	try {
		while((*current_function_)(data,&temp_cache_)) {

			++current_function_;

			if(unpack_functions_.end() == current_function_) {
				if(!unpacked_data_.empty()) data->prepend(unpacked_data_); //成功解包
				unpacked_data_.clear();
				current_function_ = unpack_functions_.begin();
				/*
				 * 未使用的数据将会保存在temp_cache_中
				 */
				return true; //已经正确完成解包
			} else {
				/*
				 * 仅完成了一部分，继续解包
				 */
				unpacked_data_.append(*data); //保存成功解包的数据
				*data = temp_cache_; //继续对未使用的数据解包
				temp_cache_.clear(); //temp_cache_必须清除
			}
		} 
		/*
		 * 不能完成解包
		 * 自动储存未使用的数据供下次使用
		 */
		temp_cache_ = *data;
		return false;
	} catch(runtime_error& e) {
		ERROR_LOG("%s.",e.what());
	} catch(...) {

	}
	/*
	 * 解包出现错误(上一步解包过程中抛出异常)
	 * 不可恢复
	 */
	clearCache();
	current_function_ = unpack_functions_.begin();
	return false;
}
void GeneralPM::addUnpackFunc(std::function<GPMUnpackFunc> func)
{
	unpack_functions_.push_back(func);
	current_function_ = unpack_functions_.begin();
	clearCache();
}
void GeneralPM::clearUnpackFunc()
{
	unpack_functions_.clear();
	current_function_ = unpack_functions_.begin();
	clearCache();
}
void GeneralPM::clearCache()
{
	unpacked_data_.clear();
	temp_cache_.clear();
}
bool GeneralPM::findHead(const WBuffer& head,WBuffer* data_in_out,WBuffer* unused_data)
{
	if(data_in_out->size() < head.size()) return false;

	auto pos = data_in_out->search(head);

	unused_data->clear();

	if(-1 == pos) {
		SIMPLE_HEX_LOG(LL_INFO,data_in_out->data(),std::min<int>(32,data_in_out->size()),"DROP%d:",data_in_out->size());
		WARNING_LOG("Drop data size=%d.",data_in_out->size());
		data_in_out->clear();
		return false;
	}

	if(0 != pos) {
		SIMPLE_HEX_LOG(LL_INFO,data_in_out->data(),std::min<int>(32,pos),"DROP%d:",pos);
		WARNING_LOG("Drop data size=%d.",pos);
		data_in_out->right(data_in_out->size()-pos);
	}
	if(data_in_out->size()>head.size()) {
		data_in_out->moveRightTo(unused_data,data_in_out->size()-head.size());
	}
	return true;
}
bool GeneralPM::checkHead(const WBuffer& head,WBuffer* data_in_out,WBuffer* unused_data)
{
	unused_data->clear();

	if(data_in_out->size() < head.size()) return false;

	if(!equal(head.begin(),head.end(),data_in_out->begin())) {
		throw runtime_error("error head");
	}
	if(data_in_out->size()>head.size()) {
		data_in_out->moveRightTo(unused_data,data_in_out->size()-head.size());
	}
	return true;
}
