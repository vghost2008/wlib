/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include "packetmodule.h"
/*
 * 如果成功解包，返回true,并根据需要设置data_in_out,如果解包时有未使用的数据，放在unused_data中
 * data_in_out中仅为与当前解包函数匹配的数据
 * 如果还未完成解包，返回false,不需要设置unused_data,程序会自动将data_in_out储存起来供下次使用
 * 如果出现异常，无法解包(包括需要丢弃部分数据才能解包的情况)，抛出runtime_error
 * 如果是函数是用于查找包头，需要丢弃部分数据，不需要抛出runtime_error,但是为了检查是否是包头
 * 需要抛出异常
 */
using GPMUnpackFunc = bool (WBuffer* data_in_out,WBuffer* unused_data);
class GeneralPM:public WAbstractPacketModule
{
	public:
		GeneralPM();
		virtual bool unpack(WBuffer* data)override;
		/*
		 * 添加一个解包函数
		 */
		void addUnpackFunc(std::function<GPMUnpackFunc> func);
		/*
		 * 添加类ClassType的成员函数FuncType为解包函数
		 */
		template<class ClassType,class FuncType>
			inline void addUnpackMemFunc(ClassType* ptr,FuncType func) {
				addUnpackFunc([ptr,func](WBuffer* data_in_out,WBuffer* unused_data)->bool{
						return (ptr->*func)(data_in_out,unused_data);
						});
			}
		/*
		 * 清除解包函数
		 */
		void clearUnpackFunc();
	public:
		/*
		 * 查找包头的解包函数
		 * 为了查找包头需要丢弃数据时不会抛异常
		 */
		static bool findHead(const WBuffer& head,WBuffer* data_in_out,WBuffer* unused_data);
		/*
		 * 检查数据是否是以指定的包头开始
		 * 如果不是则抛出异常
		 */
		static bool checkHead(const WBuffer& head,WBuffer* data_in_out,WBuffer* unused_data);
		/*
		 * 获取指定长度的数据
		 */
		template<unsigned size>
			static bool getDataNum(WBuffer* data_in_out,WBuffer* unused_data) {
				if(data_in_out->size() < size) {
					return false;
				}
				if(data_in_out->size()>size)
					data_in_out->moveRightTo(unused_data,data_in_out->size()-size);
				return true;
			}
	public:
		/*
		 * 返回已经解包的数据
		 */
		inline const WBuffer& unpacked_data()const {
			return unpacked_data_;
		}
	protected:
		void clearCache();
	private:
		std::vector<std::function<GPMUnpackFunc>>           unpack_functions_;
		std::vector<std::function<GPMUnpackFunc>>::iterator current_function_;
		WBuffer                                             unpacked_data_;
};
