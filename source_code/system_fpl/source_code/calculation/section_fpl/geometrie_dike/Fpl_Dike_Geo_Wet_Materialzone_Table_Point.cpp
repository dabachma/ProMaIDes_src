#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Dike_Geo_Wet_Materialzone_Table_Point.h"


//Default constructor
Fpl_Dike_Geo_Wet_Materialzone_Table_Point::Fpl_Dike_Geo_Wet_Materialzone_Table_Point(void){

	this->waterlevel=0.0;
	this->area_perc=0.0;
	this->number_zones=0;
	this->ptr_zone_connector=NULL;


	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Dike_Geo_Wet_Materialzone_Table_Point), _sys_system_modules::FPL_SYS);//count the memory

}
//Default destructor
Fpl_Dike_Geo_Wet_Materialzone_Table_Point::~Fpl_Dike_Geo_Wet_Materialzone_Table_Point(void){
	this->delete_zone_connector();
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Dike_Geo_Wet_Materialzone_Table_Point), _sys_system_modules::FPL_SYS);//count the memory
}
//__________
//public
//Set the waterlevel
void Fpl_Dike_Geo_Wet_Materialzone_Table_Point::set_waterlevel(const double h){
	this->waterlevel=h;
}
//Get the waterlevel
double Fpl_Dike_Geo_Wet_Materialzone_Table_Point::get_waterlevel(void){
	return this->waterlevel;
}
//Set the percentage of the total area of the slice
void Fpl_Dike_Geo_Wet_Materialzone_Table_Point::set_perc_area(const double percentage_area){
	this->area_perc=percentage_area;
}
//Get the percentage of the total area of the slice
double Fpl_Dike_Geo_Wet_Materialzone_Table_Point::get_perc_area(void){
	return this->area_perc;
}
//Set and allocate the connected material zones
void Fpl_Dike_Geo_Wet_Materialzone_Table_Point::set_allocate_mat_zones(const int number){
	this->number_zones=number;
	this->allocate_zone_connector();
}
//Get the number of connected material zones
int Fpl_Dike_Geo_Wet_Materialzone_Table_Point::get_number_material_zones(void){
	return this->number_zones;
}
//Set the connected material zones
void Fpl_Dike_Geo_Wet_Materialzone_Table_Point::set_connected_material_zones(Fpl_Dike_Geo_Materialzone_Connector* zone_connector){
	for(int i=0; i< this->number_zones; i++){
		this->ptr_zone_connector[i].set_percentage_mat_zone(zone_connector[i].get_percentage_mat_zone());
		this->ptr_zone_connector[i].set_ptr_mat_zone(zone_connector[i].get_ptr_mat_zone());

	}
}
//Copy the table point
void Fpl_Dike_Geo_Wet_Materialzone_Table_Point::copy_table_point(Fpl_Dike_Geo_Wet_Materialzone_Table_Point *src){
	this->waterlevel=src->get_waterlevel();
	this->area_perc=src->get_perc_area();
	this->number_zones=src->get_number_material_zones();
	this->allocate_zone_connector();
	for(int i=0; i< this->number_zones; i++){
		this->ptr_zone_connector[i].set_percentage_mat_zone(src->get_ptr_connected_mat_zones(i)->get_percentage_mat_zone());
		this->ptr_zone_connector[i].set_ptr_mat_zone(src->get_ptr_connected_mat_zones(i)->get_ptr_mat_zone());
	}
}
//Get a pointer to the material zones
Fpl_Dike_Geo_Materialzone_Connector* Fpl_Dike_Geo_Wet_Materialzone_Table_Point::get_ptr_connected_mat_zones(const int index){
	if(index<0 || index>=this->number_zones){
		return NULL;
	}
	else{
		return &this->ptr_zone_connector[index];
	}
}
//_________
//private
//Allocate the material zone connector
void Fpl_Dike_Geo_Wet_Materialzone_Table_Point::allocate_zone_connector(void){
	this->delete_zone_connector();
	try{
		this->ptr_zone_connector=new Fpl_Dike_Geo_Materialzone_Connector[this->number_zones];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the the material zone connector
void Fpl_Dike_Geo_Wet_Materialzone_Table_Point::delete_zone_connector(void){
	if(this->ptr_zone_connector!=NULL){
		delete []this->ptr_zone_connector;
		this->ptr_zone_connector=NULL;
	}
}
//Set the error(s)
Error Fpl_Dike_Geo_Wet_Materialzone_Table_Point::set_error(const int err_type){
	string place="Fpl_Dike_Geo_Wet_Materialzone_Table_Point::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("allocate_zone_connector(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		default:
			place.append("set_error(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;

}