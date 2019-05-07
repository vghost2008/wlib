/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

_Pragma("once")
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace wgraph_bg   =  boost::geometry;
namespace wgraph_bgi  =  boost::geometry::index;

using WGraphPoint  =  wgraph_bg::model::point<float,2, wgraph_bg::cs::cartesian>;
using WGraphBox    =  wgraph_bg::model::box<WGraphPoint>;

struct WGraphTValue
{
	int   id;
	WGraphBox v;
};
struct WGraphTValueEqual
{
	using result_type = bool;
	inline bool operator()(const WGraphTValue& lhv, const WGraphTValue& rhv) const {
		return lhv.id == rhv.id;
	}
};
struct WGraphTValueIndexable
{
	using result_type = const WGraphBox&;
	inline result_type operator()(const WGraphTValue& v)const {
		return v.v;
	}
};
using WGraphTree=wgraph_bgi::rtree<WGraphTValue, wgraph_bgi::quadratic<128>,WGraphTValueIndexable,WGraphTValueEqual>;
