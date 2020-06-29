//#include "Hyd_Profile_Segment.h"
#include "Hyd_Headers_Precompiled.h"

//constructor
Hyd_Profile_Segment::Hyd_Profile_Segment(void){
	this->n_value=0.0;
	this->activity_lu=1; 
	this->luXn_value=0.0;
	this->lu=0.0;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Profile_Segment)-(sizeof(Geo_Segment)),_sys_system_modules::HYD_SYS);//count the memory
}
//Copy constructor
Hyd_Profile_Segment::Hyd_Profile_Segment(const Hyd_Profile_Segment& object){
	Geo_Segment::operator =(object);
	this->n_value=object.n_value;
	this->activity_lu=object.activity_lu;
	this->luXn_value=object.luXn_value;
	this->lu=object.lu;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Profile_Segment)-(sizeof(Geo_Segment)),_sys_system_modules::HYD_SYS);//count the memory
}
//destructor
Hyd_Profile_Segment::~Hyd_Profile_Segment(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Profile_Segment)-(sizeof(Geo_Segment)),_sys_system_modules::HYD_SYS);//count the memory

}
//___________
//public
//set the specific members for a segment which is used in a river profile
void Hyd_Profile_Segment::set_profilespecific_members(const double n_value, const int activity){
	this->n_value=n_value;
	this->activity_lu=activity;
	this->calculate_specific_values();
}
//reclaculate the profile specific members
void Hyd_Profile_Segment::recalc_profilespecific_members(void){
	this->calculate_specific_values();
}
//Get the effective wetted diameter of this segment
double Hyd_Profile_Segment::get_wetted_diameter(void){
	return this->lu;
}
//Get the product of effective wetted diameter with the  material-value
double Hyd_Profile_Segment::get_luXn_value(void){
	return this->luXn_value;
}
//Get the effective wetted diameter
double Hyd_Profile_Segment::get_lu(void){
	return this->lu;
}
//Get the material-value
double  Hyd_Profile_Segment::get_mat_value(void){
	return this->n_value;
}
//Get the minimal y_coordinate
double Hyd_Profile_Segment::get_minimal_y_coordinate(void){
	double y_min;
	if(this->vertical_dist==0.0){
		y_min=this->point1.get_ycoordinate();
	}
	else if(this->point1.check_below_of(&this->point2)==true){
		y_min=this->point1.get_ycoordinate();
	}
	else{
		y_min=this->point2.get_ycoordinate();	
	}

	return y_min;

}
//Output the segment members
void Hyd_Profile_Segment::output_members(ostringstream *cout){
	*cout <<" x1 " << this->point1.get_xcoordinate() <<" y1 " << this->point1.get_ycoordinate();
	*cout <<" x2 " << this->point2.get_xcoordinate() << " y2 " << this->point2.get_ycoordinate()<< endl;
	if(this->vertical_flag==false){
		*cout <<" Gradient " << this->gradient << label::no_unit << endl;
	}
	else{
		*cout <<" Gradient infinite " << endl;
	}
	*cout <<" Horizontal distance " << this->horizontal_dist << label::m << endl;
	*cout <<" Vertical distance " << this->vertical_dist << label::m << endl;
	*cout <<" Distance " << this->distance << label::m << endl;
	*cout <<" n_value " << this->n_value << label::n_unit << endl;
	*cout <<" Activity of lu " << this->activity_lu << label::no_unit << endl;
	*cout <<" Wetted Radius " << this->lu << label::m << endl;
}
//Switch the points of the segment (point1->point2)
void Hyd_Profile_Segment::switch_points(void){
	Geo_Point buffer;
	buffer=this->point1;
	this->point1=this->point2;
	this->point2=buffer;
}
//operator copy
Hyd_Profile_Segment& Hyd_Profile_Segment::operator=(const Hyd_Profile_Segment& object){
	Geo_Segment::operator =(object);
	this->n_value=object.n_value;
	this->activity_lu=object.activity_lu;
	this->luXn_value=object.luXn_value;
	this->lu=object.lu;
	return *this;
}

//_______________________
//private
//Calculate the specific values, e.g. wetted diameter
void Hyd_Profile_Segment::calculate_specific_values(void){
	this->lu=this->distance*this->activity_lu;
	this->luXn_value=this->lu*pow(this->n_value,1.5);
}