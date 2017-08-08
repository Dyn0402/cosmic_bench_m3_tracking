#ifndef acceptanceFunction_h
#define acceptanceFunction_h
#include <TH2D.h>
#include <set>
#include <vector>

using std::set;
using std::vector;

class acceptanceFunction{
	public:
		acceptanceFunction(double x_min_Up_,double x_max_Up_,double y_min_Up_,double y_max_Up_,double x_min_Down_,double x_max_Down_,double y_min_Down_,double y_max_Down_,double z_Up_x_,double z_Down_x_,double z_Up_y_,double z_Down_y_,double bench_angle_);
		~acceptanceFunction();
		void plot_3D();
		TH2D plot_XY(int nbin_x,double x1,double x2,int nbin_y,double y1,double y2, double z, double y_angle = 0);
		//TH2D plot_XY(int nbin_x, int nbin_y, double z, double y_angle = 0);
		double operator()(double x,double y,double z);
		double x_min_Up;
		double x_max_Up;
		double y_min_Up;
		double y_max_Up;
		double x_min_Down;
		double x_max_Down;
		double y_min_Down;
		double y_max_Down;
		double z_Up_x;
		double z_Down_x;
		double z_Up_y;
		double z_Down_y;
		double bench_angle;
};

class FreeSkyFunction{
	public:
		FreeSkyFunction(double x_min_,double x_max_,double y_min_,double y_max_,vector<double> z_, double bench_angle_);
		~FreeSkyFunction();
		void plot_3D();
		TH2D plot_PhiTheta(int nbin_phi,double phi1,double phy22,int nbin_theta,double theta1,double theta2,unsigned int mult);
		TH2D plot_PhiTheta(int nbin_phi, int nbin_theta, unsigned int mult);
		double operator()(double phi, double theta, double delta_z);
		double x_min;
		double x_max;
		double y_min;
		double y_max;
		double bench_angle;
		set<double> z;
};

#endif