/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <ogrsf_frmts.h>
#include <GL/glwidget.h>
#include <math.h>
#include <gmatrix.h>
#include <future>
#include <chrono>
#include "gis.h"
#include "wmapdata_fwd.h"
/*
 * Client:客户坐标系统[-gl_width_/2,gl_width_,-gl_height_/2,gl_height_]
 * Word:世界坐标系，地图所在的投影坐标系
 */
class WMapView:public WGLWidget
{
	private:
		struct BuildListControlD
		{
			void init();
			bool createList();
			bool isTimeOut();
			void reset();
			bool                                  available   = false;
			bool                                  create_list = false; //是否可以创建显示列表，如果available,表示可以创建，或者时间满足要求
			bool                                  fin         = false; //本次创建显示列表是否全部完成
			std::chrono::steady_clock::time_point time;                //显示列表创建完成后的时间(还没创建显示列表时为超时时间)
			int                                   nb_nr       = 0;     //创建显示列表未完成时，仅显示图像而不创建显示列表的次数
			int                                   timer_id    = -1;    //定时器id
		};
	public:
		WMapView();
		~WMapView();
	public:
		void reloadMap();
		virtual void setMap(WMap* map);
		void update();
		/*
		 * 返回模型视图坐标系统中(包含了g_trans_matrix与g_object_trans_matrix，当前窗口的区域
		 */
		QRectF getViewRect()const;
	public:
		inline bool moveTo(const GPSPoint& pos) { return moveTo(pos.x,pos.y); }
		bool moveTo(double gps_long,double gps_lat);
		/*
		 * 顺时针方向旋转v度
		 */
		bool rotate(double v);
		/*
		 * 初始化平移,旋转及放缩变换
		 */
		void initTrans();
	public:
		/*
		 * 地图坐标(地图的投影坐标)与没有附加模型视图变换的opengl坐标之间的转换
		 */
		bool worldToClient(double* gps_long,double* gps_lat);
		bool worldToClient(const WSpace::GMatrix& w2cm,double* gps_long,double* gps_lat);
		/*
		 * 需要通过计算获取
		 */
		WSpace::GMatrix worldToCliendMatrix()const;
		/*
		 * GPS坐标与没有附加模型视图变换的opengl坐标之间的转换
		 */
		bool gpsToClient(double* gps_long,double* gps_lat);
		/*
		 * GPS与地图坐标的转换
		 */
		bool gpsToWorld(double* gps_long,double* gps_lat);
		inline bool gpsToWorld(GPSPoint* pos) {
			return gpsToWorld(&pos->x,&pos->y);
		}
		bool worldToGPS(double* x,double* y);
		bool worldToGPS(GPSPoint* pos);
		bool screenToWord(int x,int y, double* gps_long, double* gps_lat);
		inline bool screenToWord(const QPoint& p,GPSPoint* gps) {
			return screenToWord(p.x(),p.y(),&gps->x,&gps->y);
		}
		bool screenToGPS(const QPoint& p,GPSPoint* pos);
	protected:
		/*
		 * 绘制地图之前的用户自定义绘图
		 * 在地图的投影坐标系中
		 */
		virtual void paintBeforeMap();
		/*
		 * 绘制地图之后的用户自定义绘图
		 * 在地图的投影坐标系中
		 */
		virtual void paintAfterMap();
		/*
		 * 地图数据发生了改变
		 */
		virtual void mapChanged();
		/*
		 * 显示一个罗盘
		 */
		void drawCompass(float height,float radius);
		/*
		 * 显示一个罗盘的一个水平的指针
		 */
		void drawCompassPin(float height,float radius,const float* color0,const float* color1);
	protected:
		/*
		 * 仅调用glVertex3f
		 */
		void showGPSTrace(const std::vector<GPSPoint>& data);
		void showGPSTrace(const std::vector<GPSPoint>& data,GLuint type);
		/*
		 * 使用地图坐标绘制
		 */
		void showWorldTrace(std::vector<std::pair<double,double>>&& data,GLuint type);
		/*
		 * 直接绘制多边形
		 */
		void showGPSAreaPolygon(const std::vector<GPSPoint>& data);
		/*
		 * 直接使用地图坐标绘制多边形
		 */
		void showWorldAreaPolygon(std::vector<GLdouble>&& data);
	protected:
		virtual void buildList();
	protected:
		void mouseDoubleClickEvent(QMouseEvent* event)override;
		void wheelEvent(QWheelEvent* e)override;
	protected:
		/*
		 * 地图范围
		 */
		OGREnvelope     map_extent_;
	private:
		MapViewConfig  map_config_;
		bool           first_show_  = true;
		int            scale_count_ = 0;
		WMap          *map_         = nullptr; //数据源，保留方便reloadMap操作
		WMap           map_cache_;             //绘图使用的数据
	protected:
		GLuint fill_circle_list_ = 0;
		GLuint line_circle_list_ = 0;
	private:
		void paintGL()override;
		void buildMapList();
		bool buildMapListOfLayout(WMapLayout& ml);
		bool isTotalBuild(const WMapLayout& ml)const;
		void buildCircleList();
	private:
		void showMapListInLevel(int level_begin,int level_end,const QRectF& rect);
		void showMapTextInLevel(int level_begin,int level_end,const QRectF& rect);
		void showMapListOfLayout(WMapLayout& l,const QRectF& rect);
		void showMapTextOfLayout(WMapLayout& l,const QRectF& rect);
	private:
		/*
		 * 显示多边形填充
		 */
		bool showLayoutPolygon(WMapLayout* ml);
		/*
		 * 显示多边形的边及其它线图形
		 */
		bool showBaseMapLayout(WMapLayout* ml);
		void showPolygon(const WMPolygon& polygon,GLUtesselator* tesselator);
		void showPolygonEdge(const WMPolygon& polygon);
		void showLineString(const WMLineString& line_string);
		void showPoints(WMapLayout& l,const QRectF& rect,const WMPoints& points);
	private:
		void setCacheMap();
	private:
		inline double radiusOfGPSPoint(const GPSPoint& p0,const GPSPoint& p1) {
			auto dx = p0.x-p1.x;
			auto dy = p0.y-p1.y;
			return sqrt(dx*dx+dy*dy);
		}
		bool equal_glcolor(const float* lhv,const float* rhv);
		virtual void timerEvent(QTimerEvent* event)override;
	private:
		std::unique_ptr<OGRSpatialReference> proj_spatial_;
		std::unique_ptr<OGRSpatialReference> geo_spatial_;
		static const int                     kTimeThresholdForBBGL;
		static const int                     kTimeThresholdForIdle;
		BuildListControlD                    blcd_;
};
