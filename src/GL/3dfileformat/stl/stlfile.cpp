/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <stlfile.h>
#include <iostream>
#include <fstream>
#include <strings.h>
#include <wlogmacros.h>
#include <stdexcept>

using namespace std;

StlFile::StlFile(const string& file_path)noexcept(false)
{
	if(false == open(file_path)) {
		throw runtime_error("open fild faild");
	}
}
StlFile::StlFile()
{
}
bool StlFile::open(const string& file_path)
{
	const string solid_name    = "solid";
	const string endsolid_name = "endsolid";
	string       junk;
	ifstream     file;

	solids_.clear();
	file.open(file_path,ios::binary);
	if(!file) {
		ERROR_LOG("打开文件\"%s\"失败.",file_path.c_str());
		return false;
	}
	file_path_ = file_path;

	while(!file.eof()) {
		do {
			file >> junk;
		}while(solid_name != junk&&!file.eof()&&file.good());

		if(file.eof() || file.fail()) break;
		StlSolid solid;
		solid.faces.reserve(4096);
		do {
			StlFace face;
			bzero(&face,sizeof(face));
			file >> junk;
			if(endsolid_name == junk) break;
			file>>junk;
			file >> face.normal[0]>> face.normal[1] >> face.normal[2];
			file >> junk >> junk >> junk;
			file >> face.vertexs[0] >> face.vertexs[1] >> face.vertexs[2];
			file >> junk;
			file >> face.vertexs[3] >> face.vertexs[4] >> face.vertexs[5];
			file >> junk;
			file >> face.vertexs[6] >> face.vertexs[7] >> face.vertexs[8];
			file >> junk >> junk;
			if(file.fail()) break;
			solid.faces.push_back(face);
		} while(!file.eof()&&file.good());
		if(!solid.faces.empty()) solids_.push_back(move(solid));
	}
	file.close();
	return true;
}
