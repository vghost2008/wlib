/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
#include <gis.h>
#include <wmath.h>
#include <boost/geometry.hpp>
#include <fstream>
#include <wgeometry.h>
#include <gmatrix.h>

using namespace std;

namespace bg=boost::geometry;
namespace WSpace
{
	template<typename T>
	T correctDirectionRef(T& v) {
		while(v>360)v-=360;
		while(v<0)v+=360;
		return v;
	}
	template<typename T>
	T correctDirection(T v) {
		while(v>360)v-=360;
		while(v<0)v+=360;
		return v;
	}
	GPSPoint& correct(GPSPoint* v) {

		if(isInRange<double>(v->x,-180,180)&& isInRange<double>(v->y,-90,90)) return *v;

		const int k = v->y/360;
		v->y -= k*360;
		if(v->y > 90 ) {
			if(v->y > 90 && v->y<= 180) {
				v->y = 180 - v->y;
				v->x += 180;
			} else if(v->y > 180 && v->y<=270) {
				v->y =  180-v->y;
				v->x += 180;
			} else if(v->y > 270) {
				v->y =  v->y-360;
			}
		} else if(v->y < -90) {
			if(v->y < -90 && v->y >=  -180) {
				v->y = -180 - v->y;
				v->x += 180;
			} else if(v->y < -180 && v->y>=-270) {
				v->y =  -180-v->y;
				v->x += 180;
			} else if(v->y < -270) {
				v->y =  360+v->y;
			}
		}
		if(isInRange<double>(v->x,-180,180)) return *v;
		if(v->x > 180) {
			const int k0 = (v->x+180)/360;
			v->x -= k0*360;
		} else if(v->x < -180) {
			const int k0 = (-v->x-180)/360;
			v->x -= k0*360;
		}
		return *v;
	}
	double azimuth(const GPSPoint& p0,const GPSPoint& p1) {
		return azimuth(p0.x,p0.y,p1.x,p1.y);
	}
	double azimuth(const WSubStarPoint& p0,const WSubStarPoint& p1) {
		return azimuth(p0.longi,p0.lati,p1.longi,p1.lati);
	}
	double azimuth(double x0, double y0, double x1,double y1) {
		if(x0 > 90 && x1 <-90) 
			x1 += 360;
		else if(x0 < -90 && x1 > 90) 
			x1 -= 360;
		auto direction = atan2(y1-y0,x1-x0);
		return 90-r2d(direction);
	}
	bool getGPSPointByDistanceAndAzimuth(double longi,double sati,double distance,double azimuth,double* longo,double* lato)
	{
		auto azimuth_r = d2r(azimuth);
		correctRRef(azimuth_r);
		auto        res         = getPosOnShpere(d2r(longi),d2r(sati),distance/kEarthRadius,azimuth_r,longo,lato);
		if(false == res) return false;
		*longo  =  correctD(r2d(*longo));
		*lato   =  correctD(r2d(*lato));
		return true;
	}
	bool getSubStarStripe(const WSubStarPoints& trace,double radius,WSubStarStripe* stripe,bool round_end)
	{
		int                 i          = 0;
		float               direction  = 0;
		const int           round_num  = 20;
		vector<GPSPoint>    gps_points;
		WSubStarStripePoint temp_ssp;
		auto check_point = [](const WSubStarStripePoint& ref,WSubStarStripePoint* out) {
			if(((ref.longi<-90) && (out->longi>90))
					|| (( ref.longi>90) &&( out->longi<-90)))
				return false;
			return true;
		};

		if(trace.empty()) return false;

		stripe->data.clear();

		if(1 == trace.size()) {
			if(!getCircleOnEarth(trace.at(0).longi,trace.at(0).lati,radius,gps_points,round_end*2)) 
				return false;
			temp_ssp =  trace.at(0);
			for(auto& gp:gps_points) {
				temp_ssp.longi  =  gp.x;
				temp_ssp.lati   =  gp.y;
				if(check_point(trace.at(0),&temp_ssp))
					stripe->data.push_back(temp_ssp);
			}
			return true;	
		}

		i = 1;

		direction = azimuth(trace[i-1],trace[i]);
		correctDirectionRef(direction);

		if(round_end) {
			temp_ssp=  trace[0];
			for(int j=0; j<round_num; ++j) {
				auto td = direction+90+180.0*j/(round_num-1);
				correctDirectionRef(td);
				getGPSPointByDistanceAndAzimuth(trace[0],radius,td,&temp_ssp);
				if(check_point(trace.at(0),&temp_ssp))
					stripe->data.push_back(temp_ssp);
			}
		} else {
			temp_ssp=  trace[0];
			getGPSPointByDistanceAndAzimuth(trace[0],radius,correctDirection(direction+90),&temp_ssp);
			if(check_point(trace.at(0),&temp_ssp))
				stripe->data.push_back(temp_ssp);
			getGPSPointByDistanceAndAzimuth(trace[0],radius,correctDirection(direction+270),&temp_ssp);
			if(check_point(trace.at(0),&temp_ssp))
				stripe->data.push_back(temp_ssp);
		}

		for(i=1;i<trace.size(); ++i) {
			direction = azimuth(trace[i-1],trace[i])+270;

			temp_ssp=  trace[i];

			correctDirectionRef(direction);
			getGPSPointByDistanceAndAzimuth(trace[i],radius,direction,&temp_ssp);
			if(check_point(trace.at(i),&temp_ssp))
				stripe->data.push_back(temp_ssp);
		}

		if(round_end) {
			temp_ssp=  trace.back();
			for(int j=0; j<round_num; ++j) {
				auto td = direction+180.0*j/(round_num-1);
				correctDirectionRef(td);
				getGPSPointByDistanceAndAzimuth(trace.back(),radius,td,&temp_ssp);
				if(check_point(trace.back(),&temp_ssp))
					stripe->data.push_back(temp_ssp);
			}
		} else {
			temp_ssp=  trace.back();
			getGPSPointByDistanceAndAzimuth(trace.back(),radius,correctDirection(direction),&temp_ssp);
			if(check_point(trace.back(),&temp_ssp))
				stripe->data.push_back(temp_ssp);
			getGPSPointByDistanceAndAzimuth(trace.back(),radius,correctDirection(direction+180),&temp_ssp);
			if(check_point(trace.back(),&temp_ssp))
				stripe->data.push_back(temp_ssp);
		}

		for(i=trace.size()-1; i>1; --i) {
			temp_ssp=  trace[i-1];
			direction = azimuth(trace[i-1],trace[i])+90;
			correctDirectionRef(direction);
			getGPSPointByDistanceAndAzimuth(trace[i-1],radius,direction,&temp_ssp);
			if(check_point(trace.at(i-1),&temp_ssp))
				stripe->data.push_back(temp_ssp);
		}
		return true;
	}
	bool splitSubStarPoints(const WSubStarPoints& in,std::list<WSubStarPoints>* out)
	{
		out->clear();
		pointsClusteringByDistance(in,out);
		return true;
	}
	void correct(const WSubStarStripe& ref,WSubStarStripe* in_and_out)
	{
		constexpr auto zero = 1E-8;

		memcpy(in_and_out->color,ref.color,sizeof(ref.color));
		in_and_out->sat_index = ref.sat_index;

		if(ref.data.empty() || in_and_out->data.empty()) return;

		auto less = [](const WSubStarStripePoint& lhv,const WSubStarStripePoint& rhv){
			return lhv.longi< rhv.longi; };

		auto min_and_max  =  minmax_element(in_and_out->data.begin(),in_and_out->data.end(),less);
		const auto min_longi = min_and_max.first->longi;
		const auto max_longi = min_and_max.second->longi;
		auto it           =  min_element(ref.data.begin(),ref.data.end(),[min_longi](const WSubStarStripePoint& lhv,const WSubStarStripePoint& rhv) {
			return fabs(lhv.longi-min_longi)<fabs(rhv.longi-min_longi);
		});
		auto jt           =  min_element(ref.data.begin(),ref.data.end(),[max_longi](const WSubStarStripePoint& lhv,const WSubStarStripePoint& rhv) {
			return fabs(lhv.longi-max_longi)<fabs(rhv.longi-max_longi);
		});
		if(max_longi - min_longi < zero) {
			for(auto& p:in_and_out->data) {
				p.time = it->time;
				p.pass = it->pass;
				p.flag = it->flag;
			}
		} else {
			auto width      = max_longi-min_longi;
			auto time_width = double(jt->time-it->time);
			auto pass_width = double(jt->pass-it->pass);

			for(auto& p:in_and_out->data) {
				p.time = it->time+(p.longi-min_longi)*time_width/width;
				p.pass = it->pass+(p.longi-min_longi)*pass_width/width;
				p.flag = jt->flag;
			}
		}

	}
	/*
	 */
	void correct(const WSubStarPoints& ref,WSubStarPoints* in_and_out)
	{
		constexpr auto zero = 1E-8;

		if(ref.empty() || in_and_out->empty()) return;

		auto less = [](const WSubStarPoint& lhv,const WSubStarPoint& rhv){
			return lhv.longi< rhv.longi; };

		auto       min_and_max = minmax_element(in_and_out->begin(),in_and_out->end(),less);
		const auto min_longi   = min_and_max.first->longi;
		const auto max_longi   = min_and_max.second->longi;
		auto it           =  min_element(ref.begin(),ref.end(),[min_longi](const WSubStarPoint& lhv,const WSubStarPoint& rhv) {
			return fabs(lhv.longi-min_longi)<fabs(rhv.longi-min_longi);
		});
		auto jt           =  min_element(ref.begin(),ref.end(),[max_longi](const WSubStarPoint& lhv,const WSubStarPoint& rhv) {
			return fabs(lhv.longi-max_longi)<fabs(rhv.longi-max_longi);
		});
		if(max_longi - min_longi < zero) {
			for(auto& p:*in_and_out) {
				p.time = it->time;
				p.pass = it->pass;
				p.flag = it->flag;
			}
		} else {
			auto width      = max_longi-min_longi;
			auto time_width = double(jt->time-it->time);
			auto pass_width = double(jt->pass-it->pass);

			for(auto& p:*in_and_out) {
				p.time = it->time+(p.longi-min_longi)*time_width/width;
				p.pass = it->pass+(p.longi-min_longi)*pass_width/width;
				p.flag = it->flag;
			}
		}
	}
	QString toGPSString(double v,GPSCoordinateType type)
	{
		int  d  = v;
		auto s0 = (fabs(v)-fabs(d)) *60;
		int  f  = s0;
		auto s  = (s0-f) *60;

		return QString("%1°%2'%3\"").arg(d).arg(f).arg(s,0,'f',3);
	}
	double  toGPSValue(const QString& _str,GPSCoordinateType type)
	{
		auto str   = _str;
		auto index = str.indexOf("°");
		int  d     = str.left(index).toInt();
		auto pd    = std::abs(d);

		str    =  str.right(str.size()-index-1);
		index  =  str.indexOf("'");

		int f = str.left(index).toInt();

		str    =  str.right(str.size()-index-1);
		index  =  str.indexOf("\"");

		auto s = str.left(index).toDouble();
		return copysign(double(pd)+f/60.0+s/3600.0,d);
	}
	double satPeriod(double a)noexcept(false)
	{
		if(a <= 1) throw runtime_error("error a");

		a = a/1000;

		constexpr double GM = 398600.4415; //忽略卫星质量
		const double t = M_PI *2*sqrt(a*a*a/GM);

		return t;
	}
	GMatrix attitudeTransMatrixD(double h,double fai, double theta)
	{
		return attitudeTransMatrix(d2r(h),d2r(fai),d2r(theta));
	}
	GMatrix attitudeTransMatrix(double h,double fai, double theta)
	{
		/*
		 * 将ENU视为x,y,z
		 * 变换矩阵为Mry(-theta)*Mrx(-fai)*Mrz(-h)
		 * 即一个先通过Mrz(-h)消除方位角度，再通过Mrx(-fai)消除府仰角度，再通过Mry(theta)消除横滚角度
		 */
		GMatrix    trans     = GMatrix::identityMatrix();
		const auto sin_h     = sin(h);
		const auto cos_h     = cos(h);
		const auto sin_fai   = sin(fai);
		const auto cos_fai   = cos(fai);
		const auto sin_theta = sin(theta);
		const auto cos_theta = cos(theta);

		trans(0,0) = cos_h*cos_theta+sin_h*sin_fai*sin_theta;
		trans(0,1) = cos_h*sin_fai*sin_theta-sin_h*cos_theta;
		trans(0,2) = -cos_fai*sin_theta;
		trans(1,0) = sin_h*cos_fai;
		trans(1,1) = cos_fai*cos_h;
		trans(1,2) = sin_fai;
		trans(2,0) = cos_h*sin_theta-sin_h*sin_fai*cos_theta;
		trans(2,1) = -sin_h*sin_theta-cos_h*sin_fai*cos_theta;
		trans(2,2) = cos_fai*cos_theta;
		return trans;
	}
	bool attitudeTransD(const GMatrix& m,double a,double b,double* a_o,double* b_o)
	{
		if(!attitudeTrans(m,d2r(a),d2r(b),a_o,b_o)) return false;
		*a_o = r2d(*a_o);
		*b_o = r2d(*b_o);
		return true;
	}
	bool attitudeTrans(const GMatrix& m,double a,double b,double* a_o,double* b_o)
	{
		const auto e   = cos(b)*sin(a);
		const auto n   = cos(b)*cos(a);
		const auto u   = sin(b);
		double     e_o;
		double     n_o;
		double     u_o;

		if(!attitudeTrans(m,e,n,u,&e_o,&n_o,&u_o)) return false;

		*b_o = asin(u_o);

		const auto cos_b = sqrt(1.0-u_o*u_o);
		if(fabs(cos_b)<1E-8) {
			*a_o = 0;
			*b_o = M_PI_2;
			return true;
		}
		*a_o = atan2(n_o,e_o);
		*a_o = M_PI_2-(*a_o);
		if(*a_o < 0) *a_o += (M_PI*2.0);
		return true;
	}
	bool attitudeTrans(const GMatrix& m,double e,double n,double u,double* e_o,double* n_o,double* u_o)
	{
		GVector enu({e,n,u});
		GVector res;

		res = m*enu;

		*e_o = res(0);
		*n_o = res(1);
		*u_o = res(2);

		return true;
	}
}
