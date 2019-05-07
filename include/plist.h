/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <list>
#include <wthread.h>
template<class DataType>
class WPList:public std::list<DataType>
{
	public:
		int lock_shared()const;
		int lock()const;
		int unlock()const;
		int unlock_shared()const;
		int safe_push_back(const DataType& data);
		int safe_pop_front(DataType* data);
		int safe_clear();
		inline bool try_lock() { return mutex_.try_lock(); }
		inline bool try_lock_shared() { return mutex_.try_lock_shared(); }
		inline size_t safe_size()const {
			std::lock_guard<std::shared_mutex> guard{mutex_};
			return std::list<DataType>::size();
		}
		inline size_t safe_empty()const {
			std::lock_guard<std::shared_mutex> guard{mutex_};
			return std::list<DataType>::empty();
		}
	private:
		mutable std::shared_mutex mutex_;
};
template<class DataType>
int WPList<DataType>::lock_shared()const
{
	mutex_.lock_shared();
	return 0;
}
template<class DataType>
int WPList<DataType>::lock()const
{
	mutex_.lock();
	return 0;
}
template<class DataType>
int WPList<DataType>::unlock()const
{
	mutex_.unlock();
	return 0;
}
template<class DataType>
int WPList<DataType>::unlock_shared()const
{
	mutex_.unlock_shared();
	return 0;
}
template<class DataType>
int WPList<DataType>::safe_clear()
{
	std::lock_guard<std::shared_mutex> guard{mutex_};
	std::list<DataType>::clear();
	return 0;
}
template<class DataType>
int WPList<DataType>::safe_push_back(const DataType& data)
{
	std::lock_guard<std::shared_mutex> guard{mutex_};
	std::list<DataType>::push_back(data);
	return 0;
}
template<class DataType>
int WPList<DataType>::safe_pop_front(DataType* data)
{
	int res = 0;
	std::lock_guard<std::shared_mutex> guard{mutex_};

	if(!std::list<DataType>::empty()) {
		*data = std::list<DataType>::front();
		std::list<DataType>::pop_front();
		res = 0;
	} else {
		res = -1;
	}
	return res;
}
