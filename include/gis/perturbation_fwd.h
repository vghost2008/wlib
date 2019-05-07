_Pragma("once")

#include <stdint.h>
#include <math.h>
#include <vector>
#include <QString>
#include <QDateTime>
#include <ostream>
#include <tlelib/tlelib.h>
#include <gis/gis.h>
/*
 * PluginInfo::priv_data0首选处理的数据类型集
 * PluginInfo::priv_data1可以处理的数据类型集
 */
enum WTrackPluginDataType {
	TPDT_PE   =  0x01  ,  
	TPDT_PV   =  0x02  ,  
	TPDT_TLE  =  0x04  ,  
};
/*
 * 基本引导数据
 */
struct WTrackItem
{
	double time;    //北京时
	float  A;       //引导数据方位A,E为负表示没有引导数据,正北为0,正东90,...
	float  E;       //引导数据俯仰,水平为0,垂直90
	float  range;
	float  dr;
};
inline WTrackItem operator*(const WTrackItem& v,double k) {
	auto res = v;
	res.time *= k;
	res.A *= k;
	res.E *= k;
	res.range *= k;
	res.dr *= k;
	return res;
}
inline WTrackItem operator/(const WTrackItem& v,double k) {
	auto res = v;
	res.time /= k;
	res.A /= k;
	res.E /= k;
	res.range /= k;
	res.dr /= k;
	return res;
}
inline WTrackItem operator+(const WTrackItem& lhv,const WTrackItem& rhv) {
	auto res = lhv;
	res.time += rhv.time;
	res.A += rhv.A;
	res.E += rhv.E;
	res.range += rhv.range;
	res.dr += rhv.dr;
	return res;
}
inline WTrackItem operator-(const WTrackItem& lhv,const WTrackItem& rhv) {
	auto res = lhv;
	res.time -= rhv.time;
	res.A -= rhv.A;
	res.E -= rhv.E;
	res.range -= rhv.range;
	res.dr -= rhv.dr;
	return res;
}
struct WTrackData {
	std::string             info;
	long                    begin_time = 0; //当前引导数据所表示的开始时间(与站点位置相关)
	long                    end_time   = 0; //当前引导数据所表示的结束时间(与站点位置相关)
	std::vector<WTrackItem> data;

	inline void removeDataBeforTime(time_t t) {
		if(t<=begin_time) return;
		auto  it  = std::find_if(data.begin(),data.end(),[t](const WTrackItem &v) { return t<v.time; });
		if(it == data.end()) {
			return;
		}
		data.erase(data.begin(),it);
		begin_time = t;
	}
	/*
	 * [begin,end)指向src.data的一个区间
	 */
	template<class Iter>
		inline void assign(const WTrackData& src, Iter& begin, Iter& end) {

			data.assign(begin,end);
			if(begin == end) {
				begin_time = end_time = 0;
				return;
			}

			if(begin == src.data.begin()) {
				begin_time = src.begin_time;
			} else {
				begin_time = begin->time;
			}
			if(end == src.data.end()) {
				end_time = src.end_time;
			} else {
				end_time = end->time;
			}
		}
	inline WTrackData& operator=(const WTrackData& d) {
		begin_time  =  d.begin_time;
		end_time    =  d.end_time;
		data        =  d.data;
		return *this;
	}
	inline WTrackData& operator=(WTrackData&& d) {
		begin_time  =  d.begin_time;
		end_time    =  d.end_time;
		data        =  std::move(d.data);
		return *this;
	}
	inline WTrackData& operator+=(const WTrackData& d) {
		if(d.end_time <= end_time) return *this;
		auto it =d.data.begin();
		if(d.begin_time < end_time) {
			it = std::find_if(d.data.begin(),d.data.end(),[this](const WTrackItem& v) { return end_time<v.time; });
		}
		data.insert(data.end(),it,d.data.end());
		end_time = d.end_time;
		return *this;
	}
	inline bool empty()const { return data.empty(); }
	inline void clear() { begin_time=end_time=0; data.clear(); }
};
/*
 * 数引数据偏置
 */
struct DGDataOffset{
	int64_t time;    //时间偏置微秒数
	float   aa;      //方位偏置度数,正北0,正东90
	float   pa;      //俯仰偏置度数,水平0,过顶90
	float   theta;   //横滚
};
/*
 * 轨道根数
 */
struct WEphemerisData
{
	QString   sat_name;
	QDateTime epoch_time;   //历元时
	uint32_t  pathway;      //轨道圈号
	double    a;            //轨道半径
	double    e;            //轨道偏心率
	double    i;            //轨道倾角
	double    O;            //升交点赤径(黄道经度)
	double    w;            //近地点幅角
	double    M;            //平近点角
	inline bool isNull()const {
		return epoch_time.isNull() || epoch_time<QDateTime(QDate(2000,1,1)) || a<6000000;
	}
	inline bool isNull(const QDateTime& time_line)const {
		return epoch_time.isNull() || epoch_time<time_line;
	}
	inline void clear() {
		epoch_time  =  QDateTime();
		sat_name    =  QString();
	}
};
struct WRawEphemerisData
{
	QDateTime epoch_time;   //历元时
	uint32_t  sat_id;
	uint32_t  pathway;      //轨道圈号
	double    a;            //轨道半径(m)
	double    e;            //轨道偏心率
	double    i;            //轨道倾角(度)
	double    O;            //升交点赤径(黄道经度)
	double    w;            //近地点幅角(度)
	double    M;            //平近点角(度)
	inline bool isNull()const {
		return epoch_time.isNull() || epoch_time<QDateTime(QDate(2000,1,1)) || a<6000000;
	}
	inline bool isNull(const QDateTime& time_line)const {
		return epoch_time.isNull() || epoch_time<time_line;
	}
};
inline std::ostream& operator<<(std::ostream& s,const WEphemerisData& v) {
	s<<std::fixed;
	if(v.isNull()) {
		s<<"NULL";
	} else {
		s<<"{"<<v.sat_name.toUtf8().data()<<" ep data:"<<v.epoch_time.toString("yyyy-MM-dd hh:mm:ss").toUtf8().data()<<","<<v.pathway<<","<<v.a<<","<<v.e<<","<<v.i<<","<<v.O<<","<<v.w<<","<<v.M<<"}";
	}
	return s;
}
struct RawSatPosVelocity
{
	uint32_t  sat_id;        //卫星标识
	QDateTime epoch_time;    //历元时
	double    pos[3];        //单位米
	double    velocity[3];   //m/s
};
struct WSatPosVelocity
{
	QString   sat_name;
	QDateTime epoch_time;    //历元时
	double    pos[3];        //单位米
	double    velocity[3];   //m/s
	inline bool isNull()const {
		return epoch_time.isNull() || epoch_time<QDateTime(QDate(2000,1,1));
	}
	inline bool isNull(const QDateTime& time_line)const {
		return epoch_time.isNull() || epoch_time<time_line;
	}
	inline void clear(){
		epoch_time  =  QDateTime();
		sat_name    =  QString();
	}
};
inline std::ostream& operator<<(std::ostream& s,const WSatPosVelocity& v) {
	s<<std::fixed;
	if(v.isNull()) {
		s<<"NULL";
	} else {
		s<<"{"<<v.sat_name.toUtf8().data()<<" pv:"<<v.epoch_time.toString("yyyy-MM-dd hh:mm:ss").toUtf8().data();
		for(int i=0; i<3; ++i) {
			s<<","<<v.pos[i];
		}
		for(int i=0; i<3; ++i) {
			s<<","<<v.velocity[i];
		}
		s<<"}";
	}
	return s;
}
struct WTLE
{
	std::string data[3];
	inline QDateTime epoch_time()const {
		if(isNull()) return QDateTime();
		try {
			tlelib::Node node(data[0],data[1],data[2]);
			return QDateTime::fromTime_t(node.preciseEpoch());
		} catch(...) {
		}
		return QDateTime();
	}
	inline bool isNull()const {
		return (data[0].size()<2)||(data[1].size()<10);
	}
	inline bool isNull(const QDateTime& d)const
	{
		auto et = epoch_time();
		return et.isNull() || (et<d);
	}
	inline void clear(){
		for(auto& d:data)d.clear();
	};
};
inline std::ostream& operator<<(std::ostream& s,const WTLE& v) {
	if(v.isNull()) {
		s<<"NULL";
	} else {
		s<<"{"<<std::endl;
		s<<"TLE epoch time:"<<v.epoch_time().toString("yyyy-MM-dd hh:mm:ss").toUtf8().data()<<std::endl;
		for(auto& d:v.data)
			s<<d<<std::endl;
		s<<"}";
	}
	return s;
}
struct WSatStateData
{
	WEphemerisData  ep_data;
	WSatPosVelocity pos_data;
	WTLE            tle_data;

	inline bool isNull()const {
		return ep_data.isNull()&&pos_data.isNull()&&tle_data.isNull();
	}
	inline bool isNull(const QDateTime& time_line)const {
		return ep_data.isNull(time_line)&&pos_data.isNull(time_line)&&tle_data.isNull(time_line);
	}
	inline QDateTime epoch_time()const {
		if(ep_data.epoch_time.isNull()) return pos_data.epoch_time;
		if(pos_data.epoch_time.isNull()) return ep_data.epoch_time;
		return std::max(ep_data.epoch_time,pos_data.epoch_time);
	}
	/*
	 * 星历历元时越旧的越小
	 */
	inline bool operator<(const WSatStateData& other)const {
		if(epoch_time().isNull()) return true;
		if(other.epoch_time().isNull()) return false;
		return epoch_time()<other.epoch_time();
	}
	inline void clear() {
		ep_data.clear();
		pos_data.clear();
		tle_data.clear();
		bzero(priority,sizeof(priority));
	}
	inline QString satName()const {
		for(auto t:priority) {
			if(0 == t) break;
			switch(t) {
				case TPDT_PE:
					if(!ep_data.sat_name.isEmpty())
						return ep_data.sat_name;
					break;
				case TPDT_PV:
					if(!pos_data.sat_name.isEmpty())
						return pos_data.sat_name;
					break;
				case TPDT_TLE:
					if(!tle_data.data[0].empty())
						return tle_data.data[0].c_str();
					break;
			}
		}
		return QString();
	}
	unsigned priority[3]={0}; //priority[0]的优先级最大，priority[2]最低数据处理的优先顺序= WTrackPluginDataType
};
inline std::ostream& operator<<(std::ostream& s,const WSatStateData& v) {
	if(v.isNull()) {
		s<<"NULL";
	} else {
		s<<"{"<<std::endl;
		if(!v.ep_data.isNull()) {
			s<<v.ep_data;
			s<<std::endl;
		}
		if(!v.pos_data.isNull()) {
			s<<v.pos_data;
			s<<std::endl;
		}
		if(!v.tle_data.isNull()) {
			s<<v.tle_data;
			s<<std::endl;
		}
		s<<"}";
	}
	return s;
}
struct TPForecastItem
{
	long  time;
	float range;
	float A;
	float E;
	float dr;
};
struct TPForecast
{
	TPForecastItem data[3]; //分别为进站，过顶，出站
};
struct TPForecasts {
	long                    begin_time;   //预报的开始时间(与站点位置相关)
	long                    end_time;     //预报的结束时间(与站点位置相关)
	std::vector<TPForecast> data;

	inline bool empty()const { return data.empty(); }
	inline void clear() { begin_time=end_time=0; data.clear(); }
};
inline bool operator==(const WEphemerisData& lhv,const WEphemerisData& rhv)noexcept
{
	auto delta = 1E-7;
	if(lhv.epoch_time != rhv.epoch_time) return false;
	if(fabs(lhv.a-rhv.a) > delta) return false;
	if(fabs(lhv.e-rhv.e) > delta) return false;
	if(fabs(lhv.i-rhv.i) > delta) return false;
	if(fabs(lhv.O-rhv.O) > delta) return false;
	if(fabs(lhv.w-rhv.w) > delta) return false;
	if(fabs(lhv.M-rhv.M) > delta) return false;
	return true;
}
inline bool operator!=(const WEphemerisData& lhv,const WEphemerisData& rhv)noexcept
{
	return !(lhv==rhv);
}
inline bool operator==(const WSatPosVelocity& lhv,const WSatPosVelocity& rhv)noexcept
{
	auto delta = 1E-7;
	if(lhv.epoch_time != rhv.epoch_time) return false;
	for(int i=0; i<3; ++i) {
		if(fabs(lhv.pos[i]-rhv.pos[i]) > delta) return false;
	}
	for(int i=0; i<3; ++i) {
		if(fabs(lhv.velocity[i]-rhv.velocity[i]) > delta) return false;
	}
	return true;
}
inline bool operator!=(const WSatPosVelocity& lhv,const WSatPosVelocity& rhv)noexcept
{
	return !(lhv!=rhv);
}
inline bool operator==(const WSatStateData& lhv,const WSatStateData& rhv)noexcept
{
	if(lhv.pos_data.epoch_time> rhv.ep_data.epoch_time) {
		return lhv.pos_data == rhv.pos_data;
	} else {
		return lhv.ep_data == rhv.ep_data;
	}
}
inline bool operator!=(const WSatStateData& lhv,const WSatStateData& rhv)noexcept
{
	return !(lhv==rhv);
}
struct WStationData
{
	QString name;
	double  pos[3];   /*依次为经度longi,纬度lati,高程,单位为度，米*/
};
struct WTimeBucket
{
	time_t begin;
	time_t end;
};
inline std::ostream& operator<<(std::ostream& s,const WStationData& v) {
	s<<"Station{"<<v.name.toUtf8().data()<<std::fixed;
	for(auto&p :v.pos)
		s<<","<<p;
	s<<"}";
	return s;
}
