#include "sgp4app.h"
#include <QDir>
#include <chrono>
#include <stdlib.h>
#include <fstream>
#include <wmacros.h>
#include <CoordTopocentric.h>
#include <CoordGeodetic.h>
#include <Observer.h>
#include <SGP4.h>
#include <tlelib/tlelib.h>
#include <wmath.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace WSpace;

namespace WSGP4
{
	using namespace std;
	bool correctSubStarData(WSubStarPoints* data);
	Tle toTle(const WEphemerisData& data);
	string dataInfo(const WSatStateData& data,const WStationData& station);

	void assign(WSubStarPoint& data,time_t t,const CoordTopocentric& topo,const CoordGeodetic& geo) {

		constexpr auto kDelta  = 1E-2;

		data.flag   =  0;
		data.time   =  t;
		data.longi  =  r2d(geo.longitude);
		data.lati   =  r2d(geo.latitude);

		if(topo.elevation>0) {
			data.flag |= SPF_INRANGE;
			if(fabs(topo.elevation-90.0)<kDelta)
				data.flag |= SPF_INRANGE_TOP;
		}
	}
	ostream& output(ostream& os,double v,int w0,int w1)
	{
		int v0 = v;
		int v1 = (v-v0)*pow(10,w1);

		os<<setw(w0)<<v0<<'.'<<setfill('0')<<setw(w1)<<v1;
		return os;

	}
	int32_t check_sum(const std::string& str)
	{
		int32_t sum = 0;
		for(auto&c:str) {
			if(c == '-') {
				sum+=1;
				continue;
			}
			if(c<'0' || c>'9') continue;
			sum += int32_t(int8_t(c-'0'));
		}
		return sum%10;
	}
	void assign(TPForecastItem& data,time_t t,const CoordTopocentric& topo) {
		data.time   =  t;
		data.A      =  r2d(topo.azimuth);
		data.E      =  r2d(topo.elevation);
		data.range  =  topo.range;
		data.dr     =  topo.range_rate;
	}
	void assign(WTrackItem& data,time_t t,const CoordTopocentric& topo) {
		data.time   =  t;
		data.A      =  r2d(topo.azimuth);
		data.E      =  r2d(topo.elevation);
		data.range  =  topo.range;
		data.dr     =  topo.range_rate;
	}
	Tle toTle(const WSatStateData& data)
	{
		if(data.tle_data.isNull()) return toTle(data.ep_data);
		auto& td = data.tle_data;
		INFO_LOG("Use TLE data.");
		if(td.data[2].empty()) {
			Tle res(td.data[0],td.data[1]);
			return res;
		} else {
			Tle res(td.data[0],td.data[1],td.data[2]);
			return res;
		}
	}
	Tle toTle(const WEphemerisData& data)
	{
		tlelib::Node node;              // create "empty" node

		node.setSatelliteName(data.sat_name.toUtf8().data());    // set satellite name
		node.setSatelliteNumber("25544");// NORAD number
		node.setDesignator("98067A");    // International designator
		node.setClassification('U');     // classification
		node.setPreciseEpoch(data.epoch_time.toTime_t());// epoch
		node.set_i(data.i);             // Inclination
		node.set_Omega(data.O);        // Right Ascension of the Ascending Node
		node.set_e(data.e);           // Eccentricity
		node.set_omega(data.w);         // Argument of Perigee
		node.set_M(data.M);            // Mean Anomaly

		auto nr = 24*60*60.0/satPeriod(data.a);

		node.set_n(nr);         // Mean Motion
		node.set_dn(0.00713053);         // First Time Derivative of the Mean Motion
		node.set_d2n(0.000011562);       // Second Time Derivative of Mean Motion 
		node.set_bstar(0.00034316);      // BSTAR drag term
		node.setEphemerisType('0');      // Ephemeris type
		node.setElementNumber(5);        // Element number
		node.setRevolutionNumber(4);     // Revolution number at epoch
		INFO_LOG("Use transled tle data.");
		return Tle(node.firstString(),node.secondString(),node.thirdString());
	}

	Observer toObserver(const WStationData& data)
	{
		/*
		 * Observer参数依次为纬度，经度，高程（单位km)
		 */
		return Observer(data.pos[1],data.pos[0],data.pos[2]*1E-3);
	}
	inline DateTime transTime(const ptime& t) {
		const auto &date = t.date();
		const auto &time = t.time_of_day();

		if(time_duration::num_fractional_digits()>0) {
			long     microseconds = time.fractional_seconds()/(pow(10,time_duration::num_fractional_digits()-6));
			DateTime res;

			res.Initialise(date.year(),date.month(),date.day(),time.hours(),time.minutes(),time.seconds(),microseconds);
			return res;
		} else {
			return DateTime(date.year(),date.month(),date.day(),time.hours(),time.minutes(),time.seconds());
		}
	}

	bool calculateTrackAndSubstarData(const WSatStateData& ed,time_t begin_time,time_t end_time,const WStationData& station_data,WTrackData* track_data,WSubStarPoints* substar_data)
	{
		WTrackItem     temp_td;
		WSubStarPoint    temp_sd;

		track_data->data.clear();
		substar_data->clear();
		try {
			const ptime epoch(date(1970   ,1,1),time_duration(0,0,0));
			auto       tle       = toTle(ed);
			auto       obs       = toObserver(station_data);
			auto       t         = from_time_t(begin_time);
			const auto et        = from_time_t(end_time);
			SGP4       sgp4(tle);

			while(t<et) {
				auto             sgp4_t = transTime(t);
				Eci              eci    = sgp4.FindPosition(sgp4_t);
				CoordTopocentric topo   = obs.GetLookAngle(eci);
				CoordGeodetic    geo    = eci.ToGeodetic();

				assign(temp_td,(t-epoch).total_seconds(),topo);
				assign(temp_sd,temp_td.time,topo,geo);

				if(temp_td.E>0) track_data->data.push_back(temp_td);

				substar_data->push_back(temp_sd);
				t += seconds(1);
			}
			if(!track_data->data.empty()) {
				track_data->begin_time  =  begin_time;
				track_data->end_time    =  end_time;
			}
			correctSubStarData(substar_data);

			track_data->info = dataInfo(ed,station_data);

			return true;
		} catch(...) {
			return false;
		}
	}
	bool calculateTPForecast(const WSatStateData& ed,time_t begin_time,time_t end_time,const WStationData& station_data,TPForecasts* data_o)
	{
		TPForecast     temp_tp;

		try {
			const ptime epoch(date(1970   ,1,1),time_duration(0,0,0));
			auto             tle         = toTle(ed);
			auto             obs         = toObserver(station_data);
			auto             t           = from_time_t(begin_time);
			const auto       et          = from_time_t(end_time);
			const auto       minE        = d2r(3.0);
			const auto       minMaxE     = d2r(4.0);
			const auto       thresholdE  = d2r(2.0);
			SGP4             sgp4(tle);
			CoordTopocentric topo;

			data_o->data.clear();
			while(t<et) {
				while(t<et) {
					auto             sgp4_t = transTime(t);
					Eci              eci    = sgp4.FindPosition(sgp4_t);

					topo   = obs.GetLookAngle(eci);
					if(topo.elevation>=minE) break;
					t += seconds(1);
				}
				if(t>=et) break;
				assign(temp_tp.data[0],(t-epoch).total_seconds(),topo);

				t += seconds(1);

				while(t<et) {
					auto             sgp4_t = transTime(t);
					Eci              eci    = sgp4.FindPosition(sgp4_t);

					topo   = obs.GetLookAngle(eci);
					if((topo.elevation>=minMaxE) || (topo.elevation<=thresholdE))  break;
					t += seconds(1);
				}

				if(topo.elevation<minE)continue; 

				if(t>=et) break;

				assign(temp_tp.data[1],(t-epoch).total_seconds(),topo);
				temp_tp.data[2] = temp_tp.data[1];

				while(t<et) {
					auto             sgp4_t = transTime(t);
					Eci              eci    = sgp4.FindPosition(sgp4_t);

					topo   = obs.GetLookAngle(eci);
					if(r2d(topo.elevation)>temp_tp.data[1].E)
						assign(temp_tp.data[1],(t-epoch).total_seconds(),topo);
					if(topo.elevation<=minE)  break;
					t += seconds(1);
				}
				assign(temp_tp.data[2],(t-epoch).total_seconds(),topo);
				data_o->data.push_back(temp_tp);
				t += seconds(1);
			}
			if(!data_o->data.empty()) {
				data_o->begin_time  =  begin_time;
				data_o->end_time    =  end_time;
			}

			return true;
		} catch(...) {
			return false;
		}
		return true;
	}
	bool calculateSubStarDataAndSetInRange(const WSatStateData& ed,time_t begin_time,time_t end_time,const WStationData& station,vector<WSubStarPoint>* substar_data)
	{
		WSubStarPoint    temp_sd;

		substar_data->clear();
		try {
			const ptime epoch(date(1970   ,1,1),time_duration(0,0,0));
			auto       tle       = toTle(ed);
			auto       obs       = toObserver(station);
			auto       t         = from_time_t(begin_time);
			const auto et        = from_time_t(end_time);
			SGP4       sgp4(tle);
			cout<<transTime(t)<<","<<transTime(et)<<endl;

			while(t<et) {
				auto             sgp4_t = transTime(t);
				Eci              eci    = sgp4.FindPosition(sgp4_t);
				CoordTopocentric topo   = obs.GetLookAngle(eci);
				CoordGeodetic    geo    = eci.ToGeodetic();

				assign(temp_sd,(t-epoch).total_seconds(),topo,geo);

				substar_data->push_back(temp_sd);
				t += seconds(1);
			}
			correctSubStarData(substar_data);

			return true;
		} catch(...) {
			return false;
		}
		return true;
	}
	bool calculateTrackData(const WSatStateData& ed,time_t begin_time,time_t end_time,const WStationData& station_data,WTrackData* track_data)
	{
		WTrackItem     temp_td;

		track_data->data.clear();
		INFO_LOG("Station(%lf,%lf,%lfm)",station_data.pos[0],station_data.pos[1],station_data.pos[2]);
		try {
			const ptime epoch(date(1970   ,1,1),time_duration(0,0,0));
			auto       tle       = toTle(ed);
			auto       obs       = toObserver(station_data);
			auto       t         = from_time_t(begin_time);
			const auto et        = from_time_t(end_time);
			SGP4       sgp4(tle);

			cout<<transTime(t)<<","<<transTime(et)<<endl;

			while(t<et) {
				auto             sgp4_t = transTime(t);
				Eci              eci    = sgp4.FindPosition(sgp4_t);
				CoordTopocentric topo   = obs.GetLookAngle(eci);

				assign(temp_td,(t-epoch).total_seconds(),topo);

				if(temp_td.E>0) {
					track_data->data.push_back(temp_td);
				}
				t += seconds(1);
			}
			if(!track_data->data.empty()) {
				track_data->begin_time  =  begin_time;
				track_data->end_time    =  end_time;
			}

			track_data->info = dataInfo(ed,station_data);

			return true;
		} catch(...) {
			return false;
		}
	}
	bool correctSubStarData(WSubStarPoints* data)
	{
		if(data->size()<4) return true;

		auto d0 = data->begin();
		auto d1 = next(d0);
		auto d2 = next(d1);

		while(d2!=data->end()) {
			if((d0->longi > 90 && d2->longi>90 && d1->longi < -90)||
					(d0->longi < -90 && d2->longi < -90 && d1->longi > 90) ||
					(d0->lati> 45 && d2->lati>45 && d1->lati< -45)||
					(d0->lati< -45 && d2->lati< -45 && d1->lati> 45)) {
				d1->longi  =  int(d0->longi+d2->longi)/2;
				d1->lati   =  int(d0->lati+d2->lati)/2;
			}
			++d0;
			++d1;
			++d2;
		}
		return true;
	}
	string dataInfo(const WSatStateData& data,const WStationData& station)
	{
		stringstream ss;
		ss<<"SGP4 app"<<endl;
		ss<<"Current date time:"<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toUtf8().data()<<endl;
		ss<<data<<endl;
		ss<<station<<endl;
		return ss.str();
	}
}
