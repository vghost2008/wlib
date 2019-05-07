/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#include <wgraphscene.h>
#include <wgraphitem.h>
#include <list>
#include <wlogmacros.h>
#include <wgeometry.h>
#include <boost/geometry/geometries/register/linestring.hpp> 
#include <map>

using namespace std;
using namespace boost;
using namespace WGraph;

BOOST_GEOMETRY_REGISTER_LINESTRING(std::vector<QPointF>);

WGraphScene::WGraphScene()
{
}
WGraphScene::~WGraphScene()
{
	qDebug()<<__func__;
}
void WGraphScene::removeAllItems()
{
	auto item_list = items();
	id_to_item_map_.clear();

	clearFocus();
	clearSelection();

	for(int i=0; i<item_list.size(); ++i) {
		removeItem(item_list[i]);
	}

	clear();
}
void WGraphScene::addLinkLine()
{
	addLinkLine(item_connection_);
}
void WGraphScene::addLinkLine(const map<int,vector<int>>& item_connection)
{
	for(auto t:item_connection) {
		auto item_in = getItemById(t.first);

		if(nullptr == item_in) continue;
		for(auto id:t.second) {
			auto item_out = getItemById(id);
			if(nullptr == item_out) continue;
			auto  iol      = std::make_shared<WGraphLine>(item_in,item_out,line_type()); //Line的所有权由两个结点保存，

			addItem(iol.get());
			iol->initLine();
		}
	}
}
void WGraphScene::initItemPosition(float min_length,const QSizeF& view_size,const QSizeF& min_margin)
{
	VertexIterator     vi;
	VertexIterator     vi_end;

	Graph g(item_connection_.size());
	PositionVec    position_vec(num_vertices(g));
	PositionMap    position(position_vec.begin(), get(vertex_index, g));
	VertexIndexPM  v_index      = get(vertex_index, g);

	getItemPosition(view_size,min_length,g,position);	

	for(std::tie(vi,vi_end) = vertices(g); vi!=vi_end; ++vi) {
		auto id = next(item_connection_.begin(),get(v_index,*vi))->first;
		auto item = getItemById(id);
		if(item == nullptr) continue;
		item->setPos(position[*vi][0],position[*vi][1]);
	}
	adjustItemPosition(min_margin.width(),min_margin.height());
	setSceneRect(QRectF());
}
void WGraphScene::getItemPosition(const QSizeF& view_size,float min_length,Graph& g,PositionMap& position)
{
	EdgeWeightPM       e_index = get(edge_weight, g);
	EdgeIterator       ei;
	EdgeIterator       ei_end;
	boost::minstd_rand gen;

	for(auto it=item_connection_.begin(); it!=item_connection_.end(); ++it) {
		auto src_index = std::distance(item_connection_.begin(),it);
		for(auto& id:it->second) {
			auto jt = item_connection_.find(id);
			if(jt == item_connection_.end()) continue;
			auto dest_index = std::distance(item_connection_.begin(),jt);
			add_edge(src_index,dest_index,1,g);
		}
	}

	make_connected(g);

	for(std::tie(ei,ei_end)=edges(g); ei!=ei_end; ++ei) {
		put(e_index,*ei,1);
	}

	const topology_type topo(gen, -view_size.width()/2, 20, view_size.width()/2, view_size.height()+20);

	random_graph_layout(g, position, topo);
	kamada_kawai_spring_layout(g,position,get(edge_weight, g),topo,edge_length(min_length+10),kamada_kawai_done());
	fruchterman_reingold_force_directed_layout (g, position, topo, cooling(linear_cooling<double>(50)));
}
WGraphItem& WGraphScene::getCurrentItem()noexcept(false)
{
	auto item_list = items(current_pos_,Qt::IntersectsItemShape,Qt::AscendingOrder);

	if(item_list.isEmpty()) throw runtime_error("null current item.");

	auto res = dynamic_cast<WGraphItem*>(item_list[0]);

	if(nullptr == res) throw runtime_error("null current item.");

	return *res;
}
WGraphLine& WGraphScene::getCurrentLine()noexcept(false)
{
	auto item= itemAt(current_pos_,QTransform());

	if(nullptr == item) {
		throw runtime_error("null current item.");
	}

	auto res = dynamic_cast<WGraphLine*>(item);

	if(nullptr == res) throw runtime_error("null current item.");

	return *res;
}
void WGraphScene::checkLines()
{
	for(auto& n:id_to_item_map_) {
		n.second->checkLines();
	}
}
void WGraphScene::adjustItemPosition(float x_step, float y_step)
{
	const auto kAdjustCount = 5;
	int        adjustCount  = 0;
	auto       _its         = items();
	vector<QGraphicsItem*> its(_its.begin(),_its.end());
	sort(its.begin(),its.end(),[](QGraphicsItem* lhv,QGraphicsItem* rhv){ 
			if(lhv->pos().y() < rhv->pos().y()) return true;
			if(lhv->pos().y() > rhv->pos().y()) return false;
			return fabs(lhv->pos().x()) < fabs(rhv->pos().x());});

	for(auto item:its) {
		if(!hasOverlap(item))continue;
		const auto org_pos = item->pos();

		for(adjustCount=1; adjustCount<=kAdjustCount; ++adjustCount) {
			item->setPos(org_pos-QPointF(adjustCount*x_step,0));
			if(!hasOverlap(item)) goto adjust_success;
			item->setPos(org_pos+QPointF(adjustCount*x_step,0));
			if(!hasOverlap(item)) goto adjust_success;
			item->setPos(org_pos-QPointF(0,adjustCount*y_step));
			if(!hasOverlap(item)) goto adjust_success;
			item->setPos(org_pos+QPointF(0,adjustCount*y_step));
			if(!hasOverlap(item)) goto adjust_success;
			auto k = adjustCount/1.5;
			item->setPos(org_pos-QPointF(k*y_step,k*y_step));
			if(!hasOverlap(item)) goto adjust_success;
			item->setPos(org_pos+QPointF(k*y_step,-k*y_step));
			if(!hasOverlap(item)) goto adjust_success;
			item->setPos(org_pos+QPointF(-k*y_step,k*y_step));
			if(!hasOverlap(item)) goto adjust_success;
			item->setPos(org_pos+QPointF(k*y_step,k*y_step));
			if(!hasOverlap(item)) goto adjust_success;
		}

		for(adjustCount=kAdjustCount+1; adjustCount<=kAdjustCount*3; ++adjustCount) {
			item->setPos(org_pos+QPointF(0,adjustCount*y_step));
			if(!hasOverlap(item)) goto adjust_success;
		}
		item->setPos(org_pos);
adjust_success:
		continue;
	}
}
bool WGraphScene::hasOverlap(QGraphicsItem* item)
{
	auto const kDelta = 10;
	auto       rect   = (item->mapRectToScene(item->boundingRect())).adjusted(-kDelta,-kDelta,kDelta,kDelta);
	auto       its    = items(rect);

	if(its.size()<1) {
		ERR("Unexcepted.");
	}

	return its.size()>1;
}
void WGraphScene::graphTreeRemove(int id)const
{
	graphTree().remove({id,WGraphBox()});
}
void WGraphScene::updateGraphTree()const
{
	QList<QGraphicsItem*> item_list = items();

	graphTree().clear();

	for(auto& item: item_list) {
		if(item->data(WGI_TYPE).toInt() == WGI_LINE) continue;
		auto it = dynamic_cast<const WGraphItem*>(item);
		if(nullptr == it)continue;
		graphTreeInsert(item->data(WGI_ID).toInt(),it->rectInParent());
	}
}
void WGraphScene::updateGraphTree(int id, const QRectF& v)const
{
	graphTreeRemove(id);
	graphTreeInsert(id,v);
}
void WGraphScene::updateGraphTree(const WGraphItem& item)const
{
	updateGraphTree(item.data(WGI_ID).toInt(),item.rectInParent());
}
bool WGraphScene::collidingNode(const WGraphLine& line)const
{
	vector<WGraphTValue> returned_values;
	const auto       in_id           = line.data(WGI_IN_ID).toInt();
	const auto       out_id          = line.data(WGI_OUT_ID).toInt();

	graphTree().query(wgraph_bgi::intersects(line.allPoints()), std::back_inserter(returned_values));

	for(const auto& v:returned_values) {
		if((v.id == in_id) || (v.id==out_id)) continue;
		return true;
	}
	return false;
}
void WGraphScene::graphTreeInsert(int id,const QRectF& v)const
{
	WGraphTValue value;
	value.id = id;
	bg::assign(value.v,v.adjusted(-5,-5,5,5));
	graphTree().insert(value);
}
vector<WGraphLine*> WGraphScene::collidingLines(const WGraphItem& item)
{
	auto               items = this->items();
	const auto         id    = item.data(WGI_ID).toUInt();
	vector<WGraphLine*>  res;

	for(auto item:items) {
		if(item->data(WGI_TYPE).toUInt()==WGI_NODE) continue;

		const auto in_id  = item->data(WGI_IN_ID).toUInt();
		const auto out_id = item->data(WGI_OUT_ID).toUInt();
		auto       it     = dynamic_cast<WGraphLine *>(item);

		if((in_id == id)||(out_id==id) || (nullptr == it)) continue;
		if(!collidingNode(*it)) continue;
		res.push_back(it);
	}

	return res;
}
std::shared_ptr<WGraphItem> WGraphScene::getItemById(int id)noexcept
{
	auto it = id_to_item_map_.find(id);
	if(id_to_item_map_.end() == it) return nullptr;
	return it->second;
}
void WGraphScene::addItem(std::shared_ptr<WGraphItem> item)
{
	if(id_to_item_map_.find(item->id()) != id_to_item_map_.end()) {
		ERROR_LOG("Id %d already exists!",item->id());
		return;
	}
	id_to_item_map_[item->id()] = item;
	addItem(item.get());
}
void WGraphScene::addItem(QGraphicsItem* item)
{
	QGraphicsScene::addItem(item);
}
