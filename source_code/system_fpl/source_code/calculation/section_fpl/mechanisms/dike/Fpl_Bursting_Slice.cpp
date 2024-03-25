#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Bursting_Slice.h"


//Default constructor
Fpl_Bursting_Slice::Fpl_Bursting_Slice(void){


	this->intercept_zone=NULL;
	this->number_intercept_zone=0;
	this->size_imperm=0.0;
	this->index_seepage=-1;
	this->x_coor=-1.0;

	this->imperm_intercepted=false;
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Bursting_Slice), _sys_system_modules::FPL_SYS);//count the memory

}
///Default destructor
Fpl_Bursting_Slice::~Fpl_Bursting_Slice(void){

	this->delete_zone_connector();
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Bursting_Slice), _sys_system_modules::FPL_SYS);//count the memory
}
//________
//public
//Inititalize the material zones
void Fpl_Bursting_Slice::init_zone_connector(const double x_coor, const int number, Fpl_Dike_Geo_Materialzone *ptr_mat_zone, Fpl_Dike_Geo_Materialzone_Connector *connect_buffer){
	
	double length_buff=0.0;
	double y_imperm_zone=0.0;
	Geo_Segment buffer;
	buffer.set_coordinates(x_coor, 1000.0, x_coor, -1000.0);
	this->x_coor=x_coor;
	
	//intercept the zone
	Geo_Interception_Point_List buff_list;
	for(int i=0; i< number ; i++){
		if(ptr_mat_zone[i].get_zone_type()==_fpl_zone_type::imperm_layer){
			length_buff=ptr_mat_zone[i].segment_interception(&buffer, &buff_list);
			break;
		}
	}

	//check if there are a interception with the impermeable layer
	if(buff_list.get_number_points()>=2){
		this->imperm_intercepted=true;
		this->size_imperm=length_buff;
		//get the maximum coordinate
		y_imperm_zone=buff_list.get_max_y_coordinate();
		//set a vertical line until the maximum imperm interception point
		buffer.set_coordinates(x_coor, y_imperm_zone, x_coor, y_imperm_zone+1000.0);
		int counter_zone=0;
		//check the other zones
		for(int i=0; i< number ; i++){
			if(ptr_mat_zone[i].get_zone_type()!=_fpl_zone_type::imperm_layer){
				length_buff=ptr_mat_zone[i].segment_interception(&buffer, &buff_list);
				if(buff_list.get_number_points()>=2){
					connect_buffer[counter_zone].set_percentage_mat_zone(length_buff);
					connect_buffer[counter_zone].set_ptr_mat_zone(&ptr_mat_zone[i]);
					connect_buffer[counter_zone].set_secondary_info(buff_list.get_min_y_coordinate());
					counter_zone++;
				}
			}
		}

		//copy the buffer
		if(counter_zone>0){
			this->number_intercept_zone=counter_zone;
			this->allocate_zone_connector();

			//copy the zone connector
			for(int i=0; i< counter_zone; i++){
				this->intercept_zone[i].set_percentage_mat_zone(connect_buffer[i].get_percentage_mat_zone());
				this->intercept_zone[i].set_ptr_mat_zone(connect_buffer[i].get_ptr_mat_zone());
				this->intercept_zone[i].set_secondary_info(connect_buffer[i].get_secondary_info());
			}

		}
	}
	//impermeable zone is not intercepted
	else{
		this->imperm_intercepted=false;
	}
}
//Set the fix leakage length
void Fpl_Bursting_Slice::set_fixed_leak_length(const double length){
	this->fixed_leak_length=length;
}
//Get the fixed leakage length
double Fpl_Bursting_Slice::get_fixed_leak_length(void){
	return this->fixed_leak_length;
}
//Get the size of the impermeable layer
double Fpl_Bursting_Slice::get_size_impermeable_layer(void){
	return this->size_imperm;
}
//Get the flag if the impermeable zone is intercepted
bool Fpl_Bursting_Slice::get_impermeable_zone_intercepted(void){
	return this->imperm_intercepted;
}
//Get the extra load on the impermeable layer
double Fpl_Bursting_Slice::get_extra_load(Fpl_Seepage_Line_Point_List *seepage_list, const double density_water){

	double sum=0.0;
	double h_water=seepage_list->get_list_point(this->index_seepage)->get_waterlevel();
	double up_bound_layer=0.0;
	for(int i=0; i < this->number_intercept_zone; i++){
		up_bound_layer=this->intercept_zone[i].get_secondary_info()+this->intercept_zone[i].get_percentage_mat_zone();

		//completly wet
		if(h_water>=up_bound_layer){
			sum=sum+(this->intercept_zone[i].get_ptr_mat_zone()->get_ptr_relevant_variables()->get_uplift_density()+density_water)*this->intercept_zone[i].get_percentage_mat_zone();
		}
		//completly dry
		else if(h_water<=this->intercept_zone[i].get_secondary_info()){
			sum=sum+this->intercept_zone[i].get_ptr_mat_zone()->get_ptr_relevant_variables()->get_wet_density()*this->intercept_zone[i].get_percentage_mat_zone();
		}
		//inbetween
		else{
			sum=sum+(this->intercept_zone[i].get_ptr_mat_zone()->get_ptr_relevant_variables()->get_uplift_density()+(this->intercept_zone[i].get_ptr_mat_zone()->get_ptr_relevant_variables()->get_uplift_density()+density_water))*(h_water-this->intercept_zone[i].get_secondary_info())+
				this->intercept_zone[i].get_ptr_mat_zone()->get_ptr_relevant_variables()->get_wet_density()*(this->intercept_zone[i].get_percentage_mat_zone()-(h_water-this->intercept_zone[i].get_secondary_info()));


		}
	}

	return sum;
}
//Add the relevant points of the seepage line to the given list
void Fpl_Bursting_Slice::add_seepage_line_point(Fpl_Seepage_Line_Point_List *seepage_list, Geo_Polysegment *outer_polysegment){

	this->index_seepage=seepage_list->add_new_point(this->x_coor,0.0, true, outer_polysegment);
}
//_______
//private
//Allocate the zone connector
void Fpl_Bursting_Slice::allocate_zone_connector(void){
	this->delete_zone_connector();
	try{
		this->intercept_zone=new Fpl_Dike_Geo_Materialzone_Connector[this->number_intercept_zone];
	}
	catch(bad_alloc&t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the zone connector
void Fpl_Bursting_Slice::delete_zone_connector(void){
	if(this->intercept_zone!=NULL){
		delete []this->intercept_zone;
		this->intercept_zone=NULL;
	}
}
//Set the error(s)
Error Fpl_Bursting_Slice::set_error(const int err_type){
	string place="Fpl_Bursting_Slice::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
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