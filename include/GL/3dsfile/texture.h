/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#ifndef TEXTURE_H__ 
#define TEXTURE_H__
#include <QtOpenGL>
class CTexture
{
	GLuint	m_nTxt;	
public:
	inline GLint GetTxtID(){return m_nTxt;}
	CTexture()
	{	m_nTxt = -1;	}
	~CTexture()
	{	
		if(m_nTxt!=-1)
		{
			glDeleteTextures(1,&m_nTxt);
		}
	}
	bool MakeTextureBind(char* TextureFileName,bool bLinear,bool bMip);
	bool  MakeScreenTextureBind();
	bool MakeSkinTextureBind(char* TextureFileName,bool bLinear,bool bMip);
public:
	bool loadImage(char *Filename);
	bool LoadTGA(char *filename)	;			// Loads A TGA File Into Memory
};
#endif
