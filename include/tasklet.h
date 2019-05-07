/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
namespace WSpace
{
	template<class Func,typename DataType>
		class Runnable:public QRunnable {
			public:
				Runnable(Func func,DataType data):func_(func),private_data_(data){}
				virtual void run() {
					func_(private_data_);
				}
				Func     func_;
				DataType private_data_;
		};
	template<class Func,typename DataType>
		void taskletSchedule(Func func,DataType data) {
			Runnable<Func,DataType>* runnable = new Runnable<Func,DataType>(func,data);
			QThreadPool::globalInstance()->start(runnable);
		}
	void waitForDone();
}
