/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "opengltext.h"
#include <vector>
#include <string>
#include <wmacros.h>
#include <gmatrix.h>
using namespace std;
using namespace WSpace;
inline int next_p2 (int a ) {
	int rval=1;
	while(rval<a) rval<<=1;
	return rval;
}
std::unique_ptr<OpenglText> OpenglText::createText(const char* font_path,unsigned int text_height)
{
	std::unique_ptr<OpenglText> ret_val(new OpenglText());
	if(ret_val->init(font_path,text_height)) {
		return ret_val;
	}
	ret_val.reset();
	return ret_val;
}
OpenglText::OpenglText() 
:_textures_array(nullptr)
,_text_width(nullptr)
{
}
bool OpenglText::init(const char * font_path, unsigned int text_height) 
{
	int i;
	// 保存纹理ID.
	_textures_array      =  new GLuint[128];
	_text_width     =  new GLuint[128];
	this->_text_height  =  text_height;

	// 创建FreeType库
	FT_Library library;
	if (FT_Init_FreeType( &library )) {
		ERR("FT_Init_FreeType failed");
		goto faild0;
	}

	// 在FreeType库中保存字体信息的类叫做face
	FT_Face face;

	// 使用你输入的Freetype字符文件初始化face类
	if (FT_New_Face( library, font_path, 0, &face )) {
		ERR("FT_New_Face failed (there is probably a problem with your font file)");
		goto faild1;
	}

	// 在FreeType中使用1/64作为一个像素的高度所以我们需要缩放h来满足这个要求
	FT_Set_Char_Size( face, _text_height << 6, _text_height << 6, 96, 96);

	// 创建128个显示列表
	list_base = glGenLists(128);
	glGenTextures(128, _textures_array );
	for(i=0; i<128; ++i)
		make_dlist(face,i,list_base,_textures_array,_text_width+i);

	// 释放face类
	FT_Done_Face(face);

	// 释放FreeType库
	FT_Done_FreeType(library);
	return true;
faild1:
	FT_Done_FreeType(library);
faild0:
	if(nullptr != _textures_array){delete[] _textures_array; _textures_array=nullptr; }
	if(nullptr != _text_width){delete[] _text_width; _text_width=nullptr; }
	return false;
}
// 为给定的字符创建一个显示列表
void OpenglText::make_dlist ( FT_Face face, char ch, GLuint list_base, GLuint * tex_base,GLuint* textwidth) {
	// 载入给定字符的轮廓
	if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT ))
		ERR("FT_Load_Glyph failed");

	// 保存轮廓对象
	FT_Glyph glyph;
	if(FT_Get_Glyph( face->glyph, &glyph ))
		ERR("FT_Get_Glyph failed");

	// 把轮廓转化为位图
	FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

	// 保存位图
	FT_Bitmap& bitmap=bitmap_glyph->bitmap;
	/**********************************************************************/
	// 转化为OpenGl可以使用的大小
	const int width  = next_p2( bitmap.width );
	const int height = next_p2( bitmap.rows );
	// 保存位图数据
	GLubyte* expanded_data = new GLubyte[ 2 * width * height];

	// 这里我们使用8位表示亮度8位表示alpha值
	for(int j=0; j <height;j++) {
		for(int i=0; i < width; i++){
			expanded_data[2*(i+j*width)] = 
				expanded_data[2*(i+j*width)+1] = 
				(i>=bitmap.width || j>=bitmap.rows) ?
				0 : bitmap.buffer[i + bitmap.width*j];
		}
	}
	// 设置字体纹理的纹理过滤器
	glBindTexture( GL_TEXTURE_2D, tex_base[ch]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	// 邦定纹理
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
			GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data );


	// 创建显示列表
	glNewList(list_base+ch,GL_COMPILE);//#0
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.5f);
	glBindTexture(GL_TEXTURE_2D,tex_base[ch]);

	//首先我们向左移动一点
	glTranslatef(bitmap_glyph->left,0,0);

	//接着我们向下移动一点，这只队'g','y'之类的字符有用
	//它使得所有的字符都有一个基线
	glPushMatrix();//&0
	glTranslatef(0,bitmap_glyph->top-bitmap.rows,0);

	// 计算位图中字符图像的宽度
	const float x = (float)bitmap.width / (float)width;
	const float y = (float)bitmap.rows / (float)height;

	//绘制一个正方形，显示字符
	glBegin(GL_QUADS);
	glTexCoord2d(0,0); glVertex2f(0,bitmap.rows);
	glTexCoord2d(0,y); glVertex2f(0,0);
	glTexCoord2d(x,y); glVertex2f(bitmap.width,0);
	glTexCoord2d(x,0); glVertex2f(bitmap.width,bitmap.rows);
	glEnd();
	glPopMatrix();//&0
	glTranslatef(face->glyph->advance.x >> 6 ,0,0);
	*textwidth = face->glyph->advance.x>>6;

	glDisable(GL_ALPHA_TEST);
	glEndList();//#0
	if(nullptr != expanded_data){delete [] expanded_data;expanded_data=nullptr;}

}
GLuint OpenglText::getTextWidth(const char* fmt,...)const 
{
	va_list     ap;
	char        text[256];
	const char *p         = text;
	GLuint      width     = 0;
	if (fmt == nullptr) {
		return 0;
	} else {
		va_start(ap, fmt); 
		vsprintf(text, fmt, ap); 
		va_end(ap); 
	}

	while(*p != 0) {
		width += _text_width[*p];
		++p;
	}
	return (width+_text_width['0'])*0.63f;
}
OpenglText::~OpenglText() {
	glDeleteLists(list_base,128);
	glDeleteTextures(128,_textures_array);
	if(nullptr != _textures_array){delete [] _textures_array;_textures_array=nullptr;}
	if(nullptr != _text_width){delete[] _text_width; _text_width=nullptr;}
}
bool OpenglText::drawText(const float* trans_matrix, const char* text)const 
{
	GLuint          font       = list_base;
	float           height     = _text_height/0.63f;
	const char     *start_line = text;
	const char     *c;
	vector<string>  lines;

	for(c=text; *c; c++) {
		if(*c=='\n') {
			string line;
			for(const char *n=start_line; n<c; n++) line.append(1,*n);
			lines.push_back(line);
			start_line=c+1;
		}
	}

	if(start_line) {
		string line;
		for(const char *n=start_line;n<c;n++) line.append(1,*n);
		lines.push_back(line);
	}

	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT); 
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); //使用调整模式

	glListBase(font);
	float modelview_matrix[16];     
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	for(unsigned int i=0;i<lines.size();i++) {
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(0.0f,-height*i,0.0f);
		glMultMatrixf(modelview_matrix);
		glMultMatrixf(trans_matrix);

		glCallLists(lines[i].length(), GL_UNSIGNED_BYTE, lines[i].c_str());

		glPopMatrix();
	}

	glPopAttrib();

	return true;
}
void OpenglText::drawXYText(float x,float y,float z,const char* fmt,...)const {
	char                 text[256];
	va_list              ap;
	const GMatrix matrix    = GMatrix::translateMatrix(x,y,z);
	if (fmt == nullptr) {
		return;
	} else {
		va_start(ap, fmt); 
		vsprintf(text, fmt, ap); 
		va_end(ap); 
	}
	drawText(matrix.data(),text);
}
void OpenglText::drawXZText(float x,float y,float z,TextDirection direction,const char* fmt,...)const {
	char    text[256];
	va_list ap;
	GMatrix matrix;

	if (fmt == nullptr) {
		return ;
	} else {
		va_start(ap, fmt); 
		vsprintf(text, fmt, ap); 
		va_end(ap); 
	}

	switch(direction) {
		case TD_X_POSITIVE:
			matrix = GMatrix::translateMatrix(x+_text_width['0'],y,z) *GMatrix::rotatexMatrix(-90.0f);
			break;
		case TD_Z_POSITIVE:
			matrix = GMatrix::translateMatrix(x,y,z+getTextWidth(text)) *GMatrix::rotatezMatrix(90.0f)*GMatrix::rotateyMatrix(90.0f);
			break;
		default:
			matrix = GMatrix::translateMatrix(x,y,z) *GMatrix::rotatexMatrix(-90.0f);
			break;
	}
	drawText(matrix.data(),text);
}
void OpenglText::drawYZText(float x,float y,float z,const char* fmt,...)const {
	char    text[256];
	va_list ap;
	if (fmt == nullptr) {
		return ;
	} else {
		va_start(ap, fmt); 
		vsprintf(text, fmt, ap); 
		va_end(ap); 
	}
	const GLuint textwidth   =   getTextWidth(text);
	const GMatrix matrix     =   GMatrix::translateMatrix(x,y,z+textwidth) *GMatrix::rotateyMatrix(-90.0f)*GMatrix::mirrorxMatrix();
	drawText(matrix.data(),text);
}
