/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#ifndef DISABLE_MAPVIEW
#include <string>
#include <vector>
#include <GL/gl.h>
#include <ogrsf_frmts.h>
#include <algorithm>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <QPointF>
#include <vector>
#include <list>
#include <QString>

constexpr const char* kMapFileName = "map.xml";
/*
 * 地图标签
 */
struct WMLabel
{
	QPointF pos;
	double  area;
	QString text;
};
struct WMPointObj
{
	double x;
	double y;
	double z;
};
struct WMPoints
{
	std::vector<WMPointObj> data;
};
struct WMConvexPolygon
{
	std::vector<WMPointObj>       data;
};
struct WMPolygon
{
	std::vector<WMPointObj>            exterior;
	std::list<std::vector<WMPointObj>> interior;
	std::list<WMConvexPolygon>         convex_polygons;   //多边形的凸多边形分割
	std::vector<WMPointObj>            triangles;         //多边形的三角形分割
};
struct WMLineString
{
	std::vector<WMPointObj>    data;
};
enum WMPointType
{
	MPT_NORMAL           ,  
	MPT_TWO_CIRCLE       ,  
	MPT_TWO_FILL_CIRCLE  ,  
};
enum WMPolygonType
{
	MPOT_FILL           ,  
	MPOT_FILL_AND_EDGE  ,  
	MPOT_EDGE           ,  
};
/*
 * 地图显示时使用的数据
 */
struct WMapViewData
{
	std::list<WMLabel>      labels;
	WMPoints                points;
	std::list<WMPolygon>    polygons;
	std::list<WMLineString> linestrings;
	std::vector<GLuint>     graphics_lists;
	int                     current_polygon = 0;    //当前已经够建显示列表的多边形数目
	OGREnvelope             extent;
	float                   offset          = 0.0f;
};
namespace boost { namespace geometry { namespace traits {
	using ogre_value_type  =  decltype(OGREnvelope::MinX);
	using ogre_point       =  boost::geometry::model::point<ogre_value_type,2,boost::geometry::cs::cartesian>;

	template<> struct tag<OGREnvelope> 
	{ typedef box_tag type; };

	template<> struct point_type<OGREnvelope> 
	{ typedef ogre_point type; };

	template <>
		struct indexed_access<OGREnvelope, min_corner,0>
		{
			static inline ogre_value_type get(OGREnvelope const& b)
			{
				return b.MinX;
			}

			static inline void set(OGREnvelope& b, ogre_value_type const& value)
			{
				b.MinX = value;
			}
		};
	template <>
		struct indexed_access<OGREnvelope, min_corner, 1>
		{
			static inline ogre_value_type get(OGREnvelope const& b)
			{
				return b.MinY;
			}

			static inline void set(OGREnvelope& b, ogre_value_type const& value)
			{
				b.MinY = value;
			}
		};

	template <>
		struct indexed_access<OGREnvelope, max_corner, 0>
		{
			static inline ogre_value_type get(OGREnvelope const& b)
			{
				return b.MaxX;
			}

			static inline void set(OGREnvelope& b, ogre_value_type const& value)
			{
				b.MaxX = value;
			}
		};
	template <>
		struct indexed_access<OGREnvelope, max_corner, 1>
		{
			static inline ogre_value_type get(OGREnvelope const& b)
			{
				return b.MaxY;
			}

			static inline void set(OGREnvelope& b, ogre_value_type const& value)
			{
				b.MaxY = value;
			}
		};


}}} // namespace boost::geometry::traits
/*
 * 地图图层属性
 */
struct WMapLayoutAttr
{
	float line_color[4]  =  {0};
	float fill_color[4]  =  {0};        //面/点填充色
	float text_color[4]  =  {0};
	float line_width     =  1;
	float point_size     =  1;          //点大小
	int   text_size      =  0;
	int   level          =  0;          //级别，0表示当前图层总是显示，其它根据情况而定
	int   point_type     =  MPT_NORMAL; //点样式
	int   text_offset    =  0;
	int   polygon_type   =  MPOT_FILL_AND_EDGE;       //是否显示多边形边框
	bool  show_label     =  false;      //是否显示图层的文字标签
};
/*
 */
/*
 * 用于表示一个地图的图层
 */
struct WMapLayout
{
	std::string                          file_name;      //地图文件名,不包含路径
	WMapLayoutAttr                       attr;
	WMapViewData                         view_data;
	std::shared_ptr<OGRSpatialReference> proj_spatial;
	std::shared_ptr<OGRSpatialReference> geo_spatial;
	bool                                 visible;        //当前图层是否可见
};
/*
 * 用于表示一个地图
 */
struct WMap
{
	std::string                          file_path;      //地图文件路径
	std::shared_ptr<OGRSpatialReference> proj_spatial;
	std::shared_ptr<OGRSpatialReference> geo_spatial;
	std::vector<WMapLayout>              layouts;
};
/*
 * 其它
 */
enum MapFileFormat
{
	MFF_BITMAP   ,   
	MFF_VECTOR   ,   
};
struct MapViewConfig
{
	MapFileFormat map_format;
	QString       map_file;
};
enum MLabelElement
{
	MLE_POS_X   =   0x01,
	MLE_POS_Y   =   0x02,
	MLE_TEXT    =   0x04,
	MLE_ALL     =   MLE_POS_X|MLE_POS_Y|MLE_TEXT,
};
#endif
