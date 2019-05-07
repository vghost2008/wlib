/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#ifndef DISABLE_MAPVIEW
#include <QtCore>
#include <QtGui>
#include "wmapmanagerwig.h"
class WMapAttrWig;
class WMapManager:public QWidget
{
	public:
		WMapManager(WMap& map);
		~WMapManager();
		inline WMap& map() { return map_; }
		inline const WMap& map()const { return map_; }
		QHBoxLayout* bottomLayout();
	private:
		void initGUI();
	private:
		WMapAttrWig *map_attr_wig_  = nullptr;
		QLineEdit   *le_map_path_   = nullptr;
		QLineEdit   *le_map_filter_ = nullptr;
		QHBoxLayout *bottom_layout_ = nullptr;
		WMap        &map_;
	private:
		void slotGenerateMap();
		void slotSave();
		void slotCompile();
		void slotSelectMap();
};
#endif
