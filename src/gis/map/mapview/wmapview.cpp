/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#ifndef DISABLE_MAPVIEW
#include <wmapview.h>
#include <wmapdata.h>
#include <xmldata.h>
#include <xmlstream.h>
#include <QtAlgorithms>
#include <QGLWidget>
#include <wgui/wgui.h>
#include <limits>
#include <wmath.h>
#include <gltoolkit.h>
#include <algorithm>
#include <gis.h>
#include <thread>
#include <wgeometry.h>
#include <boost/geometry.hpp>
#include <boost/scope_exit.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/accumulators.hpp>

using namespace std;
using namespace WSpace;
using namespace boost::accumulators;
namespace bg=boost::geometry;

const int WMapView::kTimeThresholdForBBGL = 200;
const int WMapView::kTimeThresholdForIdle = 3000;

WMapView::WMapView()
:WGLWidget(1000.0f,1000.0f)
{
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");
	OGRRegisterAll();

	setMinimumSize(400,400);
	glDepthFunc(GL_LEQUAL);
}
void WMapView::paintGL()
{
	glDrawBuffer(GL_FRONT_AND_BACK);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.765F, 0.898f, 0.96F,1.0F);

	blcd_.init();
	if(first_show_) {
		first_show_ = false;
		buildList();
	}
	//const auto k = std::max(graphics_height_/(map_extent_.MaxY-map_extent_.MinY),graphics_width_/(map_extent_.MaxX-map_extent_.MinX));
	//g_object_trans_matrix = GMatrix::scaleMatrix(k,k,1.0f)*GMatrix::translateMatrix(-(map_extent_.MaxX+map_extent_.MinX)/2.0f,-(map_extent_.MaxY+map_extent_.MinY)/2.0f,0.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();//#0+

	glTranslatef(0,0,-z_offset());
	glMultMatrixf(g_trans_matrix.data());
	glMultMatrixf(g_rotate_matrix.data());
	glMultMatrixf(g_object_trans_matrix.data());

	paintBeforeMap();
	//显示地图
	auto rect = getViewRect();

	showMapListInLevel(0,0,rect);
	if(scale_count_ >= 10) {
		showMapListInLevel(1,std::numeric_limits<int>::max(),rect);
	} else {
		showMapListInLevel(1,scale_count_,rect);
	}

	blcd_.reset();
	if(blcd_.available) {
		if(blcd_.fin) {
			if(blcd_.timer_id > 0) {
				killTimer(blcd_.timer_id); blcd_.timer_id = -1;
			}
		} else {
			if(blcd_.timer_id <= 0) {
				blcd_.timer_id = startTimer(kTimeThresholdForBBGL+10);
			}
		}
	}

	showMapTextInLevel(0,0,rect);
	if(scale_count_ >= 10) {
		showMapTextInLevel(1,std::numeric_limits<int>::max(),rect);
	} else {
		showMapTextInLevel(1,scale_count_,rect);
	}
	paintAfterMap();
	glPopMatrix();//#0-

}
QRectF WMapView::getViewRect()const
{
	QRectF  res;
	GMatrix tm           = (g_trans_matrix*g_rotate_matrix*g_object_trans_matrix).inverse();
	GVector top_left     = {-gl_width_/2,gl_height_/2,0.0f};
	GVector bottom_right = {gl_width_/2,-gl_height_/2,0.0f};

	top_left       =   tm*top_left;
	bottom_right   =   tm*bottom_right;
	res.setLeft(top_left[0]);
	res.setRight(bottom_right[0]);
	res.setTop(top_left[1]);
	res.setBottom(bottom_right[1]);
	return res;
}
void WMapView::showMapListInLevel(int level_begin, int level_end,const QRectF& rect)
{
	auto &layouts            = map_cache_.layouts;

	for(auto& l:layouts) {
		auto& attr       =  l.attr;
		if(!l.visible || attr.level<level_begin||attr.level>level_end)continue;
		showMapListOfLayout(l,rect);
	}
}
bool WMapView::isTotalBuild(const WMapLayout& l)const
{
	if(l.view_data.graphics_lists.empty()
			|| l.view_data.current_polygon<l.view_data.polygons.size()) return false;
	return true;
}
void WMapView::showMapListOfLayout(WMapLayout& l,const QRectF& rect)
{
	auto& view_data  =  l.view_data;

	if(!isTotalBuild(l)
			&& !buildMapListOfLayout(l))
		return;
	if(!l.view_data.graphics_lists.empty()){
		glPushMatrix();//&0+
		glTranslatef(0,0,view_data.offset);
		glCallLists(l.view_data.graphics_lists.size(),GL_UNSIGNED_INT,l.view_data.graphics_lists.data());
		glPopMatrix();//&0-
	}

	showPoints(l,rect,view_data.points);
}
void WMapView::showPoints(WMapLayout& l,const QRectF& rect,const WMPoints& points)
{
	auto       &view_data = l.view_data;
	const auto  map_scale = graphics_width_/(map_extent_.MaxX-map_extent_.MinX);
	const auto  scale     = map_scale *scale_for_coordinate_[0];

	if(scale == 0.0f) return;

	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.0f);

	for(auto& point:points.data) {
		QPoint point_pos(point.x,point.y);
		if(!bg::intersects(point_pos,rect))continue;
		auto k = l.attr.point_size/scale;
		glPushMatrix(); //&1+
		glTranslatef(point.x,point.y,view_data.offset+wmap::kPointOffset);
		glScalef(k,k,1.0f);
		switch(l.attr.point_type) {
			case MPT_TWO_CIRCLE:
				glColor3fv(l.attr.line_color);
				glCallList(line_circle_list_);
				glPushMatrix();//&2+
				glScalef(0.5f,0.5f,1.0f);
				glCallList(line_circle_list_);
				glPopMatrix();//&2-
				break;
			case MPT_TWO_FILL_CIRCLE:
				glColor3fv(l.attr.fill_color);
				glCallList(fill_circle_list_);
				glColor3fv(l.attr.line_color);
				glCallList(line_circle_list_);
				glPushMatrix(); //&3+
				glTranslatef(0,0,0.0001f);
				glScalef(0.5f,0.5f,1.0f);
				glCallList(line_circle_list_);
				glPopMatrix();//&3-
				break;
			default:
			case MPT_NORMAL:
				glColor3fv(l.attr.fill_color);
				glCallList(fill_circle_list_);
				break;
		}
		glPopMatrix(); //&1-
	}

	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_BLEND);
}
void WMapView::showMapTextInLevel(int level_begin, int level_end,const QRectF& rect)
{
	auto& layouts = map_cache_.layouts;

	for(auto& l:layouts) {
		auto& attr       =  l.attr;
		if(!l.visible || attr.level<level_begin||attr.level>level_end)continue;
		showMapTextOfLayout(l,rect);
	}
}
void WMapView::showMapTextOfLayout(WMapLayout& l,const QRectF& rect)
{
	auto       &view_data = l.view_data;
	auto       &attr      = l.attr;
	const auto  map_scale = graphics_width_/(map_extent_.MaxX-map_extent_.MinX);
	const auto  scale     = map_scale *scale_for_coordinate_[0];

	if(scale == 0.0f) return;

	if(!attr.show_label || scale_for_coordinate_[0]<1.0f) return;

	auto  &label_list = view_data.labels;
	QFont  font;
	font.setPointSize(attr.text_size);
	QFontMetrics font_metrics(font);
	//const auto zay_offset = font_metrics.height()/(win_div_gl()*scale);
	glColor3fv(attr.text_color);
	for(auto it=label_list.begin(); it!=label_list.end(); ++it) {
		if(!bg::intersects(it->pos,rect))continue;
		const auto x = it->pos.x();
		//const auto y = fabs(it->area)<1E-2?it->pos.y()+zay_offset:it->pos.y();
		const auto y = it->pos.y();
		const auto z = -l.attr.text_offset*wmap::kLayoutOffset+l.view_data.offset+wmap::kTextOffset;
		renderText(x,y,z,it->text,font,Qt::AlignHCenter|Qt::AlignBottom);
	}
}
void WMapView::update()
{
	WGLWidget::updateGL();
}
void WMapView::buildList()
{
	buildCircleList();
	blcd_.init();
	buildMapList();
}
void WMapView::buildMapList()
{
	bool        init_matrix  = true;
	int         min_level    = std::numeric_limits<int>::max();

	//if(first_show_ || nullptr == map_) return;
	if(nullptr == map_) return;

	bg::assign_inverse(map_extent_);	

	makeCurrent();
	auto& layouts = map_cache_.layouts;

	for(int i=0; i<layouts.size(); ++i) {
		auto &ml        = layouts[i];
		if(ml.visible && ml.attr.level<min_level)
			min_level = ml.attr.level;
	}
	++min_level;

	for(int i=0; i<layouts.size(); ++i) {
		auto &ml        = layouts[i];
		auto &view_data = ml.view_data;

		view_data.offset = i*wmap::kLayoutOffset;

		if(ml.attr.level > min_level) continue;

		if(isTotalBuild(ml)) {
			init_matrix = false;
			goto label0;
		}

		if(false == buildMapListOfLayout(ml))continue;
label0:
		bg::expand(map_extent_,ml.view_data.extent);
	}

	if(init_matrix) {
		initGraphMatrix();
		scale_count_ = 0;
	} 
	if(bg::area(map_extent_)>0) {
		const auto k = std::min(gl_height_/(map_extent_.MaxY-map_extent_.MinY),gl_width_/(map_extent_.MaxX-map_extent_.MinX));
		g_object_trans_matrix = GMatrix::scaleMatrix(k,k,1.0f)*GMatrix::translateMatrix(-(map_extent_.MaxX+map_extent_.MinX)/2.0f,-(map_extent_.MaxY+map_extent_.MinY)/2.0f,0.0f);
	} else {
		LOG(LL_ERROR,"Get map_extent_ error");
	}
	if(nullptr == geo_spatial_) {
		AWARNING_LOG("没有找到地理坐标系配置，使用默认地理坐标系.");
	}
	if(nullptr == proj_spatial_) {
		AWARNING_LOG("没有找到投影坐标系配置，直接使用地理坐标系.");
	}
	mapChanged();
	INFO_LOG("构建地图显示列表成功.");
}
bool WMapView::buildMapListOfLayout(WMapLayout& ml)
{
	auto &view_data = ml.view_data;

	makeCurrent();

	if(!ml.visible ||
			!blcd_.createList()) 
		return true;

	if(blcd_.isTimeOut()) return true;

	if(view_data.current_polygon<0) 
		view_data.current_polygon=0;

	if(view_data.graphics_lists.empty()) {
		view_data.current_polygon = 0;
		auto list  = glGenLists(1);
		glNewList(list,GL_COMPILE);
		showBaseMapLayout(&ml);
		glEndList();
		view_data.graphics_lists.push_back(list);
		return true;
	}
	if(blcd_.isTimeOut()) return true;

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0,1.0);

	if(view_data.current_polygon<view_data.polygons.size()) {
		auto polygon_list = glGenLists(1);
		glNewList(polygon_list,GL_COMPILE);
		//INFO_LOG("Build %s, %f.",ml.file_name.c_str(),view_data.offset);
		showLayoutPolygon(&ml);
		view_data.graphics_lists.push_back(polygon_list);
		if(view_data.current_polygon >= view_data.polygons.size())
			wmap::releaseMemAfterVList(&ml.view_data);
		glEndList();
	}

	return true;
}
WMapView::~WMapView()
{
	qDebug()<<__func__;
}
bool WMapView::showBaseMapLayout(WMapLayout* ml)
{
	auto    &view_data  = ml->view_data;

	glLineWidth(ml->attr.line_width);
	glColor3fv(ml->attr.line_color);
	for(auto& line_string:view_data.linestrings) {
		showLineString(line_string);
	}

	auto it = view_data.polygons.begin();
	glColor3fv(ml->attr.line_color);
	glLineWidth(ml->attr.line_width);
	switch(ml->attr.polygon_type) {
		case MPOT_EDGE:
		case MPOT_FILL_AND_EDGE:
			while(it != view_data.polygons.end()){
				auto& polygon = *it++;
				showPolygonEdge(polygon);
			}
			break;
		default:
			break;
	}
	return true;
}
bool WMapView::showLayoutPolygon(WMapLayout* ml)
{
	auto          &view_data  = ml->view_data;

	if(view_data.current_polygon<0) 
		view_data.current_polygon=0;
	if(view_data.current_polygon>=view_data.polygons.size()) 
		return true;

	GLUtesselator *tesselator = gluNewTess();

	gluTessCallback(tesselator,GLU_TESS_BEGIN,(WTESS_CALLBACK_FUNC)glBegin);
	gluTessCallback(tesselator,GLU_TESS_END,(WTESS_CALLBACK_FUNC)glEnd);
	gluTessCallback(tesselator,GLU_TESS_VERTEX,(WTESS_CALLBACK_FUNC)glVertex3dv);
	gluTessCallback(tesselator,GLU_TESS_ERROR,(WTESS_CALLBACK_FUNC)defaultTessError);
	gluTessProperty(tesselator,GLU_TESS_WINDING_RULE,GLU_TESS_WINDING_ODD);
	gluTessCallback(tesselator,GLU_TESS_COMBINE,(WTESS_CALLBACK_FUNC)defaultTessCombineCallback);

	gluTessNormal(tesselator,0,0,1);

	auto it = next(view_data.polygons.begin(),view_data.current_polygon);

	glColor3fv(ml->attr.fill_color);
	switch(ml->attr.polygon_type) {
		case MPOT_FILL:
		case MPOT_FILL_AND_EDGE:
			while(it != view_data.polygons.end()
					&& !blcd_.isTimeOut()) {
				auto& polygon = *it++;
				showPolygon(polygon,tesselator);
			}
			break;
		default:
			view_data.current_polygon = view_data.polygons.size();
			break;
	}
	view_data.current_polygon = distance(view_data.polygons.begin(),it);
	gluDeleteTess(tesselator); tesselator = nullptr;
	return true;
}
void WMapView::showPolygon(const WMPolygon& polygon,GLUtesselator* tesselator)
{
	if(polygon.exterior.empty()) return;

	if(polygon.convex_polygons.empty()&&polygon.triangles.empty()) {
		/*
		 * 外部环
		 */
		gluTessBeginPolygon(tesselator,nullptr);

		gluTessBeginContour(tesselator);
		for(auto& point:polygon.exterior) {
			gluTessVertex(tesselator,(double*)&point,(double*)&point);
		}
		gluTessEndContour(tesselator);
		/*
		 * 内部环
		 */
		for(auto& points:polygon.interior) {
			gluTessBeginContour(tesselator);
			for(auto& point:points) {
				gluTessVertex(tesselator,(double*)&point,(double*)&point);
			}
			gluTessEndContour(tesselator);
		}
		gluTessEndPolygon(tesselator);
	} else {
		glEnableClientState(GL_VERTEX_ARRAY);
		if(!polygon.convex_polygons.empty()) {
			for(auto& p:polygon.convex_polygons) {
				glVertexPointer(3,GL_DOUBLE,0,p.data.data());
				glDrawArrays(GL_POLYGON,0,p.data.size());
			}
		} else if(!polygon.triangles.empty()) {
			glVertexPointer(3,GL_DOUBLE,0,polygon.triangles.data());
			glDrawArrays(GL_TRIANGLES,0,polygon.triangles.size());
		}
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}
void WMapView::showPolygonEdge(const WMPolygon& polygon)
{
	/*
	 * 外部环
	 */
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_DOUBLE,0,polygon.exterior.data());
	glDrawArrays(GL_LINE_STRIP,0,polygon.exterior.size());
	/*
	 * 内部环
	 */
	for(auto& points:polygon.interior) {
		glVertexPointer(3,GL_DOUBLE,0,points.data());
		glDrawArrays(GL_LINE_STRIP,0,points.size());
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}
void WMapView::showLineString(const WMLineString& line_string)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_DOUBLE,0,line_string.data.data());
	glDrawArrays(GL_LINE_STRIP,0,line_string.data.size());
	glDisableClientState(GL_VERTEX_ARRAY);
}
void WMapView::wheelEvent(QWheelEvent* e)
{
	const auto kMinMapScale = 0.4;
	const auto kMaxMapScale = 50.0;

	if((scale_for_coordinate_[0] <= kMinMapScale && e->delta()<0)
		||((scale_for_coordinate_[0] >= kMaxMapScale && e->delta() > 0) && (!(e->modifiers()&Qt::ShiftModifier))))
		return;

	constexpr float kBaseScale = 1.1f;
	float k = kBaseScale;
	if(e->delta() < 0) {
		k = 1.0f/k;
	} 
	zoom(e->x(),e->y(),k);
	scale_count_ = log(scale_for_coordinate_[0])/log(kBaseScale)/3;
}
void WMapView::setMap(WMap* map)
{
	map_ = map;
	setCacheMap();
}
void WMapView::reloadMap()
{
	setMap(map_);

	buildMapList();
	update();
}
WSpace::GMatrix WMapView::worldToCliendMatrix()const
{
	return  g_trans_matrix*g_rotate_matrix*g_object_trans_matrix;
}
bool WMapView::worldToClient(const WSpace::GMatrix& w2cm,double* gps_long,double* gps_lat)
{
	GVector v = {float(*gps_long),float(*gps_lat),0.0f};
	GVector res = w2cm*v;

	*gps_long  =  res[0];
	*gps_lat   =  res[1];

	return true;
}
bool WMapView::worldToClient(double* gps_long, double* gps_lat)
{
	return worldToClient(worldToCliendMatrix(),gps_long,gps_lat);
}
bool WMapView::gpsToClient(double* gps_long,double* gps_lat)
{
	if(false == gpsToWorld(gps_long,gps_lat))
		return false;
	return worldToClient(gps_long,gps_lat);
}
bool WMapView::worldToGPS(double* x,double* y)
{
	if(nullptr == proj_spatial_) {
		return true;
	}

	auto transformation = std::unique_ptr<OGRCoordinateTransformation>(OGRCreateCoordinateTransformation(proj_spatial_.get(),geo_spatial_.get()));

	if(nullptr == transformation) 
		return false;

	if(!transformation->Transform(1,x,y)) {
		return false;
	}
	return true;
}
bool WMapView::worldToGPS(GPSPoint* pos)
{
	return worldToGPS(&pos->x,&pos->y);
}
bool WMapView::gpsToWorld(double* gps_long,double* gps_lat)
{
	correctDRef(*gps_long);

	if(nullptr == proj_spatial_) {
		return true;
	}

	auto transformation = std::unique_ptr<OGRCoordinateTransformation>(OGRCreateCoordinateTransformation(geo_spatial_.get(),proj_spatial_.get()));

	if(nullptr == transformation) 
		return false;

	if(!transformation->Transform(1,gps_long,gps_lat)) {
		return false;
	}
	return true;
}
bool WMapView::screenToWord(int x, int y, double* gps_long,double* gps_lat)
{
	float tx;
	float ty;
	auto  res = DC2MC(x,y,g_trans_matrix *g_rotate_matrix*g_object_trans_matrix,&tx,&ty);

	*gps_long  =  tx;
	*gps_lat   =  ty;

	return res;
}
bool WMapView::screenToGPS(const QPoint& p,GPSPoint* pos)
{
	if(false == screenToWord(p,pos)) return false;
	return worldToGPS(pos);
}
void WMapView::mouseDoubleClickEvent(QMouseEvent* event)
{
	if(event->button() == Qt::LeftButton) {
		initTrans();
		update();
	}
}
void WMapView::initTrans()
{
	g_trans_matrix            =  GMatrix::identityMatrix();
	g_rotate_matrix           =  GMatrix::identityMatrix();
	scale_count_              =  0;
	scale_for_coordinate_[0]  =  scale_for_coordinate_[1]    =  scale_for_coordinate_[2]  =  1.0f;
}
void WMapView::showGPSTrace(const vector<GPSPoint>& data)
{
	for(auto gps:data) {
		if(!gpsToWorld(&gps))continue;
		glVertex2f(gps.x,gps.y);
	}
}
void WMapView::showGPSTrace(const vector<GPSPoint>& data,GLuint type)
{
	vector<pair<double,double>> world_datas;

	for(auto gps:data) {
		if(!gpsToWorld(&gps))continue;
		world_datas.emplace_back(gps.x,gps.y);
	}
	showWorldTrace(std::move(world_datas),type);
}
void WMapView::showWorldTrace(vector<pair<double,double>>&& data,GLuint type)
{
	showMultiPolygons(std::move(data),type);
#if 0
	if(data.size()<2) return;

	accumulator_set<float,stats<tag::variance(lazy)>> acc;
	vector<float> distance;
	distance.reserve(data.size());

	auto end=prev(data.end());
	for(auto it = data.begin(); it!=end; ++it) {
		auto jt = next(it);
		distance.push_back((jt->first-it->first)*(jt->first-it->first)+(jt->second-it->second)*(jt->second-it->second));
		acc(distance.back());
	}

	{
		auto it = data.begin();
		auto jt = prev(data.end());

		distance.push_back((jt->first-it->first)*(jt->first-it->first)+(jt->second-it->second)*(jt->second-it->second));
	}
	const auto delta = sqrt(variance(acc))*5;
	const auto ave   = mean(acc);
	auto       it    = find_if(distance.begin(),distance.end(),[delta](GLdouble v){ return v>delta;});

	if(it != distance.end()) {
		rotate(distance.begin(),it,distance.end());
		rotate(data.begin(),next(data.begin(),std::distance(distance.begin(),it)),data.end());
	}

	it    = distance.begin();
	for(auto& p:data) {
		glVertex2f(p.first,p.second);
		if(fabs((*it++)-ave)>delta) {
			glEnd();
			glBegin(type);
		}
	}
#endif
}
void WMapView::showGPSAreaPolygon(const vector<GPSPoint>& data)
{
	int j = 0;
	vector<GLdouble> glpoints(data.size() *3);

	for(j=0; j<data.size(); ++j) {
		auto gps = data[j];
		if(!gpsToWorld(&gps)) {
			--j;
			continue;
		}
		glpoints[j*3+0] = gps.x;
		glpoints[j*3+1] = gps.y; 
		glpoints[j*3+2] = 0;
	}
	glpoints.resize(j*3);

	showWorldAreaPolygon(std::move(glpoints));
}
void WMapView::showWorldAreaPolygon(vector<GLdouble>&& data)
{
	if(data.size()<9) return;

	const auto size = data.size()/3;
	accumulator_set<float,stats<tag::variance(lazy)>> acc;
	vector<float> distance;

	distance.reserve(data.size()/3);

	for(int i=0; i<size; ++i) {
		distance.push_back((data[i*3]-data[i*3+3])*(data[i*3]-data[i*3+3])+(data[i*3+1]-data[i*3+4])*(data[i*3+1]-data[i*3+4]));
		acc(distance.back());
	}

	distance.push_back((data[0]-data[size*3-3])*(data[0]-data[size*3-3])+(data[1]-data[size*3-2])*(data[1]-data[size*3-2]));

	const auto delta = sqrt(variance(acc)) *5;
	const auto ave   = mean(acc);
	auto       it    = find_if(distance.begin(),distance.end(),[delta](GLdouble v){ return v>delta; });

	if(it==distance.end()) {
		drawPolygon(data.data(),data.size()/3);
		return;
	}

	auto n_first     = next(data.begin(),std::distance(distance.begin(),it)*3);
	auto begin_index = 0;

	std::rotate(data.begin(),n_first,data.end());
	std::rotate(distance.begin(),it,distance.end());

	it = distance.begin();
	for(int i=0; i<size; ++i,++it) {
		if(fabs((*it)-ave)>delta) {
			drawPolygon(data.data()+begin_index*3,i-begin_index+1);
			begin_index = i+1;
		}
	}
	if(size-begin_index>=3)
		drawPolygon(data.data()+begin_index*3,size-begin_index);
}
void WMapView::setCacheMap()
{
	if(nullptr == map_) return;
	auto& src = *map_;

	INFO_LOG("加载新地图.");
	for(auto& l:map_cache_.layouts) {
		for(auto list:l.view_data.graphics_lists) {
			glDeleteLists(list,1);
		}
	}

	map_cache_ = src;
	proj_spatial_.reset();
	geo_spatial_.reset();
	if(map_cache_.proj_spatial != nullptr) {
		proj_spatial_ = make_unique<OGRSpatialReference>(*map_cache_.proj_spatial);
	}
	if(map_cache_.geo_spatial != nullptr) {
		geo_spatial_ = make_unique<OGRSpatialReference>(*map_cache_.geo_spatial);
	}
}
bool WMapView::equal_glcolor(const float* lhv,const float* rhv)
{
	const int size = 3;
	return equal(lhv,lhv+size,rhv,[](float v0,float v1) { 
		return fabs(v0-v1)<1E-5;
	});
}
void WMapView::buildCircleList()
{
	if(glIsList(fill_circle_list_)) return;
	fill_circle_list_ = glGenLists(1);
	glNewList(fill_circle_list_,GL_COMPILE);
	drawCircle(0,0,0,1.0f,20,GL_POLYGON);
	glEndList();
	line_circle_list_ = glGenLists(1);
	glNewList(line_circle_list_,GL_COMPILE);
	drawCircle(0,0,0,1.0f,20,GL_LINE_LOOP);
	glEndList();
}
void WMapView::paintBeforeMap()
{
}
void WMapView::paintAfterMap()
{
}
void WMapView::mapChanged()
{
}
void WMapView::timerEvent(QTimerEvent* event) 
{
	update();
	WGLWidget::timerEvent(event);
}
void WMapView::BuildListControlD::init()
{
	available  =  false;
	fin        =  true;
}
bool WMapView::BuildListControlD::createList()
{
	auto delta    = chrono::steady_clock::now()-time;
	if(available) return create_list;
	available  =  true;
	/*
	 * 以下三种情况创建显示列表
	 * 1)时间异常
	 * 2)上一次创建列表到现在已经超过阀值
	 * 3)图形已经重绘过三次(在没有创建显示列表的情况下)
	 */
	if(delta<chrono::steady_clock::duration::zero() || delta>chrono::milliseconds(kTimeThresholdForIdle) || nb_nr>3) {
		fin          =  true;
		create_list  =  true;
		time         =  chrono::steady_clock::now()+chrono::milliseconds(kTimeThresholdForBBGL);
		nb_nr        =  0;
	} else {
		create_list  =  false;
		fin          =  false;
		++nb_nr;
	}
	return create_list;
}
void WMapView::BuildListControlD::reset()
{
	if(available) {
		if(create_list) 
			time  =  chrono::steady_clock::now(); 
	} else {
		fin = true;
	}
}
bool WMapView::BuildListControlD::isTimeOut()
{
	if(chrono::steady_clock::now()>time) {
		/*
		 * 如果超时，表明没有完成显示列表的创建
		 */
		fin = false;
		return true;
	}
	return false;
}
bool WMapView::moveTo(double gps_long,double gps_lat)
{
	if(!gpsToClient(&gps_long,&gps_lat)) return false;
	g_trans_matrix = GMatrix::translateMatrix(-gps_long,-gps_lat,0)*g_trans_matrix;
	return true;
}
bool WMapView::rotate(double v)
{
	constexpr auto kDelta = 1E-8;

	if(fabs(v)<kDelta) 
		g_rotate_matrix = GMatrix::identityMatrix();
	else
		g_rotate_matrix = GMatrix::rotatezMatrix(v);
	return true;
}
void WMapView::drawCompass(float height,float radius)
{
	float color[] = {
		0.0f,0.0f,0.8f,
		0.3f,0.3f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,0.1f,0.2f,
	};


	glPushMatrix();
	drawCompassPin(height,radius,color,color+3);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90,0.0f,0.0f,1.0f);
	drawCompassPin(height,radius,color+6,color+9);
	glPopMatrix();
}
void WMapView::drawCompassPin(float height,float radius,const float* color0,const float* color1)
{
	const auto h= radius*0.5;
	const float points[] = {
		-radius  ,  0        ,  0  ,  
		0        ,  0        ,  h  ,  
		0        ,  -height  ,  0  ,  
		radius   ,  0        ,  0  ,  
		0        ,  height   ,  0  ,  

	};

	glEnable(GL_LINE_SMOOTH);
	glBegin(GL_TRIANGLES);
	glColor3fv(color0);
	glNormal3f(-1,1,-1);
	glVertex3fv(points);
	glVertex3fv(points+3);
	glVertex3fv(points+6);

	glNormal3f(1,1,-1);
	glVertex3fv(points+3);
	glVertex3fv(points+9); 
	glVertex3fv(points+6);

	glColor3fv(color1);
	glNormal3f(-1,1,1);
	glVertex3fv(points+12);
	glVertex3fv(points+3);
	glVertex3fv(points);

	glNormal3f(1,1,1);
	glVertex3fv(points+12);
	glVertex3fv(points+9);
	glVertex3fv(points+3);
	glEnd();
	glDisable(GL_LINE_SMOOTH);
}
#endif
