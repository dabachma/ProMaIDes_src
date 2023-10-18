#include "Sys_Headers_Precompiled.h"
//#include "Geo_Complex_Polygon.h"


//Default constructor
Geo_Complex_Polygon::Geo_Complex_Polygon(void){


	//common
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Complex_Polygon)-sizeof(Geo_Simple_Polygon), _sys_system_modules::GEOSYS_SYS);//count the memory
}
//Default destructor
Geo_Complex_Polygon::~Geo_Complex_Polygon(void){

	//common
	Sys_Memory_Count::self()->minus_mem(sizeof(Geo_Complex_Polygon)-sizeof(Geo_Simple_Polygon), _sys_system_modules::GEOSYS_SYS);//count the memory

}
//___________________
//public
//Set Geo_Points of the polygon
void Geo_Complex_Polygon::set_points(Geo_Point *points){

	//check the points
	try{
		this->check_points(points);
	}
	catch(Error msg){
		throw msg;
	}

	//set the segments
	for (int i=0; i<this->number_segments-1; i++){
		this->segments[i].set_points(&(points[i]),&(points[i+1]));
	}
	//last segment is connect to the first point=> polygon is closed
	this->segments[this->number_segments-1].set_points(&(points[this->number_segments-1]),&(points[0]));



}
//Set the points of the polygon
void Geo_Complex_Polygon::set_points(Geo_Point_List *point_list){
	if(point_list->get_number_points()<3){
		return;
	}

	try{
		this->set_number_points(point_list->get_number_points());
		this->set_points(point_list->get_total_list());
	}
	catch(Error msg){
		throw msg;
	}
}