/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <QString>
#include <gis/perturbation_fwd.h>
namespace TrackApp
{
	/*
	 * 需要在调用其它函数前执行
	 */
	void init();

	bool calculateTrackData(const QString& dir_path,const WSatStateData& ed,time_t begin_time,time_t end_time,
			const WStationData& station_data,
			WTrackData* track_data);
	bool calculateSubStarData(const QString& dir_path,const WSatStateData& ed,time_t begin_time,time_t end_time,std::vector<WSubStarPoint>* substar_data);
	bool calculateTPForecast(const QString& dir_path,const WSatStateData& ed,time_t begin_time,time_t end_time,const WStationData& station_data,TPForecasts* data_o);
	bool calculateSubStarDataAndSetInRange(const QString& dir_path,const WSatStateData& ed,time_t begin_time,time_t end_time,const WStationData& station,std::vector<WSubStarPoint>* substar_data);
	/*
	 * 根据站点信息及已有的星下点数据，设置过境信息
	 */
	bool setInRange(const QString& dir_path,const WSatStateData& ed,const WStationData& station,std::vector<WSubStarPoint>* substar_data_io);
}
