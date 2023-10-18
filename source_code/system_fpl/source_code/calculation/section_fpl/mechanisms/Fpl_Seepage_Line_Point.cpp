#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Seepage_Line_Point.h"


//Default constructor
Fpl_Seepage_Line_Point::Fpl_Seepage_Line_Point(void){

	this->x_coordinate=0.0;;
	this->waterlevel=0.0;
	this->inside_dike_body=false;
	this->y_cubature=0.0;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Seepage_Line_Point), _sys_system_modules::FPL_SYS);//count the memory
}
//Copy constructor
Fpl_Seepage_Line_Point::Fpl_Seepage_Line_Point(const Fpl_Seepage_Line_Point& object){

	this->x_coordinate=object.x_coordinate;
	this->waterlevel=object.waterlevel;
	this->inside_dike_body=object.inside_dike_body;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Seepage_Line_Point), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Seepage_Line_Point::~Fpl_Seepage_Line_Point(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Seepage_Line_Point), _sys_system_modules::FPL_SYS);//count the memory

}
//___________
//public
void Fpl_Seepage_Line_Point::set_x_coordinate(const double x){
	this->x_coordinate=x;	
}
//Set the waterlevel 
void Fpl_Seepage_Line_Point::set_waterlevel(const double h){
	this->waterlevel=h;
}
//Get the x_coordinate
double Fpl_Seepage_Line_Point::get_x_coordinate(void){
	return this->x_coordinate;
}
//Get the waterlevel
double Fpl_Seepage_Line_Point::get_waterlevel(void){
	return this->waterlevel;
}
//Set the flag, if the point is inside the dike body
void Fpl_Seepage_Line_Point::set_inside_dike_body_flag(const bool flag){
	this->inside_dike_body=flag;
}
//Get the flag, if the point is inside the dike body
bool Fpl_Seepage_Line_Point::get_inside_dike_body_flag(void){
	return this->inside_dike_body;
}
//Get the y-coordinate of the cubature
double Fpl_Seepage_Line_Point::get_y_coordinate_cubature(void){
	return this->y_cubature;
}
//Set the y-coordinate of the cubature
void Fpl_Seepage_Line_Point::set_y_coordinate_cubature(Geo_Polysegment *cubature){
	
	
	Geo_Straight_Line buff_vertikal;
	_geo_interception_point buff_point;
	buff_point.interception_point.set_point_name(label::interception_point);
	bool int_found=false;

	buff_vertikal.set_coordinates(this->x_coordinate);

	for(int i=0; i< cubature->get_number_segments(); i++){
		cubature->get_segment(i)->calc_interception(&buff_vertikal,&buff_point);
		if(buff_point.interception_flag==true){
			int_found=true;
			break;
		}	
	}
	if(int_found==false){
		Warning msg=this->set_warning(0);
		msg.output_msg(1);
	}
	else{
		this->y_cubature=buff_point.interception_point.get_ycoordinate();
	}


}
//copy operator
Fpl_Seepage_Line_Point& Fpl_Seepage_Line_Point::operator=(const Fpl_Seepage_Line_Point& object){
	this->x_coordinate=object.x_coordinate;
	this->waterlevel=object.waterlevel;
	this->inside_dike_body=object.inside_dike_body;
	this->y_cubature=object.y_cubature;
	return *this;
}
//__________
//private
//Set warning(s)
Warning Fpl_Seepage_Line_Point::set_warning(const int warn_type){
	string place="Fpl_Seepage_Line_Point::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://not all matzone connected
			place.append("set_y_coordinate_cubature(Geo_Polysegment *cubature)");
			reason="The x-coordinate of the seepage point is outside the cubature";
			help="Check the cubature";
			reaction="The coordinate is set to 0.0";
			type=5;
			break;
		default:
			place.append("set_warning(const int warn_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=5;
	}
	msg.set_msg(place,reason,help,reaction,type);
	msg.make_second_info(info.str());
	return msg;

}