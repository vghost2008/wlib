/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <QtCore>
#include <QtWidgets>
#include "wioanalysis_fwd.h"

class WIOAnalysis;
struct WIOAnalysisUISet
{
	unsigned   id;
	QCheckBox *cb_is_show;
	QComboBox *comb_view_mode;
	QComboBox *comb_monitor_type;
};
class WIOAnalysisSet:public QDialog
{
	private:
		enum UIMask {
			UM_CHECKBOX       =   0x01000000,
			UM_VIEW_MODE      =   0x02000000,
			UM_MONITOR_TYPE   =   0x04000000,
			UM_MASK           =   0x00FFFFFF, //相应的位用于位置索引
		};
	public:
		WIOAnalysisSet(const std::map<unsigned,WIOAnalysisItem>& items);
		void updateItems(std::map<unsigned,WIOAnalysisItem>* items);
	private:
		void slotUIEvent(int index);
		void initGUI();
	private:
		std::map<unsigned,WIOAnalysisItem> items_;
		std::map<unsigned,WIOAnalysisUISet> index_to_ui_;
		QSignalMapper            signal_mapper_;
};
