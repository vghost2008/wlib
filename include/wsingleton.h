/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <cmacros.h>
#include <new>
#include <assert.h>
#include <stdexcept>
#include <typeinfo>
/*
 * 单件模板
 * 可在多个库中使用
 */
template<class Type>
class WSingleton
{
	public:
		template<typename... DataType>
			static int init(DataType... argus) {
                if(wunlikely(Type::s_instance != nullptr)) {
					WARNING("重复初始化");
					return -1;
				}
				Type * p = nullptr;
				if((p= new(std::nothrow) Type(argus...)) == nullptr) {
					ERR("申请内存失败");
					return -1;
				}
                Type::s_instance = p;
				int res = p->minit();
				return res;
			}
		static int deinit() {
            if(wunlikely(nullptr == Type::s_instance)) {
				INFO("无需反初始化");
				return 0;
			}
            delete Type::s_instance;
            Type::s_instance = nullptr;
			return 0;
		}
		static bool isInited() {
            return nullptr != Type::s_instance;
		}
        static Type& instance() {
#ifndef NDEBUG
            if(wunlikely(nullptr == Type::s_instance)) {
				ERR("Not inited %s",typeid(Type).name());
                throw std::runtime_error(typeid(Type).name());
			}
#endif
            return *Type::s_instance;
		}
		template<typename... DataType>
            static Type& instance_i(DataType...argus) {
                if(wunlikely(nullptr == Type::s_instance)) {
					if(0 != init(argus...)) {
                        throw std::bad_alloc();
					}
				}
                return *Type::s_instance;
			}
	protected:
		virtual int minit();
		WSingleton(){}
};
template<class Type>
int WSingleton<Type>::minit() 
{ 
	return 0; 
}
