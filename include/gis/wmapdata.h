_Pragma("once")
#ifndef DISABLE_MAPVIEW
#include <memory>
#include <list>
#include <vector>
#include <cmath>
#include <iostream>
#include <QDebug>
#include "wmapdata_fwd.h"

namespace wmap
{
	constexpr float kLayoutOffset      = -1.0f;
	constexpr float kPolygonOffset     = -0.5f;
	constexpr float kLineOffset        = -0.4f;
	constexpr float kPointOffset       = -0.3f;
	constexpr float kTextOffset        = 0.0f;

	enum WMapBlockId:uint32_t
	{
		MBI_FILE  =  0xFFFFFF00               ,  
		MBI_PROJ                              ,  
		MBI_LAYOUT                            ,  
		MBI_ATTR                              ,  
		MBI_VIEWDATA                          ,  
		MBI_LABEL                             ,  
		MBI_POLYGON                           ,  
		MBI_CONVEX_POLYGON       =0xFFFFFF20  ,  
		MBI_LINESTRING                        ,  
		MBI_ENVELOPE                          ,  
		MBI_STRING               =0xFFFFFF40  ,  
		MBI_VECTOR                            ,  
		MBI_LIST                              ,  
	};
	template<class T>
		inline bool isgood(T v) {
			if(std::isnan(v) || std::isinf(v)) {
				qDebug()<<"Not good num";
				return false;
			}
			return true;
		}
	inline bool isgood(const OGRPoint& p) {
		if(isgood(p.getX()) && isgood(p.getY())) return true;
		return false;
	}
	bool saveMap(const QString& file_path,const WMap& map);
	bool readMap(const QString& file_path,WMap* map);
	bool saveXmlMap(const QString& file_path,const WMap& map);
	bool readXmlMap(const QString& file_path,WMap* map);
	bool saveWMap(const QString& file_path,const WMap& map);
	bool readWMap(const QString& file_path,WMap* map);

	bool insertPoint(const OGRPoint& point,WMapViewData* view_data,float z);
	bool insertMultiPoint(const OGRMultiPoint& mpoint, WMapViewData* view_data,float z);
	bool insertMultiLineString(const OGRMultiLineString& mlstring,WMapViewData* view_data,float z);
	bool insertLineString(const OGRLineString& line_string,WMapViewData* view_data,float z);
	bool insertPolygon(const OGRPolygon& polygon, WMapViewData* view_data,float z);
	bool insertMultiPolygon(const OGRMultiPolygon& mpolygon, WMapViewData* view_data,float z);
	void getTextLabel(OGRFeature* pofeature,OGRFeatureDefn* pofdefn,OGRGeometry* pogeometry,std::list<WMLabel>* label_list);
	bool pushLabel(WMLabel&& label,std::list<WMLabel>* label_list);
	void insertOGRGeometry(const OGRGeometry& ogeometry,WMapViewData* view_data,float point_z,float line_z,float polygon_z);
	void clearViewData(WMapViewData* view_data);
	void releaseMemAfterVList(WMapViewData* view_data);
	bool addLayout(OGRLayer& layout, WMapViewData* view_data,std::shared_ptr<OGRSpatialReference>* proj_spatial,std::shared_ptr<OGRSpatialReference>* geo_spatial,float point_z,float line_z,float polygon_z);
	bool partitionPolygon(WMPolygon* polygon);
	bool meshPolygon(WMPolygon* polygon);
	bool init(const OGRLayer& layout);
	bool readMapViewData(const std::string& map_path,WMapViewData* view_data,std::shared_ptr<OGRSpatialReference>* proj_spatial,std::shared_ptr<OGRSpatialReference>* geo_spatial,float point_z,float line_z,float polygon_z);
	bool readWMap(std::istream& stream,WMap* map);
	bool saveWMap(std::ostream& stream,const WMap& map);
	bool readWMap_data(std::istream& stream,std::string* str);
	bool saveWMap_data(std::ostream& stream,const std::string& str);
	bool readWMap_data(std::istream& stream,std::shared_ptr<OGRSpatialReference>& proj);
	bool saveWMap_data(std::ostream& stream,const std::shared_ptr<OGRSpatialReference>& proj);
	bool readWMap_data(std::istream& stream,WMapLayout* ml);
	bool saveWMap_data(std::ostream& stream,const WMapLayout& ml);
	bool readWMap_data(std::istream& stream,WMapLayoutAttr* attr);
	bool saveWMap_data(std::ostream& stream,const WMapLayoutAttr& attr);
	bool readWMap_data(std::istream& stream,WMapViewData* viewdata);
	bool saveWMap_data(std::ostream& stream,const WMapViewData& viewdata);
	bool readWMap_data(std::istream& stream,WMPolygon* viewdata);
	bool saveWMap_data(std::ostream& stream,const WMPolygon& poly);
	bool readWMap_data(std::istream& stream,WMLineString* viewdata);
	bool saveWMap_data(std::ostream& stream,const WMLineString& poly);
	bool readWMap_data(std::istream& stream,OGREnvelope* env);
	bool saveWMap_data(std::ostream& stream,const OGREnvelope& env);
	bool readWMap_data(std::istream& stream,WMPointObj* env);
	bool saveWMap_data(std::ostream& stream,const WMPointObj& env);
	bool readWMap_data(std::istream& stream,WMConvexPolygon* env);
	bool saveWMap_data(std::ostream& stream,const WMConvexPolygon& env);
	bool readWMap_data(std::istream& stream,WMLabel* env);
	bool saveWMap_data(std::ostream& stream,const WMLabel& env);
	bool readAndCheckId(std::istream& stream,const uint32_t id);
	template<class T>
		bool readWMap_data(std::istream& stream,std::vector<T>* data)
		{
			uint32_t size = 0;
			T        v;

			data->clear();
			if(!readAndCheckId(stream,MBI_VECTOR)) return false;
			stream.read((char*)&size,sizeof(size));
			data->reserve(size);
			for(int i=0; i<size; ++i) {
				if(!readWMap_data(stream,&v)) return false;
				data->push_back(std::move(v));
			}

			return stream.good();
		}
	template<class T>
		bool saveWMap_data(std::ostream& stream,const std::vector<T>& data)
		{
			const uint32_t id   = MBI_VECTOR;
			const uint32_t size = data.size();

			stream.write((const char*)&id,sizeof(id));
			stream.write((const char*)&size,sizeof(size));
			for(auto& v:data) {
				if(!saveWMap_data(stream,v)) return false;
			}
			return stream.good();
		}
	template<class T>
		bool readWMap_data(std::istream& stream,std::list<T>* data)
		{
			uint32_t size = 0;
			T        v;

			data->clear();
			if(!readAndCheckId(stream,MBI_LIST)) return false;
			stream.read((char*)&size,sizeof(size));
			for(int i=0; i<size; ++i) {
				if(!readWMap_data(stream,&v)) return false;
				data->push_back(std::move(v));
			}

			return stream.good();
		}
	template<class T>
		bool saveWMap_data(std::ostream& stream,const std::list<T>& data)
		{
			const uint32_t id   = MBI_LIST;
			const uint32_t size = data.size();

			stream.write((const char*)&id,sizeof(id));
			stream.write((const char*)&size,sizeof(size));
			for(auto& v:data) {
				if(!saveWMap_data(stream,v)) return false;
			}
			return stream.good();
		}
	bool loadLayoutData(const WMap& map,WMapLayout* ml);
	bool loadMapFile(WMap* map);
	/*
	 * 删除不同图层中重复的文字标签
	 */
	void uniqueTextLabels(const WMap& map,WMapLayout* ml);
	/*
	 * 根据layouts中指定的图层顺序，图层属性更新map
	 * 图层通过file_name标识
	 */
	bool updateMapLayouts(const std::vector<WMapLayout>& layouts,WMap* map);
}
#endif
