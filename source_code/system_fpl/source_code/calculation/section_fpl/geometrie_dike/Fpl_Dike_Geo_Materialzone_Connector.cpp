#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Dike_Geo_Materialzone_Connector.h"

//Default constructor
Fpl_Dike_Geo_Materialzone_Connector::Fpl_Dike_Geo_Materialzone_Connector(void){
	this->perc=0.0;
	this->ptr_mat_zone=NULL;
	this->second_info=0.0;
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Dike_Geo_Materialzone_Connector), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Dike_Geo_Materialzone_Connector::~Fpl_Dike_Geo_Materialzone_Connector(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Dike_Geo_Materialzone_Connector), _sys_system_modules::FPL_SYS);//count the memory
}
//__________
//public
//Get the percentage of the material zone
double Fpl_Dike_Geo_Materialzone_Connector::get_percentage_mat_zone(void){
	return this->perc;
}
//Get the secondary information
double Fpl_Dike_Geo_Materialzone_Connector::get_secondary_info(void){
	return this->second_info;
}
//Get the pointer to the material zone
Fpl_Dike_Geo_Materialzone *Fpl_Dike_Geo_Materialzone_Connector::get_ptr_mat_zone(void){
	return this->ptr_mat_zone;
}
//Get the percentage of the material zone
void Fpl_Dike_Geo_Materialzone_Connector::set_percentage_mat_zone(const double percentage){
	 this->perc=percentage;
}
//Set the secondary information
void Fpl_Dike_Geo_Materialzone_Connector::set_secondary_info(const double second_info){
	this->second_info=second_info;
}
//Get the pointer to the material zone
void Fpl_Dike_Geo_Materialzone_Connector::set_ptr_mat_zone(Fpl_Dike_Geo_Materialzone *zone){
	this->ptr_mat_zone=zone;
}
//Reset the zone connector
void Fpl_Dike_Geo_Materialzone_Connector::reset_connector(void){
	this->perc=0.0;
	this->ptr_mat_zone=NULL;
}