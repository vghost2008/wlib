/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <xmlwidgetview.h>
#include <toolkit.h>
#include <mutex>
#include <wgui/wgui.h>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/size.hpp>

using namespace std;

std::array<XMLWidgetView::MakeEditorFunc,IT_NR> XMLWidgetView::make_editor_funcs_;
std::array<XMLWidgetView::MakeEditorFunc,IT_NR> XMLWidgetView::make_readonly_editor_funcs_;

typedef boost::mpl::vector<void,
		StringXMLWidgetVDImp,
		IntXMLWidgetVDImp,
		UIntXMLWidgetVDImp,
		FloatXMLWidgetVDImp,
		ListXMLWidgetVDImp,
		DListXMLWidgetVDImp,
		HexXMLWidgetVDImp,
		TimeXMLWidgetVDImp,
		IPXMLWidgetVDImp,
		FileXMLWidgetVDImp,
		DirXMLWidgetVDImp,
		ColorXMLWidgetVDImp,
		BoolXMLWidgetVDImp,
		StatusXMLWidgetVDImp,
		EnumXMLWidgetVDImp
		> EditorTypes;
typedef boost::mpl::vector<void,
		StringXMLWidgetVDImp,
		StringXMLWidgetVDImp,
		StringXMLWidgetVDImp,
		StringXMLWidgetVDImp,
		StringXMLWidgetVDImp,
		StringXMLWidgetVDImp,
		StringXMLWidgetVDImp,
		StringXMLWidgetVDImp,
		StringXMLWidgetVDImp,
		StringXMLWidgetVDImp,
		StringXMLWidgetVDImp,
		ColorXMLWidgetVDImp,
		StringXMLWidgetVDImp,
		StatusXMLWidgetVDImp,
		StringXMLWidgetVDImp
		> ReadOnlyEditorTypes;
static_assert(IT_NR==boost::mpl::size<EditorTypes>::type::value,"error size");
static_assert(IT_NR==boost::mpl::size<ReadOnlyEditorTypes>::type::value,"error size");
template<int index>
struct MakeEditor{
	void operator()(std::array<XMLWidgetView::MakeEditorFunc,IT_NR>* funcs) {
		if(index>1) {
			(*funcs)[index] = [](std::shared_ptr<WXmlItem>& item,int gid) { return std::make_unique<typename boost::mpl::at_c<EditorTypes,index>::type>(item,gid);};
			MakeEditor<index-1>()(funcs);
		} else if(index == 1) {
			(*funcs)[index] = [](std::shared_ptr<WXmlItem>& item,int gid) { return std::make_unique<typename boost::mpl::at_c<EditorTypes,index>::type>(item,gid);};
		}
	}
};
template<int index>
struct ReadOnlyMakeEditor{
	void operator()(std::array<XMLWidgetView::MakeEditorFunc,IT_NR>* funcs) {
		if(index>1) {
			(*funcs)[index] = [](std::shared_ptr<WXmlItem>& item,int gid) { return std::make_unique<typename boost::mpl::at_c<ReadOnlyEditorTypes,index>::type>(item,gid);};
			ReadOnlyMakeEditor<index-1>()(funcs);
		} else if(index == 1) {
			(*funcs)[index] = [](std::shared_ptr<WXmlItem>& item,int gid) { return std::make_unique<typename boost::mpl::at_c<ReadOnlyEditorTypes,index>::type>(item,gid);};
		}
	}
};
template<>
struct MakeEditor<0>{
	void operator()(std::array<XMLWidgetView::MakeEditorFunc,IT_NR>* ) { }
};
template<>
struct ReadOnlyMakeEditor<0>{
	void operator()(std::array<XMLWidgetView::MakeEditorFunc,IT_NR>* ) { }
};

XMLWidgetView::XMLWidgetView(WXmlData& data,int column_count)
:data_(data)
,column_count_(column_count)
{
	static once_flag init_flag;
	call_once(init_flag,initMakeEditorFuncs);
	initGUI();
} 
void XMLWidgetView::initGUI()
{
	auto      widget        = new QWidget;
	auto      layout        = new QGridLayout(widget);
	QColor    colors[]      = {QColor(0,92,175),QColor(159,53,58),QColor(226,148,59),QColor(111,51,129)};
	int       out_row       = 0;
	const int rcolumn_count = column_count() *3;
	int       column;
	int       group_index   = 0;
	int       width         = 0;

	for(int i=0; i<column_count(); ++i) {
		layout->setColumnMinimumWidth(i*3+2,30);
		layout->setColumnStretch(i*3,1);
		layout->setColumnStretch(i*3+1,1);
	}

	layout->setHorizontalSpacing(8);

	for(auto& g:data_.groups()) {
		auto color_index = (group_index++)%ARRAY_SIZE(colors);
		auto label       = new WColorLabel(g->name(),colors[color_index]);

		layout->addWidget(label,out_row,0,1,rcolumn_count);

		++out_row;
		column=0;
		for(auto& item:g->items()) {
			try {
				if(item->flag().test(XIF_HIDE_ITEM)) continue;
			} catch(...) {
			}
			auto imp = createEditor(item,g->id());

			/*
			 * item的宽度不能超过column_count()
			 */
			if(item->width()>column_count())
				item->setWidth(column_count());

			width = column_count()-column;
			/*
			 * 如果当前行剩余的宽度不满足需求则另起一行
			 */
			if(width < item->width()) {
				column=0;
				++out_row;
			}
			auto title = item->name();
			if(!title.isEmpty() && title.right(1) != ":") title += ":";

			layout->addWidget(new QLabel(title),out_row,column*3,Qt::AlignRight);
			layout->addWidget(imp->createWidget(this),out_row,column*3+1,1,localWidth(item));
			if(item->is_arithmetic() && !item->unit().isEmpty()) {
				layout->addWidget(new QLabel(item->unit()),out_row,column*3+1+localWidth(item));
			}

			imp->setEditorData();
			imps_.push_back(std::move(imp));

			column += localWidth(item);
			if(item->margin() >= 0) {
				column += item->margin();
				if(column >= column_count()) {
					column = 0;
					++out_row;
				}
			} else {
				++out_row;
				column = 0;
			}

		}
		if(column != 0) ++out_row;
	}
	setWidget(widget);
	widget->setMinimumWidth(390*column_count());
	setAlignment(Qt::AlignHCenter);
	setMinimumWidth(10+390*column_count());
	setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	layout->setContentsMargins(0,0,0,0);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
unique_ptr<AbstractXMLWidgetVDImp>  XMLWidgetView::createEditor(shared_ptr<WXmlItem>& item,int gid)
{
	if(item->type()>0 && item->type()<IT_NR) {
		if(item->isReadOnly()) {
			return (make_readonly_editor_funcs_[item->type()])(item,gid);
		} else {
			auto res = (make_editor_funcs_[item->type()])(item,gid);
			connect(res.get(),&AbstractXMLWidgetVDImp::sigValueChanged,this,&XMLWidgetView::sigValueChanged);
			return res;
		}
	} else {
		return nullptr;
	}
}
bool XMLWidgetView::setModelData()
{
	for(auto& vd:imps_) {
		vd->setModelData();
	}
	return true;
}
bool XMLWidgetView::setModelData(int gid)
{
	for(auto& vd:imps_) {
		if(vd->gid() != gid) continue;
		vd->setModelData();
	}
	return true;
}
bool XMLWidgetView::setModelData(int gid, int iid)
{
	for(auto& vd:imps_) {
		if((vd->gid() != gid) || ( vd->iid() != iid)) continue;
		vd->setModelData();
		break;
	}
	return true;
}
bool XMLWidgetView::setViewData()
{
	for(auto& vd:imps_) {
		vd->setEditorData();
	}
	return true;
}
bool XMLWidgetView::setViewData(int gid)
{
	for(auto& vd:imps_) {
		if(vd->gid() != gid) continue;
		vd->setEditorData();
	}
	return true;
}
bool XMLWidgetView::setViewData(int gid, int iid)
{
	for(auto& vd:imps_) {
		if((vd->gid() != gid) || ( vd->iid() != iid)) continue;
		vd->setEditorData();
	}
	return true;
}
int XMLWidgetView::localWidth(std::shared_ptr<WXmlItem>& item)
{
	return 1+3*(item->width()-1);
}
void XMLWidgetView::initMakeEditorFuncs()
{
	MakeEditor<IT_NR-1>()(&make_editor_funcs_);
	ReadOnlyMakeEditor<IT_NR-1>()(&make_readonly_editor_funcs_);
}
