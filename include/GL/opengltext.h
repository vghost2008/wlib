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

class OpenglText {
	public:
		enum TextDirection {
			TD_X_POSITIVE  ,  
			TD_Y_POSITIVE  ,  
			TD_Z_POSITIVE  ,  
			TD_X_NEGATIVE  ,  
			TD_Y_NEGATIVE  ,  
			TD_Z_NEGATIVE  ,  
		};
		static std::unique_ptr<OpenglText> createText(const char* font_path,unsigned int text_height);
		GLuint getTextWidth(const char* fmt,...)const;
		void drawXYText(float x,float y,float z,const char* fmt,...)const;
		void drawXZText(float x,float y,float z,TextDirection direction,const char* fmt,...)const;
		void drawYZText(float x,float y,float z,const char* fmt,...)const;
		inline unsigned int textHeight()const {
			return _text_height;
		}
		~OpenglText();
	private:
		unsigned int  _text_height;
		GLuint       *_textures_array;
		GLuint       *_text_width;
		GLuint        list_base;
	private:
		bool drawText(const float* trans_matrix,const char* text)const;
		void make_dlist (FT_Face face, char ch, GLuint list_base, GLuint * tex_base,GLuint* textwidth);
		bool init(const char* font_path,unsigned int text_height);
		OpenglText();
};
