/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <cmacros.h>
#include <new>
#include <assert.h>
#include <exception>
/*
 * 单件模板
 * 仅在一个库中使用(兼容win32)
 */
template<class Type>
class LSingleton
{
	public:
		template<typename... DataType>
			static int init(DataType... argus) {
				if(wunlikely(s_instance != nullptr)) {
					WARNING("重复初始化");
					return -1;
				}
				Type * p = nullptr;
				if((p= new(std::nothrow) Type(argus...)) == nullptr) {
					ERR("申请内存失败");
					return -1;
				}
				s_instance = p;
				int res = p->minit();
				return res;
			}
		static int deinit() {
			if(wunlikely(nullptr == s_instance)) {
				INFO("无需反初始化");
				return 0;
			}
			delete s_instance;
			s_instance = nullptr;
			return 0;
		}
		static bool isInited() {
			return nullptr != s_instance;
		}
		static Type& instance() {
#ifndef NDEBUG
			if(wunlikely(nullptr == s_instance))
				throw std::bad_exception();
#endif
			return *s_instance;
		}
		template<typename... DataType>
			static Type& instance_i(DataType...argus) {
				if(wunlikely(nullptr == s_instance)) {
					if(0 != init(argus...)) {
						throw std::bad_alloc();
					}
				}
				return *s_instance;
			}
	protected:
		virtual int minit();
		static Type* s_instance;
		LSingleton(){}
};
template<class Type>
int LSingleton<Type>::minit() 
{ 
	return 0; 
}
template<class Type>
Type* LSingleton<Type>::s_instance = nullptr;
