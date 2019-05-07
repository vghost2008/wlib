/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
namespace WSpace {
	enum class MutexState:bool
	{
		MS_LOCKED     ,   
		MS_UNLOCKED   ,   
	};
	enum class Direction:unsigned char
	{
		D_EAST    ,   
		D_NORTH   ,   
		D_WEST    ,   
		D_SOUTH   ,   
	};
	enum class Corner:unsigned char
	{
		C_TopLeft       ,   
		C_BottomLeft    ,   
		C_BottomRight   ,   
		C_TopRight      ,   
	};
	enum LinePos
	{
		LP_BEGIN    =   0x01   ,   
		LP_END      =   0x02   ,   
		LP_MID      =   0x04   ,   
		LP_RANDOM   =   0x08   ,   
	};
}
