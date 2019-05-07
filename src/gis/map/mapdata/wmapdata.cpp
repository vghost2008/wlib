#include <wmapdata.h>
#include <wlogmacros.h>
#include <toolkit.h>
#include <toolkit_qt.h>
#include <boost/property_tree/xml_parser.hpp>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Partition_traits_2.h>
#include <CGAL/partition_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Projection_traits_xy_3.h>
#include <list>
#include <fstream>
#include <gltoolkit.h>
#include <wthreadtools.h>

#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>

#ifndef     DISABLE_MAPVIEW
#define     LABEL_XPOS_STR           "CENTROID_X"
#define     LABEL_YPOS_STR           "CENTROID_Y"
#define     LABEL_NAME_STR           "NAME"            
#define     LABEL_TEXT_CODEC         "GB2312"
#define     MAP_ROOT_TAG             "Map"
#define     LAYOUTS_TAG              "Layouts"
#define     LAYOUT_TAG               "Layout"
#define     LAYOUT_FILE_NAME_TAG     "FileName"
#define     LAYOUT_VISIBLE_TAG       "Visible"
#define     LINE_COLOR_TAG           "LineColor"
#define     LINE_WIDTH_TAG           "LineWidth"
#define     TEXT_COLOR_TAG           "TextColor"
#define     TEXT_SIZE_TAG            "TextSize"
#define     FILL_COLOR_TAG           "FillColor"
#define     SHOW_LABEL_TAG           "ShowLabel"
#define     MAP_LEVEL_TAG            "MapLevel"
#define     XML_ATTR_TAG             "<xmlattr>"
#define     POINT_SIZE_TAG           "PointSize"
#define     POINT_TYPE               "PointType"
#define     POLYGON_TYPE             "PolygonType"
#define     TEXT_OFFSET              "TextOffset"


using namespace std;
using namespace boost::property_tree;
using namespace WSpace;
namespace wmap
{
	struct FaceInfo2
	{
		FaceInfo2(){}
		int nesting_level;
		bool in_domain(){
			return nesting_level%2 == 1;
		}
	};
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	typedef CGAL::Triangulation_vertex_base_2<K> Vb;
	typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2,K> Fbb;
	typedef CGAL::Constrained_triangulation_face_base_2<K,Fbb> Fb;
	typedef CGAL::Triangulation_data_structure_2<Vb,Fb> TDS;
	typedef CGAL::Exact_predicates_tag Itag;
	typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag> CDT;
	typedef CDT::Point Point;
	typedef CGAL::Polygon_2<K> Polygon_2;
	typedef CDT::Vertex_handle Vertex_handle;
	template<class IterInput>
		bool insert_constraint(CDT& cdt,IterInput begin, IterInput end) {
			auto          it     = begin;
			Vertex_handle h0     = cdt.insert(Point(it->x,it->y));
			Vertex_handle fh     = h0;
			Vertex_handle sh;

			++it;

			try {
				for(; it!=end; ++it) {
					auto& point = *it;
					sh = cdt.insert(Point(point.x,point.y));
					cdt.insert_constraint(fh,sh);
					fh = sh;
				}
				cdt.insert_constraint(fh,h0);
			} catch(...) {
#if 0
				cout<<"{";
				for(auto&p :polygon->exterior) {
					cout<<"("<<p.x<<","<<p.y<<"), ";
				}
				cout<<"}"<<endl;
				qDebug()<<"Unexpected.";
#endif
				return false;
			}
			return true;
		}
	void mark_domains(CDT& ct, CDT::Face_handle start, int index, std::list<CDT::Edge>& border ) {
		if(start->info().nesting_level != -1){
			return;
		}
		std::list<CDT::Face_handle> queue;
		queue.push_back(start);
		while(! queue.empty()){
			CDT::Face_handle fh = queue.front();
			queue.pop_front();
			if(fh->info().nesting_level == -1){
				fh->info().nesting_level = index;
				for(int i = 0; i < 3; i++){
					CDT::Edge e(fh,i);
					CDT::Face_handle n = fh->neighbor(i);
					if(n->info().nesting_level == -1){
						if(ct.is_constrained(e)) border.push_back(e);
						else queue.push_back(n);
					}
				}
			}
		}
	}
	void mark_domains(CDT& cdt)
	{
		for(CDT::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); ++it){
			it->info().nesting_level = -1;
		}
		std::list<CDT::Edge> border;
		mark_domains(cdt, cdt.infinite_face(), 0, border);
		while(! border.empty()){
			CDT::Edge e = border.front();
			border.pop_front();
			CDT::Face_handle n = e.first->neighbor(e.second);
			if(n->info().nesting_level == -1){
				mark_domains(cdt, n, e.first->info().nesting_level+1, border);
			}
		}
	}
}

bool wmap::insertPoint(const OGRPoint& point,WMapViewData* view_data,float z)
{
	if(!isgood(point)) return false;
	view_data->points.data.push_back(WMPointObj({point.getX(),point.getY(),z}));
	return true;
}
bool wmap::insertMultiPoint(const OGRMultiPoint& mpoint, WMapViewData* view_data,float z)
{
	const int num = mpoint.getNumGeometries();
	for(auto i=0; i<num; ++i) {
		try {
			insertPoint(dynamic_cast<const OGRPoint&>(*mpoint.getGeometryRef(i)),view_data,z);
		} catch(...) {
		}
	}
	return true;
}
bool wmap::insertLineString(const OGRLineString& line_string,WMapViewData* view_data,float z)
{
	OGRPoint     point;
	WMLineString mls;

	mls.data.reserve(line_string.getNumPoints());
	for(auto j=0; j<line_string.getNumPoints(); ++j) {
		line_string.getPoint(j,&point);
		if(!isgood(point)) continue;
		mls.data.emplace_back(WMPointObj({point.getX(),point.getY(),z}));
	}
	if(mls.data.size() < 2) return false;
	view_data->linestrings.emplace_back(std::move(mls));
	return true;
}
bool wmap::insertMultiLineString(const OGRMultiLineString& mlstring,WMapViewData* view_data,float z)
{
	const int num = mlstring.getNumGeometries();
	for(auto i=0; i<num; ++i) {
		try {
			insertLineString(dynamic_cast<const OGRLineString&>(*mlstring.getGeometryRef(i)),view_data,z);
		} catch(...) {
		}
	}
	return true;
}
bool wmap::insertPolygon(const OGRPolygon& polygon, WMapViewData* view_data,float z)
{
	int                  j;
	OGRPoint             point;
	const OGRLinearRing *polinearring = nullptr;
	WMPolygon            mpoly;
	/*
	 * 外部环
	 */
	polinearring = polygon.getExteriorRing();

	auto point_count = polinearring->getNumPoints();

	mpoly.exterior.reserve(point_count);

	for(j=0; j<point_count; ++j) {
		polinearring->getPoint(j,&point);
		if(!isgood(point)) continue;
		mpoly.exterior.emplace_back(WMPointObj({point.getX(),point.getY(),z}));
	}
	if(mpoly.exterior.size() < 3) return false;
	/*
	 * 内部环
	 */
	auto interior_count = polygon.getNumInteriorRings();
	for(auto i=0; i<interior_count; ++i) {
		vector<WMPointObj> points;
		polinearring = polygon.getInteriorRing(i);
		point_count = polinearring->getNumPoints();
		points.reserve(point_count);
		for(j=0; j<point_count; ++j) {
			polinearring->getPoint(j,&point);
			if(!isgood(point)) continue;
			points.emplace_back(WMPointObj({point.getX(),point.getY(),z}));
		}
		if(points.size() < 3) continue;
		mpoly.interior.emplace_back(std::move(points));
	}

	view_data->polygons.emplace_back(std::move(mpoly));
	return true;
}
bool wmap::insertMultiPolygon(const OGRMultiPolygon& mpolygon, WMapViewData* view_data,float z)
{
	const int num = mpolygon.getNumGeometries();

	for(auto i=0; i<num; ++i) {
		try {
			insertPolygon(dynamic_cast<const OGRPolygon&>(*mpolygon.getGeometryRef(i)),view_data,z);
		} catch( ...) {
		}
	}
	return true;
}
void wmap::clearViewData(WMapViewData* view_data)
{
	view_data->points.data.clear();
	view_data->labels.clear();
	releaseMemAfterVList(view_data);
}
void wmap::releaseMemAfterVList(WMapViewData* view_data)
{
	view_data->polygons.clear();
	view_data->linestrings.clear();
}
bool wmap::addLayout(OGRLayer& layout, WMapViewData* view_data,shared_ptr<OGRSpatialReference>* proj_spatial,shared_ptr<OGRSpatialReference>* geo_spatial,float point_z,float line_z,float polygon_z)
{
	auto        &extent = view_data->extent;
	OGREnvelope  e;

	{ //坐标系
		char                *pretty_wkt = NULL;
		OGRSpatialReference *spatial    = layout.GetSpatialRef();

		if(nullptr != spatial) {
			spatial->exportToPrettyWkt(&pretty_wkt);
			INFO_LOG("ISPROJECTED:%d, ISGEOGRAPHICS:%d.",spatial->IsProjected(),spatial->IsGeographic());
			INFO_LOG("WKT:%s.",pretty_wkt);
			if(spatial->IsProjected()) {
				if(nullptr == *proj_spatial) {
					*proj_spatial = shared_ptr<OGRSpatialReference>(spatial->Clone());
					*geo_spatial = shared_ptr<OGRSpatialReference>(spatial->CloneGeogCS());
				}
			}
			OGRFree(pretty_wkt);
		} 
		OGRStyleTable* style_table = layout.GetStyleTable();
		if(nullptr != style_table) {
			style_table->Print(stdout);
		} 
	}

	layout.ResetReading();
	if(OGRERR_NONE==layout.GetExtent(&e,true)) {
		extent.Merge(e);
	}

	OGRFeature * pofeature = NULL;

	layout.ResetReading();
	while((pofeature=layout.GetNextFeature()) != NULL) {
		OGRGeometry* pogeometry = NULL;
		if((pogeometry=pofeature->GetGeometryRef()) == NULL)continue;
		OGRFeatureDefn* pofdefn = layout.GetLayerDefn();
		getTextLabel(pofeature,pofdefn,pogeometry,&view_data->labels);
		insertOGRGeometry(*pogeometry,view_data,point_z,line_z,polygon_z);
		OGRFeature::DestroyFeature(pofeature);
	}
	INFO_LOG("Layout info:polygons=%d,line strings=%d,points=%d,labels=%d,",view_data->polygons.size(),view_data->linestrings.size(),view_data->points.data.size(),view_data->labels.size());
	return true;
}
void wmap::getTextLabel(OGRFeature* pofeature,OGRFeatureDefn* pofdefn,OGRGeometry* pogeometry,list<WMLabel>* label_list)
{
	int      j;
	WMLabel  text_label;
	unsigned text_label_data_flag = MLE_ALL;

	if(nullptr == pofdefn) return;

	for(j=0; j<pofdefn->GetFieldCount(); ++j) {
		OGRFieldDefn* pofielddefn = pofdefn->GetFieldDefn(j);
		switch(pofielddefn->GetType()) {
			case OFTInteger:
				if(strncmp(pofielddefn->GetNameRef(),LABEL_XPOS_STR,strlen(LABEL_XPOS_STR))==0) {
					text_label.pos.setX(pofeature->GetFieldAsInteger(j));
					text_label_data_flag &= (~MLE_POS_X);
				} else if(strncmp(pofielddefn->GetNameRef(),LABEL_YPOS_STR,strlen(LABEL_YPOS_STR))==0) {
					text_label_data_flag &= (~MLE_POS_Y);
					text_label.pos.setY(pofeature->GetFieldAsInteger(j));
				} 
				break;
			case OFTReal:
				if(strncmp(pofielddefn->GetNameRef(),LABEL_XPOS_STR,strlen(LABEL_XPOS_STR))==0) {
					text_label.pos.setX(pofeature->GetFieldAsDouble(j));
					text_label_data_flag &= (~MLE_POS_X);
				} else if(strncmp(pofielddefn->GetNameRef(),LABEL_YPOS_STR,strlen(LABEL_YPOS_STR))==0) {
					text_label.pos.setY(pofeature->GetFieldAsDouble(j));
					text_label_data_flag &= (~MLE_POS_Y);
				}
				break;
			case OFTString:
				if(strncmp(pofielddefn->GetNameRef(),LABEL_NAME_STR,strlen(LABEL_NAME_STR))==0) {
					text_label_data_flag &= (~MLE_TEXT);
					text_label.text = WSpace::transDecoding(pofeature->GetFieldAsString(j),LABEL_TEXT_CODEC);
				}
				break;
			default:
				printf("FD%s\n",pofeature->GetFieldAsString(j));
				break;
		}
	}
	if(text_label_data_flag&MLE_TEXT)  return;
	if((text_label_data_flag&MLE_POS_X)||(text_label_data_flag&MLE_POS_Y)) {
		OGRPoint point;
		if(OGRERR_NONE != pogeometry->Centroid(&point)) return;
		text_label.pos.setX(point.getX());
		text_label.pos.setY(point.getY());
		text_label_data_flag &= (~MLE_POS_X);
		text_label_data_flag &= (~MLE_POS_Y);
	}

	if(0 != text_label_data_flag) return;

	if(wkbFlatten(pogeometry->getGeometryType()) == wkbPolygon)
		text_label.area = reinterpret_cast<OGRPolygon*>(pogeometry)->get_Area();
	else
		text_label.area = 0;
	if(!isgood(text_label.pos.x()) || !isgood(text_label.pos.y())) return;
	pushLabel(std::move(text_label),label_list);
}
bool wmap::pushLabel(WMLabel&& label,std::list<WMLabel>* label_list)
{
	auto it = find_if(label_list->begin(),label_list->end(),[&label](const WMLabel& l)->bool{ return l.text == label.text;});

	if(it != label_list->end()) {
		if(it->area>label.area) return false; //保留区域大的名字
		label_list->erase(it);
	}
	label_list->push_back(label);
	return true;
}
void wmap::insertOGRGeometry(const OGRGeometry& ogeometry,WMapViewData* view_data,float point_z,float line_z,float polygon_z)
{
	try {
		switch(wkbFlatten(ogeometry.getGeometryType())) {
			case wkbPolygon:
			    insertPolygon(dynamic_cast<const OGRPolygon&>(ogeometry),view_data,polygon_z);
				break;
			case wkbLineString:
				insertLineString(dynamic_cast<const OGRLineString&>(ogeometry),view_data,line_z);
				break;
			case wkbPoint:
				insertPoint(dynamic_cast<const OGRPoint&>(ogeometry),view_data,point_z);
				break;
			case wkbMultiPolygon:
				insertMultiPolygon(dynamic_cast<const OGRMultiPolygon&>(ogeometry),view_data,polygon_z);
				break;
			case wkbMultiPoint:
				insertMultiPoint(dynamic_cast<const OGRMultiPoint&>(ogeometry),view_data,point_z);
				break;
			case wkbMultiLineString:
				insertMultiLineString(dynamic_cast<const OGRMultiLineString&>(ogeometry),view_data,line_z);
				break;
			default:
				ERR("Error:%d",int(ogeometry.getGeometryType()));
				break;
		}
	} catch(...) {
	}
}
bool wmap::readMapViewData(const std::string& map_path,WMapViewData* view_data,shared_ptr<OGRSpatialReference>* proj_spatial,shared_ptr<OGRSpatialReference>* geo_spatial,float point_z,float line_z,float polygon_z)
{
	int            i;
	int            count;
	OGRDataSource *pods                 = NULL;
	OGREnvelope    e;
	OGRLayer      *polayer              = NULL;

	clearViewData(view_data);

	if((pods=OGRSFDriverRegistrar::Open(map_path.c_str(),false)) == NULL) {
		QALL_LOG(LL_ERROR,QString("打开地图文件%1失败").arg(map_path.c_str()));
		INFO_LOG("Fails msg:%s",CPLGetLastErrorMsg());
		return false;
	} 

	count = pods->GetLayerCount();
	INFO_LOG("打开地图%s成功",QFileInfo(map_path.c_str()).fileName().toUtf8().data());
	for(i=0; i<count; ++i) {
		polayer = pods->GetLayer(i);
		addLayout(*polayer,view_data,proj_spatial,geo_spatial,point_z,line_z,polygon_z);
	}

	OGRDataSource::DestroyDataSource(pods);
	return true;
}
bool wmap::partitionPolygon(WMPolygon* polygon)
{
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	typedef CGAL::Partition_traits_2<K>                         Traits;
	typedef Traits::Point_2                                     Point_2;
	typedef Traits::Polygon_2                                   Polygon_2;
	typedef Polygon_2::Vertex_iterator                          Vertex_iterator;
	typedef std::list<Polygon_2>                                Polygon_list;

	if(nullptr == polygon || polygon->exterior.empty() || (!polygon->interior.empty())) return false;

	const double z                    = polygon->exterior.front().z;
	Polygon_2    cgal_polygon;
	Polygon_list cgal_partition_polys;

	polygon->convex_polygons.clear();
	polygon->triangles.clear();
	/*
	 * CGAL要求为逆时针方向
	 * GDAL为顺时针方向，并且GDAL第一个点与最后一个点相同，CGAL要求不能相同
	 */
	auto end_it = prev(polygon->exterior.rend());
	for(auto it=polygon->exterior.rbegin(); it!=end_it; ++it) {
		auto& point = *it;
		cgal_polygon.push_back(Point_2(point.x,point.y));
	}

	try {
		CGAL::greene_approx_convex_partition_2(cgal_polygon.vertices_begin(),
				cgal_polygon.vertices_end(),
				std::back_inserter(cgal_partition_polys));
	} catch(...) {
		qDebug()<<"Try mesh polygon";
		if(meshPolygon(polygon)) return true;

		polygon->convex_polygons.clear();
		polygon->triangles.clear();

		auto delta = 1E-3;
		auto p0    = polygon->exterior.front();

		for(auto& p:polygon->exterior) {
			if(fabs(p.x-p0.x)>delta || fabs(p.y-p0.y)>delta) return false;
		}

		polygon->exterior.clear();
		polygon->convex_polygons.clear();
		return true;
	}

	for(auto& cp:cgal_partition_polys) {
		WMConvexPolygon tcp;
		tcp.data.reserve(cp.container().size());
		for(auto& p:cp.container()) {
			tcp.data.emplace_back(WMPointObj({p.x(),p.y(),z}));
		}
		polygon->convex_polygons.push_back(std::move(tcp));
	}

	return true;
}
bool wmap::meshPolygon(WMPolygon* polygon)
{
	if(nullptr == polygon || polygon->exterior.size()<3 || (!polygon->interior.empty())) return false;

	CDT          cdt;
	Polygon_2    cgal_polygon;
	const double z            = polygon->exterior.front().z;

	polygon->convex_polygons.clear();
	polygon->triangles.clear();
	/*
	 * CGAL要求为逆时针方向
	 * GDAL为顺时针方向，并且GDAL第一个点与最后一个点相同，CGAL要求不能相同
	 */
	if(!insert_constraint(cdt,polygon->exterior.rbegin(),prev(polygon->exterior.rend()))) return false;
	mark_domains(cdt);
	auto triangles_nr = 0;
	for(auto it=cdt.finite_faces_begin(); it!=cdt.finite_faces_end(); ++it) {
		if(!it->info().in_domain())continue;
		++triangles_nr;
	}
	polygon->triangles.reserve(triangles_nr*3);
	for(auto it=cdt.finite_faces_begin(); it!=cdt.finite_faces_end(); ++it) {
		if(!it->info().in_domain())continue;
		for(int i=0; i<3; ++i) {
			auto v = it->vertex(i);
			polygon->triangles.emplace_back(WMPointObj({v->point().x(),v->point().y(),z}));
		}
	}
	return true;
}
bool wmap::saveXmlMap(const QString& file_path,const WMap& map)
{
	ptree tree;
	boost::property_tree::xml_parser::xml_writer_settings<string> settings(' ',2);

	for(auto& l:map.layouts) {
		auto  &attr  = l.attr;
		ptree  ltree;

		ltree.put(XML_ATTR_TAG "." LAYOUT_VISIBLE_TAG,l.visible);
		ltree.put(LAYOUT_FILE_NAME_TAG,l.file_name);
		ltree.put(FILL_COLOR_TAG,glColorToString4f(attr.fill_color));
		ltree.put(LINE_COLOR_TAG,glColorToString4f(attr.line_color));
		ltree.put(LINE_WIDTH_TAG,attr.line_width);
		ltree.put(TEXT_COLOR_TAG,glColorToString4f(attr.text_color));
		ltree.put(TEXT_SIZE_TAG,attr.text_size);
		ltree.put(TEXT_OFFSET,attr.text_offset);
		ltree.put(POINT_SIZE_TAG,attr.point_size);
		ltree.put(POINT_TYPE,attr.point_type);
		ltree.put(MAP_LEVEL_TAG,attr.level);
		ltree.put(SHOW_LABEL_TAG,attr.show_label);
		ltree.put(POLYGON_TYPE,attr.polygon_type);
		tree.add_child(MAP_ROOT_TAG "." LAYOUTS_TAG "." LAYOUT_TAG,ltree);
	}
	try {
		write_xml(file_path.toUtf8().data(),tree,std::locale(),settings);
		AINFO_LOG("写地图配置成功.");
		return true;
	} catch(...) {
		AERROR_LOG("写地图配置失败.");
		return false;
	}
	return false;
}
bool wmap::readXmlMap(const QString& map_path,WMap* map)
{
	ptree tree;

	try {
		read_xml(map_path.toUtf8().data(),tree);
		map->file_path = map_path.toUtf8().data();
		map->layouts.clear();
		map->proj_spatial.reset();
		map->geo_spatial.reset();

		auto        lt   = tree.get_child(MAP_ROOT_TAG "." LAYOUTS_TAG);
		WMapLayout  ml;
		auto       &attr = ml.attr;

		for(auto& l:lt) {
			if(l.first != LAYOUT_TAG) {
				ERROR_LOG("Unexpected tag:%s.",l.first.c_str());
			}
			string  str;
			auto   &node = l.second;

			ml.visible = node.get<bool>(XML_ATTR_TAG "." LAYOUT_VISIBLE_TAG);
			ml.file_name = node.get<string>(LAYOUT_FILE_NAME_TAG);
			attr.show_label = node.get<bool>(SHOW_LABEL_TAG);
			str = node.get<string>(FILL_COLOR_TAG);
			toGLColor4f(str,attr.fill_color);
			str = node.get<string>(LINE_COLOR_TAG);
			toGLColor4f(str,attr.line_color);
			str = node.get<string>(TEXT_COLOR_TAG);
			toGLColor4f(str,attr.text_color);
			attr.line_width    =  node.get<float>(LINE_WIDTH_TAG);
			attr.text_size     =  node.get<int>(TEXT_SIZE_TAG);
			attr.text_offset   =  node.get<int>(TEXT_OFFSET,0);
			attr.level         =  node.get<int>(MAP_LEVEL_TAG);
			attr.point_size    =  node.get<float>(POINT_SIZE_TAG,2.0f);
			attr.point_type    =  node.get<unsigned>(POINT_TYPE,MPT_NORMAL);
			attr.polygon_type  =  node.get<int>(POLYGON_TYPE,MPOT_FILL_AND_EDGE);
			map->layouts.push_back(ml);
		}
	} catch(...) {
		AERROR_LOG("读取地图图配置文件失败.");
		return false;
	}
	AINFO_LOG("读地图配置成功，共获取%u个图层.",map->layouts.size());
	return true;
}
bool wmap::loadMapFile(WMap* map)
{

	if(nullptr == map) return false;

	map->proj_spatial.reset();
	map->geo_spatial.reset();

	INFO_LOG("开始加载地图数据.");

	for(auto& ml:map->layouts) {
		loadLayoutData(*map,&ml);
		uniqueTextLabels(*map,&ml);
	}
	mutex mtx;

	parallel_for_each(map->layouts.begin(),map->layouts.end(),[map,&mtx](WMapLayout& ml) { 

		{
		lock_guard<mutex> guard{mtx};
		if(((nullptr == map->proj_spatial) || (nullptr==map->geo_spatial)) 
			&& (ml.proj_spatial!=nullptr) 
			&& (ml.geo_spatial!=nullptr)) {
		map->proj_spatial  =  ml.proj_spatial;
		map->geo_spatial   =  ml.geo_spatial;
		}
		}

		for(auto& polygon:ml.view_data.polygons) {
			if(!wmap::partitionPolygon(&polygon)) {
				WARNING_LOG("多边形分割失败");
			}
		}
	});
	INFO_LOG("加载地图数据完成.");

	return true;
}
bool wmap::loadLayoutData(const WMap& map,WMapLayout* ml)
{
	auto& view_data  =  ml->view_data;

	QDir dir(QFileInfo(map.file_path.c_str()).absolutePath());

	if(!wmap::readMapViewData(dir.absoluteFilePath(ml->file_name.c_str()).toUtf8().data(),&view_data,
				&ml->proj_spatial,&ml->geo_spatial,
				kPointOffset,kLineOffset,kPolygonOffset)) {
		wmap::clearViewData(&view_data);
		return false;
	}

	return true;
}
void wmap::uniqueTextLabels(const WMap& map,WMapLayout* ml)
{
	auto &layouts  = map.layouts;
	auto &lhlabels = ml->view_data.labels;

	for(auto& l:layouts) {
		if(ml->file_name == l.file_name)continue;
		if(ml->attr.level < l.attr.level)continue;
		auto& rhlabels = l.view_data.labels;
		for(auto it = lhlabels.begin(); it!=lhlabels.end(); ++it) {
			auto ij = find_if(rhlabels.begin(),rhlabels.end(),[&it](const WMLabel& label){
					return label.text == it->text;
					});
			if(ij != rhlabels.end()) {
				if(ml->attr.level > l.attr.level || it->area<ij->area || (fabs(it->area-it->area)<1E-4&&ml->attr.text_size<l.attr.text_size)) {
					it->text = QString();
				}
			}
		}
	}
	auto p = remove_if(lhlabels.begin(),lhlabels.end(),[](const WMLabel& label){ return label.text.isEmpty();});
	lhlabels.erase(p,lhlabels.end());
}
bool wmap::readWMap(const QString& file_path,WMap* map)
{
	fstream stream;

	stream.open(file_path.toUtf8().data(),ios_base::in|ios_base::binary);
	if(!stream.is_open()) {
		ERROR_LOG("打开地图文件%s失败",file_path.toUtf8().data());
		return false;
	}

	if(readWMap(stream,map)) {
		map->file_path = file_path.toUtf8().data();
		return true;
	}
	return false;
}
bool wmap::saveWMap(const QString& file_path,const WMap& map)
{
	fstream stream;

	stream.open(file_path.toUtf8().data(),ios_base::out|ios_base::binary|ios_base::trunc);

	if(!stream.is_open()) {
		ERROR_LOG("打开地图文件%s失败",file_path.toUtf8().data());
		return false;
	}

	auto res = saveWMap(stream,map);
	stream.flush();
	stream.close();

	if(res)
		INFO_LOG("保存地图%s成功",file_path.toUtf8().data());
	else
		ERROR_LOG("保存地图%s失败",file_path.toUtf8().data());
	return res;
}
bool wmap::readWMap(std::istream& stream,WMap* map)
{
	uint32_t id      = 0;
	uint32_t version = 0;

	stream.read((char*)&id,sizeof(id));
	stream.read((char*)&version,sizeof(version));

	if(MBI_FILE != id) {
		ERROR_LOG("Error file id:%x",id);
		return false;
	}
	if(1 != version) {
		ERROR_LOG("Error file version:%d",version);
		return false;
	}
	if(!readWMap_data(stream,map->proj_spatial)) return false;
	if(!readWMap_data(stream,map->geo_spatial)) return false;
	if(!readWMap_data(stream,&map->layouts)) return false;

	return true;
}
bool wmap::saveWMap(std::ostream& stream,const WMap& map)
{
	const uint32_t id      = MBI_FILE;
	const uint32_t version = 1;

	stream.write((const char*)&id,sizeof(id));
	stream.write((const char*)&version,sizeof(version));
	if(!saveWMap_data(stream,map.proj_spatial)) return false;
	if(!saveWMap_data(stream,map.geo_spatial)) return false;
	if(!saveWMap_data(stream,map.layouts)) return false;

	return true;
}
bool wmap::readWMap_data(std::istream& stream,std::string* str)
{
	uint32_t size = 0;
	if(!readAndCheckId(stream,MBI_STRING)) return false;
	stream.read((char*)&size,sizeof(size));
	if(0 == size) return stream.good();
	auto d = unique_ptr<char[]>(new char[size]);

	stream.read((char*)d.get(),size);
	str->assign(d.get(),d.get()+size);

	return stream.good();
}
bool wmap::saveWMap_data(std::ostream& stream,const std::string& str)
{
	const uint32_t id   = MBI_STRING;
	const uint32_t size = str.size();

	stream.write((const char*)&id,sizeof(id));
	stream.write((const char*)&size,sizeof(size));
	if(0 != size)
		stream.write((const char*)str.data(),str.size());
	return stream.good();
}
bool wmap::readWMap_data(std::istream& stream,shared_ptr<OGRSpatialReference>& proj)
{
	string str;
	proj.reset();

	if(!readAndCheckId(stream,MBI_PROJ)) return false;
	if(!readWMap_data(stream,&str)) return false;
	if(str.empty()) return stream.good();
	proj = make_shared<OGRSpatialReference>();

	auto ptr = new char[str.size()+1];
	auto d = ptr;
	strcpy(d,str.c_str());
	char** p = &d;
	proj->importFromWkt(p);
	delete[] ptr;

	return stream.good();
}
bool wmap::saveWMap_data(std::ostream& stream,const shared_ptr<OGRSpatialReference>& proj)
{
	const uint32_t  id   = MBI_PROJ;
	char           *data = nullptr;
	string          str;

	stream.write((const char*)&id,sizeof(id));
	if(nullptr == proj) {
		return saveWMap_data(stream,str);
	}
	proj->exportToWkt(&data);
	str = string(data,strlen(data));
	OGRFree(data);
	return saveWMap_data(stream,str);
}
bool wmap::readWMap_data(std::istream& stream,WMapLayout* ml)
{
	if(!readAndCheckId(stream,MBI_LAYOUT)) return false;
	if(!readWMap_data(stream,&ml->file_name)) return false;
	if(!readWMap_data(stream,&ml->attr)) return false;
	if(!readWMap_data(stream,&ml->view_data)) return false;
	if(!readWMap_data(stream,ml->proj_spatial)) return false;
	if(!readWMap_data(stream,ml->geo_spatial)) return false;
	stream.read((char*)&ml->visible,sizeof(ml->visible));

	return stream.good();
}
bool wmap::saveWMap_data(std::ostream& stream,const WMapLayout& ml)
{
	const uint32_t  id   = MBI_LAYOUT;

	stream.write((const char*)&id,sizeof(id));
	if(!saveWMap_data(stream,ml.file_name)) return false;
	if(!saveWMap_data(stream,ml.attr)) return false;
	if(!saveWMap_data(stream,ml.view_data)) return false;
	if(!saveWMap_data(stream,ml.proj_spatial)) return false;
	if(!saveWMap_data(stream,ml.geo_spatial)) return false;
	stream.write((const char*)&ml.visible,sizeof(ml.visible));

	return stream.good();
}
bool wmap::readWMap_data(std::istream& stream,WMapLayoutAttr* attr)
{
	if(!readAndCheckId(stream,MBI_ATTR)) return false;
	stream.read((char*)attr,sizeof(*attr));
	return stream.good();
}
bool wmap::saveWMap_data(std::ostream& stream,const WMapLayoutAttr& attr)
{
	static_assert(std::is_standard_layout<WMapLayoutAttr>::value,"error type");

	const uint32_t  id   = MBI_ATTR;
	stream.write((const char*)&id,sizeof(id));
	stream.write((const char*)&attr,sizeof(attr));

	return stream.good();
}
bool wmap::readWMap_data(std::istream& stream,WMapViewData* viewdata)
{
	if(!readAndCheckId(stream,MBI_VIEWDATA)) return false;
	if(!readWMap_data(stream,&viewdata->labels)) return false;
	if(!readWMap_data(stream,&viewdata->points.data)) return false;
	if(!readWMap_data(stream,&viewdata->polygons)) return false;
	if(!readWMap_data(stream,&viewdata->linestrings)) return false;
	if(!readWMap_data(stream,&viewdata->extent)) return false;

	viewdata->graphics_lists.clear();
	viewdata->current_polygon = 0;
	viewdata->offset= 0;

	return stream.good();
}
bool wmap::saveWMap_data(std::ostream& stream,const WMapViewData& viewdata)
{
	const uint32_t  id   = MBI_VIEWDATA;

	stream.write((const char*)&id,sizeof(id));
	if(!saveWMap_data(stream,viewdata.labels)) return false;
	if(!saveWMap_data(stream,viewdata.points.data)) return false;
	if(!saveWMap_data(stream,viewdata.polygons)) return false;
	if(!saveWMap_data(stream,viewdata.linestrings)) return false;
	if(!saveWMap_data(stream,viewdata.extent)) return false;

	return stream.good();
}
bool wmap::readWMap_data(std::istream& stream,WMPolygon* poly)
{
	if(!readAndCheckId(stream,MBI_POLYGON)) return false;
	if(!readWMap_data(stream,&poly->exterior)) return false;
	if(!readWMap_data(stream,&poly->interior)) return false;
	if(!readWMap_data(stream,&poly->convex_polygons)) return false;
	if(!readWMap_data(stream,&poly->triangles)) return false;

	return stream.good();
}
bool wmap::saveWMap_data(std::ostream& stream,const WMPolygon& poly)
{
	const uint32_t  id   = MBI_POLYGON;

	stream.write((const char*)&id,sizeof(id));
	if(!saveWMap_data(stream,poly.exterior)) return false;
	if(!saveWMap_data(stream,poly.interior)) return false;
	if(!saveWMap_data(stream,poly.convex_polygons)) return false;
	if(!saveWMap_data(stream,poly.triangles)) return false;

	return stream.good();
}
bool wmap::readWMap_data(std::istream& stream,WMLineString* ls)
{
	if(!readAndCheckId(stream,MBI_LINESTRING)) return false;
	if(!readWMap_data(stream,&ls->data)) return false;

	return stream.good();
}
bool wmap::saveWMap_data(std::ostream& stream,const WMLineString& ls)
{
	const uint32_t  id   = MBI_LINESTRING;

	stream.write((const char*)&id,sizeof(id));
	if(!saveWMap_data(stream,ls.data)) return false;

	return stream.good();
}
bool wmap::readWMap_data(std::istream& stream,OGREnvelope* env)
{
	if(!readAndCheckId(stream,MBI_ENVELOPE)) return false;

	stream.read((char*)&env->MinX,sizeof(env->MinX));
	stream.read((char*)&env->MaxX,sizeof(env->MaxX));
	stream.read((char*)&env->MinY,sizeof(env->MinY));
	stream.read((char*)&env->MaxY,sizeof(env->MaxY));

	return stream.good();
}
bool wmap::saveWMap_data(std::ostream& stream,const OGREnvelope& env)
{
	const uint32_t  id   = MBI_ENVELOPE;

	stream.write((const char*)&id,sizeof(id));
	stream.write((const char*)&env.MinX,sizeof(env.MinX));
	stream.write((const char*)&env.MaxX,sizeof(env.MaxX));
	stream.write((const char*)&env.MinY,sizeof(env.MinY));
	stream.write((const char*)&env.MaxY,sizeof(env.MaxY));

	return stream.good();
}
bool wmap::readWMap_data(std::istream& stream,WMPointObj* p)
{
	stream.read((char*)p,sizeof(*p));
	return stream.good();
}
bool wmap::saveWMap_data(std::ostream& stream,const WMPointObj& p)
{
	static_assert(std::is_standard_layout<WMPointObj>::value,"error type");

	stream.write((const char*)&p,sizeof(p));
	return stream.good();
}
bool wmap::readWMap_data(std::istream& stream,WMConvexPolygon* env)
{
	if(!readAndCheckId(stream,MBI_CONVEX_POLYGON)) return false;
	return readWMap_data(stream,&env->data);
}
bool wmap::saveWMap_data(std::ostream& stream,const WMConvexPolygon& env)
{
	const uint32_t  id   = MBI_CONVEX_POLYGON;

	stream.write((const char*)&id,sizeof(id));
	return saveWMap_data(stream,env.data);
}
bool wmap::readWMap_data(std::istream& stream,WMLabel* label)
{
	double         v  = 0;
	string str;
	if(!readAndCheckId(stream,MBI_LABEL)) return false;
	stream.read((char*)&v,sizeof(v));
	label->pos.setX(v);
	stream.read((char*)&v,sizeof(v));
	label->pos.setY(v);
	stream.read((char*)&label->area,sizeof(label->area));
	if(!readWMap_data(stream,&str)) return false;
	label->text = QString(str.c_str());
	return stream.good();
}
bool wmap::saveWMap_data(std::ostream& stream,const WMLabel& label)
{
	const uint32_t id = MBI_LABEL;
	double         v  = 0;
	string str(label.text.toUtf8().data());

	stream.write((const char*)&id,sizeof(id));
	v = label.pos.x();
	stream.write((const char*)&v,sizeof(v));
	v = label.pos.y();
	stream.write((const char*)&v,sizeof(v));
	stream.write((const char*)&label.area,sizeof(label.area));

	return saveWMap_data(stream,str);
}
bool wmap::readAndCheckId(std::istream& stream,const uint32_t id)
{
	uint32_t v = 0;
	stream.read((char*)&v,sizeof(v));
	if(v != id) {
		ERROR_LOG("Error id:%x, expected: %x.",v,id);
		return false;
	}
	return stream.good();
}
bool wmap::readMap(const QString& file_path,WMap* map)
{
	auto fp = file_path.toLower();
	if(fp.endsWith(".xml")) {
		return readXmlMap(file_path,map);
	}
	return readWMap(file_path,map);
}
bool wmap::saveMap(const QString& file_path,const WMap& map)
{
	auto fp = file_path.toLower();
	if(fp.endsWith(".xml")) {
		return saveXmlMap(file_path,map);
	}
	return saveWMap(file_path,map);
}
bool wmap::updateMapLayouts(const std::vector<WMapLayout>& layouts,WMap* map)
{
	auto& dest_layouts = map->layouts;

	if(layouts.size()> dest_layouts.size()) {
		ERROR_LOG("不正确的图层数量.");
		return false;
	}

	for(int i=0; i<layouts.size(); ++i) {
		auto& l = layouts[i];
		auto it = find_if(next(dest_layouts.begin(),i),dest_layouts.end(),[&l](const WMapLayout& layout){ return l.file_name==layout.file_name;});
		if(it == dest_layouts.end()) {
			ERROR_LOG("不正确的图层文件名:%s",l.file_name.c_str());
			return false;
		}
		auto di = distance(dest_layouts.begin(),it);
		if(di != i) {
			auto layout = std::move(dest_layouts[di]);
			dest_layouts.erase(it);
			layout.attr     =  l.attr;
			layout.visible  =  l.visible;
			dest_layouts.insert(next(dest_layouts.begin(),i),std::move(layout));
		} else {
			auto& layout = dest_layouts[i];
			layout.attr     =  l.attr;
			layout.visible  =  l.visible;
		}
	}
	dest_layouts.erase(next(dest_layouts.begin(),layouts.size()),dest_layouts.end());
	INFO_LOG("更新地图图层成功.");
	return true;
}
#endif
