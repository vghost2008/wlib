/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "tasklet.h"
namespace WTasklet
{
	void waitForDone() 
	{
		QThreadPool::globalInstance()->waitForDone();
	}
}
