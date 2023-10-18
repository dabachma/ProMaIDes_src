#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Dike_Geo_Wet_Materialzone_Table.h"

//Default constructor
Fpl_Dike_Geo_Wet_Materialzone_Table::Fpl_Dike_Geo_Wet_Materialzone_Table(void):block_elems(5){

	this->number_points=0;
	this->max_number_mat_zone=0;
	this->last_index=0;
	this->table_points=NULL;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Dike_Geo_Wet_Materialzone_Table), _sys_system_modules::FPL_SYS);//count the memory

}
//Default destructor
Fpl_Dike_Geo_Wet_Materialzone_Table::~Fpl_Dike_Geo_Wet_Materialzone_Table(void){
	this->delete_table_points();
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Dike_Geo_Wet_Materialzone_Table), _sys_system_modules::FPL_SYS);//count the memory
}
//__________
//public
//Get the number of table points
int Fpl_Dike_Geo_Wet_Materialzone_Table::get_number_table_points(void){
	return this->number_points;
}
//Set the number of the maximum of connected material zones
void Fpl_Dike_Geo_Wet_Materialzone_Table::set_number_connected_mat_zones(const int max_number_connected){
	this->max_number_mat_zone=max_number_connected;
	this->interpol_point.set_allocate_mat_zones(this->max_number_mat_zone);
}
//Set a new table point
void Fpl_Dike_Geo_Wet_Materialzone_Table::add_table_point(const double waterlevel, const double perc_area, Fpl_Dike_Geo_Materialzone_Connector* zone_connector){

	if(this->number_points!=0 && this->number_points % this->block_elems!=0){

		
		this->table_points[this->number_points].set_waterlevel(waterlevel);
		this->table_points[this->number_points].set_perc_area(perc_area);
		this->table_points[this->number_points].set_allocate_mat_zones(this->max_number_mat_zone);
		this->table_points[this->number_points].set_connected_material_zones(zone_connector);
		this->number_points++;


	}
	else{

		//allocate a buffer
		Fpl_Dike_Geo_Wet_Materialzone_Table_Point *buffer=NULL;
		try{
			int full_blocks=(int)((this->number_points+1)/this->block_elems);
			if(((this->number_points+1)-full_blocks*this->block_elems)>0){
				full_blocks++;
			}
			buffer=new Fpl_Dike_Geo_Wet_Materialzone_Table_Point[full_blocks*this->block_elems];
			//buffer=new Geo_Polygon_Interception_Point[this->number];
		}
		catch(bad_alloc &){
			Error msg=this->set_error(0);
			throw msg;
		}
		//copy it
		for(int i=0; i< this->number_points;i++){
			buffer[i].copy_table_point(&this->table_points[i]);
		}
		//delete
		this->delete_table_points();
		//count it
		this->number_points++;

		this->table_points=buffer;

		//set the new one
		this->table_points[this->number_points-1].set_waterlevel(waterlevel);
		this->table_points[this->number_points-1].set_perc_area(perc_area);
		this->table_points[this->number_points-1].set_allocate_mat_zones(this->max_number_mat_zone);
		this->table_points[this->number_points-1].set_connected_material_zones(zone_connector);

	}


	//Fpl_Dike_Geo_Wet_Materialzone_Table_Point *buffer=NULL;
	//try{
	//	buffer=new Fpl_Dike_Geo_Wet_Materialzone_Table_Point[this->number_points+1];
	//}
	//catch(bad_alloc &t){
	//	Error msg=this->set_error(0);
	//	ostringstream info;
	//	info<< "Info bad alloc: " << t.what() << endl;
	//	msg.make_second_info(info.str());
	//	throw msg;
	//}

	////copy the points
	//for(int i=0; i< this->number_points; i++){
	//	buffer[i].copy_table_point(&this->table_points[i]);
	//}
	//
	////set the last point
	//buffer[this->number_points].set_waterlevel(waterlevel);
	//buffer[this->number_points].set_perc_area(perc_area);
	//buffer[this->number_points].set_allocate_mat_zones(this->max_number_mat_zone);
	//buffer[this->number_points].set_connected_material_zones(zone_connector);
	//this->number_points++;

	////delete the old table points
	//this->delete_table_points();
	////set the new pointer to the table points
	//this->table_points=buffer;


}
//Get a pointer to table point
Fpl_Dike_Geo_Wet_Materialzone_Table_Point *Fpl_Dike_Geo_Wet_Materialzone_Table::get_ptr_table_point(const int index){
	if(index <0 || index >=this->number_points){
		return NULL;
	}
	else{
		return &this->table_points[index];
	}
}
//Interpolate a given waterlevel
Fpl_Dike_Geo_Wet_Materialzone_Table_Point *Fpl_Dike_Geo_Wet_Materialzone_Table::interpolate_table_values(const double water_h){
	
	//check if value to interploated is smaller than min value
	if(water_h < this->table_points[0].get_waterlevel()){
		Error msg=this->set_error(1);
		throw msg;
		return NULL;
	}
	else if(water_h == this->table_points[0].get_waterlevel()){
		return &this->table_points[0];
	}
	//value to extrapolate is greater than max value in table
	if(water_h>=this->table_points[this->number_points-1].get_waterlevel()){
		return &this->table_points[this->number_points-1];
	}
	//value to interploated is greater than the old value
	if(water_h>=this->table_points[this->last_index].get_waterlevel()){
		return this->interpolateupwards_value(water_h);
	}
	//value to interploated is greater than the old value
	if(water_h<this->table_points[this->last_index].get_waterlevel()){
		return this->interpolatedownwards_value(water_h);
	}

	//interpolation problem
	Error msg=this->set_error(2);
	throw msg;
	return NULL;	
}
//Sort the table after a ascending waterlevel level
void Fpl_Dike_Geo_Wet_Materialzone_Table::sort_table_waterlevel(void){
	//make the buffer for sorting
	Fpl_Dike_Geo_Wet_Materialzone_Table_Point swap;
	int j=0;

	for (int i = 1; i < this->number_points; i++) {
		swap.copy_table_point(&this->table_points[i]);
		//the higher value goes first => ">" ??
		//the smaller value goes first => "<" 
		j = i-1;
		while ( j >= 0 && swap.get_waterlevel() < this->table_points[j].get_waterlevel()){
			this->table_points[j+1].copy_table_point(&this->table_points[j]);
			j--;
		}
		this->table_points[j+1].copy_table_point(&swap);
	}
	
}
//_________
//private
//Delete the table points
void Fpl_Dike_Geo_Wet_Materialzone_Table::delete_table_points(void){
	if(this->table_points!=NULL){
		delete []this->table_points;
		this->table_points=NULL;
	}
}
//Find the value to interpolate via a downwards search
Fpl_Dike_Geo_Wet_Materialzone_Table_Point *Fpl_Dike_Geo_Wet_Materialzone_Table::interpolatedownwards_value(const double water_h){

	
	
	int node=this->last_index;
	double interpolated_result;
	//search exact
	while(water_h <= this->table_points[node].get_waterlevel()){
		node=node-1;
	}
	//now interpolation
	this->interpol_point.set_waterlevel(water_h);
	//linear interploation scheme
	interpolated_result=this->table_points[node].get_perc_area()+ (this->table_points[node+1].get_perc_area()-this->table_points[node].get_perc_area())
		/ (this->table_points[node+1].get_waterlevel() -this->table_points[node].get_waterlevel())*(water_h-this->table_points[node].get_waterlevel());
	this->interpol_point.set_perc_area(interpolated_result);
	for(int i=0; i<this->interpol_point.get_number_material_zones(); i++){
		interpolated_result=this->table_points[node].get_ptr_connected_mat_zones(i)->get_percentage_mat_zone()+ 
			(this->table_points[node+1].get_ptr_connected_mat_zones(i)->get_percentage_mat_zone()-this->table_points[node].get_ptr_connected_mat_zones(i)->get_percentage_mat_zone())
		/ (this->table_points[node+1].get_waterlevel() -this->table_points[node].get_waterlevel())*(water_h-this->table_points[node].get_waterlevel());
		
		this->interpol_point.get_ptr_connected_mat_zones(i)->set_percentage_mat_zone(interpolated_result);
		this->interpol_point.get_ptr_connected_mat_zones(i)->set_ptr_mat_zone(this->table_points[node].get_ptr_connected_mat_zones(i)->get_ptr_mat_zone());
	}
	//set the old node
	this->last_index=node;
	
	
	
	return &this->interpol_point;
}
//Find the value to interpolate via a upwards search
Fpl_Dike_Geo_Wet_Materialzone_Table_Point *Fpl_Dike_Geo_Wet_Materialzone_Table::interpolateupwards_value(const double water_h){
	
	int node=this->last_index;
	double interpolated_result;
	//search exact
	while(water_h >= this->table_points[node].get_waterlevel()){
		node=node+1;
	}
	//now interpolation
	this->interpol_point.set_waterlevel(water_h);
	//linear interploation scheme
	interpolated_result=this->table_points[node-1].get_perc_area()+ (this->table_points[node].get_perc_area()-this->table_points[node-1].get_perc_area())
		/ (this->table_points[node].get_waterlevel() -this->table_points[node-1].get_waterlevel())*(water_h-this->table_points[node-1].get_waterlevel());
	this->interpol_point.set_perc_area(interpolated_result);
	
	for(int i=0; i<this->interpol_point.get_number_material_zones(); i++){
		interpolated_result=this->table_points[node-1].get_ptr_connected_mat_zones(i)->get_percentage_mat_zone()+ 
			(this->table_points[node].get_ptr_connected_mat_zones(i)->get_percentage_mat_zone()-this->table_points[node-1].get_ptr_connected_mat_zones(i)->get_percentage_mat_zone())
		/ (this->table_points[node].get_waterlevel() -this->table_points[node-1].get_waterlevel())*(water_h-this->table_points[node-1].get_waterlevel());
		
		this->interpol_point.get_ptr_connected_mat_zones(i)->set_percentage_mat_zone(interpolated_result);
		this->interpol_point.get_ptr_connected_mat_zones(i)->set_ptr_mat_zone(this->table_points[node-1].get_ptr_connected_mat_zones(i)->get_ptr_mat_zone());
	}
	//set the old node
	this->last_index=node;
	
	
	
	return &this->interpol_point;
}
//Set the error(s)
Error Fpl_Dike_Geo_Wet_Materialzone_Table::set_error(const int err_type){
	string place="Fpl_Dike_Geo_Wet_Materialzone_Table::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("add_table_point(const double waterlevel, const double perc_area, Fpl_Dike_Geo_Materialzone_Connector* zone_connector)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://value is not in the range of the table
			place.append("interpolate_table_values(const double water_h)");
			reason="Value to interpolate is smaller than the minimum value; Can not interpolate";
			help="Check the tables of the wet area zones of the slope slices";
			type=32;
			break;
		case 2://problem with the interpolation
			place.append("interpolate_table_values(const double water_h)");
			reason="Interpolation problem";
			help="The value to interpolate is not found";
			type=32;
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