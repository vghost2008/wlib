/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <stdio.h>
#include <unistd.h>
#define __WLIB_VERSION 11
extern int getWlibVersion();
#define CHECK_WLIB_VERSION(x) \
if(getWlibVersion() != __WLIB_VERSION) { \
	printf("--------------------------------------------------------->\n"); \
	printf("Program %s compile with wlib version %d and use wlib version %d\n.",x,__WLIB_VERSION, \
	getWlibVersion()); \
	printf("<---------------------------------------------------------\n"); \
	sync(); \
	exit(-1); \
}
#define WEAK_CHECK_WLIB_VERSION(x) \
if(getWlibVersion() != __WLIB_VERSION) { \
	printf("--------------------------------------------------------->\n"); \
	printf("ERROR: Program %s compile with wlib version %d and use wlib version %d\n.",x,__WLIB_VERSION, \
	getWlibVersion()); \
	printf("<---------------------------------------------------------\n"); \
}
