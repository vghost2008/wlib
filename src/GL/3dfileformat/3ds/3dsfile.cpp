/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include "3dsfile.h"
#include <gltoolkit.h>
#ifdef Q_OS_MAC
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#endif
#include <QtOpenGL>
#include <QOpenGLWidget>
#include <stdio.h>
#include <stdlib.h>
#include <wmath.h>
#include "basereader.h"
#include "chunkdefine.h"

#define  FLT_MAX 3.402823466e+38F 
#define  FLT_MIN 1.175494351e-38F 
using namespace std;
using namespace WSpace;

static_assert(std::is_same<GVector::value_type,float>::value,"error type");

W3DSFile::W3DSFile()
{
}

W3DSFile::~W3DSFile()
{
	releaseMem();
}
void W3DSFile::releaseMem()
{
	for(int i = 0;i<m_model.vObject.size();i++)
	{
		if(m_model.vObject[i].pVerts) {
			delete []m_model.vObject[i].pVerts;
			m_model.vObject[i].pVerts = 0;
		}
		if(m_model.vObject[i].pTexVerts) {
			delete []m_model.vObject[i].pTexVerts;
			m_model.vObject[i].pTexVerts=0;
		}
		if(m_model.vObject[i].pFaces) {
			delete []m_model.vObject[i].pFaces;
			m_model.vObject[i].pFaces = 0;
		}
	}
	m_model.vObject.clear();
	m_model.vMaterials.clear();
}
void ReadString(FILE *file,char *str)
{
	int i=0;char l_char;
	do
	{
		fread (&l_char, 1, 1, file);
        str[i]=l_char;
		i++;
    }while(l_char != '\0' && i<256);
}
bool W3DSFile::load3DS ( char *p_filename)
{
	releaseMem();
	t3DModel       *pModel         = &m_model;
	int             i;                         //Index variable
	FILE           *l_file;                    //File pointer
	t3DObject      *pObj;
	unsigned short  l_chunk_id;                //Chunk identifier
	unsigned int    l_chunk_lenght;            //Chunk lenght
	unsigned short  l_qty;                     //Number of elements in each chunk
	unsigned short  l_face_flags;              //Flag that stores some face information
	int             filelength     = 0;
	int             chunkstart     = 0;

	if ((l_file=fopen (p_filename, "rb"))== NULL) return false; //Open the file

	fseek(l_file,0,SEEK_END);
	filelength = ftell(l_file);
	fseek(l_file,0,SEEK_SET);
	
	while (ftell (l_file) < filelength) //Loop to scan the whole file 
	{
		fread (&l_chunk_id, 2, 1, l_file); //Read the chunk header
		fread (&l_chunk_lenght, 4, 1, l_file); //Read the lenght of the chunk
		switch (l_chunk_id)
		{
			//----------------- MAIN3DS -----------------
			// Description: Main chunk, contains all the other chunks
			// Chunk ID: 4d4d 
			// Chunk Lenght: 0 + sub chunks
			//-------------------------------------------
			case 0x4d4d: 
				break;    

				//----------------- EDIT3DS -----------------
				// Description: 3D Editor chunk, objects layout info 
				// Chunk ID: 3d3d (hex)
				// Chunk Lenght: 0 + sub chunks
				//-------------------------------------------
			case 0x3d3d:
				break;

				//--------------- EDIT_OBJECT ---------------
				// Description: Object block, info for each object
				// Chunk ID: 4000 (hex)
				// Chunk Lenght: len(object name) + sub chunks
				//-------------------------------------------
			case 0x4000: 
				{
					t3DObject obj;
					obj.numOfFaces = 0;
					obj.numOfVerts = 0;
					obj.numTexVertex = 0;
					obj.pFaces = 0;
					obj.pTexVerts = 0;
					obj.pVerts = 0;
					//obj.pNormals = 0;
					//	memset(&obj,0,sizeof(t3DObject));
					ReadString(l_file,obj.strName);
					pModel->vObject.push_back(obj);

				}
				break;

				//--------------- OBJ_TRIMESH ---------------
				// Description: Triangular mesh, contains chunks for 3d mesh info
				// Chunk ID: 4100 (hex)
				// Chunk Lenght: 0 + sub chunks
				//-------------------------------------------
			case 0x4100:
				break;

				//--------------- TRI_VERTEXL ---------------
				// Description: Vertices list
				// Chunk ID: 4110 (hex)
				// Chunk Lenght: 1 x unsigned short (number of vertices) 
				//             + 3 x float (vertex coordinates) x (number of vertices)
				//             + sub chunks
				//-------------------------------------------
			case 0x4110: 
			{
				float ftemp[3];
				fread (&l_qty, sizeof (unsigned short), 1, l_file);
				pObj = &pModel->vObject[pModel->vObject.size()-1];
				pObj->numOfVerts = l_qty;
				pObj->pVerts = new tVector3[l_qty];
				for (i=0; i<l_qty; i++) {
					fread(ftemp,sizeof(float),3,l_file);
					auto& v = pObj->pVerts[i];
					v[0] = -ftemp[0]; //将x反向
					v[1] = ftemp[1];
					v[2] = ftemp[2];
				}
				break;
			}

				//--------------- TRI_FACEL1 ----------------
				// Description: Polygons (faces) list
				// Chunk ID: 4120 (hex)
				// Chunk Lenght: 1 x unsigned short (number of polygons) 
				//             + 3 x unsigned short (polygon points) x (number of polygons)
				//             + sub chunks
				//-------------------------------------------
			case 0x4120://face
				fread (&l_qty, sizeof (unsigned short), 1, l_file);
				pObj = &pModel->vObject[pModel->vObject.size()-1];
				pObj->numOfFaces = l_qty;
				pObj->pFaces = new tFace[l_qty];
				//	memset(pObj->pFaces,0,sizeof(tFace)*l_qty);
				for (i=0; i<l_qty; i++) {
					fread (&pObj->pFaces[i].a, sizeof (unsigned short), 1, l_file);
					fread (&pObj->pFaces[i].b, sizeof (unsigned short), 1, l_file);
					fread (&pObj->pFaces[i].c, sizeof (unsigned short), 1, l_file);
					fread (&l_face_flags, sizeof (unsigned short), 1, l_file);
					pObj->pFaces[i].matIndex = -1;
				}
				break;
			case 0x4130:// This is found if the object has a material, either texture map or color
				{
					char strMaterial[256];
					ReadString(l_file,strMaterial);
					unsigned short count,nf;
					fread (&count, sizeof(short), 1, l_file);

					pObj = &pModel->vObject[pModel->vObject.size()-1];
					// Go through all of the textures
					for( i = 0; i < m_model.vMaterials.size(); i++) {
						// If the material we just read in matches the current texture name
						if(strcmp(strMaterial, m_model.vMaterials[i].strName) == 0) {
							while (count-- > 0) {	// Material number assigned to any face
								// Numero di material assegnato ad ogni faccia
								fread (&nf, sizeof(short), 1, l_file);
								pObj->pFaces[nf].matIndex = i;
							}
							// Now that we found the material, check if it's a texture map.
							// If the strFile has a string length of 1 and over it's a texture
							if(strlen(pModel->vMaterials[i].strFile) > 0) {
								// Set the object's flag to say it has a texture map to bind.
								//	pObj->bHasTexture = true;
							}
							break;
						}
					}
					break;
				}
				//------------- TRI_MAPPINGCOORS ------------
				// Description: Vertices list
				// Chunk ID: 4140 (hex)
				// Chunk Lenght: 1 x unsigned short (number of mapping points) 
				//             + 2 x float (mapping coordinates) x (number of mapping points)
				//             + sub chunks
				//-------------------------------------------
			case 0x4140:
				fread (&l_qty, sizeof (unsigned short), 1, l_file);
				pObj = &pModel->vObject[pModel->vObject.size()-1];
				pObj->pTexVerts = new tVector2[l_qty];

				for (i=0; i<l_qty; i++) {
					fread (&pObj->pTexVerts[i].x, sizeof (float), 1, l_file);
					fread (&pObj->pTexVerts[i].y, sizeof (float), 1, l_file);
				}
				break;
			case 0xAFFF://MATERIAL:
			/*
				{
					tMaterialInfo newTexture ;//= {0};
					newTexture.strFile[0] = '\0';
					newTexture.strName[0] = '\0';
					m_model.vMaterials.push_back(newTexture);
				}
				break;
				*/
				if (!ReadMatEntry(filelength, chunkstart, l_chunk_lenght, l_file)) 
					return false;
				break;
			case 0xA300: //// This holds the file name of the texture
				{
					tMaterialInfo &matInfo = m_model.vMaterials[m_model.vMaterials.size()-1];
					ReadString(l_file,matInfo.strFile);
					break;
				}
			case 0xA200:	//texture map flag//	// This is the header for the texture info
				break;
				//----------- Skip unknow chunks ------------
				//We need to skip all the chunks that currently we don't use
				//We use the chunk lenght information to set the file pointer
				//to the same level next chunk
				//-------------------------------------------
			default:
				fseek(l_file, l_chunk_lenght-6, SEEK_CUR);
		} 
		chunkstart = ftell(l_file);
	}
	fclose (l_file); // Closes the file stream
	return true; // Returns ok
}
bool W3DSFile::ReadMatEntry(int fileSize,int matStart,int matSize,FILE* l_file) 
{
	tMaterialInfo newTexture ;//= {0};
	newTexture.strFile[0] = '\0';
	newTexture.strName[0] = '\0';
	m_model.vMaterials.push_back(newTexture);

	unsigned short l_chunk_id; //Chunk identifier
	int l_chunk_lenght; //Chunk lenght
	//
	int			chunkstart = ftell(l_file);
	float			red, green, blue;
	float			percentage;

	while (chunkstart < matStart + matSize &&
			Read3DSChunk(l_file, l_chunk_id,l_chunk_lenght)) {

		switch (l_chunk_id) {
			case MAT_NAME:
				{
					tMaterialInfo &matInfo = m_model.vMaterials[m_model.vMaterials.size()-1];
					ReadString(l_file,matInfo.strName);
					break;
				}
				break;
			case MAT_AMBIENT:
				if (!ReadColor(l_file, red, green, blue)) return false;
				{
					tMaterialInfo &material= m_model.vMaterials[m_model.vMaterials.size()-1];
					material.ambientcolor[0] = red;
					material.ambientcolor[1] = green;
					material.ambientcolor[2] = blue;
				}
				break;
			case MAT_DIFFUSE:
				if (!ReadColor(l_file, red, green, blue)) return false;
				{
					tMaterialInfo &material= m_model.vMaterials[m_model.vMaterials.size()-1];
					material.diffusecolor[0] = red;
					material.diffusecolor[1] = green;
					material.diffusecolor[2] = blue;
				}
				break;
			case MAT_SPECULAR:
				if (!ReadColor(l_file, red, green, blue)) return false;
				{
					tMaterialInfo &material= m_model.vMaterials[m_model.vMaterials.size()-1];
					material.specularcolor[0] = red;
					material.specularcolor[1] = green;
					material.specularcolor[2] = blue;
				}
				break;
			case MAT_SHININESS:
				if (!ReadPercentage(l_file, percentage)) return false;
				{
					tMaterialInfo &material= m_model.vMaterials[m_model.vMaterials.size()-1];
					material.shininess = ((float)percentage)/100.0f;
				}
				break;
			case 0xA300: //// This holds the file name of the texture
				{
					tMaterialInfo &matInfo = m_model.vMaterials[m_model.vMaterials.size()-1];
					ReadString(l_file,matInfo.strFile);
					break;
				}
			case 0xA200:	//texture map flag//	// This is the header for the texture info
				break;
			case MAT_TRANSPARENCY:
				/*
				   if (!ReadPercentage(l_file, percentage)) return false;
				   material.transparency = ((float)percentage)/100.0f;
				   break;
				 */
			default:
				// 忽略一些不需要的块
				fseek(l_file, l_chunk_lenght-6, SEEK_CUR);
		}
		chunkstart = ftell(l_file);
	}
	return true;
}
int W3DSFile::ReadColor(FILE* fp, float& red, float& green, float& blue)
{
	unsigned char  tmp;
	unsigned short chunk_id;
	int            chunk_length;
	int            chunkstart   = ftell(fp);

	if (!Read3DSChunk(fp, chunk_id,chunk_length)) return false;
	switch (chunk_id) 
	{
		case COLOR_F:
			if (!ReadFloat(fp, red)) return false;
			if (!ReadFloat(fp, green)) return false;
			if (!ReadFloat(fp, blue)) return false;
			break;
		case COLOR_24:
			if (!ReadUByte(fp, tmp)) return false;
			red = (float) tmp / (float) 255.0;
			if (!ReadUByte(fp, tmp)) return false;
			green = (float) tmp / (float) 255.0;
			if (!ReadUByte(fp, tmp)) return false;
			blue = (float) tmp / (float) 255.0;
			break;
		default:
			fseek(fp, chunkstart + chunk_length, SEEK_SET);
			return false;
	}
	return true;
}
bool W3DSFile::ReadPercentage(FILE* fp, float& value)
{
	unsigned short chunk_id;
	int chunk_length;
	int		chunkStart = ftell(fp);

	if (!Read3DSChunk(fp, chunk_id,chunk_length)) return false;

	if (chunk_id == INT_PERCENTAGE) 
	{
		short	svalue;
		if (!ReadShort(fp, svalue)) return false;
		value = (float) svalue/ (float) 100.0;
		return true;
	} else if (chunk_id == FLOAT_PERCENTAGE) 
	{
		if (!ReadFloat(fp, value)) return false;
		return true;
	} 

	fseek(fp, chunkStart + chunk_length, SEEK_SET);
	return false;
}
bool W3DSFile::Read3DSChunk(FILE* fp, unsigned short& chunk_id,int& chunk_length)
{
	if (!ReadUShort(fp, chunk_id)) return false;
	if (!ReadLong(fp, chunk_length)) return false;
	return true;
}
bool W3DSFile::init(char *sFileName)
{
	if(!load3DS(sFileName))
		return false;
	QString dirpath = QFileInfo(sFileName).absoluteDir().absolutePath()+"/";
	for(int i = 0;i<m_model.vMaterials.size();i++) {
		if(strlen(m_model.vMaterials[i].strFile)>0) {
			m_model.vMaterials[i].cTxt.loadImage(QString(dirpath+m_model.vMaterials[i].strFile).toUtf8().data());
		}
	}
	return true;
}
void W3DSFile::getNormal(struct t3DObject& obj,tFace& face,float* normal) {
	auto p0 = obj.pVerts[face.a].data();
	auto p1 = obj.pVerts[face.b].data();
	auto p2 = obj.pVerts[face.c].data();
	cross_prod(p0,p1,p2,normal);
}
void W3DSFile::show3DS(bool busematerial,float* pcolor)
{
	int   i;
	int   j;
	float normal[3];

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	if(busematerial)
		glDisable(GL_COLOR_MATERIAL);//
	for(i = 0;i<m_model.vObject.size();i++) {
		if(pcolor)
			glColor3fv(pcolor+3*i);

		t3DObject &obj = m_model.vObject[i];	
		for (j=0;j<obj.numOfFaces;j++) {

			tFace &face = obj.pFaces[j];
			if(busematerial && face.matIndex>=0 && face.matIndex <m_model.vMaterials.size()) {
				glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,m_model.vMaterials[face.matIndex].ambientcolor);
				glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,m_model.vMaterials[face.matIndex].diffusecolor);
				glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,m_model.vMaterials[face.matIndex].specularcolor);
				glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,&m_model.vMaterials[face.matIndex].shininess);
			} 

			getNormal(obj,face,normal);
			glNormal3fv(normal);

			if(obj.pTexVerts) {
				glBindTexture(GL_TEXTURE_2D, m_model.vMaterials[face.matIndex].cTxt.GetTxtID());
			}

			glBegin(GL_TRIANGLES); 

			if(obj.pTexVerts) {
				glTexCoord2f( obj.pTexVerts[ obj.pFaces[j].a ].x,
						obj.pTexVerts[ obj.pFaces[j].a ].y);
			}

			glVertex3fv(obj.pVerts[face.a].data());

			if(obj.pTexVerts)
				glTexCoord2f( obj.pTexVerts[ obj.pFaces[j].b ].x,
						obj.pTexVerts[ obj.pFaces[j].b ].y);
			glVertex3fv( obj.pVerts[face.b].data());

			if(obj.pTexVerts)
				glTexCoord2f( obj.pTexVerts[ obj.pFaces[j].c ].x,
						obj.pTexVerts[ obj.pFaces[j].c ].y);
			glVertex3fv( obj.pVerts[face.c].data());
			glEnd();
		}
	}
	if(busematerial)
		glEnable(GL_COLOR_MATERIAL);//启用颜色材质
	glDisable(GL_TEXTURE_2D);
}
void W3DSFile::showObject(unsigned i,bool busematerial)
{
	if(i >= m_model.vObject.size())return;

	float normal[3];
	t3DObject &obj = m_model.vObject[i];	
	for (int j=0;j<obj.numOfFaces;j++) {

		tFace &face = obj.pFaces[j];
		if(busematerial && face.matIndex>=0 && face.matIndex <m_model.vMaterials.size()) {
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,m_model.vMaterials[face.matIndex].ambientcolor);
			glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,m_model.vMaterials[face.matIndex].diffusecolor);
			glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,m_model.vMaterials[face.matIndex].specularcolor);
			glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,&m_model.vMaterials[face.matIndex].shininess);
		} 

		getNormal(obj,face,normal);
		glNormal3fv(normal);

		if(obj.pTexVerts) {
			glBindTexture(GL_TEXTURE_2D, m_model.vMaterials[face.matIndex].cTxt.GetTxtID());
		}

		glBegin(GL_TRIANGLES); 

		if(obj.pTexVerts) {
			glTexCoord2f( obj.pTexVerts[ obj.pFaces[j].a ].x,
					obj.pTexVerts[ obj.pFaces[j].a ].y);
		}

		glVertex3fv( obj.pVerts[face.a].data());

		if(obj.pTexVerts)
			glTexCoord2f( obj.pTexVerts[ obj.pFaces[j].b ].x,
					obj.pTexVerts[ obj.pFaces[j].b ].y);
		glVertex3fv( obj.pVerts[face.b].data());

		if(obj.pTexVerts)
			glTexCoord2f( obj.pTexVerts[ obj.pFaces[j].c ].x,
					obj.pTexVerts[ obj.pFaces[j].c ].y);
		glVertex3fv( obj.pVerts[face.c].data());
		glEnd();
	}
}
void W3DSFile::beginShowObject(bool busematerial)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	if(busematerial)
		glDisable(GL_COLOR_MATERIAL);//
}
void W3DSFile::endShowObject(bool busematerial )
{
	if(busematerial)
		glEnable(GL_COLOR_MATERIAL);//启用颜色材质
	glDisable(GL_TEXTURE_2D);
}
