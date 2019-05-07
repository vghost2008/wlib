/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QtGui>
#include <vector>
#include <QDebug>
#include <type_traits>
#include "flowlayout.h"
#include <wglobal.h>
#include <boost/lexical_cast.hpp>
#include "wgui_fwd.h"
#include "guibutton.h"
#include "guiedit.h"
class WGuiSeparator:public QWidget
{
	public:
		WGuiSeparator(WOrientation orientation=Vertical);
	private:
		WOrientation orientation_;
	private:
		void paintEvent(QPaintEvent* event)override;
};
class WCheckBox:public QCheckBox
{
	Q_OBJECT
	public:
		WCheckBox(const QString& title,unsigned id);
	private:
		unsigned id_;
	private:
		void slotClicked(bool ischecked);
	signals:
		void explain();
		void sigClicked(unsigned id,bool ischecked);
};
enum class SBState:unsigned char
{
    SB_OK,
    SB_ERROR,
    SB_UNKNOW,
};
class WPixmapLabel:public QWidget
{
	public:
		WPixmapLabel(const QString& text,const QString& pixmap_name=QString());
	private:
		const unsigned offset_;
		QString        text_;
		QPixmap        background_pix_;
		QSize          icon_size_;
	private:
		void paintEvent(QPaintEvent* event)override;
};
/*
 * 显示文字下方显示一个颜色条带
 */
class WColorLabel:public QWidget
{
	public:
		WColorLabel(const QString& text,const QColor& color=QColor(0,92,175));
		void setText(const QString& text);
		inline QString text()const {
			return text_;
		}
	private:
		const unsigned offset_;
		const unsigned margin_;
		const unsigned color_box_height_;
		QColor         label_color_;
		QString        text_;
	private:
		void paintEvent(QPaintEvent* event)override;
};
/*
 * 显示颜色窗口
 * 用户可以设置新的颜色
 */
class WColorWidget:public QWidget
{
	public:
		WColorWidget(const QColor& color,QWidget* parent=nullptr);
		inline QColor color()const { return color_;}
		void setColor(const QColor& color);
	protected:
		void mouseDoubleClickEvent(QMouseEvent* event)override;
		void paintEvent(QPaintEvent* event)override;
	private:
		QColor color_;
};
class WDateTimeLabel:public QLabel
{
	public:
		WDateTimeLabel();
		~WDateTimeLabel();
		void setUpdateable(bool update=true);
	private:
		const int timer_interval_;
		int timer_id_;
		void timerEvent(QTimerEvent* event)override;
};
class WComboBox:public QComboBox
{
	public:
		WComboBox(QWidget* parent=nullptr);
		void setReadOnly(bool readonly) { read_only_ = readonly; }
	private:
		bool read_only_;
	private:
		void mousePressEvent(QMouseEvent* event)override;
		void mouseReleaseEvent(QMouseEvent* event)override;
};
class ArrowLine
{
	public:
		ArrowLine();
		ArrowLine(float x0,float y0,float x1,float y1,float arrow_size,unsigned arrow_pos=WSpace::LinePos::LP_END);
		ArrowLine(const QPointF& p0,const QPointF& p1,float arrow_size,unsigned arrow_pos=WSpace::LinePos::LP_END);
		void update();
		void paint(QPainter* painter);
		QPainterPath clipPath()const;
		inline QPointF first()const { return QPointF(point0_[0],point0_[1]); }
		inline QPointF second()const { return QPointF(point1_[0],point1_[1]); }
	private:
		float    x0_;
		float    y0_;
		float    x1_;
		float    y1_;
		float    arrow_size_;
		unsigned arrow_pos_;
		float    point0_[2];
		float    point1_[2];
		float    arrow_points0_[6];
		float    arrow_points1_[6];
};
class LineStripWithArrow
{
	public:
		enum LineDirection
		{
			LD_UP          =  0x01,
			LD_DOWN        =  0x02,
			LD_LEFT        =  0x04,
			LD_RIGHT       =  0x10,
			LD_HORIZONTAL  =  LD_LEFT|LD_RIGHT,
			LD_VERTICAL    =  LD_UP|LD_DOWN,
			LD_ANY         =  LD_HORIZONTAL|LD_VERTICAL,
		};
	public:
		LineStripWithArrow();
		/*
		 * Iter指向QPointF的迭代器
		 */
		template<class Iter>
			LineStripWithArrow(Iter begin,Iter end,float arrow_size,unsigned arrow_pos=WSpace::LinePos::LP_END)
			:arrow_pos_(arrow_pos)
			 ,arrow_size_(arrow_size)
			 ,is_orth_(false)
	{
		if(std::distance(begin,end) <= 1) return;
		points_.assign(begin,end);
		if((arrow_pos_&WSpace::LP_BEGIN)
				&& points_.size()>=2) {
			begin_arrow_ = ArrowLine(points_.front(),points_[1],arrow_size_,WSpace::LP_BEGIN);
			points_.erase(points_.begin());
		} 
		if((arrow_pos_&WSpace::LP_END)
				&& (points_.size()>=2)) {
			end_arrow_ = ArrowLine(points_[points_.size()-2],points_.back(),arrow_size_,WSpace::LP_END);
			points_.erase(--points_.end());
		} 
	}
		template<class Iter>
			LineStripWithArrow(Iter begin,Iter end,float arrow_size,unsigned arrow_pos,LineDirection bd,LineDirection ed=LD_ANY)
			:arrow_pos_(arrow_pos)
			 ,arrow_size_(arrow_size)
			 ,is_orth_(true)
	{
		if(std::distance(begin,end) <= 1) return;
#if 0
		if(!initLine(begin,end,bd,ed)
				&& !initLine(begin,end,LD_ANY,ed)
				&& !initLine(begin,end,bd,LD_ANY)
				&& !initLine(begin,end,LD_ANY,LD_ANY)) {
			points_.assign(begin,end);
		}
#endif
		points_.push_back(*begin);
		if(!initLine(*begin,std::next(begin),end,bd,ed)){
			points_.clear();
			points_.push_back(*begin);
			initLine(*begin,std::next(begin),end,bd,ed);
			points_.assign(begin,end);
		}
		if((arrow_pos_&WSpace::LP_BEGIN)
				&& points_.size()>=2) {
			begin_arrow_ = ArrowLine(points_.front(),points_[1],arrow_size_,WSpace::LP_BEGIN);
			points_.erase(points_.begin());
		} 
		if((arrow_pos_&WSpace::LP_END)
				&& (points_.size()>=2)) {
			end_arrow_ = ArrowLine(points_[points_.size()-2],points_.back(),arrow_size_,WSpace::LP_END);
			points_.erase(--points_.end());
		} 
	}
		template<class Iter>
			bool doInit(Iter begin,Iter end,float arrow_size,unsigned arrow_pos,LineDirection bd,LineDirection ed=LD_ANY)
			{
				arrow_pos_   =  arrow_pos;
				arrow_size_  =  arrow_size;
				is_orth_     =  true;

				points_.clear();

				if(std::distance(begin,end) <= 1) return false;

				points_.push_back(*begin);
				if(!initLine(*begin,std::next(begin),end,bd,ed)){
					return false;
				}
				if((arrow_pos_&WSpace::LP_BEGIN)
						&& points_.size()>=2) {
					begin_arrow_ = ArrowLine(points_.front(),points_[1],arrow_size_,WSpace::LP_BEGIN);
					points_.erase(points_.begin());
				} 
				if((arrow_pos_&WSpace::LP_END)
						&& (points_.size()>=2)) {
					end_arrow_ = ArrowLine(points_[points_.size()-2],points_.back(),arrow_size_,WSpace::LP_END);
					points_.erase(--points_.end());
				} 
				return true;
			}
		template<typename Point,class Iter>
			bool initLine(Point& p0, Iter next,Iter end, int bd,int ed) {
				const auto dis      = std::distance(next,end);
				const auto kDelta   = 2;
				if(dis < 1) return true;
				auto          &p1 = *next;
				LineDirection  d;

				if((p1-p0).manhattanLength() < dis) {
					return initLine(p1,std::next(next),end,bd,ed);
				}


				if((fabs(p1.x()-p0.x())>kDelta) && (fabs(p1.y()-p0.y())>kDelta)) {
					const auto p2 = QPointF(p1.x(),p0.y());
					d = getDirection(p0,p2);
					if(d&bd) {
						points_.push_back(p2);
						if(initLine(p2,next,end,noreverseDirection(d),ed)) {
							return true;
						}
						points_.pop_back();
					}
					const auto p3 = QPointF(p0.x(),p1.y());
					d = getDirection(p0,p3);
					if(d&bd) {
						points_.push_back(p3);
						if(initLine(p3,next,end,noreverseDirection(d),ed)) {
							return true;
						}
						points_.pop_back();
					}
					return false;
				} else  {
					if(dis==1) {
						if(!(bd&ed)) return false;
						bd = (bd&ed);
					}
					d = getDirection(p0,p1);
					if(d&bd) {
						points_.push_back(p1);
						if(initLine(p1,std::next(next),end,noreverseDirection(d),ed)) {
							return true;
						}
						points_.pop_back();
					}
					return false;
				}
			}
		void paint(QPainter* painter);
		QPainterPath clipPath()const;
		inline const std::vector<QPointF>& points()const {
			return points_; 
		}
		inline std::vector<QPointF> allPoints() const {
			auto res = points_;
			if(arrow_pos_&WSpace::LP_BEGIN) {
				res.insert(res.begin(),begin_arrow_.first());
			}
			if(arrow_pos_&WSpace::LP_END) {
				res.push_back(end_arrow_.second());
			}
			return res;
		}
		static LineDirection getDirection(const QPointF& p0, const QPointF& p1);
		template<int v>
			static constexpr LineDirection reverseDirection(){
				return LD_ANY;
			}
		static int noreverseDirection(LineDirection d);
		inline bool is_orth()const { return is_orth_; }
	private:
		std::vector<QPointF> points_;
		float                arrow_size_;
		unsigned             arrow_pos_;
		static float         zero_;
		ArrowLine            begin_arrow_;
		ArrowLine            end_arrow_;
		bool                 is_orth_     = false;
};
template<> constexpr LineStripWithArrow::LineDirection LineStripWithArrow::reverseDirection<LineStripWithArrow::LD_UP>(){
	return LD_DOWN;
}
template<> constexpr LineStripWithArrow::LineDirection LineStripWithArrow::reverseDirection<LineStripWithArrow::LD_DOWN>(){
	return LD_UP;
}
template<> constexpr LineStripWithArrow::LineDirection LineStripWithArrow::reverseDirection<LineStripWithArrow::LD_LEFT>(){
	return LD_RIGHT;
}
template<> constexpr LineStripWithArrow::LineDirection LineStripWithArrow::reverseDirection<LineStripWithArrow::LD_RIGHT>(){
	return LD_LEFT;
}
template<> constexpr LineStripWithArrow::LineDirection LineStripWithArrow::reverseDirection<LineStripWithArrow::LD_ANY>(){
	return static_cast<LineDirection>(0);
}
