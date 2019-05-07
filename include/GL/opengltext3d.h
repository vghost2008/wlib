/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtGlobal>
#ifndef Q_OS_MAC
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include <memory>
#include <wmath.h>
class OpenglText3D
{
	public:
		struct GPoint {
			double pos[3];
		};
		struct GBox{
			double xMin;
			double yMin;
			double xMax;
			double yMax;
		};
	private:
		using DrawOutlineFunc = void (OpenglText3D:: *)(const FT_Outline&)const;
	public:
		enum TextDirection {
			TD_X_POSITIVE  ,  
			TD_Y_POSITIVE  ,  
			TD_Z_POSITIVE  ,  
			TD_X_NEGATIVE  ,  
			TD_Y_NEGATIVE  ,  
			TD_Z_NEGATIVE  ,  
		};
		static std::unique_ptr<OpenglText3D> create(const char* font_path,float size=72,float thickness=1);
	public:
		~OpenglText3D();
		/*
		 * 返回文字的长度
		 */
		float drawXYText(float x,float y,float z,const char* fmt,...)const;
		float drawXYText2d(float x,float y,float z,const char* fmt,...)const;
		float drawXYText(float x,float y,float z,const std::wstring& text)const;
		float drawXYText2d(float x,float y,float z,const std::wstring& text)const;
		float drawXZText(float x,float y,float z,const char* fmt,...)const;
		float drawXZText(float x,float y,float z,TextDirection direction,const char* fmt,...)const;
		float drawXZText2d(float x,float y,float z,const char* fmt,...)const;
		float drawXZText2d(float x,float y,float z,TextDirection direction,const char* fmt,...)const;
		void getBoundingBox(const std::wstring& text,GBox* box)const;
		double getWidth(const std::wstring& text)const;
		double getHeight(const std::wstring& text)const;
		void setSize(float size=72);
		void setThickness(float thickness=1);
	private:
		OpenglText3D(const char* font_path,float size,float thickness)noexcept(false);
		float drawChar(unsigned long c)const;
		void drawOutline(const FT_Outline& outline)const;
		float drawChar2d(unsigned long c)const;
		void drawOutline2d(const FT_Outline& outline)const;
		std::vector<GPoint> getPolygonPoints(const FT_Outline& outline,int first,int last)const;
	private:
		float __drawText(float x,float y,float z,const std::wstring& text,DrawOutlineFunc func)const;
		float __drawChar(unsigned long c,DrawOutlineFunc func)const;
	private:
		static inline void vertex3f(const FT_Vector& p,float z=0){
			glVertex3f(WSpace::int26p62f(p.x),WSpace::int26p62f(p.y),z);
		}
		static inline GPoint togpoint(const FT_Vector& p) {
			GPoint res;
			res.pos[0] = WSpace::int26p62f(p.x);
			res.pos[1] = WSpace::int26p62f(p.y);
			res.pos[2] = 0;
			return res;
		}
		static inline GBox togbox(const FT_BBox& box) {
			GBox res;
			res.xMin  = WSpace::int26p62f(box.xMin);
			res.xMax  = WSpace::int26p62f(box.xMax);
			res.yMin  = WSpace::int26p62f(box.yMin);
			res.yMax  = WSpace::int26p62f(box.yMax);
			return res;
		}
		/*
		 * 获取outline上以first_pos为第一个点的曲线的最后一个点的位置
		 * curve_last为整个封闭区域的最后一个点的位置
		 */
		static int lastPosOfCurve(const FT_Outline& outline,int first_pos,int curve_last);
	private:
		float          size_             = 1;
		float          thickness_        = 1;
		FT_Library     ft_library_;
		FT_Face        ft_face_;
		mutable GLenum gl_mode_          = GL_POLYGON;
		unsigned       curve_points_num_ = 20;
};
