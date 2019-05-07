/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#include <gltoolkit.h>
#include <QOpenGLWidget>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace WSpace
{
	/*
	 * 其它辅助函数
	 */
	void  drawCross(float x,float y,float z,float size)
	{
		glBegin(GL_LINES);
		glVertex3f(x-size,y+size,z);
		glVertex3f(x+size,y-size,z);

		glVertex3f(x-size,y-size,z);
		glVertex3f(x+size,y+size,z);
		glEnd();
	}
	void drawCylinder(float xpos,float ypos,float zpos,float radius,float height,int quad_num)
	{
		float       x;
		float       z;
		float       old_x;
		float       old_z;
		float       angle;
		const float delta_angle = GLW_2PI/quad_num;

		old_x = xpos+radius;
		old_z = zpos;

		glBegin(GL_QUADS);
		for(angle = delta_angle; angle < GLW_2PI; angle += delta_angle) {
			x = xpos + radius*cos(angle);
			z = zpos + radius*sin(angle);
			glVertex3f(old_x,ypos,old_z);
			glVertex3f(x,ypos,z);
			glVertex3f(x,ypos+height,z);
			glVertex3f(old_x,ypos+height,old_z);
			old_x = x;
			old_z = z;
		}
		x = xpos+radius;
		z = zpos;
		glVertex3f(old_x,ypos,old_z);
		glVertex3f(x,ypos,z);
		glVertex3f(x,ypos+height,z);
		glVertex3f(old_x,ypos+height,old_z);
		glEnd();
	}
	void drawArrow(float xpos,float ypos,int index,float size)
	{
		const float k=0.5;
		float tsize=size;
		switch(index) {
			case GLW_X:
				glBegin(GL_TRIANGLES);
				glVertex3f(xpos-size,ypos+k*tsize,0);
				glVertex3f(xpos,ypos,0);
				glVertex3f(xpos-size,ypos-k*tsize,0);
				glEnd();
				break;
			case GLW_Y:
				glBegin(GL_TRIANGLES);
				glVertex3f(xpos-k*tsize,ypos-size,0);
				glVertex3f(xpos,ypos,0);
				glVertex3f(xpos+k*tsize,ypos-size,0);
				glEnd();
				break;
		}
	}
	bool getColor(float* tcolor,float fin,float min,float max,int flag)
	{
		assert(nullptr != tcolor);
		switch(flag) {
			case GLW_COLORFLAG_GRAY:
				tcolor[GLW_R]=tcolor[GLW_G]=tcolor[GLW_B]=(fin-min)/(max-min);
				break;
			case GLW_COLORFLAG_OCEAN:
				if(fin>max) {
					tcolor[GLW_R]=1.0;
					tcolor[GLW_G]=0;
					tcolor[GLW_B]=0;
				} else if(fin>=0.5*(max+min)) {
					tcolor[GLW_R]=1.0f;
					tcolor[GLW_G]=2*(fin-max)/(min-max);
					tcolor[GLW_B]=0;
				} else if(fin>=0.75*min+0.25*max) {
					tcolor[GLW_R]=(4*fin-max-3*min)/(max-min);
					tcolor[GLW_G]=1.0f;
					tcolor[GLW_B]=0;
				} else if(fin>=min) {
					tcolor[GLW_R]=0;
					tcolor[GLW_G]=4*(fin-min)/(max-min);
					tcolor[GLW_B]=1-tcolor[GLW_G];
				} else {
					tcolor[GLW_R]=tcolor[GLW_G]=0;
					tcolor[GLW_B]=1;
				}
				break;
		}
		return true;
	}
	void drawCircle(float x0,float y0,float z0,float radius,unsigned short num,GLenum mode)
	{
		unsigned short i           = 0;
		float          angle       = 0.0f;
		float          x;
		float          y;
		const float    delta_angle = M_PI *2.0f/num;

		glBegin(mode);
		for(i=0; i<num; ++i,angle += delta_angle) {
			x = x0+cos(angle)*radius;
			y = y0+sin(angle)*radius;
			glVertex3f(x,y,z0);
		}
		glEnd();
	}
	void getArcPoints3f(float x0,float y0,float z0,float radius,float begangle,float endangle,int num,float *pdata)
	{
		if(num <=0 ) num = endangle-begangle;
		if(num <= 1) return;
		/*>---------------------------------*/
		if(fabs(endangle) > 360) {
			int angle = int(fabs(endangle))%360;
			if(endangle < 0)angle *= -1;
			endangle = angle+endangle-int(endangle);
		}
		begangle = begangle*M_PI/180.0f;
		endangle = endangle*M_PI/180.0f;

		const float delta_angle = (endangle-begangle)/(num-1);
		float angle = begangle;
		for(int i=0; i<num; ++i,angle += delta_angle) {
			*(pdata++) = x0+radius*cos(angle);
			*(pdata++) = y0;
			*(pdata++) = z0+radius*sin(angle);
		}
	}
	bool toGLColor3f(const QColor& i_color,float* o_color) {
		o_color[0] = float(i_color.red())/255.0f;
		o_color[1] = float(i_color.green())/255.0f;
		o_color[2] = float(i_color.blue())/255.0f;
		return true;
	}
	bool toGLColor4f(const QColor& i_color,float* o_color) {
		if(!toGLColor3f(i_color,o_color)) return false;
		o_color[3] = float(i_color.alpha())/255.0f;
		return true;
	}
	bool toGLColor3f(uint32_t di,float* color)
	{
		color[0] = ((di&0xFF0000)>>16)/255.0f;
		color[1] = ((di&0xFF00)>>8)/255.0f;
		color[2] = (di&0xFF)/255.0f;
		return true;
	}
	bool toGLColor4f(uint32_t di,float* color)
	{
		color[0] = ((di&0xFF0000)>>16)/255.0f;
		color[1] = ((di&0xFF00)>>8)/255.0f;
		color[2] = (di&0xFF)/255.0f;
		color[3] = (di>>24)/255.0f;
		return true;
	}
	bool toQColor3f(const float* i_color,QColor* o_color) {
		o_color->setRed(i_color[0]*255.0f);
		o_color->setGreen(i_color[1]*255.0f);
		o_color->setBlue(i_color[2]*255.0f);
		return true;
	}
	bool toQColor4f(const float* i_color, QColor* o_color) {
		if(!toQColor3f(i_color,o_color)) return false;
		o_color->setAlpha(i_color[3]*255);
		return true;
	}
	bool toGLColor3f(const string& str,float* color)
	{
		vector<string> split_vec;
		split(split_vec,str,[](char c){ return ',' == c;});
		memset(color,0,sizeof(float)*3);
		auto size = min<unsigned>(3,split_vec.size());
		try {
			for(int i=0; i<size; ++i) {
				if(split_vec[i].empty())continue;
				color[i] = lexical_cast<float>(split_vec[i]);
			}
		} catch(...) {
			return false;
		}
		return true;
	}
	bool toGLColor4f(const string& str,float* color)
	{
		vector<string> split_vec;
		split(split_vec,str,[](char c){ return ',' == c;});
		memset(color,0,sizeof(float)*4);
		auto size = min<unsigned>(4,split_vec.size());
		try {
			for(int i=0; i<size; ++i) {
				if(split_vec[i].empty())continue;
				color[i] = lexical_cast<float>(split_vec[i]);
			}
		} catch(...) {
			return false;
		}
		return true;
	}
	string glColorToString3f(const float* color)
	{
		string res;

		res += lexical_cast<string>(color[0])+",";
		res += lexical_cast<string>(color[1])+",";
		res += lexical_cast<string>(color[2]);
		return res;
	}
	string glColorToString4f(const float* color)
	{
		string res;

		res += lexical_cast<string>(color[0])+",";
		res += lexical_cast<string>(color[1])+",";
		res += lexical_cast<string>(color[2])+",";
		res += lexical_cast<string>(color[3]);
		return res;
	}
	void drawPolygon(double* data,size_t size)
	{
		GLUtesselator       *tesselator   = gluNewTess();

		gluTessCallback(tesselator,GLU_TESS_BEGIN,(WTESS_CALLBACK_FUNC)glBegin);
		gluTessCallback(tesselator,GLU_TESS_END,(WTESS_CALLBACK_FUNC)glEnd);
		gluTessCallback(tesselator,GLU_TESS_VERTEX,(WTESS_CALLBACK_FUNC)glVertex3dv);
		gluTessCallback(tesselator,GLU_TESS_ERROR,(WTESS_CALLBACK_FUNC)defaultTessError);
		gluTessProperty(tesselator,GLU_TESS_WINDING_RULE,GLU_TESS_WINDING_ODD);
		gluTessCallback(tesselator,GLU_TESS_COMBINE,(WTESS_CALLBACK_FUNC)defaultTessCombineCallback);


		gluTessBeginPolygon(tesselator,nullptr);//&0

		gluTessBeginContour(tesselator);//&1
		for(int j=0; j<size; ++j) {
			gluTessVertex(tesselator,data+j*3,data+j*3);
		}
		gluTessEndContour(tesselator);//&1

		gluTessEndPolygon(tesselator);//&0

		gluDeleteTess(tesselator); tesselator = nullptr;
	}
	void defaultTessError(GLenum error)
	{
		const char* error_str = (const char*)gluErrorString(error);
		std::cout<<"Tess Error:"<<error_str<<endl;
	}
	void defaultTessCombineCallback(GLdouble coords[3],GLdouble* vertex_data[4],GLfloat weight[4],GLdouble** data_out)
	{
		GLdouble *vertext = (GLdouble *)malloc(6*sizeof(GLdouble));
		auto      zero    = 1E-8;

		vertext[0] = coords[0];
		vertext[1] = coords[1];
		vertext[2] = coords[2];

		for(int i=3; i<6; ++i) {
			vertext[i] = 0;
			for(int j=0; j<4; ++j) {
				if(fabs(weight[j]) < zero || vertex_data[j] == nullptr)continue;
				vertext[i] += weight[j]*vertex_data[j][i];
			}
		}
		*data_out = vertext;
	}
}
