/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <boost/graph/random_layout.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topology.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/make_connected.hpp>
#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/geometry/geometries/geometries.hpp> 
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry.hpp>
#include <vector>

namespace WGraph
{
	using topology_type   =  boost::rectangle_topology<> ;
	using point_type      =  topology_type::point_type ;
	using Graph           =  boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::property<boost::vertex_index_t,int>,boost::property<boost::edge_weight_t,double>>;
	using Vertex          =  boost::graph_traits<Graph>::vertex_descriptor ;
	using PositionVec     =  std::vector<point_type> ;
	using PositionMap     =  boost::iterator_property_map<PositionVec::iterator, boost::property_map<Graph, boost::vertex_index_t>::type>;
	using VertexIndexPM   =  boost::property_map<Graph, boost::vertex_index_t>::type;
	using EdgeWeightPM    =  boost::property_map<Graph, boost::edge_weight_t>::type;
	using VertexIterator  =  boost::graph_traits<Graph>::vertex_iterator;
	using EdgeIterator    =  boost::graph_traits<Graph>::edge_iterator;
	namespace bg   =  boost::geometry;
	using EPoint          =  bg::model::d2::point_xy<float>;
	using EMultiPoint     =  bg::model::multi_point<EPoint>;

	struct kamada_kawai_done 
	{
		kamada_kawai_done() : last_delta() {}

		template<typename Graph>
			bool operator()(double delta_p, 
					typename boost::graph_traits<Graph>::vertex_descriptor /*p*/,
					const Graph& /*g*/,
					bool global)
			{
				if(count++>10000) return true;
				if (global) {
					double diff = last_delta - delta_p;
					if (diff < 0) diff = -diff;
					last_delta = delta_p;
					return diff < 1;
				} else {
					return delta_p < 1;
				}   
			}

		double last_delta;
		int    count      = 0;
	};
}
enum WGraphItemType {
	WGI_NODE,
	WGI_LINE,
};
enum WGraphItemID {
	WGI_TYPE    ,   
	WGI_IN_ID   ,  //for line
	WGI_OUT_ID  ,  //for line
	WGI_ID      ,  //for node
};
enum WGraphLineType
{
	WGLT_FREE,
	WGLT_ORTH,
	WGLT_STRAIGHT_LINE,
};
