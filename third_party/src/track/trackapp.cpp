#include <trackapp.h>
#include <QDir>
#include <chrono>
#include <stdlib.h>
#include <fstream>
#include <toolkit_qt.h>
#include <wmacros.h>

constexpr const char *kTrackAppName     = "mXjklslIre3w";
constexpr const char *kDefaultTrackData = ":/third_party/src/data/track_data";
constexpr const char *kDefaultTrackExec = ":/third_party/src/data/track";

namespace TrackApp
{
	using namespace std;

	static QString trackdata_dir_;
	static QString app_path_;
	bool runTrackApp(const QString& dir_path,const WEphemerisData& ed,time_t begin_time,time_t end_time,int type);
	bool runTrackApp(const QString& dir_path,const WSatPosVelocity& ed,time_t begin_time,time_t end_time,int type);
	bool runTrackApp(const QString& dir_path,const WSatStateData& data,time_t begin_time,time_t end_time,int type);
	bool writeStationFile(const QString& dir_path,const WStationData& data);
	bool readSubStarFile(const std::string& file_path,std::vector<WSubStarPoint>* data_o);
	bool readBaseTrackFile(const std::string& file_path,WTrackData* data_o);
	bool readTPForecast(const std::string& file_path,TPForecasts* data_o);
	bool readTPForecastItem(std::ifstream& fs,TPForecastItem* item);
	bool correctSubStarData(WSubStarPoints* data);
	string dataInfo(const WSatStateData& data,const WStationData& station);

	void init()
	{
		const string kill_cmd = string("killall ")+kTrackAppName;

		system(kill_cmd.c_str());

		trackdata_dir_  =  QDir::home().absoluteFilePath(".trackdata");
		if(!QFile::exists(trackdata_dir_)) {
			WSpace::copy(kDefaultTrackData,trackdata_dir_);
		}
		app_path_ = QDir::temp().absoluteFilePath(kTrackAppName);
		if(QFile::exists(app_path_)) {
			QFile::remove(app_path_);
		}

		QFile::copy(kDefaultTrackExec,app_path_);
		if(!QFile::exists(app_path_)) {
			assert(0);
		}

		QFile::setPermissions(app_path_,QFile::permissions(app_path_)|QFileDevice::ExeUser);
	}
	void checkTrackAppDirPath(const QString& dir_path)
	{
		const auto app_dir       = QDir(dir_path);
		const auto input_dir     = app_dir.absoluteFilePath("input");
		const auto output_dir    = app_dir.absoluteFilePath("output");
		const auto const_datadir = app_dir.absoluteFilePath("CONST");

		if(!QDir(input_dir).exists()) {
			QDir().mkpath(input_dir);
		}
		if(!QDir(output_dir).exists()) {
			QDir().mkpath(output_dir);
		}
		if(!QFile::exists(const_datadir)) {
			QFile::link(trackdata_dir_+"/CONST",const_datadir);
		}
	}

	bool runTrackApp(const QString& dir_path,const WSatStateData& data,time_t begin_time,time_t end_time,int type)
	{

		if(data.ep_data.epoch_time.isNull()
				&& data.pos_data.epoch_time.isNull()) {
			return false;
		}

		checkTrackAppDirPath(dir_path);

		if(data.pos_data.epoch_time.isNull()) {
			return runTrackApp(dir_path,data.ep_data,begin_time,end_time,type);
		} else if(data.ep_data.epoch_time.isNull()) {
			return runTrackApp(dir_path,data.pos_data,begin_time,end_time,type);
		} else if(data.ep_data.epoch_time >= data.pos_data.epoch_time) {
			return runTrackApp(dir_path,data.ep_data,begin_time,end_time,type);
		} else {
			return runTrackApp(dir_path,data.pos_data,begin_time,end_time,type);
		}
		return false;
	}
	bool runTrackApp(const QString& dir_path,const WEphemerisData& ed,time_t begin_time,time_t end_time,int type)
	{
		const auto  app_dir          = QDir(dir_path);
		const auto  input_file       = app_dir.absoluteFilePath("input/orbit.txt");
		const char *time_format      = "%Y %m %d %H:%M:%S.000";
		ofstream    fs(input_file      .toUtf8().data());
		char        time_buffer[256] = {0};
		const auto  kMinEpochTime    = begin_time-7*24*60*60;

		if(ed.epoch_time.toTime_t() < kMinEpochTime) {
			ERROR_LOG("星历数据已经过期");
			return false;
		}

		if(!fs) {
			ERROR_LOG("打开文件%s失败",input_file.toUtf8().data());
			return false;
		}

		fs<<dec<<fixed;
		fs<<"satelliteName              "<<ed.sat_name.toUtf8().data()<<endl;
		WSpace::strftime(time_buffer,sizeof(time_buffer),time_format,ed.epoch_time.toTime_t());
		fs<<"epochTime(BJT)          "<<time_buffer<<endl;
		fs<<"orbKind                  1"<<endl;
		fs<<"orbData                  "<<ed.a<<"        "<<ed.e<<"        "<<ed.i<<"        "<<ed.O<<"        "<<ed.w<<"        "<<ed.M<<endl;
		WSpace::strftime(time_buffer,sizeof(time_buffer),time_format,begin_time);
		fs<<"pre_startTime(BJT)       "<<time_buffer<<endl;
		WSpace::strftime(time_buffer,sizeof(time_buffer),time_format,end_time);
		fs<<"pre_endTime(BJT)         "<<time_buffer<<endl;
		fs<<"passNumber_start      "<<ed.pathway<<endl;
		fs<<"track_interval(sec)   1.00"<<endl;
		fs<<"satellite_mass(kg)     3600.0"<<endl;
		fs<<"satellite_area_atm(m2)      3.800"<<endl;
		fs<<"satellite_area_solar(m2)     12.000"<<endl;
		fs<<"flux_fluxAverage_KP 108.0 108.0   2.0"<<endl;
		fs<<"useDrag  T"<<endl;
		fs<<"CD  2.200"<<endl;
		fs<<"SRP  0.160"<<endl;
		fs<<"order_LM  16  16"<<endl;
		fs<<"calcType  "<<type<<endl;
		fs.close();

		QProcess process;
		process.setWorkingDirectory(app_dir.absolutePath());
		process.start(app_path_);
		return process.waitForFinished(20*60*1000);
	}
	bool runTrackApp(const QString& dir_path,const WSatPosVelocity& data,time_t begin_time,time_t end_time,int type)
	{
		const auto  app_dir          = QDir(dir_path);
		const auto  input_file       = app_dir.absoluteFilePath("input/orbit.txt");
		ofstream    fs(input_file.toUtf8().data());
		const char *time_format      = "%Y %m %d %H:%M:%S.000";
		char        time_buffer[256] = {0};
		const auto  kMinEpochTime    = begin_time-7*24*60*60;

		if(data.epoch_time.toTime_t() < kMinEpochTime) {
			ERROR_LOG("星历数据已经过期");
			return false;
		}

		if(!fs) {
			ERROR_LOG("打开文件%s失败",input_file.toUtf8().data());
			return false;
		}
		INFO_LOG("Use sat pos.");

		fs<<dec<<fixed;
		fs<<"satelliteName              "<<data.sat_name.toUtf8().data()<<endl;
		WSpace::strftime(time_buffer,sizeof(time_buffer),time_format,data.epoch_time.toTime_t());
		fs<<"epochTime(BJT)          "<<time_buffer<<endl;
		fs<<"orbKind                  2"<<endl;
		fs<<"orbData                  "<<data.pos[0]<<"        "<<data.pos[1]<<"        "<<data.pos[2]<<"        "<<data.velocity[0]<<"        "<<data.velocity[1]<<"        "<<data.velocity[2]<<endl;
		WSpace::strftime(time_buffer,sizeof(time_buffer),time_format,begin_time);
		fs<<"pre_startTime(BJT)       "<<time_buffer<<endl;
		WSpace::strftime(time_buffer,sizeof(time_buffer),time_format,end_time);
		fs<<"pre_endTime(BJT)         "<<time_buffer<<endl;
		fs<<"passNumber_start      1"<<endl;
		fs<<"track_interval(sec)   1.00"<<endl;
		fs<<"satellite_mass(kg)     3600.0"<<endl;
		fs<<"satellite_area_atm(m2)      3.800"<<endl;
		fs<<"satellite_area_solar(m2)     12.000"<<endl;
		fs<<"flux_fluxAverage_KP 108.0 108.0   2.0"<<endl;
		fs<<"useDrag  T"<<endl;
		fs<<"CD  2.200"<<endl;
		fs<<"SRP  0.160"<<endl;
		fs<<"order_LM  16  16"<<endl;
		fs<<"calcType  "<<type<<endl;
		fs.close();

		QProcess process;
		process.setWorkingDirectory(app_dir.absolutePath());
		process.start(app_path_);
		return process.waitForFinished(20*60*1000);
	}
	bool writeStationFile(const QString& dir_path,const WStationData& data)
	{
		const auto input_dir =  QDir(dir_path).absoluteFilePath("input");
		if(!QFile::exists(input_dir)) {
			QDir().mkpath(input_dir);
		}
		const auto                input_file      = QDir(input_dir).absoluteFilePath("stationData.txt");
		ofstream                  fs(input_file.toUtf8().data());

		if(!fs) {
			ERROR_LOG("打开文件%s失败",input_file.toUtf8().data());
			return false;
		}

		fs<<dec<<fixed;
		fs<<"station_name            "<<data.name<<endl;
		fs<<"coordinate_type(DX1_DX2_WGS84)   DX2"<<endl;
		fs<<"lat_long_height_minE(deg_deg_m_deg)   "<<data.pos[1]<<"    "<<data.pos[0]<<"     "<<data.pos[2]<<"    "<<"3.0"<<endl;
		fs.close();

		return true;
	}
	bool readSubStarFile(const string& file_path,vector<WSubStarPoint>* data_o)
	{
		string            str_temp;
		unsigned          y;
		unsigned          M;
		unsigned          d;
		unsigned          h;
		unsigned          m;
		unsigned          s;
		unsigned          z;
		char              c;
		float             ftemp;
		WSubStarPoint temp_data;
		ifstream          fs(file_path);

		data_o->clear();
		if(!fs) {
			ERROR_LOG("打开文件%s失败.",file_path.c_str());
			return false;
		}
		getline(fs,str_temp);
		while(1) {
			fs>>str_temp; //satName
			fs>>ftemp; //圈号
			temp_data.pass= ftemp;
			fs>>y>>M>>d>>h>>c>>m>>c>>s>>c>>z;
			fs>>temp_data.longi>>temp_data.lati;
			temp_data.time = WSpace::toTime_t(y,M,d,h,m,s);
			getline(fs,str_temp);
			if((temp_data.time==0) && fs) continue;
			if(fs)
				data_o->push_back(temp_data);
			else
				break;
		}

		correctSubStarData(data_o);
		QFile::remove(file_path.c_str());
		return true;
	}
	bool readTPForecast(const string& file_path,TPForecasts* data_o)
	{
		string            str_temp;
		ifstream          fs(file_path);

		data_o->data.clear();
		if(!fs) {
			ERROR_LOG("打开文件%s失败.",file_path.c_str());
			return false;
		}
		getline(fs,str_temp);
		getline(fs,str_temp);
		while(fs) {
			TPForecast data;
			if(readTPForecastItem(fs,&data.data[0]) &&
					readTPForecastItem(fs,&data.data[1])&&
					readTPForecastItem(fs,&data.data[2])) {
				data_o->data.push_back(data);
			}
			if(!fs)break;
			getline(fs,str_temp);
		}

		QFile::remove(file_path.c_str());

		if(!data_o->data.empty()) {
			data_o->begin_time  =  data_o->data.front().data[0].time;
			data_o->end_time    =  data_o->data.back().data[2].time;
		}
		return true;
	}
	bool readTPForecastItem(std::ifstream& fs,TPForecastItem* item)
	{
		string            str_temp;
		unsigned          y;
		unsigned          M;
		unsigned          d;
		unsigned          h;
		unsigned          m;
		unsigned          s;
		char              c;

		/*
		 * 三点预报没有毫秒
		 */
		fs>>y>>M>>d>>h>>c>>m>>c>>s;
		fs>>item->range>>item->A>>item->E>>item->dr;
		item->time = WSpace::toTime_t(y,M,d,h,m,s);
		getline(fs,str_temp);
		if(fs && item->time>0)
			return true;
		else
			return false;
	}
	bool readBaseTrackFile(const string& file_path,WTrackData* data_o)
	{
		string         str_temp;
		unsigned       y;
		unsigned       M;
		unsigned       d;
		unsigned       h;
		unsigned       m;
		unsigned       s;
		unsigned       z;
		char           c;
		WTrackItem temp_data;
		ifstream       fs(file_path);

		data_o->data.clear();
		if(!fs) {
			ERROR_LOG("打开文件%s失败.",file_path.c_str());
			return false;
		}
		getline(fs,str_temp);
		while(1) {
			fs>>y>>M>>d>>h>>c>>m>>c>>s>>c>>z;
			fs>>temp_data.range;
			fs>>temp_data.A>>temp_data.E;
			fs>>temp_data.dr;
			getline(fs,str_temp);
			temp_data.time = WSpace::toTime_t(y,M,d,h,m,s);
			if((temp_data.time==0) && fs) continue;
			if(fs)
				data_o->data.push_back(temp_data);
			else
				break;
		}
		if(!data_o->data.empty()) {
			data_o->begin_time  =  data_o->data.front().time;
			data_o->end_time    =  data_o->data.back().time;
		}

		QFile::remove(file_path.c_str());
		return true;
	}
	bool calculateTrackData(const QString& dir_path,const WSatStateData& ed,time_t begin_time,time_t end_time,const WStationData& station_data,WTrackData* base_track_data)
	{
		const auto app_dir         = QDir(dir_path);
		const auto base_track_file = app_dir.absoluteFilePath("output/TrackData_001.txt");
		const auto cbt             = chrono::system_clock::now();

		base_track_data->clear();

		if(ed.isNull()) return false;

		writeStationFile(dir_path,station_data);

		if(false == TrackApp::runTrackApp(dir_path,ed,begin_time,end_time,3)) return false;
		INFO("%d track calculate use0 %d ms.",base_track_data->data.size(),chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now()-cbt).count());

		if(!readBaseTrackFile(base_track_file.toUtf8().data(),base_track_data)) {
			ERROR_LOG("读取引导数据失败");
			return false;
		} else {
			base_track_data->begin_time  =  begin_time;
			base_track_data->end_time    =  end_time;

			const QString time_format = "hh:mm:ss";
			INFO_LOG("获取%s引导数据(%f,%f)成功",ed.ep_data.sat_name.toUtf8().data(),station_data.pos[0],station_data.pos[1]
					);
		}
		INFO("%d track calculate use %d ms.",base_track_data->data.size(),chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now()-cbt).count());
		base_track_data->info = dataInfo(ed,station_data);
		return true;
	}
	bool calculateSubStarData(const QString& dir_path,const WSatStateData& ed,time_t begin_time,time_t end_time,vector<WSubStarPoint>* substar_data)
	{
		const auto         app_dir         = QDir(dir_path);
		const auto         sub_star_file   = app_dir.absoluteFilePath("output/subStar.txt");
		WStationData       station_data{"NULL",{120,30,0}};

		if(ed.isNull()) return false;

		INFO_LOG("获取星下点数据");
		TrackApp::writeStationFile(dir_path,station_data);

		if(false == TrackApp::runTrackApp(dir_path,ed,begin_time,end_time,2)) return false;

		if(!TrackApp::readSubStarFile(sub_star_file.toUtf8().data(),substar_data)) {
			ERROR_LOG("读取星下点数据失败");
			return false;
		}
		if(substar_data->empty()) {
			ERROR_LOG("获取的星下点数据为空(%s,%s)",dir_path.toUtf8().data(),sub_star_file.toUtf8().data());
		}

		return true;
	}
	bool calculateTPForecast(const QString& dir_path,const WSatStateData& ed,time_t begin_time,time_t end_time,const WStationData& station_data,TPForecasts* data_o)
	{
		const auto app_dir     = QDir(dir_path);
		const auto output_file = app_dir.absoluteFilePath("output/TrackGeneral.txt");

		data_o->clear();
		if(ed.isNull()) return false;

		INFO_LOG("获取三点预报数据");

		TrackApp::writeStationFile(dir_path,station_data);

		if(false == TrackApp::runTrackApp(dir_path,ed,begin_time,end_time,1)) return false;

		if(!TrackApp::readTPForecast(output_file.toUtf8().data(),data_o)) {
			ERROR_LOG("读取三点预报数据失败");
			return false;
		} else {
			data_o->begin_time  =  begin_time;
			data_o->end_time    =  end_time;
		}
		return true;
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
	bool calculateSubStarDataAndSetInRange(const QString& dir_path,const WSatStateData& ed,time_t begin_time,time_t end_time,const WStationData& station,vector<WSubStarPoint>* substar_data)
	{
		TPForecasts data;

		if(false == calculateSubStarData(dir_path,ed,begin_time,end_time,substar_data)) return false;

		if(substar_data->empty()) return true;

		if(false == calculateTPForecast(dir_path,ed,begin_time,end_time,station,&data)) return true;

		if(data.data.empty()) return true;

		for(auto& fd:data.data) {
			WSubStarPoint temp;
			temp.time = fd.data[0].time;
			auto begin = lower_bound(substar_data->begin(),substar_data->end(),temp,[](const WSubStarPoint& lhv,const WSubStarPoint& rhv){ return lhv.time<rhv.time;});
			temp.time = fd.data[2].time;
			auto end = upper_bound(substar_data->begin(),substar_data->end(),temp,[](const WSubStarPoint& lhv,const WSubStarPoint& rhv){ return lhv.time<rhv.time;});
			if(begin == end) continue;
			for_each(begin,end,[](WSubStarPoint& v){ v.flag |= SPF_INRANGE;});
			temp.time = fd.data[1].time;
			auto mid = equal_range(substar_data->begin(),substar_data->end(),temp,[](const WSubStarPoint& lhv,const WSubStarPoint& rhv){ return lhv.time<rhv.time;});
			for_each(mid.first,mid.second,[](WSubStarPoint& v){ v.flag |= SPF_INRANGE_TOP;});
		}

		return true;
	}
	bool setInRange(const QString& dir_path,const WSatStateData& ed,const WStationData& station,std::vector<WSubStarPoint>* substar_data_io)
	{
		TPForecasts data;

		if(substar_data_io->empty()) return true;

		auto begin_time = substar_data_io->front().time;
		auto end_time   = substar_data_io->back().time;

		for(auto& sp:*substar_data_io) {
			sp.flag &= (~SPF_INRANGE);
			sp.flag &= (~SPF_INRANGE_TOP);
		}

		if(false == calculateTPForecast(dir_path,ed,begin_time,end_time,station,&data)) return true;

		if(data.data.empty()) return true;

		for(auto& fd:data.data) {
			WSubStarPoint temp;
			temp.time = fd.data[0].time;
			auto begin = lower_bound(substar_data_io->begin(),substar_data_io->end(),temp,[](const WSubStarPoint& lhv,const WSubStarPoint& rhv){ return lhv.time<rhv.time;});
			temp.time = fd.data[2].time;
			auto end = upper_bound(substar_data_io->begin(),substar_data_io->end(),temp,[](const WSubStarPoint& lhv,const WSubStarPoint& rhv){ return lhv.time<rhv.time;});
			if(begin == end) continue;
			for_each(begin,end,[](WSubStarPoint& v){ v.flag |= SPF_INRANGE;});
			temp.time = fd.data[1].time;
			auto mid = equal_range(substar_data_io->begin(),substar_data_io->end(),temp,[](const WSubStarPoint& lhv,const WSubStarPoint& rhv){ return lhv.time<rhv.time;});
			for_each(mid.first,mid.second,[](WSubStarPoint& v){ v.flag |= SPF_INRANGE_TOP;});
		}

		return true;
	}
	string dataInfo(const WSatStateData& data,const WStationData& station)
	{
		stringstream ss;
		ss<<string("track app")<<endl;
		ss<<"Current date time:"<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toUtf8().data()<<endl;
		ss<<data<<endl;
		ss<<station<<endl;
		return ss.str();
	}
}
