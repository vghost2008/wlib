/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
enum IOError
{
	IOE_NO_ERROR      ,  
	IOE_NORMAL_ERROR  ,  
	IOE_CHECK         ,  
	IOE_REPEAT_FRAME  ,  
	IOE_NO_ACK        ,  
	IOE_GET_NAK       ,  
	IOE_OACK          ,  
	IOE_TIME_OUT      ,  
	IOE_PACK          ,  
	IOE_UNPACK        ,  
	IOE_PM_NEEDED     ,   //操作必须要打包解包模块
};
