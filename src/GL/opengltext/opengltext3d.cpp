/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <opengltext3d.h>
#include <toolkit.h>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/box.hpp>
#include <QDebug>
#include <wmath.h>

BOOST_GEOMETRY_REGISTER_POINT_2D(FT_Vector, signed long, boost::geometry::cs::cartesian, x, y);
BOOST_GEOMETRY_REGISTER_POINT_2D(OpenglText3D::GPoint, double, boost::geometry::cs::cartesian, pos[0], pos[1]);
BOOST_GEOMETRY_REGISTER_BOX_2D_4VALUES(OpenglText3D::GBox, OpenglText3D::GPoint, xMin, yMin, xMax, yMax)
typedef void (*TESS_CALLBACK_FUNC )();
static void tessError(GLenum error);
using namespace std;
using namespace WSpace;
ostream& operator<<(ostream& out,const OpenglText3D::GPoint& p)
{
	out<<"("<<p.pos[0]<<","<<p.pos[1]<<")";
	return out;
}
namespace bg=boost::geometry;

OpenglText3D::OpenglText3D(const char* font_path,float size,float thickness)noexcept(false)
:size_(size)
,thickness_(thickness)
{
	if(FT_Init_FreeType(&ft_library_)) {
		ERR("FT_Init_FreeType faild.");
		throw std::runtime_error("FT_Init_FreeType faild");
	}
	if(FT_New_Face(ft_library_,font_path,0,&ft_face_)) {
		ERR("FT_New_Face (%s) faild.",font_path);
		FT_Done_FreeType(ft_library_);
		throw std::runtime_error("FT_New_Face faild.");
	}
	FT_Set_Pixel_Sizes(ft_face_,size_,size_);
}
std::unique_ptr<OpenglText3D> OpenglText3D::create(const char* font_path,float size,float thickness)
{
	try {
		auto p = new OpenglText3D(font_path,size,thickness);
		return unique_ptr<OpenglText3D>(p);
	} catch(...) {
		return nullptr;
	}
}
OpenglText3D::~OpenglText3D()
{
	FT_Done_Face(ft_face_);
	FT_Done_FreeType(ft_library_);
}
float OpenglText3D::__drawText(float x,float y,float z,const wstring& text,DrawOutlineFunc func)const
{
	auto offset = 0;

	glPushMatrix();
	glTranslatef(x,y,z);
	for(int i=0; i<text.size(); ++i) {
		glTranslatef(offset,0,0);
		offset = __drawChar(text[i],func);
	}
	glPopMatrix();
	return offset;
}
float OpenglText3D::__drawChar(unsigned long c,DrawOutlineFunc func)const
{
	auto         glyph_index = FT_Get_Char_Index(ft_face_,c);
	FT_Int32     load_flags  = FT_LOAD_DEFAULT|FT_LOAD_NO_BITMAP;
	FT_Error     error       = FT_Load_Glyph(ft_face_,glyph_index,load_flags);
	FT_GlyphSlot glyph_slot  = ft_face_->glyph;

	(this->*func)(glyph_slot->outline);

	return int26p62f(glyph_slot->advance.x);
}
float OpenglText3D::drawXYText(float x,float y,float z,const char* fmt,...)const
{
	va_list ap;
	wchar_t text[256];
	char    ltext[256];

	if(nullptr == fmt) return 0;

	va_start(ap, fmt); 
	vsprintf(ltext, fmt, ap); 
	va_end(ap); 
	swprintf(text,ARRAY_SIZE(text),L"%s",ltext);
	return __drawText(x,y,z,text,&OpenglText3D::drawOutline);
}
float OpenglText3D::drawXYText2d(float x,float y,float z,const char* fmt,...)const
{
	va_list ap;
	wchar_t text[256];
	char    ltext[256];

	if(nullptr == fmt) return 0;

	va_start(ap, fmt); 
	vsprintf(ltext, fmt, ap); 
	va_end(ap); 
	swprintf(text,ARRAY_SIZE(text),L"%s",ltext);
	return __drawText(x,y,z,text,&OpenglText3D::drawOutline2d);
}
float OpenglText3D::drawXYText(float x,float y,float z,const wstring& text) const
{
	return __drawText(x,y,z,text,&OpenglText3D::drawOutline);
}
float OpenglText3D::drawXYText2d(float x,float y,float z,const wstring& text)const
{
	return __drawText(x,y,z,text,&OpenglText3D::drawOutline2d);
}
float OpenglText3D::drawXZText(float x,float y,float z,const char* fmt,...)const
{
	va_list ap;
	char    ltext[256];

	if(nullptr == fmt) return 0;

	va_start(ap, fmt); 
	vsprintf(ltext, fmt, ap); 
	va_end(ap); 

	return drawXZText(x,y,z,TD_Z_POSITIVE,ltext);
}
float OpenglText3D::drawXZText(float x,float y,float z,OpenglText3D::TextDirection direction ,const char* fmt,...)const
{
	va_list ap;
	wchar_t text[256];
	char    ltext[256];

	if(nullptr == fmt) return 0;

	va_start(ap, fmt); 
	vsprintf(ltext, fmt, ap); 
	va_end(ap); 
	swprintf(text,ARRAY_SIZE(text),L"%s",ltext);

	glPushMatrix();
	glTranslatef(x,y,z);
	glRotatef(OpenglText3D::TD_Z_NEGATIVE==direction?90:-90,0,1,0);
	glRotatef(-90,0,0,1);
	auto res = __drawText(0,0,0,text,&OpenglText3D::drawOutline);
	glPopMatrix();

	return res;
}
float OpenglText3D::drawXZText2d(float x,float y,float z,const char* fmt,...)const
{
	va_list ap;
	char    ltext[256];

	if(nullptr == fmt) return 0;

	va_start(ap, fmt); 
	vsprintf(ltext, fmt, ap); 
	va_end(ap); 

	return drawXZText2d(x,y,z,TD_Z_POSITIVE,ltext);
}
float OpenglText3D::drawXZText2d(float x,float y,float z,OpenglText3D::TextDirection direction,const char* fmt,...)const
{
	va_list ap;
	wchar_t text[256];
	char    ltext[256];

	if(nullptr == fmt) return 0;

	va_start(ap, fmt); 
	vsprintf(ltext, fmt, ap); 
	va_end(ap); 
	swprintf(text,ARRAY_SIZE(text),L"%s",ltext);
	glPushMatrix();
	glTranslatef(x,y,z);
	glRotatef(OpenglText3D::TD_Z_NEGATIVE==direction?90:-90,0,1,0);
	glRotatef(-90,0,0,1);
	auto res = __drawText(0,0,0,text,&OpenglText3D::drawOutline2d);
	glPopMatrix();
	return res;
}
void OpenglText3D::drawOutline(const FT_Outline& outline)const
{
	vector<GPoint> points;

	glPushMatrix();
	//正面
	glNormal3f(0,0,1);
	drawOutline2d(outline);

	//背面
	glPushMatrix();
	glNormal3f(0,0,-1);
	glTranslatef(0,0,-thickness_);
	drawOutline2d(outline);
	glPopMatrix();
	glNormal3f(-1,0,0);

	//侧面
	for(auto i=0; i<outline.n_contours; ++i) {
		auto first  = (i==0)?0:outline.contours[i-1]+1;
		auto last   = outline.contours[i];
		auto points = getPolygonPoints(outline,first,last);
		if(points.empty())continue;

		vector<OpenglText3D::GPoint> normal;
		GPoint  n;
		normal.reserve(points.size());

		for(auto it = points.begin(); it!=prev(points.end()); ++it) {
			GPoint &p0 = *it;
			GPoint  p1   {{p0.pos[0],p0.pos[1],-thickness_}};
			GPoint &p2 = *next(it);

			cross_prod<double>(p0.pos,p1.pos,p2.pos,n.pos);

			normal.push_back(n);
		}

		const auto delta = 0.1;
		glBegin(GL_TRIANGLE_STRIP);
		int k=0; 
		for(auto& p:points) {
			glNormal3dv(normal[k++].pos);
			glVertex3d(p.pos[0],p.pos[1],0+delta);
			glVertex3d(p.pos[0],p.pos[1],-thickness_-delta);
		}
		glNormal3dv(normal[0].pos);

		auto& p = points.front();
		glVertex3d(p.pos[0],p.pos[1],0+delta);
		glVertex3d(p.pos[0],p.pos[1],-thickness_-delta);
		glEnd();
	}
	glPopMatrix();
}
void OpenglText3D::drawOutline2d(const FT_Outline& outline)const
{
	list<vector<GPoint>> points_list;

	for(auto i=0; i<outline.n_contours; ++i) {
		auto first   = (i==0)?0:outline.contours[i-1]+1;
		auto last    = outline.contours[i];
		points_list.emplace_back(std::move(getPolygonPoints(outline,first,last)));
	}

	glDisable(GL_BLEND);
	glDisable(GL_POLYGON_SMOOTH);

	GLUtesselator       *tesselator = gluNewTess();


	gluTessCallback(tesselator,GLU_TESS_BEGIN,(TESS_CALLBACK_FUNC)glBegin);
	gluTessCallback(tesselator,GLU_TESS_END,(TESS_CALLBACK_FUNC)glEnd);
	gluTessCallback(tesselator,GLU_TESS_VERTEX,(TESS_CALLBACK_FUNC)glVertex3dv);
	gluTessCallback(tesselator,GLU_TESS_ERROR,(TESS_CALLBACK_FUNC)tessError);
	gluTessProperty(tesselator,GLU_TESS_WINDING_RULE,GLU_TESS_WINDING_ODD);

	gluTessBeginPolygon(tesselator,nullptr);
	for(auto& points:points_list) {
		gluTessBeginContour(tesselator);
		for(auto& p:points) {
			p.pos[2] = 0;
			gluTessVertex(tesselator,p.pos,p.pos);
		}
		gluTessEndContour(tesselator);
	}
	gluTessEndPolygon(tesselator);

	gluDeleteTess(tesselator); tesselator = nullptr;
#if 0
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH,GL_NICEST);
	glLineWidth(0.5f);
	for(auto& points:points_list) {
		glBegin(GL_LINE_LOOP);
		for(auto& p:points) {
			glVertex2dv(p.pos);
		}
		glEnd();
	}
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_BLEND);
#endif
}
vector<OpenglText3D::GPoint> OpenglText3D::getPolygonPoints(const FT_Outline& outline,int first,int last)const
{
	auto                 l_first    = first;
	auto                 l_last     = l_first;
	std::vector<GPoint>  points;

	do {
		l_last = lastPosOfCurve(outline,l_first,last);
		if(l_last == -1) {
			ERR("Error");
			break;
		}
		if(l_last-l_first == 1) {
			auto point0 = outline.points[l_first];
			points.push_back(togpoint(point0));
			auto point1 = outline.points[l_last];
			points.push_back(togpoint(point1));
		} else {
			auto tag = FT_CURVE_TAG(*(outline.tags+l_first+1));
			switch(tag) {
				case FT_CURVE_TAG_CONIC:
					{
						auto last_on_point = togpoint(outline.points[l_first]);
						for(auto index = l_first+2; index<=l_last; ++index) {
							auto tag = FT_CURVE_TAG(*(outline.tags+index));
							switch(tag) {
								case FT_CURVE_TAG_ON:
									{
										const auto delta = 1.0f/curve_points_num_;
										auto       t     = 0.0;
										GPoint     gps[] = { last_on_point,togpoint(outline.points[index-1]),togpoint(outline.points[index]) };

										while(t<=1.0f) {
											auto p = bezierQuad(gps,t);
											points.push_back(p);
											t += delta;
										}
									}
									break;
								case FT_CURVE_TAG_CONIC:
									{
										const auto delta   = 1.0f/curve_points_num_;
										auto       t       = 0.0;
										auto       end_pos = togpoint(outline.points[index-1]);

										bg::add_point(end_pos,togpoint(outline.points[index]));
										bg::divide_value(end_pos,2);

										GPoint     gps[]   = { last_on_point,togpoint(outline.points[index-1]),end_pos };

										last_on_point = end_pos;
										while(t<=1.0f) {
											auto p = bezierQuad(gps,t);
											points.push_back(p);
											t += delta;
										}
									}
									break;
								default:
									ERR("Unexpected tag:%x.",int(tag));
									l_first = l_last;
									break;
							}
						}
					}
					break;
				case FT_CURVE_TAG_CUBIC:
					{
						if(l_last -l_first != 3) {
							ERR("Unexpected.");
							l_first = l_last;
							continue;
						}
						const auto delta = 1.0f/curve_points_num_;
						auto       t     = 0.0;
						GPoint     gps[] = {togpoint(outline.points[l_first]),togpoint(outline.points[l_first+1]),togpoint(outline.points[l_first+2]),togpoint(outline.points[l_first+3])};

						while(t<=1.0f) {
							auto p = bezierCubic(gps,t);
							points.push_back(p);
							t += delta;
						}
					}
					break;
				default:
					ERR("Unexpected tag:%x.",int(tag));
					break;
			}
		}

		l_first  =  l_last;
	}while(l_first<last); // for

	return points;
}
int OpenglText3D::lastPosOfCurve(const FT_Outline& outline,int first_pos,int curve_last)
{
	char                    tag = 0;
	auto last_pos = first_pos;

	do {
		++last_pos;
		tag = FT_CURVE_TAG(*(outline.tags+last_pos));
	} while(!(FT_CURVE_TAG_ON&tag) && last_pos<curve_last);
	return last_pos;
}
static void tessError(GLenum error)
{
	//const char* error_str = (const char*)gluErrorString(error);
	//std::cout<<"Tess Error:"<<error_str<<endl;
}
void OpenglText3D::getBoundingBox(const std::wstring& text,GBox* box)const
{
	auto offset = 0.0;
	bg::assign_inverse(*box);

	for(int i=0; i<text.size(); ++i) {
		auto         glyph_index = FT_Get_Char_Index(ft_face_,text[i]);
		FT_Int32     load_flags  = FT_LOAD_DEFAULT|FT_LOAD_NO_BITMAP;
		FT_Error     error       = FT_Load_Glyph(ft_face_,glyph_index,load_flags);
		FT_GlyphSlot glyph_slot  = ft_face_->glyph;
		GBox         temp_box    = togbox(ft_face_->bbox);

		temp_box.xMin += offset;
		temp_box.xMax += offset;

		bg::expand(*box,temp_box);

		offset += int26p62f(glyph_slot->advance.x);
	}
}
double OpenglText3D::getWidth(const std::wstring& text)const
{
	auto offset = 0.0;

	for(int i=0; i<text.size(); ++i) {
		auto         glyph_index = FT_Get_Char_Index(ft_face_,text[i]);
		FT_Int32     load_flags  = FT_LOAD_DEFAULT|FT_LOAD_NO_BITMAP;
		FT_Error     error       = FT_Load_Glyph(ft_face_,glyph_index,load_flags);
		FT_GlyphSlot glyph_slot  = ft_face_->glyph;
		offset += int26p62f(glyph_slot->advance.x);
	}
	return offset;
}
double OpenglText3D::getHeight(const std::wstring& text)const
{
	auto height = 0.0;

	for(int i=0; i<text.size(); ++i) {
		auto         glyph_index = FT_Get_Char_Index(ft_face_,text[i]);
		FT_Int32     load_flags  = FT_LOAD_DEFAULT|FT_LOAD_NO_BITMAP;
		FT_Error     error       = FT_Load_Glyph(ft_face_,glyph_index,load_flags);
		FT_GlyphSlot glyph_slot  = ft_face_->glyph;
		GBox         temp_box    = togbox(ft_face_->bbox);

		height = max(height,temp_box.yMax-temp_box.yMin);
	}
	return height;
}
void OpenglText3D::setSize(float size)
{
	if(size < 0.1) return;
	size_ = size;
	FT_Set_Pixel_Sizes(ft_face_,size_,size_);
}
void OpenglText3D::setThickness(float thickness)
{
	if(thickness< 0.1) return;
	thickness_ = thickness;
}
