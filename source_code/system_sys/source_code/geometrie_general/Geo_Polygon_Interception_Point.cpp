#include "Sys_Headers_Precompiled.h"
//#include "Geo_Polygon_Interception_Point.h"

//constructor
Geo_Polygon_Interception_Point::Geo_Polygon_Interception_Point(void){

	this->polygon_point=false;
	this->interception_point=false;
	this->inside_flag=false;
	this->index_other_list=-1;
	this->is_used=false;
	//common
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Polygon_Interception_Point)-sizeof(Geo_Point), _sys_system_modules::GEOSYS_SYS);//count the memory

}
//copy constructor
Geo_Polygon_Interception_Point::Geo_Polygon_Interception_Point(const Geo_Polygon_Interception_Point& object){
	Geo_Point::operator =(object);
	this->polygon_point=object.polygon_point;
	this->interception_point=object.interception_point;
	this->inside_flag=object.inside_flag;
	this->index_other_list=object.index_other_list;
	this->is_used=object.is_used;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Polygon_Interception_Point)-sizeof(Geo_Point), _sys_system_modules::GEOSYS_SYS);//count the memory	
}
//destructor
Geo_Polygon_Interception_Point::~Geo_Polygon_Interception_Point(void){
	//common
	Sys_Memory_Count::self()->minus_mem(sizeof(Geo_Polygon_Interception_Point)-sizeof(Geo_Point), _sys_system_modules::GEOSYS_SYS);//count the memory

}
//_____________________________
//public
//Get if the point is a polygon point
bool Geo_Polygon_Interception_Point::get_polygon_point_flag(void){
	return this->polygon_point;
}
//Set, that the point is a polygon point
void Geo_Polygon_Interception_Point::set_polygon_point_flag(const bool flag){
	this->polygon_point=flag;
}
//Get if the point is a interception point 
bool Geo_Polygon_Interception_Point::get_interception_point_flag(void){
	return this->interception_point;
}
//Set, that the point is a interception point
void Geo_Polygon_Interception_Point::set_interception_point_flag(const bool flag){
	this->interception_point=flag;
}
//Get if the point lies inside for a pure polygon point or the segment goes inside for a interception point
bool Geo_Polygon_Interception_Point::get_inside_flag(void){
	return this->inside_flag;
}
//Set that the point lies inside for a pure polygon point or the segment goes inside for a interception point
void Geo_Polygon_Interception_Point::set_inside_flag(const bool flag){
	this->inside_flag=flag;
}
//Get the index to the correspondig point of the second list
int Geo_Polygon_Interception_Point::get_index_corresponding_point(void){
	return this->index_other_list;
}
//Set the index to the correspondig point of the second list
void Geo_Polygon_Interception_Point::set_index_corresponding_point(const int index){
	this->index_other_list=index;
}
//Set the geometrical point data
void Geo_Polygon_Interception_Point::set_geometrical_point_data(Geo_Point *point){
	this->set_point(point);
}
//Reset the flags
void Geo_Polygon_Interception_Point::reset_flags(void){
	this->polygon_point=false;
	this->interception_point=false;
	this->inside_flag=false;
	this->index_other_list=-1;
}
//Get is used flag
bool Geo_Polygon_Interception_Point::get_is_used_flag(void){
	return this->is_used;
}
//Set is used flag
void Geo_Polygon_Interception_Point::set_is_used_flag(const bool is_used){
	this->is_used=is_used;	
}
//copy operator
Geo_Polygon_Interception_Point& Geo_Polygon_Interception_Point::operator=(const Geo_Polygon_Interception_Point& object){
	this->polygon_point=object.polygon_point;
	this->interception_point=object.interception_point;
	this->inside_flag=object.inside_flag;
	this->index_other_list=object.index_other_list;
	this->is_used=object.is_used;

	Geo_Point::operator =(object);
	return *this;
}