/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#ifndef DISABLE_MAPVIEW
#include <algorithm>
#include <wmapmanagerwig.h>
#include <gltoolkit.h>
#include <wmacros.h>

using namespace std;
using namespace WSpace;

void WMapAttrViewDelegate::paint(QPainter* painter,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	switch(index.column()) {
		case MAV_LINE_COLOR:
		case MAV_FILL_COLOR:
		case MAV_TEXT_COLOR:
			{
				auto color = index.data().value<QColor>();
				painter->fillRect(option.rect,QBrush(color));
			}
			break;
		case MAV_SHOW_LABEL:
			if(0 == index.data().toInt()) {
				painter->drawText(option.rect,Qt::AlignCenter,"不显示");
			} else {
				painter->drawText(option.rect,Qt::AlignCenter,"显示");
			}
			break;
		case MAV_POLYGON_TYPE:
			{
				QString text;
				switch(index.data().toInt()) {
					case MPOT_FILL:
						text = "填充";
						break;
					case MPOT_EDGE:
						text = "描边";
						break;
					default:
					case MPOT_FILL_AND_EDGE:
						text = "描边&填充";
						break;
				}
				painter->drawText(option.rect,Qt::AlignCenter,text);
			}
			break;
		case MAV_POINT_TYPE:
		{
			QString text;
			switch(index.data().toInt()) {
				case MPT_TWO_CIRCLE:
				text = "双环";
				break;
				case MPT_TWO_FILL_CIRCLE:
				text = "填充双环";
				break;
				case MPT_NORMAL:
				text = "普通圆点";
				default:
				break;
			}
			painter->drawText(option.rect,Qt::AlignCenter,text);
		}
			break;
		case MAV_TEXT_SIZE:
		case MAV_LEVEL:
		case MAV_TEXT_OFFSET:
			painter->drawText(option.rect,Qt::AlignCenter,QString::number(index.data().toInt()));
			break;
		case MAV_LINE_WIDTH:
		case MAV_POINT_SIZE:
			painter->drawText(option.rect,Qt::AlignCenter,QString::number(index.data().toFloat()));
			break;
		default:
			return QItemDelegate::paint(painter,option,index);
	}
}
QSize WMapAttrViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	const int height        = option.fontMetrics.height() *1.5;
	const int default_width = option.fontMetrics.width("XXXX");
	const int delta_width   = 15;

	switch(index.column()) {
		case MAV_LINE_COLOR:
		case MAV_FILL_COLOR:
		case MAV_TEXT_COLOR:
			return QSize(height,default_width);
			break;
		case MAV_SHOW_LABEL:
			return QSize(option.fontMetrics.width("不显示")+delta_width,height);
			break;
		case MAV_POLYGON_TYPE:
			return QSize(option.fontMetrics.width("描边&填充")+delta_width,height);
			break;
		case MAV_POINT_TYPE:
			return QSize(option.fontMetrics.width("填充双环")+delta_width,height);
			break;
		case MAV_TEXT_SIZE:
		case MAV_LEVEL:
		case MAV_TEXT_OFFSET:
			return QSize(option.fontMetrics.width(QString::number(index.data().toInt()))+delta_width,height);
			break;
		case MAV_LINE_WIDTH:
		case MAV_POINT_SIZE:
			return QSize(option.fontMetrics.width(QString::number(index.data().toFloat()))+delta_width,height);
			break;
		default:
			return QItemDelegate::sizeHint(option,index);
	}
}
QWidget* WMapAttrViewDelegate::createEditor(QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const
{
	switch(index.column()) {
		case MAV_FILE_NAME:
			return nullptr;
			break;
		case MAV_LINE_COLOR:
		case MAV_FILL_COLOR:
		case MAV_TEXT_COLOR:
			{
				return new QColorDialog(parent);
			}
			break;
		case MAV_LINE_WIDTH:
		case MAV_POINT_SIZE:
			{
				auto le = new QLineEdit(parent);
				le->setValidator(new QDoubleValidator(0.2,10,1,le));
				return le;
			}
			break;
		case MAV_TEXT_SIZE:
			{
				auto le = new QLineEdit(parent);
				le->setValidator(new QIntValidator(4,72,le));
				return le;
			}
			break;
		case MAV_TEXT_OFFSET:
			{
				auto le = new QLineEdit(parent);
				le->setValidator(new QIntValidator(-10,10,le));
				return le;
			}
			break;
		case MAV_LEVEL:
			{
				auto le = new QLineEdit(parent);
				le->setValidator(new QIntValidator(0,10,le));
				return le;
			}
			break;
		case MAV_SHOW_LABEL:
		case MAV_POLYGON_TYPE:
		case MAV_POINT_TYPE:
			return new QComboBox(parent);
			break;
		default:
			return QItemDelegate::createEditor(parent,option,index);
			break;
	}
	return nullptr;
}
void WMapAttrViewDelegate::setEditorData ( QWidget * editor, const QModelIndex & index ) const
{
	switch(index.column()) {
		case MAV_LINE_COLOR:
		case MAV_FILL_COLOR:
		case MAV_TEXT_COLOR:
			{
				auto dlg = qobject_cast<QColorDialog*>(editor);
				dlg->setCurrentColor(index.data().value<QColor>());
			}
			break;
		case MAV_TEXT_SIZE:
		case MAV_TEXT_OFFSET:
		case MAV_LEVEL:
			{
				auto le = qobject_cast<QLineEdit*>(editor);
				auto v = index.data().toInt();
				le->setText(QString::number(v));
			}
			break;
		case MAV_LINE_WIDTH:
		case MAV_POINT_SIZE:
			{
				auto le = qobject_cast<QLineEdit*>(editor);
				auto v = index.data().toFloat();
				le->setText(QString::number(v));
			}
			break;
		case MAV_SHOW_LABEL:
			{
				auto co = qobject_cast<QComboBox*>(editor);
				co->addItem("不显示",0);
				co->addItem("显示",1);
				co->setCurrentIndex(index.data().toInt());
			}
			break;
		case MAV_POLYGON_TYPE:
			{
				auto co = qobject_cast<QComboBox*>(editor);
				co->addItem("填充",MPOT_FILL);
				co->addItem("填充&描边",MPOT_FILL_AND_EDGE);
				co->addItem("描边",MPOT_EDGE);
				co->setCurrentIndex(index.data().toInt());
			}
			break;
		case MAV_POINT_TYPE:
			{
				auto co = qobject_cast<QComboBox*>(editor);
				co->addItem("普通圆点",0);
				co->addItem("双环",1);
				co->addItem("填充双环",2);
				co->setCurrentIndex(index.data().toInt());
			}
			break;
		default:
			return QItemDelegate::setEditorData(editor,index);
			break;
	}
}
void WMapAttrViewDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
{
	switch(index.column()) {
		case MAV_LINE_COLOR:
		case MAV_FILL_COLOR:
		case MAV_TEXT_COLOR:
			{
				auto dlg   = qobject_cast<QColorDialog *>(editor);
				auto color = dlg->currentColor();
				model->setData(index,color);
			}
			break;
		case MAV_TEXT_SIZE:
		case MAV_LEVEL:
		case MAV_TEXT_OFFSET:
			{
				auto v = qobject_cast<QLineEdit*>(editor)->text().toInt();
				model->setData(index,v);
			}
			break;
		case MAV_LINE_WIDTH:
		case MAV_POINT_SIZE:
			{
				auto v = qobject_cast<QLineEdit*>(editor)->text().toFloat();
				model->setData(index,v);
			}
			break;
		case MAV_SHOW_LABEL:
		case MAV_POLYGON_TYPE:
		case MAV_POINT_TYPE:
			{
				auto co = qobject_cast<QComboBox*>(editor);
				auto i = co->currentIndex();
				auto v = co->itemData(i).toInt();
				model->setData(index,v);
			}
			break;
		default:
			return QItemDelegate::setModelData(editor,model,index);
			break;
	}
}
/******************************************************************************/
WMapLayoutAttrWig::WMapLayoutAttrWig()
{
	initGUI();
}
bool WMapLayoutAttrWig::getLayout(WMapLayout* layout)
{
	return true;
}
bool WMapLayoutAttrWig::setLayout(const WMapLayout& layout)
{
	return true;
}
void WMapLayoutAttrWig::initGUI()
{
}

/******************************************************************************/
WMapAttrWig::WMapAttrWig()
{
	initGUI();
}
void WMapAttrWig::initGUI()
{
	auto layout = new QVBoxLayout(this);

	table_view_ = new QTableWidget(this);
	table_view_->setItemDelegate(new WMapAttrViewDelegate);
	table_view_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	layout->addWidget(table_view_);

	menu_ = new QMenu;

	auto action = menu_->addAction("上移");
	connect(action,&QAction::triggered,this,&WMapAttrWig::slotMenuAction);
	action = menu_->addAction("下移");
	connect(action,&QAction::triggered,this,&WMapAttrWig::slotMenuAction);
	action = menu_->addAction("顶部");
	connect(action,&QAction::triggered,this,&WMapAttrWig::slotMenuAction);
	action = menu_->addAction("底部");
	connect(action,&QAction::triggered,this,&WMapAttrWig::slotMenuAction);
	action = menu_->addAction("删除");
	connect(action,&QAction::triggered,this,&WMapAttrWig::slotMenuAction);
}
void WMapAttrWig::slotMenuAction()
{
	auto               action  = qobject_cast<QAction *>(sender());
	auto               text    = action->text();
	vector<WMapLayout> layouts;
	auto               row     = selectedRow();

	if(-1 == row) {
		return ;
	}
	if(!getLayouts(&layouts)) {
		return ;
	}
	if(row >= layouts.size()) return;


	if(text == "上移") {

		if(0 == row ) return;

		auto urow        = row -1;
		auto temp_layout = layouts[urow];

		layouts[urow]  =  layouts[row];
		layouts[row]   =  temp_layout;
	} else if(text == "下移") {
		if(row >= layouts.size()-1) return;

		auto drow        = row+1;
		auto temp_layout = layouts[drow];

		layouts[drow]  =  layouts[row];
		layouts[row]   =  temp_layout;
	} else if(text == "顶部") {
		if(0 == row || row >= layouts.size()) return;

		auto temp_layout = layouts[row];

		layouts.erase(layouts.begin()+row);
		layouts.insert(layouts.begin(),temp_layout);
	} else if(text == "底部") {
		if(row >= layouts.size()) return;

		auto temp_layout = layouts[row];

		layouts.erase(layouts.begin()+row);
		layouts.push_back(temp_layout);
	} else if(text == "删除") {
		layouts.erase(layouts.begin()+row);
	}
	setLayouts(layouts);
}
int WMapAttrWig::selectedRow()const
{
	auto selectionmodel = table_view_->selectionModel();
	auto modelindexlist = selectionmodel->selectedIndexes();

	if(modelindexlist.isEmpty()) return -1;
	return modelindexlist.at(0).row();
}
bool WMapAttrWig::getLayouts(vector<WMapLayout>* layouts)
{
	auto model      =  table_view_->model();
	auto row_count  =  model->rowCount();

	layouts->clear();
	layouts->reserve(row_count);

	for(int i=0; i<row_count; ++i) {
		WMapLayout  l;
		auto       &attr = l.attr;
		auto        item = table_view_->item(i,0);

		if(nullptr == item || item->checkState() == Qt::Checked)
			l.visible    =  true;
		else
			l.visible = false;
		l.file_name  =  model->data(model->index(i,MAV_FILE_NAME),Qt::UserRole).toString().toUtf8().data();
		toGLColor4f(model->data(model->index(i,MAV_LINE_COLOR)).value<QColor>(),attr.line_color);
		toGLColor4f(model->data(model->index(i,MAV_FILL_COLOR)).value<QColor>(),attr.fill_color);
		toGLColor4f(model->data(model->index(i,MAV_TEXT_COLOR)).value<QColor>(),attr.text_color);
		attr.line_width    =  model->data(model->index(i,MAV_LINE_WIDTH)).toFloat();
		attr.point_size    =  model->data(model->index(i,MAV_POINT_SIZE)).toFloat();
		attr.text_size     =  model->data(model->index(i,MAV_TEXT_SIZE)).toInt();
		attr.level         =  model->data(model->index(i,MAV_LEVEL)).toInt();
		attr.show_label    =  model->data(model->index(i,MAV_SHOW_LABEL)).toInt();
		attr.polygon_type  =  model->data(model->index(i,MAV_POLYGON_TYPE)).toInt();
		attr.text_offset   =  model->data(model->index(i,MAV_TEXT_OFFSET)).toInt();
		attr.point_type    =  model->data(model->index(i,MAV_POINT_TYPE)).toInt();
		layouts->push_back(l);
	}
	return true;
}
bool WMapAttrWig::setLayouts(const vector<WMapLayout>& layouts)
{
	auto model = table_view_->model();

	if(nullptr == model) {
		ERROR_LOG("没有model");
		return false;
	}

	model->removeRows(0,model->rowCount());
	if(model->columnCount() == 0)
	model->insertColumns(0,kMavNr);

	const QStringList head_names({"图层","多边形","填充色","线颜色","线宽","点大小","点类型","文字颜色","文字大小","文字偏移","层级","是否显示标签"});

	table_view_->setHorizontalHeaderLabels(head_names);

	if(layouts.empty()) return true;

	model->insertRows(0,layouts.size());
	for(int i=0; i<layouts.size(); ++i) {

		auto   &l     = layouts[i];
		auto   &attr  = l.attr;
		QColor  color;
		auto    item  = new QTableWidgetItem;

		table_view_->setItem(i,0,item);

		model->setData(model->index(i,MAV_FILE_NAME),QFileInfo(l.file_name.c_str()).baseName());
		model->setData(model->index(i,MAV_FILE_NAME),l.file_name.c_str(),Qt::UserRole);
		toQColor4f(attr.line_color,&color);
		model->setData(model->index(i,MAV_LINE_COLOR),color);
		toQColor4f(attr.fill_color,&color);
		model->setData(model->index(i,MAV_FILL_COLOR),color);
		toQColor4f(attr.text_color,&color);
		model->setData(model->index(i,MAV_TEXT_COLOR),color);
		model->setData(model->index(i,MAV_LINE_WIDTH),attr.line_width);
		model->setData(model->index(i,MAV_POINT_SIZE),attr.point_size);
		model->setData(model->index(i,MAV_TEXT_SIZE),attr.text_size);
		model->setData(model->index(i,MAV_LEVEL),attr.level);
		model->setData(model->index(i,MAV_SHOW_LABEL),attr.show_label);
		model->setData(model->index(i,MAV_POLYGON_TYPE),attr.polygon_type);
		model->setData(model->index(i,MAV_POINT_TYPE),attr.point_type);
		model->setData(model->index(i,MAV_TEXT_OFFSET),attr.text_offset);
		if(l.visible)
			item->setCheckState(Qt::Checked);
		else
			item->setCheckState(Qt::Unchecked);
	}
	table_view_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	return true;
}
void WMapAttrWig::contextMenuEvent(QContextMenuEvent* event)
{
	menu_->exec(event->globalPos());
}
#endif
