/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include "texture.h"
#include <vector>
#include <gmatrix.h>
using tVector3=WSpace::GVector;
#include <list>
class CPortal;
class CFrustum;
class W3DSFile  
{
	private:
		// This is our 2D point class.  This will be used to store the UV coordinates.
		struct tVector2 {
			float x;
			float y;
		};
		// This is our face structure.  This is is used for indexing into the vertex 
		// and texture coordinate arrays.  From this information we know which vertices
		// from our vertex array go to which face, along with the correct texture coordinates.
		struct tFace
		{
			unsigned short a;
			unsigned short b;
			unsigned short c;          //vertIndex[3]; // indicies for the verts that make up this triangle
			short          matIndex;   //The texture ID to use, which is the index into our texture array
		};

		// This holds the information for a material.  It may be a texture map of a color.
		// Some of these are not used, but I left them because you will want to eventually
		// read in the UV tile ratio and the UV tile offset for some models.
		struct tMaterialInfo
		{
			char     strName[255];       // The texture name
			char     strFile[255];       // The texture file name (If this is set it's a texture map)
			float    ambientcolor[3];
			float    diffusecolor[3];
			float    specularcolor[3];
			float    shininess;
			CTexture cTxt;
			tMaterialInfo() {
				ambientcolor[0]   =  ambientcolor[1]   =  ambientcolor[2]   =  0.3;
				diffusecolor[0]   =  diffusecolor[1]   =  diffusecolor[2]   =  0.3;
				specularcolor[0]  =  specularcolor[1]  =  specularcolor[2]  =  0.3;
				shininess         =  8;
			}
		} ;
		// This holds all the information for our model/scene. 
		// You should eventually turn into a robust class that 
		// has loading/drawing/querying functions like:
		// LoadModel(...); show3DSObject(...); show3DSModel(...); DestroyModel(...);
		struct t3DObject 
		{
			int       numOfVerts;     // The number of verts in the model
			int       numOfFaces;     // The number of faces in the model
			int       numTexVertex;   // The number of texture coordinates
			char      strName[255];   // The name of the object
			tVector3 *pVerts;         // The object's vertices
			//tVector3 *pNormals; // The object's normals
			tVector2 *pTexVerts;      // The texture's UV coordinates
			tFace    *pFaces;         // The faces information of the object
			//  子物体列表
			std::list<t3DObject* >lstObj;
		};
		// This holds our model information.  This should also turn into a robust class.
		// We use STL's (Standard Template Library) vector class to ease our link list burdens. :)
		struct t3DModel 
		{
			std::vector<tMaterialInfo> vMaterials;	// The list of material information (Textures and colors)
			std::vector<t3DObject> vObject;			// The object list for our model
		};
	public:
		bool load3DS (char *p_filename);
		void show3DS(bool busematerial = false,float* pcolor=0);
		void beginShowObject(bool busematerial = false);
		void showObject(unsigned i,bool busematerial = false);
		void endShowObject(bool busematerial = false);
		bool init(char *sFileName);
		W3DSFile();
		void releaseMem();
		virtual ~W3DSFile();
	private:
		t3DModel m_model;
		CTexture m_txt;
	private:
		void getNormal(struct t3DObject& obj,tFace& face,float* normal);
		int ReadColor(FILE* fp, float& red, float& green, float& blue);
		bool ReadMatEntry(int fileSize,int chunkstart,int l_chunk_lenght,FILE* fp);
		bool Read3DSChunk(FILE* fp, unsigned short& chunk_id,int& chunk_length);
		bool ReadPercentage(FILE* fp, float& value);
};
