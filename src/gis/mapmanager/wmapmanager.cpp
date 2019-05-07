/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#ifndef DISABLE_MAPVIEW
#include <wgui.h>
#include <wmapmanager.h>
#include <toolkit.h>
#include <toolkit_qt.h>
#include <gltoolkit.h>
#include <wmacros.h>
#include <wmapdata.h>
#include <thread>
#include <future>
#include <chrono>
#include <boost/scope_exit.hpp>

using namespace WSpace;
using namespace std;

WMapManager::WMapManager(WMap& map)
:map_(map)
{
	initGUI();
	setMinimumSize(940,600);
	setWindowTitle("地图管理");
	setAttribute(Qt::WA_DeleteOnClose);
}
WMapManager::~WMapManager()
{
	qDebug()<<__func__;
}
void WMapManager::initGUI()
{
	QVBoxLayout *layout           = new QVBoxLayout(this);
	QHBoxLayout *hlayout0         = new QHBoxLayout;
	QHBoxLayout *hlayout1         = new QHBoxLayout;
	QPushButton *btn_ok           = new QPushButton("确定 &保存");
	QPushButton *btn_save         = new QPushButton("保存");
	QPushButton *btn_compile      = new QPushButton("编译");
	QPushButton *btn_generate_map = new QPushButton("生成地图");
	QPushButton *btn_select_map   = new QPushButton("...");
	auto        &map              = this->map();

	le_map_path_    =  new QLineEdit(map.file_path.c_str());
	le_map_filter_  =  new QLineEdit("*.shp");
	map_attr_wig_   =  new WMapAttrWig;
	bottom_layout_  =  new QHBoxLayout;
	map_attr_wig_->setLayouts(map.layouts);

	//layout->setSpacing(0);
	layout->setContentsMargins(0,0,0,5);
	layout->addWidget(new WPixmapLabel("地图管理"));
	layout->addWidget(map_attr_wig_);
	hlayout0->addWidget(new QLabel("地图路径:"));
	hlayout0->addWidget(le_map_path_);
	hlayout0->addWidget(btn_select_map);
	hlayout0->addWidget(new QLabel("地图类型:"));
	hlayout0->addWidget(le_map_filter_);
	le_map_filter_->setMaximumWidth(80);
	hlayout1->addStretch();
	hlayout1->addLayout(bottom_layout_);
	hlayout1->addWidget(btn_generate_map);
	hlayout1->addWidget(btn_save);
	hlayout1->addWidget(btn_compile);
	hlayout1->addWidget(btn_ok);
	layout->addLayout(hlayout0);
	layout->addLayout(hlayout1);

	le_map_path_->setFocusPolicy(Qt::NoFocus);

	connect(btn_select_map,&QPushButton::clicked,this,&WMapManager::slotSelectMap);
	connect(btn_generate_map,&QPushButton::clicked,this,&WMapManager::slotGenerateMap);
	connect(btn_save,&QPushButton::clicked,this,&WMapManager::slotSave);
	connect(btn_compile,&QPushButton::clicked,this,&WMapManager::slotCompile);
}
QHBoxLayout* WMapManager::bottomLayout()
{
	return bottom_layout_;
}
void WMapManager::slotGenerateMap()
{
	auto map_path = QFileDialog::getSaveFileName(this,"指定地图文件",map().file_path.c_str(),"XML file (*.xml);;All file (*)");

	if(map_path.isEmpty()) return;

	auto file_list = fileNameListInDir(QFileInfo(map_path).absolutePath(),le_map_filter_->text().split(';'));

	WMap        map;
	WMapLayout  l;
	auto       &attr = l.attr;

	toGLColor4f(QColor(128,128,128),attr.line_color);
	toGLColor4f(QColor(255,255,255),attr.fill_color);
	toGLColor4f(QColor(0,0,0),attr.text_color);
	attr.line_width  =  1.0;
	attr.text_size   =  8;
	attr.show_label  =  0;
	l.visible        =  1;

	for(int i=0; i<file_list.size(); ++i) {
		l.file_name = file_list[i].toUtf8().data();
		map.layouts.push_back(l);
	}

	map.file_path= map_path.toUtf8().data();

	wmap::saveXmlMap(map_path,map);
}
void WMapManager::slotSave()
{
	if(le_map_path_->text().toLower().endsWith(".xml")) {
		if(map_attr_wig_->getLayouts(&map().layouts)) {
			wmap::saveMap(le_map_path_->text(),map());
		}
	} else {
		vector<WMapLayout> layouts;
		if(map_attr_wig_->getLayouts(&layouts)) {
			if(wmap::updateMapLayouts(layouts,&map())) {
				wmap::saveMap(le_map_path_->text(),map());
			}
		}
	}
}
void WMapManager::slotCompile()
{
	static volatile bool busy = false;
	if(busy) return;
	busy = true;
	BOOST_SCOPE_EXIT_ALL() {
		busy = false;
	};

	auto map_path = QFileDialog::getSaveFileName(this,"指定地图文件",map().file_path.c_str(),"wmap file (*.wmap);; All file (*)");

	if(map_path.isEmpty()) return;

	auto map = this->map();

	auto it = remove_if(map.layouts.begin(),map.layouts.end(),[](const WMapLayout& l) { return !l.visible; });
	map.layouts.erase(it,map.layouts.end());

	packaged_task<void()> task([&map]() {
			wmap::loadMapFile(&map);
			});
	auto fu = task.get_future();
	QApplication::setOverrideCursor(Qt::WaitCursor);
	try {
		std::thread t(std::move(task));
		while(fu.wait_for(chrono::milliseconds(100)) != std::future_status::ready) {
			qApp->processEvents(QEventLoop::ExcludeUserInputEvents|QEventLoop::ExcludeSocketNotifiers);
		}
		t.join();
	} catch(...) {}
	QApplication::setOverrideCursor(Qt::ArrowCursor);
	wmap::saveMap(map_path,map);
}
void WMapManager::slotSelectMap()
{
	auto file_path = QFileDialog::getOpenFileName(this,"打开地图文件.",le_map_path_->text(),"地图文件(*.xml);;All File(*)");

	if(file_path.isEmpty()) return;

	if(wmap::readMap(file_path,&map())) {
		map_attr_wig_->setLayouts(map().layouts);
		le_map_path_->setText(file_path);
	} else {
		AERROR_LOG("打开地图文件失败");
	}
}
#endif
