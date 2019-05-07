/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <list>
#include <vector>
#include <string>
struct StlFace
{
	float normal[3];
	float vertexs[9];
};
struct StlSolid
{
	std::vector<StlFace> faces;
};
class StlFile
{
	public:
		StlFile(const std::string& file_path)noexcept(false);
		StlFile();
		bool open(const std::string& file_path);
		inline const std::list<StlSolid>& solids()const {
			return solids_;
		}
	private:
		std::string file_path_;
		std::list<StlSolid> solids_;
};
