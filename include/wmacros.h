/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QCoreApplication>
#include <type_traits>
#include <cmacros.h>
#include <wlogmacros.h>

#define APPDIR(x) (QCoreApplication::applicationDirPath()+x)
#define DISABLE_COPY_AND_ASSIGN(Type) \
	Type(const Type& Type)=delete; \
	Type(Type&& Type)=delete; \
	Type& operator=(const Type& type)=delete; \
	Type& operator=(Type&& type)=delete
#define DISABLE_COPY(Type) \
	Type(const Type& Type)=delete; \
	Type(Type&& Type)=delete; 
#define DISABLE_ASSIGN(Type) \
	Type& operator=(const Type& type)=delete; \
	Type& operator=(Type&& type)=delete
#if __cplusplus < 201103L
#define override
#define final
#endif

