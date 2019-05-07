#include <wthread_pipe.h>
namespace WSpace
{
	WTPipeBase::~WTPipeBase()
	{
		if(t_.joinable())t_.join();
	}
	WTPipe::~WTPipe()
	{
		for(auto it = datas_.rbegin(); it!=datas_.rend(); ++it)
			it->reset();
	}
}
