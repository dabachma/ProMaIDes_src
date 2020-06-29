//#include "_Geo_Geometrie.h"
#include "Sys_Headers_Precompiled.h"
//static members

//Default constructor
_Geo_Geometrie::_Geo_Geometrie(void){
}
//Default constructor
_Geo_Geometrie::~_Geo_Geometrie(void){
}
//__________
//public
//Convert grad to rad (static)
double _Geo_Geometrie::grad_to_rad(const double grad){
	double rad=0.0;
	rad=grad*constant::Cpi/180.0;
	return rad;
}
//Convert rad to grad (static)
double _Geo_Geometrie::rad_to_grad(const double rad){
	double grad=0.0;
	grad=rad*180.0/constant::Cpi;
	return grad;
}
//Convert any given angle to 0 to 360° (static)
double _Geo_Geometrie::angle_to_360(const double angle){
	double angle_return=0.0;
	if(angle>=0.0 && angle<360.0){
		angle_return=angle;
	}
	else{
		double int_part=0.0;
		modf((abs(angle)/360.0), &int_part);
		if(angle>0.0){
			
			angle_return=angle-int_part*360.0;
		}
		else{
			angle_return=360.0+(angle+int_part*360.0);
			if(angle_return==360.0){
				angle_return=0.0;
			}
		}
	}
	return angle_return;
}