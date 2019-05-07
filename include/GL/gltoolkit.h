/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QtGui>
#include <GL/glu.h>
#include <GL/gl.h>
#include <string>
#include <wgeometry.h>
#include <toolkit.h>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/accumulators.hpp>
#include "gl_fwd.h"
#include "gmatrix.h"

namespace WSpace
{
	void  drawCross(float x,float y,float z,float size=8);
	void drawArrow(float xpos,float ypos,int nIndex,float size=8);
	void drawCylinder(float xpos,float ypos,float zpos,float radius,float height,int quad_num=100);
	/*
	 * 在x-y平面画圆
	 */
	void drawCircle(float x0,float y0,float z0,float radius,unsigned short num=100,GLenum mode = GL_LINE_LOOP);
	/*
	 * 绘制一个简单的只有外环的凹多边形
	 * @data:依次为x,y,z的点集
	 * @size:点的数量(data.size/3)
	 */
	void drawPolygon(double* data,size_t size);
	/*
	 * 在X-Y平面生成一段圆弧
	 * @x0,y0,z0:圆弧的圆心坐标
	 * @radius:圆弧半径
	 * @begangle:圆弧起始角度[0-360)
	 * @endangle:圆弧结束角度(0-360]
	 * @num:拟合点数(如果等于零,num=endangle-begangle)
	 * @pdata:输出数据x-y-z,依次排列(预先分配内存)
	 */
	void getArcPoints3f(float x0,float y0,float z0,float radius,float begangle,float endangle,int num,float*pdata);
	//通过线性比例关系确定颜色
	bool getColor(float* color,float fz,float fmin=-1,float fmax=-2,int flag=GLW_COLORFLAG_OCEAN);
	/*
	 * 将QColor转换为glcolor
	 */
	bool toGLColor3f(const QColor& i_color,float* color);
	bool toGLColor4f(const QColor& i_color,float* color);
	/*
	 * 将glcolor转换为QColor
	 */
	bool toQColor3f(const float* i_color,QColor* color);
	bool toQColor4f(const float* i_color,QColor* color);
	/*
	 * 将字符串转换为glcolor
	 */
	bool toGLColor3f(const std::string& str,float* color);
	bool toGLColor4f(const std::string& str,float* color);
	/*
	 * 从低位到高位依次为b,g,r,a
	 */
	bool toGLColor3f(uint32_t di,float* color);
	bool toGLColor4f(uint32_t di,float* color);
	/*
	 * 将glcolor转换为字符串
	 */
	std::string glColorToString3f(const float* color);
	std::string glColorToString4f(const float* color);

	typedef void (*WTESS_CALLBACK_FUNC )();
	void defaultTessError(GLenum error);
	void defaultTessCombineCallback(GLdouble coords[3],GLdouble* vertex_data[4],GLfloat weight[4],GLdouble** data_out);
	/*
	 * 根据[begin,end)中数据的距离判断[begin,end)中包含多少个不同的图形
	 */
	template<typename Container>
		void showMultiPolygons(Container&& data, GLuint type) 
		{
			std::list<std::decay_t<Container>> datas;
			pointsClusteringByDistance(data,&datas);
			for(auto& d:datas) {
				glBegin(type);
				for(auto& p:d) {
					glVertex2f(std::get<0>(p),std::get<1>(p));
				}
				glEnd();
			}
		}
};
