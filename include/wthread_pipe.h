/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 建立一个数据及多个操作连接而成的管道，每个操作都在不同的线程中执行
 *   使用示例:
 *   std::vector<D0> data....;
 *   D1 func1(const D0& v) 
 *   {
 *   }
 *   D2 func2(const D1& v)
 *   {
 *   }
 *   ...
 *   WTPipe pipe;
 *   pipe>>data>>func1>>func2;
 *   pipe析构时自动完成所有操作
 *
 ********************************************************************************/
 
_Pragma("once")
#include <thread>
#include <stdexcept>
#include <sfifo.h>
#include <chrono>
#include <toolkit.h>
namespace WSpace
{
	/*
	 * 管道节点的基类
	 * 用户不直接使用管道节点，仅内部使用
	 */
	class WTPipeBase
	{
		public:
			virtual ~WTPipeBase();
			inline bool is_finish()const { return is_finish_; }
		protected:
			std::thread      t_;
			std::atomic_bool is_finish_  {false};
	};
	/*
	 * 用于表示会产生数据的管道节点
	 * Producer:继承自WTPipeProducer的类(递归模板)
	 * T0:输入类型
	 * T1:输出类型
	 */
	template<typename Producer,typename T0,typename T1>
		class WTPipeProducer
		{
			public:
				using in_type       =  remove_cvr_t<T0>;
				using out_type      =  remove_cvr_t<T1>;
			public:
				static inline void dequeue(Producer& p,std::list<out_type>* outs) {
					while(true) {
						if(p.fifo_.empty() && p.is_finish()) throw std::out_of_range("out of range");
						if(p.fifo_.getAllData(std::chrono::seconds(3),outs)) break;
					}
				}
				template<typename DT,typename=std::enable_if_t<!std::is_same<DT,out_type>::value>,
					typename=std::enable_if_t<std::is_convertible<out_type,DT>::value>>
						static inline void dequeue(Producer& p,std::list<DT>* outs) {
							std::list<out_type> tmp;
							dequeue(p,&tmp);
							outs->assign(tmp.begin(),tmp.end());
						}
			protected:
				WSFIFO<out_type> fifo_;
		};
	/*
	 * 用于表示会使用消费数据的管道节点
	 * Consume;继承自WTPipeConsume的类型(递归定义)
	 * T0:输入类型
	 * T1:输出类型
	 */
	template<typename Consume,typename T0,typename T1>
		class WTPipeConsume
		{
			public:
				using in_type       =  remove_cvr_t<T0>;
				using out_type      =  remove_cvr_t<T1>;
		};
	/*
	 * 管道的开始节点
	 */
	template<typename T0>
		class WTPipeBegin:public WTPipeBase,public WTPipeProducer<WTPipeBegin<T0>,void,remove_cvr_t<typename T0::value_type>>
	{
		public:
			using in_type       =  void;
			using out_type      =  remove_cvr_t<typename T0::value_type>;
			using self_type     =  WTPipeBegin<T0>;
			using ProducerType  =  WTPipeProducer<self_type,in_type,out_type>;
		public:
			WTPipeBegin(const T0& data,size_t capacity=0){
				if(capacity>0)ProducerType::fifo_.set_capacity(capacity);
				t_ = std::thread([&data,this]() {
						for(auto& v:data) {
						this->fifo_.enqueue(v);
						}
						is_finish_ = true;
						this->fifo_.stop();
						});
			}
	};
	/*
	 * 管道的中间节点，也可以作为结束节点
	 */
	template<typename T0,typename T1>
		class WTPipeMid:public WTPipeBase,public WTPipeProducer<WTPipeMid<T0,T1>,T0,T1>,public WTPipeConsume<WTPipeMid<T0,T1>,T0,T1>
	{
		public:
			using in_type       =  remove_cvr_t<T0>;
			using out_type      =  remove_cvr_t<T1>;
			using self_type     =  WTPipeMid<T0,T1>;
			using ProducerType  =  WTPipeProducer<self_type,in_type,out_type>;
			using ConsumeType   =  WTPipeConsume<self_type,in_type,out_type>;
		public:
			template<typename Last>
				WTPipeMid(std::function<T1 (T0 )> f,Last& l,size_t capacity=0){
					if(capacity>0)ProducerType::fifo_.set_capacity(capacity);
					t_ = std::thread([this,&l,f]() {
							std::list<in_type> datas;
							try {

							while(true) {
							Last::dequeue(l,&datas);
							for(auto& d:datas){
							this->fifo_.enqueue(f(d));
							}
							} //endl while

							} catch(std::out_of_range ){
							is_finish_ = true;
							} catch(...) {
							is_finish_ = true;
							}
							this->fifo_.stop();
							});
				}
	};
	/*
	 * 管道的结束节点，结束节点后不能再有其它节点
	 */
	template<typename T0,typename T1=void>
		class WTPipeEnd:public WTPipeBase,public WTPipeConsume<WTPipeEnd<T0,T1>,T0,T1>
	{
		public:
			using in_type      =  remove_cvr_t<T0>;
			using out_type     =  void;
			using self_type    =  WTPipeEnd<T0,T1>;
			using ConsumeType  =  WTPipeConsume<self_type,in_type,out_type>;
		public:
			template<typename Last>
				WTPipeEnd(std::function<T1 (T0)> f,Last& l){
					t_ = std::thread([this,&l,f]() {
							std::list<in_type> datas;
							try {

							while(true) {
							Last::dequeue(l,&datas);
							for(auto& d:datas) 
							f(d);
							} //endl while

							} catch(std::out_of_range ){
							is_finish_ = true;
							} catch(...) {
							is_finish_ = true;
							}

							});
				}

	};
	/*
	 * 用于设置fifo的缓存能力
	 */
	struct PFifoCap
	{
		PFifoCap(size_t cap=0):cap_(cap){}
		size_t cap_;
	};

	class WTPipe
	{
		public:
			template<typename T>
				class WTPipeWrapper
				{
					public:
						using in_type = typename T::out_type;
					public:
						WTPipeWrapper(WTPipe& pipe,std::shared_ptr<T> last):pipe_(pipe),last_(last){}
						/*
						 * 用于处理函数指针/普通函数
						 */
						template<typename T0,typename T1,typename PT=WTPipeMid<remove_cvr_t<T0>,remove_cvr_t<T1>>>
							WTPipeWrapper<PT> operator>>(T1 (*func)(T0)) {
								auto res = pipe_.push_back(func,last_);
								return WTPipeWrapper<PT>(pipe_,res);
							}
						/*
						 * 用于处理function对象
						 */
						template<typename T0,typename T1,typename PT=WTPipeMid<remove_cvr_t<T0>,remove_cvr_t<T1>>>
							WTPipeWrapper<PT> operator>>(std::function<T1 (T0)> func) {
								auto res = pipe_.push_back(func,last_);
								return WTPipeWrapper<PT>(pipe_,res);
							}
						/*
						 * 用于处理拟函数
						 */
						template<typename Func,typename PT=WTPipeMid<std::result_of_t<Func(in_type)>,in_type>,
							typename=std::enable_if_t<!std::is_same<std::result_of_t<Func(in_type)>,void>::value>>
								WTPipeWrapper<PT> operator>>(Func func) {
									auto f = std::function<typename PT::out_type (in_type)>(func);
									auto res = pipe_.push_back(f,last_);
									return WTPipeWrapper<PT>(pipe_,res);
								}


						template<typename T0>
							void operator>>(void (*func)(T0)) {
								pipe_.push_back(func,last_);
							}
						template<typename T0>
							void operator>>(std::function<void (T0)>& func) {
								pipe_.push_back(func,last_);
							}
						template<typename Func,
							typename=std::enable_if_t<std::is_same<std::result_of_t<Func(in_type)>,void>::value>>
								void operator>>(Func func) {
									auto f = std::function<void (in_type)>(func);
									pipe_.push_back(f,last_);
								}
						WTPipeWrapper& operator>>(const PFifoCap& cap) {
							pipe_>>cap;
							return *this;
						}
						WTPipeWrapper& operator<<(const PFifoCap& cap) {
							pipe_<<cap;
							return *this;
						}
					private:
						WTPipe& pipe_;
						std::shared_ptr<T> last_;
				};
		public:
			/*
			 * 板构时阻塞直到完成所有操作
			 */
			~WTPipe();
			template<typename T0,typename PT=WTPipeBegin<T0>>
				WTPipeWrapper<PT> operator>>(const T0& data) {
					auto res = push_back(data);
					return WTPipeWrapper<PT>(*this,res);
				}
			WTPipe& operator>>(const PFifoCap& cap) {
				set_fifo_cap(cap.cap_);
				return *this;
			}
			WTPipe& operator<<(const PFifoCap& cap) {
				set_fifo_cap(cap.cap_);
				return *this;
			}
		private:
				/*
				 * 管道开始为一个放在容器中的数据
				 */
			template<typename T0,typename=typename T0::value_type>
				std::shared_ptr<WTPipeBegin<T0>> push_back(const T0& data) {
					auto res = std::make_shared<WTPipeBegin<T0>>(data,fifo_cap_);
					datas_.push_back(res);
					return res;
				}
				/*
				 * 后面的为函数，输入为前一个的输出
				 */
			template<typename T0,typename T1,typename T,typename PT=WTPipeMid<remove_cvr_t<T0>,remove_cvr_t<T1>>>
				std::shared_ptr<PT> push_back(T1 (*func)(T0),T& l) {
					auto res = std::make_shared<PT>(func,*l,fifo_cap_);
					datas_.push_back(res);
					return res;
				}
			template<typename T0,typename T1,typename T,typename PT=WTPipeMid<remove_cvr_t<T0>,remove_cvr_t<T1>>>
				std::shared_ptr<PT> push_back(std::function<T1 (T0)>& func,T& l) {
					auto res = std::make_shared<PT>(func,*l,fifo_cap_);
					datas_.push_back(res);
					return res;
				}
			template<typename T0,typename T,typename PT=WTPipeEnd<remove_cvr_t<T0>>>
				std::shared_ptr<PT> push_back(void (*func)(T0),T& l) {
					auto res = std::make_shared<PT>(func,*l);
					datas_.push_back(res);
					return res;
				}
			template<typename T0,typename T,typename PT=WTPipeEnd<remove_cvr_t<T0>>>
				std::shared_ptr<PT> push_back(std::function<void (T0)>& func,T& l) {
					auto res = std::make_shared<PT>(func,*l);
					datas_.push_back(res);
					return res;
				}
			/*
			 * 每个节点使用的fifo缓存能力，默认为无限制
			 */
			inline void set_fifo_cap(size_t cap){ fifo_cap_ = cap; }
		private:
			std::vector<std::shared_ptr<WTPipeBase>> datas_;
			size_t fifo_cap_ = 0;
	};
}
