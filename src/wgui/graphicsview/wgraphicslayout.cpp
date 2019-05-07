#include <wgraphicslayout.h>
#include <QDebug>
#include <boost/scope_exit.hpp>

WGraphicsBaseLayout::WGraphicsBaseLayout(QGraphicsScene* parent)
:parent_(parent)
{
	if(nullptr != parent_) {
		rect_ = parent_->sceneRect();
		QObject::connect(parent_,&QGraphicsScene::sceneRectChanged,[this](const QRectF& rect){sceneRectChanged(rect);});
	}
}
void WGraphicsBaseLayout::sceneRectChanged(const QRectF& rect)
{
	rect_ = rect;
	rectChanged();
}
QRectF WGraphicsBaseLayout::rect()const 
{
	return rect_;
}
void WGraphicsBaseLayout::set_rect(const QRectF& rect)
{
	rect_ = rect;
	if(nullptr == parent_)
		rectChanged();
}
void WGraphicsBaseLayout::rectChanged()
{
}
void WGraphicsBaseLayout::setItemRect()
{
}
WGraphicsBaseLayout::~WGraphicsBaseLayout()
{
}
/******************************************************************************/
WGraphicsStretchItem::WGraphicsStretchItem(WGraphicsBoxLayout* parent)
:parent_(parent)
{
}
QSizeF WGraphicsStretchItem::min_size()const
{
	const auto rsize      = parent_->get_min_size_without_stretch();
	const auto stretch_num = parent_->stretchNum();

	if(stretch_num == 0) return QSizeF(0,0);
	const auto item_stretch = parent_->items().size()-stretch_num;
	return (parent_->rect().size()-rsize-QSizeF(item_stretch,item_stretch))/stretch_num;
}
/******************************************************************************/
void WGraphicsBoxLayout::checkItems()
{
	auto it = remove_if(items_.begin(),items_.end(),[](const WGraphicsLayoutItem& v) { return boost::apply_visitor(WGraphicsLayoutItemIsNull(),v);});
	items_.erase(it,items_.end());
}
void WGraphicsBoxLayout::addItem(std::weak_ptr<QGraphicsItem> item)
{
	items_.push_back(item);
	checkItems();
	setItemRect();
}
void WGraphicsBoxLayout::addItem(std::weak_ptr<WGraphicsBaseLayout> item)
{
	items_.push_back(item);
	checkItems();
	setItemRect();
}
void WGraphicsBoxLayout::rectChanged()
{
	setItemRect();
}
QSizeF WGraphicsBoxLayout::get_min_size_without_stretch()const
{
	return QSizeF(0,0);
}
size_t WGraphicsBoxLayout::stretchNum()const
{
	return count_if(items_.begin(),items_.end(),[](const WGraphicsLayoutItem& item){ return item.which()==2;});
}
void WGraphicsBoxLayout::addStretch()
{
	items_.push_back(WGraphicsStretchItem(this));
}
/******************************************************************************/
QSizeF WGraphicsVBoxLayout::min_size()const
{
	return get_min_size<WGraphicsLayoutItemMinSize>();
}
QSizeF WGraphicsVBoxLayout::get_min_size_without_stretch()const
{
	return get_min_size<WGraphicsLayoutItemMinSizeFM>();
}
void WGraphicsVBoxLayout::setItemRect()
{
	auto size = min_size();

	if(items_.empty()) return;

	const auto r             = rect();
	const auto height_margin = (r.height()-size.height())/items_.size();
	auto       top_left      = rect().topLeft();

	for(int i=0; i<items_.size(); ++i) {
		auto       &item        = items_[i];
		const auto  m_size      = apply_visitor(WGraphicsLayoutItemMinSize(),item);
		const auto  target_rect = QRectF(top_left,QSize(r.width(),height_margin+m_size.height()));

		apply_visitor(WGraphicsLayoutItemSetRect(target_rect),item);
		top_left.setY(top_left.y()+m_size.height()+height_margin);
	}
}
/******************************************************************************/
QSizeF WGraphicsHBoxLayout::min_size()const
{
	return get_min_size<WGraphicsLayoutItemMinSize>();
}
QSizeF WGraphicsHBoxLayout::get_min_size_without_stretch()const
{
	return get_min_size<WGraphicsLayoutItemMinSizeFM>();
}
void WGraphicsHBoxLayout::setItemRect()
{
	auto size = min_size();

	if(items_.empty()) return;

	const auto r             = rect();
	const auto width_margin  = (r.width()-size.width())/items_.size();
	auto       top_left      = rect().topLeft();

	for(int i=0; i<items_.size(); ++i) {
		auto       &item        = items_[i];
		const auto  m_size      = apply_visitor(WGraphicsLayoutItemMinSize(),item);
		const auto  target_rect = QRectF(top_left,QSize(width_margin+m_size.width(),r.height()));
		apply_visitor(WGraphicsLayoutItemSetRect(target_rect),item);
		top_left.setX(top_left.x()+width_margin+m_size.width());
	}
}
