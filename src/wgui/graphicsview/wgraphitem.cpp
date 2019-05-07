#include <wgraphitem.h>
#include <wgraphscene.h>
#include <wgraphics.h>
#include <wlogmacros.h>

using namespace std;

WGraphItem::WGraphItem(int id)
:QGraphicsItem(nullptr)
{
	int width  = 200;
	int height = 100;

	width   =  align(width,20);
	height  =  align(height,20);
	rect_   =  QRectF(-width/2,-height/2,width,height);
	srand(time(nullptr));
	setFlags(flags()|QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemSendsGeometryChanges);
	auto effect = new QGraphicsDropShadowEffect;
	effect->setOffset(8);
	effect->setColor(QColor(80,80,80,150));
	setGraphicsEffect(effect);
	setData(WGI_ID,id);
	setData(WGI_TYPE,WGI_NODE);
}
QRectF WGraphItem::boundingRect()const
{
	return rect_;
}
QVariant WGraphItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if ((change==QGraphicsItem::ItemPositionHasChanged)
			&& isSelected()){
		try {
			auto& sc = dynamic_cast<WGraphScene&>(*scene());

			sc.updateGraphTree(*this);
			sc.update();

			for(auto& l:io_lines_) {
				l->reinit();
			}

			auto       items = sc.collidingLines(*this);
			for(auto item:items) {
				item->reinit();
			}
			sc.update();
		} catch(...) {
		}
	}
	return QGraphicsItem::itemChange(change,value);
}
bool WGraphItem::addIOLine(shared_ptr<WGraphLine> line)
{
	if(find(io_lines_.begin(),io_lines_.end(),line) != io_lines_.end()) {
		return false;
	}
	io_lines_.push_back(line);
	return true;
}
bool WGraphItem::removeIOLine(shared_ptr<WGraphLine> line)
{
	auto it = find(io_lines_.begin(),io_lines_.end(),line);
	if(it!=io_lines_.end())io_lines_.erase(it);
	return true;
}
void WGraphItem::checkLines()
{
	auto it = partition(io_lines_.begin(),io_lines_.end(),[](const shared_ptr<WGraphLine>& l) {
		return !l.unique();
	});

	for(auto ij=it; ij!=io_lines_.end(); ++ij) {
		auto scene = (*ij)->scene();
		if(nullptr == scene) continue;
		scene->removeItem((*ij).get());
	}
	io_lines_.erase(it,io_lines_.end());
}
WGraphItem::~WGraphItem()
{
	QGraphicsScene* scene = nullptr;
	for(auto line:io_lines_) {
		if(line.unique() && (scene = line->scene()) != nullptr) {
			scene->removeItem(line.get());
		}
	}
	io_lines_.clear();
}
void WGraphItem::paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget)
{
	std::array<QColor,8> colors{QColor(255,0,0),QColor(255,196,8),QColor(0,92,175),QColor(249,191,69),QColor(226,148,59),QColor(111,51,129),QColor(134,193,102),QColor(0,137,167)};
	auto& color   =  colors[rand()%8];
	painter->save();//&0
	BOOST_SCOPE_EXIT_ALL(painter) {
		painter->restore();//&-0
	};
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setBrush(color);
	painter->setPen(Qt::NoPen);
	painter->drawRoundedRect(rect_,round_radius_,round_radius_);
}
QPointF WGraphItem::center()const
{
	return mapToParent(rect_.center());
}
QPointF WGraphItem::topMid()const
{
	return mapToParent(QPointF(rect_.center().x(),rect_.top()));
}
QPointF WGraphItem::bottomMid()const
{
	return mapToParent(QPointF(rect_.center().x(),rect_.bottom()));
}
QPointF WGraphItem::leftMid()const
{
	return mapToParent(QPointF(rect_.left(),rect_.center().y()));
}
QPointF WGraphItem::rightMid()const
{
	return mapToParent(QPointF(rect_.right(),rect_.center().y()));
}
int WGraphItem::randomOffset()const
{
	const int range = rect_.height()-40;

	if(range<=0) return (random()%11)-5;

	return (random()%range)-(range>1);
}
QPointF WGraphItem::connectTopMid()const
{
	return mapToParent(QPointF(rect_.center().x()+randomOffset(),rect_.top()));
}
QPointF WGraphItem::connectBottomMid()const
{
	return mapToParent(QPointF(rect_.center().x()+randomOffset(),rect_.bottom()));
}
QPointF WGraphItem::connectLeftMid()const
{
	return mapToParent(QPointF(rect_.left(),rect_.center().y()+randomOffset()));
}
QPointF WGraphItem::connectRightMid()const
{
	return mapToParent(QPointF(rect_.right(),rect_.center().y()+randomOffset()));
}
QRectF WGraphItem::rectInParent() const 
{
	return QRectF(mapToParent(rect_.topLeft()),
			mapToParent(rect_.bottomRight()));
}
int WGraphItem::align(int v,int av)
{
	if(av<=1 || v%av==0) return v;
	v = (v/av)*av+av;
	return v;
}
QRectF WGraphItem::align(const QRectF& r)
{
	QRectF f = r;
	f.setWidth(align(int(f.width()),10));
	f.setHeight(align(int(f.height()),10));
	return f;
}
/*================================================================================*/
WGraphLine::WGraphLine(shared_ptr<WGraphItem>& first,shared_ptr<WGraphItem>& second,WGraphLineType type)
:QGraphicsItem(nullptr)
,first_(first)
,second_(second)
,color_(0,0,255)
,line_type_(type)
{
	std::array<QColor,8> colors{QColor(255,0,0),QColor(255,196,8),QColor(0,92,175),QColor(249,191,69),QColor(226,148,59),QColor(111,51,129),QColor(134,193,102),QColor(0,137,167)};
	color_   =  colors[rand()%8];
	setPos(0,0);

	setData(WGI_TYPE,WGI_LINE);
	setData(WGI_IN_ID,first->id());
	setData(WGI_OUT_ID,second->id());
}
QRectF WGraphLine::boundingRect()const
{
	try {
		auto& item0 = first();
		auto& item1 = second();


		const QPointF p0 = item0.bottomMid();
		const QPointF p1 = item1.topMid();
		const QRectF rect(min(p0.x(),p1.x()),min(p0.y(),p1.y()),fabs(p0.x()-p1.x()),fabs(p1.y()-p0.y()));

		return rect;
	} catch(...) {
		return QRectF();
	}
}
WGraphLine::~WGraphLine()
{
}
void WGraphLine::paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget)
{
	painter->save();
	painter->setPen(color_);
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setBrush(QBrush(QColor(color_)));
	line_.paint(painter);
	//painter->drawPath(shape());
	painter->restore();
}
shared_ptr<WGraphLine> WGraphLine::sharedRef()noexcept(false)
{
	try {
		return shared_from_this();
	} catch(...) {
		ERROR_LOG("UNexpected.");
		throw runtime_error("error.");
	}
}
QPainterPath WGraphLine::shape()const
{
	return line_.clipPath();
}
bool WGraphLine::collidingNode()const
{
	auto sc  = dynamic_cast<const WGraphScene*>(scene());

	if(nullptr == sc) return -1;
	return sc->collidingNode(*this);
}
bool WGraphLine::initLine()
{
	try {
		first().addIOLine(sharedRef());
		second().addIOLine(sharedRef());

		reinit();
	} catch(...) {
		return false;
	}
	return true;
}
void WGraphLine::reinit()
{
	switch(line_type()) {
		case WGLT_FREE:
			simpleReinit();
			if(!collidingNode()) return;
			if(fabs(first().pos().y()-second().pos().y()) < first().rectInParent().height()) {
				reinitWithSameY();
			} else {
				reinitWithDiffY();
			}
			break;
		case WGLT_ORTH:
			if(fabs(first().pos().y()-second().pos().y()) < first().rectInParent().height()) {
				reinitWithSameY();
			} else {
				reinitWithDiffY();
			}
			break;
		case WGLT_STRAIGHT_LINE:
			simpleReinit();
			break;
		default:
			ERROR_LOG("Unexpected type %d.",int(line_type()));
			break;
	}
}
void WGraphLine::simpleReinit()noexcept
{
	try {
		std::vector<QPointF> points;

		if(!WSpace::getRectConnectLineWithOffset(first().rectInParent(),second().rectInParent(),
					back_inserter(points),
					QPointF(0,0),
					QPointF(0,0))) {
			ERROR_LOG("Get points faild.");
			return ;
		}
		prepareGeometryChange();
		line_       =   LineStripWithArrow(points.begin(),points.end(),arrow_size_,WSpace::LP_END);
		update();
	} catch(...) {
	}
}
bool WGraphLine::reinitWithSameY()noexcept
{
	auto                 kDelta            = 20;
	const auto           step              = 10;
	const auto           kMaxDelta         = 60;
	std::vector<QPointF> points;

	try {
		const auto           first_top_mid     = first().connectTopMid();
		const auto           first_bottom_mid  = first().connectBottomMid();
		const auto           second_top_mid    = second().connectTopMid();
		const auto           second_bottom_mid = second().connectBottomMid();

		while(kDelta<=kMaxDelta) {

			points.clear();

			points.push_back(first_top_mid);
			if(first_top_mid.y() < second_top_mid.y())
				points.push_back(first_top_mid-QPoint(0,kDelta));
			else
				points.emplace_back(second_top_mid-QPoint(0,kDelta));

			points.push_back(second_top_mid);
			prepareGeometryChange();
			if(line_.doInit(points.begin(),points.end(),arrow_size_,WSpace::LP_END,LineStripWithArrow::LD_UP)) {
				update();
				if(!collidingNode()) return true;
			}

			points.clear();
			points.push_back(first_bottom_mid);
			if(first_bottom_mid.y() > second_bottom_mid.y())
				points.push_back(first_bottom_mid+QPoint(0,kDelta));
			else
				points.emplace_back(first_bottom_mid.x(),second_bottom_mid.y()+kDelta);

			points.push_back(second_bottom_mid);
			prepareGeometryChange();
			if(line_.doInit(points.begin(),points.end(),arrow_size_,WSpace::LP_END,LineStripWithArrow::LD_DOWN)) {
				update();
				if(!collidingNode()) return true;
			}

			kDelta += step;
		}
		simpleReinit();
	} catch(...) {
	}
	return false;
}
bool WGraphLine::reinitWithDiffY()noexcept
{
	try {
		auto r      =  second().center()-first().center();
		auto angle  =  atan2(-r.y(),r.x());

		if((angle >-M_PI_2) && (angle<=0)) {
			return reinitSBR(first(),second(),WSpace::LP_END);
		} else if(angle<-M_PI_2) {
			return reinitSBL(first(),second(),WSpace::LP_END);
		} else if(angle>M_PI_2) {
			return reinitSBR(second(),first(),WSpace::LP_BEGIN);
		} else {
			return reinitSBL(second(),first(),WSpace::LP_BEGIN);
		}
	} catch(...) {
	}
	return false;
}
bool WGraphLine::reinitSBR(const WGraphItem& first,const WGraphItem& second,int arrow_pos)
{
	auto                 kDelta    = 20+(random()%9);
	const auto           kMaxDelta = 60;
	const auto           step      = 10;
	std::vector<QPointF> points;

	try {
		const auto first_top_mid      =  first.connectTopMid();
		const auto first_bottom_mid   =  first.connectBottomMid();
		const auto first_left_mid     =  first.connectLeftMid();
		const auto first_right_mid    =  first.connectRightMid();
		const auto second_top_mid     =  second.connectTopMid();
		const auto second_bottom_mid  =  second.connectBottomMid();
		const auto second_left_mid    =  second.connectLeftMid();
		const auto second_right_mid   =  second.connectRightMid();

		//0
		points.push_back(first_right_mid);
		points.push_back(second_top_mid);

		prepareGeometryChange();
		if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,LineStripWithArrow::LD_RIGHT,LineStripWithArrow::LD_DOWN)) {
			update();

			if(!collidingNode()) return true;
		}

		points.clear();
		points.push_back(first_bottom_mid);
		points.push_back(second_left_mid);

		prepareGeometryChange();
		if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,LineStripWithArrow::LD_DOWN,LineStripWithArrow::LD_RIGHT)) {
			update();
			if(!collidingNode()) return true;
		}

		//1
		while(kDelta <= kMaxDelta) {
			points.clear();
			points.push_back(first_left_mid);
			points.emplace_back(first_left_mid.x()-kDelta,first_left_mid.y());
			points.push_back(second_left_mid);

			prepareGeometryChange();
			if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,LineStripWithArrow::LD_LEFT)) {
				update();
				if(!collidingNode()) return true;
			}

			points.clear();
			points.push_back(first_right_mid);
			points.emplace_back(second_right_mid.x()+kDelta,first_right_mid.y());
			points.push_back(second_right_mid);

			prepareGeometryChange();
			if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,LineStripWithArrow::LD_RIGHT)) {
				update();

				if(!collidingNode()) return true;
			}
			kDelta += step;
		}
		//2
		for(auto i=first_right_mid.x()+20; i<=second_left_mid.x()-20; i+= step) {
			points.clear();
			points.push_back(first_right_mid);
			points.emplace_back(i,first_right_mid.y());
			points.push_back(second_left_mid);

			prepareGeometryChange();
			if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,LineStripWithArrow::LD_RIGHT,LineStripWithArrow::LD_RIGHT)) {
				update();
				if(!collidingNode()) return true;
			}
		}
		//3
		for(auto i=first_bottom_mid.y()+20; i<=second_top_mid.y()-20; i+= step) {
			points.clear();
			points.push_back(first_bottom_mid);
			points.emplace_back(first_bottom_mid.x(),i);
			points.push_back(second_top_mid);

			prepareGeometryChange();
			if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,LineStripWithArrow::LD_DOWN,LineStripWithArrow::LD_DOWN)) {
				update();
				if(!collidingNode()) return true;
			}
		}
		//4
		if(reinitSBRML0(first_right_mid,second_top_mid,arrow_pos,LineStripWithArrow::LD_RIGHT,LineStripWithArrow::LD_ANY)) 
			return true;
		if(reinitSBRML1(first_bottom_mid,second_left_mid,arrow_pos,LineStripWithArrow::LD_DOWN,LineStripWithArrow::LD_ANY)) 
			return true;

		if(reinitSBRML0(first_top_mid-QPointF(0,20),second_top_mid,arrow_pos,LineStripWithArrow::LD_UP,LineStripWithArrow::LD_ANY,
			first.boundingRect().width()/2+20,20,
		{first_top_mid})) 
			return true;
		if(reinitSBRML0(first_right_mid,second_right_mid+QPointF(20,0),arrow_pos,LineStripWithArrow::LD_RIGHT,LineStripWithArrow::LD_ANY,
			20,second.boundingRect().height()/2+20,
		{},{second_right_mid})) 
			return true;
		if(reinitSBRML1(first_left_mid-QPointF(20,0),second_left_mid,arrow_pos,LineStripWithArrow::LD_LEFT,LineStripWithArrow::LD_ANY,
			first.boundingRect().height()/2+20,20,
		{first_left_mid})) 
			return true;
		if(reinitSBRML1(first_bottom_mid,second_bottom_mid+QPointF(0,20),arrow_pos,LineStripWithArrow::LD_DOWN,LineStripWithArrow::LD_ANY,
			20,second.boundingRect().width()/2+20,
		{},{second_bottom_mid})) 
			return true;
		//6
		points.clear();
		points.push_back(first_left_mid);
		points.push_back(first_left_mid-QPointF(20,0));
		points.push_back(second_bottom_mid+QPointF(0,20));
		points.push_back(second_bottom_mid);

		prepareGeometryChange();
		if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,LineStripWithArrow::LD_LEFT,LineStripWithArrow::LD_UP)) {
			update();
			if(!collidingNode()) return true;
		}

		simpleReinit();
	} catch(...) {
	}
	return false;
}
bool WGraphLine::reinitSBRML0(const QPointF& p0, const QPointF& p1,int arrow_pos,
LineStripWithArrow::LineDirection bd, LineStripWithArrow::LineDirection ed,
int d0,int d1,
const vector<QPointF>& ph, const vector<QPointF>& pt)
{
	const auto           step      = 10;
	std::vector<QPointF> points;

	for(auto i=p0.x()+d0; i<=p1.x()-d1; i+= step) {
		points.clear();
		points = ph;
		points.push_back(p0);
		points.emplace_back(i,p0.y());
		points.emplace_back(i,p1.y()-d1);
		points.push_back(p1-QPointF(0,d1));
		points.push_back(p1);
		points.insert(points.end(),pt.begin(),pt.end());

		prepareGeometryChange();
		if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,bd,ed)) {
			update();
			if(!collidingNode()) return true;
		}
	}
	return false;
}
bool WGraphLine::reinitSBRML1(const QPointF& p0, const QPointF& p1,int arrow_pos,
LineStripWithArrow::LineDirection bd, LineStripWithArrow::LineDirection ed,
int d0,int d1,
const vector<QPointF>& ph, const vector<QPointF>& pt)
{
	const auto           step      = 10;
	std::vector<QPointF> points;

	for(auto i=p0.y()+d0; i<=p1.y()-d1; i+= step) {
		points.clear();
		points = ph;
		points.push_back(p0);
		points.emplace_back(p0.x(),i);
		points.emplace_back(p1.x()-d1,i);
		points.push_back(p1-QPointF(d1,0));
		points.push_back(p1);
		points.insert(points.end(),pt.begin(),pt.end());

		prepareGeometryChange();
		if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,bd,ed)) {
			update();
			if(!collidingNode()) return true;
		}
	}
	return false;
}
bool WGraphLine::reinitSBL(const WGraphItem& first,const WGraphItem& second,int arrow_pos)
{
	auto                 kDelta    = 20+(random()%9);
	const auto           kMaxDelta = 70;
	const auto           step      = 10;
	std::vector<QPointF> points;

	try {
		const auto first_top_mid      =  first.connectTopMid();
		const auto first_bottom_mid   =  first.connectBottomMid();
		const auto first_left_mid     =  first.connectLeftMid();
		const auto first_right_mid    =  first.connectRightMid();
		const auto second_top_mid     =  second.connectTopMid();
		const auto second_bottom_mid  =  second.connectBottomMid();
		const auto second_left_mid    =  second.connectLeftMid();
		const auto second_right_mid   =  second.connectRightMid();

		points.push_back(first_left_mid);
		points.push_back(second_top_mid);

		prepareGeometryChange();
		if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,LineStripWithArrow::LD_LEFT,LineStripWithArrow::LD_DOWN)) {
			update();

			if(!collidingNode()) return true;
		}

		points.clear();
		points.push_back(first_bottom_mid);
		points.push_back(second_right_mid);

		prepareGeometryChange();
		if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,LineStripWithArrow::LD_DOWN,LineStripWithArrow::LD_LEFT)) {
			update();
			if(!collidingNode()) return true;
		}

		while(kDelta <= kMaxDelta) {
			points.clear();
			points.push_back(first_left_mid);
			points.emplace_back(second_left_mid.x()-kDelta,first_left_mid.y());
			points.push_back(second_left_mid);

			prepareGeometryChange();
			if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,LineStripWithArrow::LD_LEFT)) {
				update();

				if(!collidingNode()) return true;
			}

			points.clear();
			points.push_back(first_right_mid);
			points.emplace_back(first_right_mid.x()+kDelta,first_right_mid.y());
			points.push_back(second_right_mid);

			prepareGeometryChange();
			if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,LineStripWithArrow::LD_RIGHT)) {
				update();
				if(!collidingNode()) return true;
			}
			kDelta += step;
		}
		for(auto i=first_left_mid.x()-20; i>=second_right_mid.x()+20; i-= step) {
			points.clear();
			points.push_back(first_left_mid);
			points.emplace_back(i,first_left_mid.y());
			points.push_back(second_right_mid);

			prepareGeometryChange();
			if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,LineStripWithArrow::LD_LEFT,LineStripWithArrow::LD_LEFT)) {
				update();
				if(!collidingNode()) return true;
			}
		}
		for(auto i=first_bottom_mid.y()+20; i<=second_top_mid.y()-20; i+= step) {
			points.clear();
			points.push_back(first_bottom_mid);
			points.emplace_back(first_bottom_mid.x(),i);
			points.push_back(second_top_mid);

			prepareGeometryChange();
			if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,LineStripWithArrow::LD_DOWN,LineStripWithArrow::LD_DOWN)) {
				update();
				if(!collidingNode()) return true;
			}
		}

		if(reinitSBLML0(first_left_mid,second_top_mid,arrow_pos,LineStripWithArrow::LD_LEFT,LineStripWithArrow::LD_DOWN))
			return true;
		if(reinitSBLML1(first_bottom_mid,second_right_mid,arrow_pos,LineStripWithArrow::LD_DOWN,LineStripWithArrow::LD_LEFT))
			return true;
		if(reinitSBLML0(first_top_mid-QPointF(0,20),second_top_mid,arrow_pos,LineStripWithArrow::LD_UP,LineStripWithArrow::LD_DOWN,
			first.boundingRect().width()/2+20,20,
		{first_top_mid}))
			return true;
		if(reinitSBLML0(first_left_mid,second_left_mid-QPointF(20,0),arrow_pos,LineStripWithArrow::LD_LEFT,LineStripWithArrow::LD_RIGHT,
			20,second.boundingRect().height()/2+20,
		{},{second_left_mid}))
			return true;
		if(reinitSBLML1(first_right_mid+QPointF(20,0),second_right_mid,arrow_pos,LineStripWithArrow::LD_RIGHT,LineStripWithArrow::LD_LEFT,
			first.boundingRect().height()/2+20,20,
		{first_right_mid}))
			return true;
		if(reinitSBLML1(first_bottom_mid,second_bottom_mid-QPointF(0,20),arrow_pos,LineStripWithArrow::LD_DOWN,LineStripWithArrow::LD_UP,
			20,second.boundingRect().width()/2+20,
		{},{second_bottom_mid}))
			return true;

		points.clear();
		points.push_back(first_right_mid);
		points.push_back(first_right_mid+QPointF(20,0));
		points.push_back(second_bottom_mid+QPointF(0,20));
		points.push_back(second_bottom_mid);

		prepareGeometryChange();
		if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,LineStripWithArrow::LD_LEFT,LineStripWithArrow::LD_UP)) {
			update();
			if(!collidingNode()) return true;
		}
		simpleReinit();
	} catch(...) {
	}
	return false;
}
bool WGraphLine::reinitSBLML0(const QPointF& p0, const QPointF& p1,int arrow_pos,
LineStripWithArrow::LineDirection bd, LineStripWithArrow::LineDirection ed,
int d0,int d1,
const vector<QPointF>& ph, const vector<QPointF>& pt)
{
	const auto           step      = 10;
	std::vector<QPointF> points;

	for(auto i=p0.x()-d0; i>=p1.x()+d1; i-= step) {
		points.clear();
		points = ph;
		points.push_back(p0);
		points.emplace_back(i,p0.y());
		points.emplace_back(i,p1.y()-d1);
		points.push_back(p1-QPointF(0,d1));
		points.push_back(p1);
		points.insert(points.end(),pt.begin(),pt.end());

		prepareGeometryChange();
		if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,bd,ed)) {
			update();
			if(!collidingNode()) return true;
		}
	}
	return false;
}
bool WGraphLine::reinitSBLML1(const QPointF& p0, const QPointF& p1,int arrow_pos,
LineStripWithArrow::LineDirection bd, LineStripWithArrow::LineDirection ed,
int d0,int d1,
const vector<QPointF>& ph, const vector<QPointF>& pt)
{
	const auto           step      = 10;
	std::vector<QPointF> points;

	for(auto i=p0.y()+d0; i<=p1.y()-d1; i+= step) {
		points.clear();
		points = ph;
		points.push_back(p0);
		points.emplace_back(p0.x(),i);
		points.emplace_back(p1.x()+d1,i);
		points.push_back(p1+QPointF(d1,0));
		points.push_back(p1);
		points.insert(points.end(),pt.begin(),pt.end());

		prepareGeometryChange();
		if(line_.doInit(points.begin(),points.end(),arrow_size_,arrow_pos,bd,ed)) {
			update();
			if(!collidingNode()) return true;
		}
	}
	return false;
}
