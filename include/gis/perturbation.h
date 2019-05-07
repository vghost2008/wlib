/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <string>
#include "perturbation_fwd.h"

namespace WSpace
{
	bool saveTrackData(const WTrackData& data,const std::string& file_path);
	bool saveSubStarPointsData(const WSubStarPoints& data,const std::string& file_path);
}
