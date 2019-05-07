#include <perturbation.h>
#include <fstream>
#include <wlogmacros.h>

using namespace std;
namespace WSpace
{
	bool saveTrackData(const WTrackData& data,const std::string& file_path)
	{
		fstream    stream;
		const auto time_format    = "%Y-%m-%d %H:%M:%S.000";
		char       time_data[128];
		
		if(data.data.empty()) return false;

		stream.open(file_path,ios_base::out|ios_base::trunc);
		if(!stream.is_open()) {
			ERROR_LOG("打开文件\"%s\"失败!",file_path.c_str());
			return false;
		}
		stream<<"<*---------------------------------------------------*>"<<endl;
		stream<<data.info<<endl;
		stream<<"<*---------------------------------------------------*>"<<endl;
		for(auto&d:data.data) {
			WSpace::strftime(time_data,sizeof(time_data),time_format,d.time);
			stream<<time_data<<"    "<<d.A<<"   "<<d.E<<"   "<<d.range<<endl;
		}
		return true;

	}
	bool saveSubStarPointsData(const WSubStarPoints& data,const std::string& file_path)
	{
		fstream    stream;
		const auto time_format    = "%Y-%m-%d %H:%M:%S.000";
		char       time_data[128];

		if(data.empty()) return false;

		stream.open(file_path,ios_base::out|ios_base::trunc);
		if(!stream.is_open()) {
			ERROR_LOG("打开文件\"%s\"失败!",file_path.c_str());
			return false;
		}

		for(auto&d:data) {
			WSpace::strftime(time_data,sizeof(time_data),time_format,d.time);
			stream<<time_data<<"    "<<d.longi<<"   "<<d.lati<<endl;
		}

		return true;
	}
}
