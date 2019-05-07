/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <math.h>
#include <stdio.h>
#include <cassert>
#include <wlib_configs.h>

#ifndef M_PI
#define M_PI 3.1415926
#endif
#ifndef M_PI_2
#define M_PI_2 ((M_PI)/2)
#endif
#ifndef M_PI_4
#define M_PI_4 ((M_PI)/4)
#endif

#ifdef Q_OS_WIN32
#define bzero(addr,size) memset(addr,0x00,size)
#endif

#define DEFINE_ENUM_WITH_SPAN(name,width) name,name##_END=name+width-1

#if WDEBUG_MESSAGE_LEVEL >= WDBG_LEVEL
#define DBG(fmt,arg...) printf("%s, F= %s,L= %d " fmt "\n",__FILE__,__func__,__LINE__,##arg)
#define WDEBUG__
#else
#define DBG(fmt,arg...) do{ \
}while(0)
#endif

#if WDEBUG_MESSAGE_LEVEL >= WINFO_LEVEL
#define INFO(fmt,arg...) printf("%s, L=%d " fmt "\n",__FILE__,__LINE__,##arg)
#else
#define INFO(fmt,arg...) do{ \
}while(0)
#endif
#if WDEBUG_MESSAGE_LEVEL >= WWARNING_LEVEL
#define WARNING(fmt,arg...) printf("%s, F=%s,L=%d " fmt "\n",__FILE__,__func__,__LINE__,##arg)
#else
#define WARNING(fmt,arg...) do{ \
}while(0)
#endif

#if WDEBUG_MESSAGE_LEVEL >= WERROR_LEVEL
#define ERR(fmt,arg...) printf("%s, F=%s,L=%d " fmt "\n",__FILE__,__func__,__LINE__,##arg)
#else
#define ERR(fmt,arg...) do{ \
}while(0)
#endif

#define wlikely(x) __builtin_expect(!!(x),1)
#define wunlikely(x) __builtin_expect((x),0)

#define ARRAY_SIZE(x) static_cast<int>(sizeof(x)/sizeof(x[0]))
#define ARRAY_END(x) ((x)+sizeof(x)/sizeof(x[0]))

#define woffsetof(TYPE,MEMBER) ((size_t)&((TYPE*)0)->MEMBER)
#define container_of(ptr,TYPE,MEMBER) ({\
		const decltype(((TYPE*)0)->MEMBER)* __mptr = (ptr); \
		(TYPE*)((char*)__mptr-woffsetof(TYPE,MEMBER)); })
