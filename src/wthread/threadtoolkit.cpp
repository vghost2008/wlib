/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <threadtoolkit.h>
using namespace WSpace;
using namespace std;
thread_group::thread_group()
{
}
thread_group::~thread_group()
{
}
void thread_group::join_all() 
{
	lock_guard<mutex> guard{mtx_};
	for(auto& t:thread_list_) {
		t.join();
	}
}
