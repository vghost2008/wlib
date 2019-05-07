/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <list>
#include <math.h>
#include <QtCore>
#include <QtGui>
#include <QOpenGLWidget>
#include <gmatrix.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include "gl_fwd.h"

#define       GLW_SELECT_BUFFER_LENGTH       100
#define       GLW_DELTA                      8.0f
enum GLWFlag {
    GLW_FLAG_              =   0xf   ,   
    GLW_FLAG_LBUTTONDOWN   =   0x1   ,   
    GLW_FLAG_TRANSLATE     =   0x2   ,   
    GLW_FLAG_ROTATE        =   0x4   ,   
    GLW_FLAG_PICK          =   0x8   ,   
};
enum {
    GLW_UPDATE     =   true    ,   
    GLW_UNUPDATE   =   false   ,   
};
enum {
	GLW_KEEP_WG_RATIO = 0x00000001,
};
using WGLBase=QOpenGLWidget;
class WGLWidget:public WGLBase
{
	public:
		WGLWidget(float graphicswidth,float graphicsheight,QWidget* parent=nullptr);
		WGLWidget(float w_div_gl=1.0f);
		~WGLWidget();
		void setGraphicsSize(float width,float height);
	public:
		//openGL初始化函数
		void initializeGL()override;
		//窗口大小改变时的响应函数
		void resizeGL(int width,int height)override;
		//设置视点
		void setupViewport(int,int);
		virtual void initLight();
		void setKeepWGRatio(bool keep) {
			if(keep)
				attr_ |= GLW_KEEP_WG_RATIO;
			else
				attr_ &= (~GLW_KEEP_WG_RATIO);
		}
		/*
		 * x,y,z模型坐标
		 */
		void drawLVText(float x,float y,float z,const QString& text);
		void drawRVText(float x,float y,float z,const QString& text);
		void drawHText(float x,float y,float z,const QString& text);
		inline bool isMousePressed()const { return flag_&GLW_FLAG_LBUTTONDOWN; }
	public:
		//图形变换矩阵(用来保存图形的变换操作)
		WSpace::GMatrix g_trans_matrix;
		/*
		 * 用于需要总是作用于物体的旋转操作
		 */
		WSpace::GMatrix g_rotate_matrix;
		/*
		 * 针对物体本身的变换
		 */
		WSpace::GMatrix g_object_trans_matrix;
		void initGraphMatrix();
	public:
		QImage readGLPixels();
		QPixmap renderAPicture(int width=1280,int height=800);
		/*
		 * @xPos,yPos:窗口坐标
		 * 忽略名字为0的物体
		 * @selectnames:返回的名字
		 * @pwidth,pheight:选择窗口大小
		 */
		bool processSelection(int xPos, int yPos,std::vector<GLuint>* selectnames,int pwidth=2,int pheight=2);
		template<class Iter>
			bool processSelection(int xpos,int ypos,Iter begin,Iter end,int pwidth=2,int pheight=2)
			{
				std::vector<GLuint> selectnames;
				auto                max_size    = std::distance(begin,end);
				auto                res         = processSelection(xpos,ypos, &selectnames,pwidth,pheight);
				auto                iter        = begin;

				if(false == res) return res;

				for(auto it=selectnames.begin(); it!=selectnames.end()&&iter!=end; ++it,++iter)
					*iter = *it;
				return res;
			}
	public:
		//获取没有图形变换的opengl点坐标
		void getBasicGLpoint(int x,int y,float* xount,float* yout);
		/*
		 * 将设备坐标转换为模型坐标
		 * 模型坐标经过trans_matrix变换再进行后继的变换，相当于模型矩阵M = M*trans_matrix
		 */
		bool DC2MC(int x,int y,const WSpace::GMatrix& trans_matrix,float * xout,float * yout,float* zout=0);
		/*
		 * 将设备坐标转换为模型坐标
		 */
		bool DC2MC(int x,int y,float * xout,float * yout,float* zout=0);
		/*
		 * 将模型坐标转换为设备坐标
		 * zout:为深度测试用
		 */
		bool MC2DC(float x, float y,float z,float* xout,float* yout=nullptr,float* zout=nullptr);
		bool MC2DC(const WSpace::GVector& in,WSpace::GVector* out);
		inline float win_div_gl()const { return win_div_gl_; }
		inline unsigned flag()const { return flag_; }
	public:
		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void wheelEvent(QWheelEvent* event);
		/*
		 * 以x,y（屏幕坐标)为中心进行放缩
		 */
		void zoom(float x, float y,float k);
		/*
		 * 以三维坐标零点为中心进行放缩
		 */
		void zoom(float k);
		/*
		 * 移动
		 * x,y为屏幕坐标
		 */
		void move(float x, float y);
		inline float z_offset()const { return z_offset_; }

		void updateGL();
		/*
		 * x,y,z模型坐标
		 */
		void renderText(double x,double y,double z,const QString& str,const QFont& font=QFont(),unsigned align=Qt::AlignHCenter);
	protected:
		/*
		 * 仅需绘制有名字的物体
		 * 操作与paintGL类似
		 * 通过glLoadName为物体命名
		 */
		virtual void paintSelectModeGL();
	protected:
		/*
		 * openGL窗口的宽与高
		 * 显示区域为[-gl_width_/2,gl_width_/2];[-gl_height_/2,gl_height_/2]
		 */
		float gl_width_;
		float gl_height_;
		/*
		 * 一般与gl_width_ gl_height_相同
		 * 有时候用于保持一定的窗口比例
		 */
		float graphics_width_;
		float graphics_height_;
		/*
		 * 视投影矩阵中心点距视点的距离，>0
		 */
		float z_offset_;
	protected:
		float    scale_for_coordinate_[3] = {1.0f,1.0f,1.0f};
		float    x1_;
		float    y1_;
		float    x2_;
		float    y2_;
	private:
		unsigned flag_ = 0;
		unsigned attr_ = 0;
		/*从真实数据放缩为openGL坐标的放缩大小*/
		float scale_of_coord_=1.0f;
		//窗口坐标与openGL坐标的比值
		float win_div_gl_=1.0f;
	private:
		GLdouble left_   = 0;
		GLdouble right_  = 0;
		GLdouble bottom_ = 0;
		GLdouble top_    = 0;
		GLdouble near_   = 0;
		GLdouble far_    = 0;
	public:
		void drawGradualBackground();
};
