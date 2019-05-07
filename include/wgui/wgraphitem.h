/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include "wgui_fwd.h"
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "wgui.h"
#include "wgraph_fwd.h"

class WGraphLine;
class WGraphItem:public QGraphicsItem
{
	public:
		WGraphItem(int id);
		~WGraphItem();
		virtual QRectF boundingRect()const override;
		bool addIOLine(std::shared_ptr<WGraphLine> line);
		bool removeIOLine(std::shared_ptr<WGraphLine> line);
		QPointF topMid()const;
		QPointF bottomMid()const;
		QPointF leftMid()const;
		QPointF rightMid()const;
		QPointF connectTopMid()const;
		QPointF connectBottomMid()const;
		QPointF connectLeftMid()const;
		QPointF connectRightMid()const;
		QPointF center()const;
		QRectF rectInParent() const;
		int randomOffset()const;
		void checkLines();
		inline int id()const { return data(WGI_ID).toInt(); }
	protected:
		void paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget)override;
	private:
		QRectF                                  rect_;
		const int                               display_h_delta       = 5;
		const int                               display_w_delta       = 20;
		const int                               round_radius_         = 10;
		std::list<std::shared_ptr<WGraphLine>>  io_lines_;
		friend WGraphLine;
	private:
		int align(int v,int av);
		QRectF align(const QRectF& r);
		virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value)override;
};
class WGraphLine:public QGraphicsItem,public std::enable_shared_from_this<WGraphLine>
{
	public:
		WGraphLine(std::shared_ptr<WGraphItem>& first,std::shared_ptr<WGraphItem>& second,WGraphLineType type=WGLT_FREE);
		virtual ~WGraphLine();
		inline void setColor(const QColor& c) { color_ = c; }
		inline WGraphItem& first() { 
			auto res = first_.lock(); 
			if(nullptr == res) throw std::runtime_error("null first");
			return *res;
		}
		inline WGraphItem& second() { 
			auto res = second_.lock(); 
			if(nullptr == res) throw std::runtime_error("null second");
			return *res;
		}
		inline const WGraphItem& first()const { 
			auto res = first_.lock(); 
			if(nullptr == res) throw std::runtime_error("null first");
			return *res;
		}
		inline const WGraphItem& second()const { 
			auto res = second_.lock(); 
			if(nullptr == res) throw std::runtime_error("null second");
			return *res;
		}
		virtual QRectF boundingRect()const override;
		std::shared_ptr<WGraphLine> sharedRef()noexcept(false);
		bool initLine();
		virtual QPainterPath shape()const override;
		void reinit();
		bool collidingNode()const;
		void reinit(int type);
		inline std::vector<QPointF> allPoints()const { return line_.allPoints(); }
		inline WGraphLineType line_type()const { return line_type_; }
		inline void set_line_type(WGraphLineType type) { line_type_ = type; }
	public:
		LineStripWithArrow       line_;
		const int                arrow_size_  = 10;
	private:
		std::weak_ptr<WGraphItem> first_;
		std::weak_ptr<WGraphItem> second_;
		QColor                    color_;
		WGraphLineType            line_type_   = WGLT_FREE;
	private:
		void paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget);
	private:
		bool reinitWithSameY()noexcept;
		bool reinitWithDiffY()noexcept;
		void simpleReinit()noexcept;
	private:
		bool reinitSBR(const WGraphItem& first,const WGraphItem& second,int arrow_pos);
		bool reinitSBL(const WGraphItem& first,const WGraphItem& second,int arrow_pos);
		bool reinitSBRML0(const QPointF& p0, const QPointF& p1,int arrow_pos,
				LineStripWithArrow::LineDirection bd=LineStripWithArrow::LD_ANY, LineStripWithArrow::LineDirection ed=LineStripWithArrow::LD_ANY,
				int d0=20,int d1=20,
				const std::vector<QPointF>& ph=std::vector<QPointF>(),
				const std::vector<QPointF>& pt=std::vector<QPointF>()
				);
		bool reinitSBRML1(const QPointF& p0, const QPointF& p1,int arrow_pos,
				LineStripWithArrow::LineDirection bd=LineStripWithArrow::LD_ANY, LineStripWithArrow::LineDirection ed=LineStripWithArrow::LD_ANY,
				int d0=20,int d1=20,
				const std::vector<QPointF>& ph=std::vector<QPointF>(),
				const std::vector<QPointF>& pt=std::vector<QPointF>()
				);
		bool reinitSBLML0(const QPointF& p0, const QPointF& p1,int arrow_pos,
				LineStripWithArrow::LineDirection bd=LineStripWithArrow::LD_ANY, LineStripWithArrow::LineDirection ed=LineStripWithArrow::LD_ANY,
				int d0=20,int d1=20,
				const std::vector<QPointF>& ph=std::vector<QPointF>(),
				const std::vector<QPointF>& pt=std::vector<QPointF>()
				);
		bool reinitSBLML1(const QPointF& p0, const QPointF& p1,int arrow_pos,
				LineStripWithArrow::LineDirection bd=LineStripWithArrow::LD_ANY, LineStripWithArrow::LineDirection ed=LineStripWithArrow::LD_ANY,
				int d0=20,int d1=20,
				const std::vector<QPointF>& ph=std::vector<QPointF>(),
				const std::vector<QPointF>& pt=std::vector<QPointF>()
				);
};

