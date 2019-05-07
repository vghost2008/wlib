/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <boost/variant.hpp>
#include <QGraphicsScene>
#include <QtWidgets>
#include <toolkit.h>

class WGraphicsBaseLayout
{
	public:
		WGraphicsBaseLayout(QGraphicsScene* parent=nullptr);
		virtual ~WGraphicsBaseLayout();
		virtual QSizeF min_size()const=0;
		void set_rect(const QRectF& rect);
		QRectF rect()const;
		const QGraphicsScene* parent()const { return parent_; }
		QGraphicsScene* parent(){ return parent_; }
	protected:
		void sceneRectChanged(const QRectF& rect);
		virtual void rectChanged();
		virtual void setItemRect();
	private:
		QGraphicsScene        *parent_   = nullptr;
		QRectF                 rect_;
};
class WGraphicsBoxLayout;
class WGraphicsStretchItem
{
	public:
		WGraphicsStretchItem(WGraphicsBoxLayout* parent);
		QSizeF min_size()const;
	private:
		WGraphicsBoxLayout* parent_ = nullptr;
};
using WGraphicsLayoutItem = boost::variant<std::weak_ptr<WGraphicsBaseLayout>,std::weak_ptr<QGraphicsItem>,WGraphicsStretchItem>;
class WGraphicsLayoutItemMinSize:public boost::static_visitor<QSizeF>
{
	public:
		inline QSizeF operator()(const std::weak_ptr<WGraphicsBaseLayout> ptr)const {
			auto v = ptr.lock();
			return v->min_size();
		}
		inline QSizeF operator()(const std::weak_ptr<QGraphicsItem> ptr)const {
			auto v = ptr.lock();
			return v->boundingRect().size();
		}
		inline QSizeF operator()(const WGraphicsStretchItem& v)const {
			return v.min_size();
		}
};
class WGraphicsLayoutItemMinSizeFM:public boost::static_visitor<QSizeF>
{
	public:
		inline QSizeF operator()(const std::weak_ptr<WGraphicsBaseLayout> ptr)const {
			auto v = ptr.lock();
			return v->min_size();
		}
		inline QSizeF operator()(const std::weak_ptr<QGraphicsItem> ptr)const {
			auto v = ptr.lock();
			return v->boundingRect().size();
		}
		inline QSizeF operator()(const WGraphicsStretchItem& )const {
			return QSizeF(0,0);
		}
};
class WGraphicsLayoutItemIsNull:public boost::static_visitor<bool>
{
	public:
		template<typename T,typename=std::enable_if_t<!std::is_same<std::decay_t<T>,WGraphicsStretchItem>::value>>
		inline bool operator()(const T& v)const {
			return v.lock()==nullptr;
		}
		inline bool operator()(const WGraphicsStretchItem& )const {
			return false;
		}
};
class WGraphicsLayoutItemSetRect:public boost::static_visitor<>
{
	public:
		inline WGraphicsLayoutItemSetRect(const QRectF& rect):rect_(rect){}
		inline void operator()(std::weak_ptr<WGraphicsBaseLayout> ptr)const {
			auto v = ptr.lock();
			if(nullptr == v) return;
			v->set_rect(rect_);
		}
		inline void operator()(std::weak_ptr<QGraphicsItem> ptr)const {
			auto v = ptr.lock();
			if(nullptr == v) return;
			const auto b_rect = v->boundingRect();
			const auto pos = rect_.topLeft()
				+QPointF((rect_.width()-b_rect.width())/2,(rect_.height()-b_rect.height())/2)
				-b_rect.topLeft();
			v->setPos(pos);
		}
		inline void operator()(const WGraphicsStretchItem& )const {}
	private:
		QRectF rect_;
};
class WGraphicsBoxLayout:public WGraphicsBaseLayout
{
	public:
		using WGraphicsBaseLayout::WGraphicsBaseLayout;
		void checkItems();
		void addItem(std::weak_ptr<QGraphicsItem> item);
		void addItem(std::weak_ptr<WGraphicsBaseLayout> item);
		void addLayout(std::weak_ptr<WGraphicsBaseLayout> layout);
		void addStretch();
		virtual void rectChanged()override;
		virtual QSizeF get_min_size_without_stretch()const ;
		size_t stretchNum()const;
		inline const std::vector<WGraphicsLayoutItem>& items()const { return items_; };
	protected:
		std::vector<WGraphicsLayoutItem> items_;
};
class WGraphicsVBoxLayout:public WGraphicsBoxLayout
{
	public:
		using WGraphicsBoxLayout::WGraphicsBoxLayout;
	private:
		void checkItems();
		virtual QSizeF min_size()const override;
		virtual QSizeF get_min_size_without_stretch()const override;
		template<class F>
			QSizeF get_min_size()const {
				auto width  = 0.0f;
				auto height = 0.0f;

				height = accumulate(items_.begin(),items_.end(),0.0f,[](float sum,const WGraphicsLayoutItem& item) { return sum+apply_visitor(F(),item).height();});
				auto it = max_element(items_.begin(),items_.end(),[](const WGraphicsLayoutItem& lhv,const WGraphicsLayoutItem& rhv) {
						return apply_visitor(F(),lhv).width()<apply_visitor(F(),rhv).width();
						});
				if(it != items_.end())
					width = boost::apply_visitor(F(),*it).width();
				return QSizeF(width,height);
			}
		virtual void setItemRect()override;
};
class WGraphicsHBoxLayout:public WGraphicsBoxLayout
{
	public:
		using WGraphicsBoxLayout::WGraphicsBoxLayout;
	private:
		virtual void setItemRect()override;
		virtual QSizeF min_size()const override;
		virtual QSizeF get_min_size_without_stretch()const override;
		template<class F>
			QSizeF get_min_size()const {
				auto width  = 0.0f;
				auto height = 0.0f;

				width = accumulate(items_.begin(),items_.end(),0.0f,[](float sum,const WGraphicsLayoutItem& item) { return sum+boost::apply_visitor(F(),item).width();});
				auto it = max_element(items_.begin(),items_.end(),[](const WGraphicsLayoutItem& lhv,const WGraphicsLayoutItem& rhv) {
						return boost::apply_visitor(F(),lhv).height()<boost::apply_visitor(F(),rhv).height();
						});
				if(it != items_.end())
					height = boost::apply_visitor(F(),*it).height();
				return QSizeF(width,height);
			}
};
