/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <list>
#include <wthreadtools.h>
#include <wbuffer.h>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <boost/scope_exit.hpp>

template<class DataType>
class WSFIFO
{
	private:
		typedef DataType            value_type;
	public:
		WSFIFO():size_(0) {
		}
		/*
		 * 从尾部添加元素
		 */
		bool enqueue(const DataType& data);
		bool enqueue(DataType&& data);
		/*
		 * 从首部移除元素, 如果队列为空直接返回false
		 */
		template<typename CheckFunc,class Rep,class Period>
			bool dequeue(const std::chrono::duration<Rep,Period>& time_out,CheckFunc func,DataType* d) {
				bool res = false;

				BOOST_SCOPE_EXIT_ALL(&res,this) {
					if(res) cond_.notify_all();
				};

				std::unique_lock<std::mutex> guard{mutex_};

				if(nullptr == d) return false;
				res = wait_condition_.wait_for(guard,time_out,[this,&func,&d]()->bool{
						auto it =  std::find_if(queue_.begin(),queue_.end(),func);
						if(it == queue_.end()) return false;
						*d = std::move(*it);
						queue_.erase(it);
						return true;
						});
				return res;
			}
		template<class Rep,class Period>
			bool dequeue(const std::chrono::duration<Rep,Period>& time_out,DataType* d) {
				bool res = false;

				BOOST_SCOPE_EXIT_ALL(&res,this) {
					if(res) cond_.notify_all();
				};

				std::unique_lock<std::mutex> guard{mutex_};

				if(nullptr == d) return false;


				if(queue_.empty())
					wait_condition_.wait_for(guard,time_out);
				if(!queue_.empty()) {
					res = true;
					*d = std::move(queue_.front());
					queue_.erase(queue_.begin());
				}
				return res;
			}
		template<typename CheckFunc>
			bool dequeue(CheckFunc func,DataType* d) {
				bool res = false;

				BOOST_SCOPE_EXIT_ALL(&res,this) {
					if(res) cond_.notify_all();
				};

				std::unique_lock<std::mutex> guard{mutex_};

				if(nullptr == d) return false;
				res = wait_condition_.wait(guard,[this,&func,&d]()->bool{
						auto it =  std::find_if(queue_.begin(),queue_.end(),func);
						if(it == queue_.end()) return false;
						*d = std::move(*it);
						queue_.erase(it);
						return true;
						});
				return res;
			}
		bool dequeue(DataType* d) {
			bool res = false;

			BOOST_SCOPE_EXIT_ALL(&res,this) {
				if(res) cond_.notify_all();
			};

			std::unique_lock<std::mutex> guard{mutex_};

			if(nullptr == d) return false;
			wait_condition_.wait(guard,[this]()->bool{
					return !queue_.empty();
					});
			res = true;
			*d = std::move(queue_.front());
			queue_.erase(queue_.begin());
			return res;
		}
		bool try_dequeue(DataType* d) {
			bool res = false;
			BOOST_SCOPE_EXIT_ALL(&res,this) {
				if(res)cond_.notify_all();
			};
			std::unique_lock<std::mutex> guard{mutex_};

			if(nullptr == d || queue_.empty()) {
				res = false;
				return res;
			}
			*d = std::move(queue_.front());
			queue_.erase(queue_.begin());
			res = true;
			return res;
		}
		template<typename CMP>
			bool remove(CMP f)
			{
				bool res = false;
				BOOST_SCOPE_EXIT_ALL(&res,this) {
					if(res) cond_.notify_all();
				};

				std::unique_lock<std::mutex> guard{mutex_};
				auto it = std::remove_if(queue_.begin(),queue_.end(),f);
				res = (it!=queue_.end());
				queue_.erase(it,queue_.end());
				return res;
			}
		template<class Containter>
			bool getAllData(Containter* out) {
				bool res = false;
				BOOST_SCOPE_EXIT_ALL(&res,this) {
					if(res) cond_.notify_all();
				};

				std::unique_lock<std::mutex> guard{mutex_};
				if(queue_.empty()) return res;
				res = true;
				out->assign(queue_.begin(),queue_.end());
				size_ = 0;
				queue_.clear();
				return res;
			}
		bool getAllData(std::list<DataType>* out) {
				bool res = false;
				BOOST_SCOPE_EXIT_ALL(&res,this) {
					if(res) cond_.notify_all();
				};
				out->clear();
				std::unique_lock<std::mutex> guard{mutex_};
				if(queue_.empty()) return res;
				res = true;
				out->splice(out->end(),queue_);
				size_ = 0;
				return res;
			}
		template<class Rep,class Period>
		bool getAllData(const std::chrono::duration<Rep,Period>& time_out,std::list<DataType>* out) {
				bool res = false;
				BOOST_SCOPE_EXIT_ALL(&res,this) {
					if(res) cond_.notify_all();
				};
				out->clear();
				std::unique_lock<std::mutex> guard{mutex_};

				if(queue_.empty()) {
					wait_condition_.wait_for(guard,time_out);
					if(queue_.empty())
						return res;
				}

				res = true;
				out->splice(out->end(),queue_);
				size_ = 0;
				return res;
			}
		/*
		 * 唤醒所有的等待线程，可以多次重复调用
		 */
		bool stop();
		inline size_t size()const{
			return size_;
		}
		inline unsigned realSize(){ 
			size_t    s     = 0;
			std::unique_lock<std::mutex> guard{mutex_};
			s = _size();
			return s;
		}
		inline bool empty()const {
			std::unique_lock<std::mutex> guard{mutex_};
			return queue_.empty();
		}
		void clear();
		inline size_t capacity()const { return capacity_; }
		void set_capacity(size_t cap) { capacity_ = cap; }
	public:
		std::list<DataType> queue_;
	protected:
		mutable std::mutex      mutex_;
		std::condition_variable wait_condition_; //出队列等等条件
		std::condition_variable cond_; //入队列等待条件
		std::atomic_uint        size_{0};
		size_t                  capacity_       = 0;   //==0时表示无限制
	private:
		inline unsigned _size()const {
			return queue_.size();
		}
	private:
		WSFIFO(const WSFIFO& WSFIFO)=delete; 
		WSFIFO(WSFIFO&& WSFIFO)=delete; 
		WSFIFO& operator=(const WSFIFO& type)=delete; 
		WSFIFO& operator=(WSFIFO&& type)=delete;
};
template<class DataType>
bool WSFIFO<DataType>::enqueue(const DataType& data)
{
	std::unique_lock<std::mutex> guard{mutex_};

	if((capacity() > 0) 
		&& queue_.size() > capacity())
		cond_.wait(guard);

	queue_.push_back(data);
	size_ = queue_.size();
	wait_condition_.notify_one();
	return true;
}
template<class DataType>
bool WSFIFO<DataType>::enqueue(DataType&& data)
{
	std::unique_lock<std::mutex> guard{mutex_};

	if((capacity() > 0) 
		&& queue_.size() > capacity())
		cond_.wait(guard);

	queue_.push_back(std::move(data));
	size_ = queue_.size();
	wait_condition_.notify_one();
	return true;
}
template<class DataType>
bool WSFIFO<DataType>::stop()
{
	wait_condition_.notify_all();
	cond_.notify_all();
	return true;
}
template<class DataType>
void WSFIFO<DataType>::clear()
{
	std::unique_lock<std::mutex> guard{mutex_};
	size_ = 0;
	queue_.clear();
}
