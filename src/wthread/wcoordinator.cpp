#include <wcoordinator.h>
namespace WSpace
{
	WCoordinator::WCoordinator()
	{
		reset_delayer();
	}
	WCoordinator::~WCoordinator()
	{
	}
	void WCoordinator::request_stop()
	{
		should_stop_ = true;
		cv_.notify_all();
	}
}
