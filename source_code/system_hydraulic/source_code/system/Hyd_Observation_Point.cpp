#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Observation_Point.h"


//Default constructor
Hyd_Observation_Point::Hyd_Observation_Point(void){

	this->number_time_point=0;
	this->counter_time_points=0;
	this->time_point=NULL;
	this->element=NULL;
	this->profile=NULL;
	this->temp_profile = NULL;
	this->floodplain_flag=true;
	this->index_model=-1;
	this->index=-1;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Observation_Point), _sys_system_modules::HYD_SYS);
}
//Default destructor
Hyd_Observation_Point::~Hyd_Observation_Point(void){


	this->delete_time_point();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Observation_Point), _sys_system_modules::HYD_SYS);
}
//__________________
//public
//Set the number of required time point
void Hyd_Observation_Point::set_number_time_point(const int no_output, const int no_internal){
	this->delete_time_point();
	this->number_time_point=no_output*no_internal;
	this->allocate_time_point();
}
//Synchronise observation time
void Hyd_Observation_Point::synchron_obs_point(const double time){

	
	if(this->floodplain_flag==true){
		if(this->element!=NULL){
			this->time_point[this->counter_time_points].time=time;
			this->time_point[this->counter_time_points].s_value=this->element->element_type->get_s_value();
			this->time_point[this->counter_time_points].waterlevel=this->element->element_type->get_h_value();
			this->time_point[this->counter_time_points].ds2dt_fr=this->element->element_type->get_ds2dt_value()*60.0;
			this->time_point[this->counter_time_points].velocity=this->element->element_type->get_flowvelocity_vtotal();
			this->time_point[this->counter_time_points].x_velocity=this->element->element_type->get_flowvelocity_vx();
			this->time_point[this->counter_time_points].y_velocity=this->element->element_type->get_flowvelocity_vy();
			this->time_point[this->counter_time_points].y_velocity=this->element->element_type->get_flowvelocity_vy();
			this->time_point[this->counter_time_points].ds2dt_coupling=this->element->element_type->get_coupling_ds2dt();
		}
	}
	else{
		if(this->profile!=NULL){
			this->time_point[this->counter_time_points].time=time;
			this->time_point[this->counter_time_points].s_value=this->profile->typ_of_profile->get_actual_global_waterlevel();
			this->time_point[this->counter_time_points].waterlevel=this->profile->typ_of_profile->get_actual_local_waterlevel_h();
			this->time_point[this->counter_time_points].ds2dt_fr=this->profile->typ_of_profile->get_froude_number();
			this->time_point[this->counter_time_points].velocity=this->profile->typ_of_profile->get_actual_flow_velocity();
			this->time_point[this->counter_time_points].discharge=this->profile->get_actual_river_discharge();
		}
	}

	this->counter_time_points++;
	//make block allocation
	if(this->counter_time_points>0 && this->counter_time_points==this->number_time_point-1){
		const int add=250;
		_hyd_observation_time_point *buffer=NULL;
		int count_buff=this->counter_time_points;
		try{
			buffer=new _hyd_observation_time_point[this->number_time_point+add];
			Sys_Memory_Count::self()->add_mem(sizeof(_hyd_observation_time_point)*(this->number_time_point+add), _sys_system_modules::HYD_SYS);

		}catch(bad_alloc &){
			Error msg=this->set_error(1);
			throw msg;
		}
		//copy the points
		for(int i=0; i< this->number_time_point; i++){
			buffer[i].s_value=this->time_point[i].s_value;
			buffer[i].time=this->time_point[i].time;
			buffer[i].waterlevel=this->time_point[i].waterlevel;
			buffer[i].velocity=this->time_point[i].velocity;
			buffer[i].x_velocity=this->time_point[i].x_velocity;
			buffer[i].y_velocity=this->time_point[i].y_velocity;
			buffer[i].ds2dt_fr=this->time_point[i].ds2dt_fr;
			buffer[i].ds2dt_coupling=this->time_point[i].ds2dt_coupling;
			buffer[i].discharge=this->time_point[i].discharge;
		}
		//reset the new setted points
		for(int i=this->number_time_point-1; i< this->number_time_point+add; i++){
			buffer[i].s_value=-9999.9;
			buffer[i].time=-9999.9;
			buffer[i].waterlevel=-9999.9;
			buffer[i].velocity=-9999.9;
			buffer[i].x_velocity=-9999.9;
			buffer[i].y_velocity=-9999.9;
			buffer[i].ds2dt_fr=-9999.9;
			buffer[i].ds2dt_coupling=-9999.9;
			buffer[i].discharge=-9999.9;
		}
		//delete them
		this->delete_time_point();
		//recopy
		this->time_point=buffer;
		this->counter_time_points=count_buff;
		//add the new to number
		this->number_time_point=this->number_time_point+add;

	}
}
//Synchronise observation time for temperature modelling
void Hyd_Observation_Point::synchron_temp_obs_point(const double time) {
	//do not rename them...just use the sequence of the names

	if (this->floodplain_flag == false) {
		if (this->temp_profile != NULL) {
			this->time_point[this->counter_time_points].time = time;
			this->time_point[this->counter_time_points].s_value = this->temp_profile->get_actual_temperature();
			//Add with get function of other variables!! TODO UDO
			//this->time_point[this->counter_time_points].waterlevel = this->temp_profile->get;
/*			this->time_point[this->counter_time_points].ds2dt_fr = this->element->element_type->get_ds2dt_value()*60.0;
			this->time_point[this->counter_time_points].velocity = this->element->element_type->get_flowvelocity_vtotal();
			this->time_point[this->counter_time_points].x_velocity = this->element->element_type->get_flowvelocity_vx();
			this->time_point[this->counter_time_points].y_velocity = this->element->element_type->get_flowvelocity_vy();
			this->time_point[this->counter_time_points].y_velocity = this->element->element_type->get_flowvelocity_vy();
			this->time_point[this->counter_time_points].ds2dt_coupling = this->element->element_type->get_coupling_ds2dt()*/;
		}
	}

	this->counter_time_points++;
	//make block allocation
	if (this->counter_time_points > 0 && this->counter_time_points == this->number_time_point - 1) {
		const int add = 250;
		_hyd_observation_time_point *buffer = NULL;
		int count_buff = this->counter_time_points;
		try {
			buffer = new _hyd_observation_time_point[this->number_time_point + add];
			Sys_Memory_Count::self()->add_mem(sizeof(_hyd_observation_time_point)*(this->number_time_point + add), _sys_system_modules::HYD_SYS);

		}
		catch (bad_alloc &) {
			Error msg = this->set_error(1);
			throw msg;
		}
		//copy the points
		for (int i = 0; i < this->number_time_point; i++) {
			buffer[i].s_value = this->time_point[i].s_value;	
			buffer[i].time = this->time_point[i].time;
			buffer[i].waterlevel = this->time_point[i].waterlevel;
			buffer[i].velocity = this->time_point[i].velocity;
			buffer[i].x_velocity = this->time_point[i].x_velocity;
			buffer[i].y_velocity = this->time_point[i].y_velocity;
			buffer[i].ds2dt_fr = this->time_point[i].ds2dt_fr;
			buffer[i].ds2dt_coupling = this->time_point[i].ds2dt_coupling;
			buffer[i].discharge = this->time_point[i].discharge;
		}
		//reset the new setted points
		for (int i = this->number_time_point - 1; i < this->number_time_point + add; i++) {
			buffer[i].s_value = -9999.9;
			buffer[i].time = -9999.9;
			buffer[i].waterlevel = -9999.9;
			buffer[i].velocity = -9999.9;
			buffer[i].x_velocity = -9999.9;
			buffer[i].y_velocity = -9999.9;
			buffer[i].ds2dt_fr = -9999.9;
			buffer[i].ds2dt_coupling = -9999.9;
			buffer[i].discharge = -9999.9;
		}
		//delete them
		this->delete_time_point();
		//recopy
		this->time_point = buffer;
		this->counter_time_points = count_buff;
		//add the new to number
		this->number_time_point = this->number_time_point + add;

	}


}
//Set the geometrical point information
void Hyd_Observation_Point::set_geo_point_info(Geo_Point *point){
	this->set_point_coordinate(point->get_xcoordinate(), point->get_ycoordinate());
	this->set_point_name(point->get_point_name());

}
//Get the index of the element/profile
int Hyd_Observation_Point::get_index_elem_prof(void){
	return this->index;

}
//Initialize the observation points for river models
bool Hyd_Observation_Point::init_obs_point_river(Hyd_Model_River *model, const int index){
	bool found_flag=false;
	if(model->river_polygon.check_point_inside(this)==true){
		this->index=model->find_river_profile_id_by_point(this);
		if(this->index>0){
			this->profile=model->get_ptr_river_profile(this->index);
			if(this->profile!=NULL){
				this->index_model=index;
				found_flag=true;
				this->floodplain_flag=false;
			}
			else{
				this->profile=NULL;
				this->index=-1;
			}
		}
	}
	return found_flag;
}
//Initialize the observation points for river models
bool Hyd_Observation_Point::init_obs_point_floodplain(Hyd_Model_Floodplain *model, const int index){

	bool found_flag=false;

	if(model->raster.geometrical_bound.check_point_inside(this)==true){
		
		this->index=model->raster.find_elem_index_by_point_withboundary(this);
		if(this->index>0){
			
			this->element=model->get_ptr_floodplain_elem(this->index);
			if(this->element!=NULL && (this->element->get_elem_type()==_hyd_elem_type::STANDARD_ELEM ||
				this->element->get_elem_type()==_hyd_elem_type::COAST_ELEM || this->element->get_elem_type()==_hyd_elem_type::DIKELINE_ELEM ||
				this->element->get_elem_type()==_hyd_elem_type::RIVER_ELEM )){
				this->index_model=index;
				found_flag=true;
				this->floodplain_flag=true;
			}
			else{
				this->element=NULL;
				this->index=-1;
			}

		}		
	}


	return found_flag;

}
//Initialize the observation points for temperature models
bool Hyd_Observation_Point::init_temp_obs_point_river(HydTemp_Model *model, const int index) {
	bool found_flag = false;

	if (model->ptr_river_model->river_polygon.check_point_inside(this) == true) {
		this->index = model->ptr_river_model->find_river_profile_id_by_point(this);
		if (this->index > 0) {

			this->temp_profile = model->get_ptr_profile(this->index);
			if (this->temp_profile != NULL) {
				this->index_model = index;
				found_flag = true;
				this->floodplain_flag = false;
			}
			else {
				this->profile = NULL;
				this->index = -1;
			}
		}
	}
	return found_flag;


}
//Get the flag in which model (river/floodplain) the observation point is located
bool Hyd_Observation_Point::get_model_flag(void){
	return this->floodplain_flag;
}
//Ouput the observation point to file as tecplot output
void Hyd_Observation_Point::output_obs_point2file(ofstream *output, const int counter_used){

	if(this->element==NULL && this->profile==NULL){
		return;
	}
	

	if(this->floodplain_flag==true){
		*output << endl << "ZONE T= "<< "\" Name " <<this->name<<"; Floodplain id "<< this->index_model<<"; Element id "<< this->index  <<" \" "<< " I = " << counter_used << endl;

		for(int i=0; i< counter_used; i++){
			if(this->time_point[i].time>=0.0){
				*output<<this->time_point[i].time<<W(15)<<this->time_point[i].waterlevel<<W(15)<<this->time_point[i].s_value<<W(15);
				*output<<this->time_point[i].velocity<<W(15)<<this->time_point[i].x_velocity<<W(15)<<this->time_point[i].y_velocity<<W(15);
				*output<<this->time_point[i].ds2dt_fr<<W(15)<<this->time_point[i].ds2dt_coupling<<endl;
				output->flush();
			}
		}
	}
	else{
		*output << endl << "ZONE T= "<< "\" Name " <<this->name<<"; River id "<< this->index_model<<"; Profile id "<< this->index  <<" \" "<< " I = " << counter_used << endl;
		
		for(int i=0; i< counter_used; i++){
			if(this->time_point[i].time>=0.0){
				*output<<this->time_point[i].time<<W(15)<<this->time_point[i].waterlevel<<W(15)<<this->time_point[i].s_value<<W(15);
				*output<<this->time_point[i].velocity<<W(15)<<this->time_point[i].ds2dt_fr<<W(15)<< this->time_point[i].discharge<<endl;
				output->flush();
			}
		}
	}
}
//Ouput the observation point to file as csv output
void Hyd_Observation_Point::output_obs_point2csvfile(ofstream *output, const int counter_used) {
	if (this->element == NULL && this->profile == NULL) {
		return;
	}


	if (this->floodplain_flag == true) {
		
		for (int i = 0; i < counter_used; i++) {
			if (this->time_point[i].time >= 0.0) {
				*output << this->time_point[i].time <<W(15) << "," << this->time_point[i].waterlevel << W(15) << "," << this->time_point[i].s_value << W(15);
				*output << "," << this->time_point[i].velocity << W(15) << "," << this->time_point[i].x_velocity << W(15) << "," << this->time_point[i].y_velocity << W(15);
				*output << "," << this->time_point[i].ds2dt_fr << W(15) << "," << this->time_point[i].ds2dt_coupling << endl;
				output->flush();
			}
		}
	}
	else {
		
		for (int i = 0; i < counter_used; i++) {
			if (this->time_point[i].time >= 0.0) {
				*output << this->time_point[i].time << W(15) << "," << this->time_point[i].waterlevel << W(15) << "," << this->time_point[i].s_value << W(15);
				*output << "," << this->time_point[i].velocity << W(15) << "," << this->time_point[i].ds2dt_fr << W(15) << "," << this->time_point[i].discharge << endl;
				output->flush();
			}
		}
	}
}
//Ouput the temperature observation point to file as csv output
void Hyd_Observation_Point::output_temp_obs_point2csvfile(ofstream *output, const int counter_used) {
	if (this->temp_profile==NULL) {
		return;
	}




	for (int i = 0; i < counter_used; i++) {
		if (this->time_point[i].time >= 0.0) {
			*output << this->time_point[i].time << W(15) << "," << this->time_point[i].s_value << endl;
			//add further if required (UDO)
			//*output << "," << this->time_point[i].velocity << W(15) << "," << this->time_point[i].ds2dt_fr << W(15) << "," << this->time_point[i].discharge << endl;
			output->flush();
		}
	}
	

}
//Output setted members
void Hyd_Observation_Point::output_setted_members(ostringstream *cout, const int index){
	
	*cout<<" " << index << W(5)<<" "<< this->x_coordinate<<W(10)<<" "<< this->y_coordinate<<W(10)<<" "<< this->name;
	*cout<<" "<< functions::convert_boolean2string(this->floodplain_flag)<<" " << W(10)<< this->index_model<<" ";
	*cout<<" "<<W(10)<< this->index<<endl;

}
//Clear the observation points
void Hyd_Observation_Point::clear_obs_point(void){
	for(int i=0; i< this->number_time_point; i++){
		this->time_point[i].s_value=-9999.9;
		this->time_point[i].time=-9999.9;
		this->time_point[i].waterlevel=-9999.9;
		this->time_point[i].velocity=-9999.9;
		this->time_point[i].x_velocity=-9999.9;
		this->time_point[i].y_velocity=-9999.9;
		this->time_point[i].ds2dt_fr=-9999.9;
		this->time_point[i].discharge=-9999.9;
	}
	this->counter_time_points=0;
}
//Clone the observation points; the pointer are not cloned
void Hyd_Observation_Point::clone_obs_points(Hyd_Observation_Point *src, Hyd_Model_River *river,  Hyd_Model_Floodplain *floodplain){
	Geo_Point::operator =(*src);
	this->number_time_point=src->number_time_point;
	this->counter_time_points=0;
	this->allocate_time_point();
	this->floodplain_flag=src->floodplain_flag;
	this->index_model=src->index_model;
	this->index=src->index;

	if(this->index_model>=0 && this->index>=0){
		if(this->floodplain_flag==true){
			this->element=floodplain[this->index_model].get_ptr_floodplain_elem(this->index);
		}
		else{
			this->profile=river[this->index_model].get_ptr_river_profile(this->index);

		}
	}
}
//_________________
//private
//Allocate the time points
void Hyd_Observation_Point::allocate_time_point(void){
	try{
		this->time_point=new _hyd_observation_time_point[this->number_time_point];
		Sys_Memory_Count::self()->add_mem(sizeof(_hyd_observation_time_point)*this->number_time_point, _sys_system_modules::HYD_SYS);

	}catch(bad_alloc &){
		Error msg=this->set_error(0);
		throw msg;
	}

	for(int i=0; i< this->number_time_point; i++){
		this->time_point[i].s_value=-9999.9;
		this->time_point[i].time=-9999.9;
		this->time_point[i].waterlevel=-9999.9;
		this->time_point[i].velocity=-9999.9;
		this->time_point[i].x_velocity=-9999.9;
		this->time_point[i].y_velocity=-9999.9;
		this->time_point[i].ds2dt_fr=-9999.9;
		this->time_point[i].ds2dt_coupling=-9999.9;
		this->time_point[i].discharge=-9999.9;
	}

}
//Delete time points
void Hyd_Observation_Point::delete_time_point(void){
	if(this->time_point!=NULL){
		delete []this->time_point;
		this->time_point=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_hyd_observation_time_point)*this->number_time_point, _sys_system_modules::HYD_SYS);
		this->counter_time_points=0;
	}
}
//Set the error
Error Hyd_Observation_Point::set_error(const int err_type){
	string place="Hyd_Observation_Point::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_time_point(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("synchron_obs_point(const double time)");
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