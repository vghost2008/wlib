/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#define ENABLE_IO_DEVICE_MONITOR  //打开用于监测原始数据
enum WIODeviceFlag
{
	WIODF_NULL         =   0x00000000   ,   
	WIODF_BAD_DEVICE   =   0x00000001   ,   
};
enum WIODeviceOption
{
	WIODO_PACK_MODEL_OWNERSHIP   ,   
	WIODO_SOCKET                 ,   
	WIODO_TIMEOUT                ,   
};
enum WIODeviceDirection
{
	WIODD_READ    =   0,
	WIODD_WRITE   =   1,
};
struct IOTimeout
{
	unsigned connect;
	unsigned read;
	unsigned write;
};
typedef void (*IOMonitorFunc)(const void* data,size_t size,WIODeviceDirection direction,unsigned id,void* private_data);
#define IO_ID(x) unsigned(x)
struct WIOMonitorData
{
	IOMonitorFunc  func;
	void          *private_data;
};
