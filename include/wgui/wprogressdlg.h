_Pragma("once")
#include <QProgressDialog>
#include <QtCore>
#include <QString>
#include <functional>
#include <mutex>
#include <atomic>
namespace WSpace
{
	class ProgressHelper
	{
		public:
			ProgressHelper(const std::string& title);
			inline void setProgress(int v) {
				progress_.store(v);
			}
			inline int progress(QString* t)const { 
				if(nullptr != t)*t= text();
				return progress_.load(); 
			}
			inline void setProgressAndText(int v,const std::string& text) {
				std::lock_guard<std::mutex> guard{mtx_};
				progress_.store(v);
				text_ = text;
			}
			inline QString title()const {
				return title_.c_str();
			}
			inline void setText(const std::string& text) {
				std::lock_guard<std::mutex> guard{mtx_};
				text_ = text;
			}
			inline QString text()const {
				std::lock_guard<std::mutex> guard{mtx_};
				return text_.c_str();
			}
			bool isCanceled()const {
				return cancel_.load();
			}
			void cancel() {
				cancel_.store(true);
			}
			void finish();
		private:
			std::atomic_int    progress_;
			std::atomic_bool   cancel_;
			std::string        text_;
			std::string        title_;
			mutable std::mutex mtx_;
	};
}
class WProgressDlg:public QProgressDialog
{
	public:
		WProgressDlg(const QString& title,const std::function<int (QString*)>& get_progress,const std::function<void ()>& cancel);
		WProgressDlg(WSpace::ProgressHelper& helper);
	private:
		std::function<int (QString*)> get_progress_func_; //用于获取进度的函数
		std::function<void ()> cancel_func_; //用于取消的函数
		QTimer timer_;
	private:
		void timerEvent();
		void slotCancel();
};
