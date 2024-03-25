#include "Dam_Headers_Precompiled.h"
//#include "Dam_Hydimport_Element.h"

//default constructor
Dam_Hydimport_Element::Dam_Hydimport_Element(void){
	//init
	this->index=0;
	this->dhdt=0.0;
	this->duration_wet=0.0;
	this->height=0.0;
	this->velocity=0.0;
	this->waterlevel=0.0;
	this->mid_point.set_point_name(label::elem_mid);
	this->area=0.0;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Hydimport_Element), _sys_system_modules::DAM_SYS);
}
//default destructor
Dam_Hydimport_Element::~Dam_Hydimport_Element(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Hydimport_Element), _sys_system_modules::DAM_SYS);

}
//____________
//public
//Set the index of the element
void Dam_Hydimport_Element::set_index(const int index){
	this->index=index;
}
//Set the relative waterlevel in the element
void Dam_Hydimport_Element::set_waterlevel(const double waterlevel){
	this->waterlevel=waterlevel;
}
//Set the maximum flow velocity in the element
void Dam_Hydimport_Element::set_velocity(const double velocity){
	this->velocity=velocity;
}
//Set the duration of flooding in the element
void Dam_Hydimport_Element::set_duration_wet(const double duration_wet){
	this->duration_wet=duration_wet;
}
//Set the maximum speed of the waterlevel rise in the element
void Dam_Hydimport_Element::set_dhdt(const double dhdt){
	this->dhdt=dhdt;
}
//Set the geodetic height of the element
void Dam_Hydimport_Element::set_height(const double height){
	this->height=height;
}
//Set the area of the element
void Dam_Hydimport_Element::set_area_element(const double area){
	this->area=area;
}
//Get the midpoint of the element
Geo_Point* Dam_Hydimport_Element::get_mid_point(void){
	return &this->mid_point;
}
//Get the index of the element
int Dam_Hydimport_Element::get_index(void){
	return this->index;
}
//Get the relative waterlevel in the element
double Dam_Hydimport_Element::get_waterlevel(void){
	return this->waterlevel;
}
//Get the maximum flow velocity in the element
double Dam_Hydimport_Element::get_velocity(void){
	return this->velocity;
}
//Get the duration of flooding in the element
double Dam_Hydimport_Element::get_duration_wet(void){
	return this->duration_wet;
}
//Get the maximum speed of the waterlevel rise in the element
double Dam_Hydimport_Element::get_dhdt(void){
	return this->dhdt;
}
//Get the geodetic height of the element
double Dam_Hydimport_Element::get_height(void){
	return this->height;
}
//Get the area of the element
double Dam_Hydimport_Element::get_area_element(void){
	return this->area;
}


//____________
//private
