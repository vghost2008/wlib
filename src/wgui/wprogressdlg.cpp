#include "wprogressdlg.h"

using namespace std;
namespace WSpace
{
	ProgressHelper::ProgressHelper(const std::string& title)
	:title_(title)
	,progress_(0)
	,cancel_(false)
	{
	}
	void ProgressHelper::finish()
	{
		progress_ = 100;
	}
}

WProgressDlg::WProgressDlg(const QString& title, const function<int(QString*)>& get_progress, const function<void()>& cancel)
:QProgressDialog("进度","取消",0,100)
,get_progress_func_(get_progress)
,cancel_func_(cancel)
{
	setWindowTitle(title);
	setMinimumWidth(400);

	connect(this,&QProgressDialog::canceled,this,&WProgressDlg::slotCancel);
	connect(&timer_,&QTimer::timeout,this,&WProgressDlg::timerEvent);

	timer_.start(500);
}
WProgressDlg::WProgressDlg(WSpace::ProgressHelper& helper)
:WProgressDlg(helper.title(),[&helper](QString* text){return helper.progress(text);},[&helper](){ helper.cancel();})
{
}
void WProgressDlg::timerEvent()
{
	QString    text;
	const auto v    = get_progress_func_(&text);

	if(v >= maximum()) {
		accept();
	} else {
		setValue(v);
	}
	if(!text.isEmpty()) {
		setLabelText(text);
	}
}
void WProgressDlg::slotCancel()
{
	cancel_func_();
	reject();
}
