_Pragma("once")
#include <QString>
#include <gis/perturbation_fwd.h>
class Observer;
class Tle;
namespace WSGP4
{
	Tle toTle(const WSatStateData& data);
	Observer toObserver(const WStationData& data);
	bool calculateTrackAndSubstarData(const WSatStateData& ed,time_t begin_time,time_t end_time,
			const WStationData& station_data,
			WTrackData* track_data,WSubStarPoints* substar_data);
	bool calculateTPForecast(const WSatStateData& ed,time_t begin_time,time_t end_time,const WStationData& station_data,TPForecasts* data_o);
	bool calculateSubStarDataAndSetInRange(const WSatStateData& ed,time_t begin_time,time_t end_time,const WStationData& station,std::vector<WSubStarPoint>* substar_data);
	bool calculateTrackData(const WSatStateData& ed,time_t begin_time,time_t end_time,
			const WStationData& station_data,
			WTrackData* track_data);
}
