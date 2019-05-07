/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

#include "glwidget.h"
#include <wmacros.h>
#include <QGLWidget>
#include <math.h>
#include <gltoolkit.h>
#include <QSurfaceFormat>
using namespace WSpace;

WGLWidget::WGLWidget(float graphicswidth,float graphicsheight,QWidget *parent)
:WGLBase(parent)
,gl_width_(1000)
,gl_height_(1000)
,graphics_width_(graphicswidth)
,graphics_height_(graphicsheight)
,win_div_gl_(1)
,z_offset_(1000)
{
	initGraphMatrix();
#if 0
	setAutoFillBackground(true);
	setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
#endif
}
WGLWidget::WGLWidget(float w_div_g)
:win_div_gl_(w_div_g)
{
	if(win_div_gl_ <= 0.00001f) {
		win_div_gl_ = 1.0f;
	}
	setKeepWGRatio(true);
	gl_width_ = width()/win_div_gl_;
	gl_height_ = height()/win_div_gl_;
	graphics_width_ = gl_width_;
	graphics_height_ = gl_height_;
	initGraphMatrix();
}
WGLWidget::~WGLWidget()=default;
void WGLWidget::initializeGL()
{
	glClearColor ( 0.0, 0.0, 0.0, 0.0 );
	glMatrixMode(GL_MODELVIEW);
	glEnable (GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH );
	glEnable(GL_COLOR_MATERIAL);//启用颜色材质

	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	initLight();
}
void WGLWidget::setGraphicsSize(float gwidth,float gheight)
{
	if(gwidth<=0 || gheight<=0) {
		ERR("Error size");
		return;
	}
	graphics_width_ = gwidth;
	graphics_height_ = gheight;
	setupViewport(width(),height());
}
void WGLWidget::setupViewport(int width,int height)
{
	const float near = 100.0f;
	const float far  = 2000.0f;

	if((0 == width) || (0 == height)) return;

	z_offset_ = (near+far)/2.0f;
	glViewport (0, 0, (GLsizei) width, (GLsizei) height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ( );

	if(attr_&GLW_KEEP_WG_RATIO) {
		gl_width_         =  width/win_div_gl_;
		gl_height_        =  height/win_div_gl_;
		graphics_width_   =  gl_width_;
		graphics_height_  =  gl_height_;

	} else {
		if(width*graphics_height_ > height*graphics_width_) {
			gl_width_    =   graphics_height_*width/height;
			gl_height_   =   graphics_height_;
		} else {
			gl_width_    =   graphics_width_;
			gl_height_   =   graphics_width_*height/width;
		}
		win_div_gl_ = width/gl_width_;
	}

	left_    =  -gl_width_/2;
	right_   =  gl_width_/2;
	bottom_  =  -gl_height_/2;
	top_     =  gl_height_/2;
	near_    =  near;
	far_     =  far;

	glOrtho(left_,right_,bottom_,top_,near_,far_);

	glMatrixMode ( GL_MODELVIEW );
	glLoadIdentity();
}
void WGLWidget::resizeGL(int width,int height)
{
	setupViewport(width,height);
}
void WGLWidget::initGraphMatrix()
{
	scale_for_coordinate_[0]   =   scale_for_coordinate_[1]     =   scale_for_coordinate_[2]   =   1.0;
	g_trans_matrix             =   GMatrix::identityMatrix();
	g_rotate_matrix            =   GMatrix::identityMatrix();
	g_object_trans_matrix      =   GMatrix::identityMatrix();
}
bool WGLWidget::DC2MC(int x,int y,const GMatrix& trans_matrix,float * xout,float * yout,float* zout)
{
	double tyout;
	double txout;
	double tzout;
	static GLdouble modelview[16];
	static GLdouble projmatrix[16];
	static GLint    viewport[4];

	makeCurrent();

	glPushMatrix();//#0
	glMultMatrixf(trans_matrix.data());
	glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
	glGetDoublev(GL_PROJECTION_MATRIX,projmatrix);
	glGetIntegerv(GL_VIEWPORT,viewport);
	glPopMatrix();//#0

	y=viewport[3]-y;
	GLfloat z = 0;
	if(nullptr != zout)
		glReadPixels(x,y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&z);
	gluUnProject(x,y,z,modelview,projmatrix,viewport,&txout,&tyout,&tzout);
	if(xout) *xout=txout;
	if(yout) *yout=tyout;
	if(zout) *zout=tzout;
	return true;
}
bool WGLWidget::DC2MC(int x,int y,float * xout,float * yout,float* zout)
{
	double tyout;
	double txout;
	double tzout;
	static GLdouble modelview[16];
	static GLdouble projmatrix[16];
	static GLint    viewport[4];

	makeCurrent();

	glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
	glGetDoublev(GL_PROJECTION_MATRIX,projmatrix);
	glGetIntegerv(GL_VIEWPORT,viewport);

	y=viewport[3]-y;
	GLfloat z = 0;
	if(nullptr != zout)
		glReadPixels(x,y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&z);
	gluUnProject(x,y,z,modelview,projmatrix,viewport,&txout,&tyout,&tzout);
	if(xout) *xout=txout;
	if(yout) *yout=tyout;
	if(zout) *zout=tzout;
	return true;
}
bool WGLWidget::MC2DC(float x, float y,float z,float* xout,float* yout,float* zout)
{
	GVector in{x,y,z,1.0f};
	GVector out;

	if(false == MC2DC(in,&out)) return false;

	if(nullptr != xout)*xout = out[0];
	if(nullptr != yout)*yout = out[1];
	if(nullptr != zout)*zout = out[2];

	return true;
}
bool WGLWidget::MC2DC(const GVector& in,GVector* out)
{
	GMatrix model;
	GMatrix proj;
	GVector viewport;

	glGetFloatv(GL_MODELVIEW_MATRIX,model.data());
	glGetFloatv(GL_PROJECTION_MATRIX,proj.data());
	glGetFloatv(GL_VIEWPORT,viewport.data());

	*out = proj*model*in;

	if((*out)[3] == 0.0f) return false;
	for(int i=0; i<3; ++i) {
		(*out)[i] /= (*out)[3];
	}
	(*out)[0] = viewport[0]+(1+(*out)[0])*viewport[2]/2;
	(*out)[1] = height()-(viewport[1]+(1+(*out)[1])*viewport[3]/2);
	(*out)[2] = (1+(*out)[2])/2;
	return true;
}
void WGLWidget::getBasicGLpoint(int x,int y,float *xout,float *yout)
{
	if(xout) {
		*xout=2.0*gl_width_*x/width()-gl_width_;
	}
	if(yout) {
		float glheight=gl_width_*height()/width();
		*yout=glheight-2.0*glheight*y/height();
	}
}
QImage WGLWidget::readGLPixels()
{
	int   i;
	int   j;
	int   index        = 0;
	GLint iViewport[4];

	makeCurrent();
	glGetIntegerv(GL_VIEWPORT,iViewport);

	const int      imgWidth  = iViewport[2];
	const int      imgHeight = iViewport[3];
	const int      linewidth = (imgWidth+3) &(~3);
	unsigned char *pdata     = new unsigned char[(imgWidth+1) *imgHeight *3];
	QImage image(QSize(imgWidth,imgHeight),QImage::Format_RGB888);

	glReadBuffer(GL_FRONT);
	glReadPixels(0,0,imgWidth,imgHeight,GL_RGB,GL_UNSIGNED_BYTE,pdata);
	for(i=imgHeight-1; i>=0; --i) {
		for(j=0; j<imgWidth; ++j) {
			image.setPixel(j,i,QColor(pdata[index],pdata[index+1],pdata[index+2]).rgb());
			index+=3;
		}
		index=linewidth*(imgHeight-i)*3;
	}
	if(nullptr != pdata) { delete[]pdata; pdata=nullptr; }
	return image;
}
QPixmap WGLWidget::renderAPicture(int width,int height)
{
	//return renderPixmap(width,height);
}
/*
 * 忽略所有名字为0的物体
 */
bool WGLWidget::processSelection(int xPos, int yPos,std::vector<GLuint>* selectnames,int pwidth,int pheight)
{
	static GLuint selectBuff[GLW_SELECT_BUFFER_LENGTH];

	makeCurrent();
	/*c++ gui with qt4 20章,P356*/
	GLint hits, viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(GLW_SELECT_BUFFER_LENGTH, selectBuff);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);//@0

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//#0
	glLoadIdentity();
	gluPickMatrix(xPos, GLdouble(viewport[3] - yPos), pwidth,pheight, viewport);
	glOrtho(left_,right_,bottom_,top_,near_,far_);
	paintSelectModeGL();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();//#0 end

	hits = glRenderMode(GL_RENDER);
	glMatrixMode(GL_MODELVIEW);
	if(hits > 0) {
		double minz = selectBuff[1];
		selectnames->clear();
		for(int i=0; i<hits; i++) { 
			if(0 == selectBuff[3+4*i]) continue;
			if (selectBuff[1+i*4] < minz) {
				minz=selectBuff[1+i*4];
				selectnames->insert(selectnames->begin(),selectBuff[3+4*i]);
			} else {
				selectnames->push_back(selectBuff[3+4*i]);
			}
		}
		return !selectnames->empty();
	}
	return false;
}
void WGLWidget::mousePressEvent(QMouseEvent* event)
{
	if(event->button()==Qt::LeftButton) {
		x1_   =   event->x();
		y1_   =   event->y();
		x2_   =   x1_;
		y2_   =   y1_;
		flag_|=GLW_FLAG_LBUTTONDOWN;
		setCursor(Qt::ClosedHandCursor);
	} else {
		WGLBase::mousePressEvent(event);
	}
}
void WGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if(flag_&GLW_FLAG_LBUTTONDOWN) {
		x1_=y1_=x2_=y2_=0;
		flag_&=(~GLW_FLAG_LBUTTONDOWN);
		setCursor(Qt::ArrowCursor);
		return;
	}
	return WGLBase::mouseReleaseEvent(event);
}
void WGLWidget::mouseMoveEvent(QMouseEvent* event)
{
	if(flag_&GLW_FLAG_LBUTTONDOWN) {
		x2_              =   event->x();
		y2_              =   event->y();
		move(x2_-x1_,y2_-y1_);
		x1_              =   x2_;
		y1_              =   y2_;
	}
	WGLBase::mouseMoveEvent(event);
}
void WGLWidget::wheelEvent(QWheelEvent* event)
{
	float k  = 1.1;

	if(event->delta()<0) k=1.0/k;
	zoom(k);
}
void WGLWidget::zoom(float x, float y,float k)
{
	float gx;
	float gy;

	scale_for_coordinate_[0] *= k;
	scale_for_coordinate_[1] *= k;
	//scale_for_coordinate_[2] *= k;
	DC2MC(x,y,g_trans_matrix,&gx,&gy,nullptr);

	g_trans_matrix = g_trans_matrix*GMatrix::translateMatrix(gx,gy,0)*GMatrix::scaleMatrix(k,k,1)*GMatrix::translateMatrix(-gx,-gy,0);
	update();
}
void WGLWidget::zoom(float k)
{
	scale_for_coordinate_[0] *= k;
	scale_for_coordinate_[1] *= k;
	scale_for_coordinate_[2] *= k;

	g_trans_matrix = g_trans_matrix*GMatrix::scaleMatrix(k,k,k);
	update();
}
void WGLWidget::move(float x, float y)
{
	g_trans_matrix   =   GMatrix::translateMatrix(x/win_div_gl_,-y/win_div_gl_,0)*g_trans_matrix;
	update();
}
void WGLWidget::drawGradualBackground()
{
	const float z = -49.0f*graphics_width_;
	glNormal3f(1,-1,-1);
	glBegin(GL_QUADS);
	glColor3f(0.77,0.77,0.914);
	glVertex3f(-gl_width_,gl_height_,z);
	glColor3f(0.878,0.898,0.976);
	glVertex3f(gl_width_,gl_height_,z);
	glColor3f(0.867,0.878,0.937);
	glVertex3f(gl_width_,-gl_height_,z);
	glColor3f(0.8,0.812,0.906);
	glVertex3f(-gl_width_,-gl_height_,z);
	glEnd();
}
void WGLWidget::initLight()
{
	glDisable(GL_LIGHTING);
}
void WGLWidget::drawRVText(float x,float y,float z,const QString& text)
{
	QImage text_img(fontMetrics().width(text)+3,fontMetrics().height()+2,QImage::Format_ARGB32);
	QPainter painter;

	text_img.fill(0X00000000);
	painter.begin(&text_img);
	painter.setFont(font());
	painter.drawText(1,fontMetrics().height()+1,text);
	painter.end();
	text_img= text_img.transformed(QTransform().rotate(90.0f));
	text_img = QGLWidget::convertToGLFormat(text_img);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glRasterPos3f(x,y-0.5f*text_img.height()/win_div_gl_,z);
	glDrawPixels(text_img.width(),text_img.height(),GL_RGBA,GL_UNSIGNED_BYTE,text_img.bits());
	glDisable(GL_BLEND);
}
void WGLWidget::drawLVText(float x,float y,float z,const QString& text)
{
	QImage text_img(fontMetrics().width(text)+2,fontMetrics().height()+3,QImage::Format_ARGB32);
	QPainter painter;

	text_img.fill(0X00000000);
	painter.begin(&text_img);
	painter.setFont(font());
	painter.drawText(1,fontMetrics().height(),text);
	painter.end();
	text_img= text_img.transformed(QTransform().rotate(-90.0f));
	text_img = QGLWidget::convertToGLFormat(text_img);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glRasterPos3f(x,y-0.5f*text_img.height()/win_div_gl_,z);
	glDrawPixels(text_img.width(),text_img.height(),GL_RGBA,GL_UNSIGNED_BYTE,text_img.bits());
	glDisable(GL_BLEND);
}
void WGLWidget::drawHText(float x,float y,float z,const QString& text)
{
	float    color[4]               = {0,0,0};
	QImage   text_img(fontMetrics().width(text)+2,fontMetrics().height()+3,QImage::Format_ARGB32);
	QPainter painter;

	glGetFloatv(GL_CURRENT_COLOR,color);

	text_img.fill(0X00000000);

	painter.begin(&text_img);
	painter.setPen(QColor(color[0]*255,color[1]*255,color[2]*255));
	painter.setFont(font());
	painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);

	painter.drawText(1,fontMetrics().height(),text);

	painter.end();

	text_img = QGLWidget::convertToGLFormat(text_img);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glRasterPos3f(x,y-2.0f/win_div_gl_,z);
	glDrawPixels(text_img.width(),text_img.height(),GL_RGBA,GL_UNSIGNED_BYTE,text_img.bits());
	glDisable(GL_BLEND);
}
void WGLWidget::updateGL()
{
	update();
}
void WGLWidget::renderText(double x,double y,double z,const QString& text,const QFont& font,unsigned align)
{
#if 0
	QFontMetrics font_metrics(font);

	if(align&Qt::AlignHCenter) {
		x -= font_metrics.width(text)/(2*win_div_gl_);
	}
	if(align&Qt::AlignRight) {
		x -= font_metrics.width(text)/win_div_gl_;
	}
	if(align&Qt::AlignVCenter) {
		y -= font_metrics.height()/(2*win_div_gl_);
	}
	if(align&Qt::AlignTop) {
		y += font_metrics.height()/win_div_gl_;
	}

	drawHText(x,y,z,text);
#endif
#if 1
	float color[4] = {0,0,0};
	float tx       = 0;
	float ty       = 0;

	if(!MC2DC(x,y,z,&tx,&ty)) return;

	glDisable(GL_DEPTH_TEST);
	glGetFloatv(GL_CURRENT_COLOR,color);

	QPainter     painter;
	QFontMetrics font_metrics(font);

	painter.begin(this);
	painter.setPen(QColor(color[0]*255,color[1]*255,color[2]*255));
	painter.setFont(font);
	painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);

	if(align&Qt::AlignHCenter) {
		tx -= font_metrics.width(text)/2;
	}
	if(align&Qt::AlignRight) {
		tx -= font_metrics.width(text);
	}
	if(align&Qt::AlignVCenter) {
		ty -= font_metrics.height()/2;
	}
	if(align&Qt::AlignTop) {
		ty += font_metrics.height();
	}

	painter.drawText(tx,ty,text);
	painter.end();
	glEnable(GL_DEPTH_TEST);
#endif
}
void WGLWidget::paintSelectModeGL()
{
}
