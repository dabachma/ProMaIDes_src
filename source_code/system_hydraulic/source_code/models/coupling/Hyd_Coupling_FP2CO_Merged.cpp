#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_FP2CO_Merged.h"

//constructor
Hyd_Coupling_FP2CO_Merged::Hyd_Coupling_FP2CO_Merged(void){
	this->number_fp_models=0;
	this->floodplain_model=NULL;

	this->coast_model=NULL;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_FP2CO_Merged)-sizeof(Hyd_Coupling_Point_FP2CO_List), _sys_system_modules::HYD_SYS);
	
}
//destructor
Hyd_Coupling_FP2CO_Merged::~Hyd_Coupling_FP2CO_Merged(void){
	if(this->floodplain_model!=NULL){
		delete []this->floodplain_model;
		this->floodplain_model=NULL;
	}

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_FP2CO_Merged)-sizeof(Hyd_Coupling_Point_FP2CO_List), _sys_system_modules::HYD_SYS);
}
//__________
//public
//The pointer to the coastmodel is set
void Hyd_Coupling_FP2CO_Merged::set_ptr_coupling_coast(Hyd_Coast_Model *coast){
	this->coast_model=coast;
}
//Get the pointer to the coast model
Hyd_Coast_Model *Hyd_Coupling_FP2CO_Merged::get_ptr_coast_model(void){
	return this->coast_model;
}
//Get the number of coupled floodplains
int Hyd_Coupling_FP2CO_Merged::get_number_coupled_fp(void){
	return this->number_fp_models;
}
//Get the index of a coupled floodplain model by the given index (number of coupled floodplain models)
int Hyd_Coupling_FP2CO_Merged::get_index_coupled_fp_models(const int index){
	int index_fp=-1;
	if(index<0 || index >=this->number_fp_models){
		return index_fp=-1;
	}
	else{
		index_fp=this->floodplain_model[index]->Param_FP.get_floodplain_number();
	}

	return index_fp;
}
//The pointer of the models for couplings are setted and the lists are merged
void Hyd_Coupling_FP2CO_Merged::set_ptr_coupling_with_merging(Hyd_Coupling_FP2CO *couplings){
	if(couplings==NULL){
		return;
	}
	else{
		if(this->coast_model==NULL && couplings->get_is_merged()==false){
			this->coast_model=couplings->get_coastmodel_ptr();
			this->add_floodplainmodel_pointer(couplings->get_floodplainmodel_ptr());
			//set the defining polysegment
			this->list.set_defining_polysegment(couplings->list.get_defining_polysegment());
			//merge the lists
			couplings->list.merge_coupling_list(&this->list);
			couplings->set_is_merged();

		}
		else if(couplings->get_is_merged()==false){
			this->add_floodplainmodel_pointer(couplings->get_floodplainmodel_ptr());
			//merge the lists
			couplings->list.merge_coupling_list(&this->list);
			couplings->set_is_merged();
		}
	}
}
//Synchronise the models
void Hyd_Coupling_FP2CO_Merged::synchronise_models(const double timepoint, const double delta_t, const bool time_check, const int internal_counter){
	try{
		this->list.syncronisation_models_bylistpoints(timepoint, delta_t, this->coast_model->get_global_sea_h(), time_check, internal_counter);
	}
	catch(Error msg){
		ostringstream info;
		info<<"Synchronisation between coast model and multiple floodplain models" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Get the maximum waterlevel gradient 
double Hyd_Coupling_FP2CO_Merged::get_maximum_h_grad(void){
	double buffer=0.0;
	double buffer_max=0.0;
	
		
	buffer=this->list.get_maximum_h_grad();
	if(buffer_max<buffer){
		buffer_max=buffer;
	}
	
	return buffer_max;
}
//Output the header for the coupled model indizes (static)
void Hyd_Coupling_FP2CO_Merged::output_header_coupled_indices(ostringstream *cout){
		*cout << "Coast to multiple floodplain models..." << endl ;
		*cout << "Id_CO" << W(12) << "No_FPs" << W(12) << "Id_FPs" << endl ;
		Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the indices of the coupled model
void Hyd_Coupling_FP2CO_Merged::output_index_coupled_models(ostringstream *cout){
	*cout << 0 << W(12);
	*cout << this->number_fp_models<<W(12);
	for(int i=0; i<this->number_fp_models; i++){
		*cout << this->floodplain_model[i]->Param_FP.get_floodplain_number() << "  ";
	}
	*cout <<endl;

	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the setted coupling points in the list
void Hyd_Coupling_FP2CO_Merged::output_setted_coupling_points(void){
	ostringstream cout;
	ostringstream prefix;
	prefix << "FP_CO_MERG> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout << "List of coupling points" << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	this->list.output_setted_members(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Insert river junctions into the list
void Hyd_Coupling_FP2CO_Merged::insert_river_junctions(Hyd_Coupling_RV2CO *rv2co_coupling, const int number){
	for(int i=0; i<number ; i++){
		Geo_Segment *last_river_segment=NULL;
		Geo_Interception_Point_List buff_list;
		_Hyd_Coupling_Point point1;
		double distance=-1.0;

		last_river_segment=rv2co_coupling[i].get_ptr_rivermodel()->river_midline.get_last_segment();
		this->list.get_defining_polysegment()->calculate_segment_interception(&buff_list, last_river_segment);
		if(buff_list.get_number_points()>0){
			Geo_Point buffer_point;
			buffer_point.set_point(buff_list.get_first_point().line_interception.interception_point);
			distance=this->list.get_defining_polysegment()->get_distance_along_polysegment(&buffer_point);
			point1.set_total_distance_along_polysegment(distance);
			if (this->list.find_coupling_point(distance, &point1) == NULL) {
				return;
			};
			this->list.insert_constricting_coupling_points(rv2co_coupling[i].get_ptr_rivermodel()->outflow_river_profile.typ_of_profile->get_width_end_start_profile(), &point1);
		}
	}
}
//Clone the coast to floodplain merged coupling
void Hyd_Coupling_FP2CO_Merged::clone_couplings(Hyd_Coupling_FP2CO_Merged *coupling, Hyd_Hydraulic_System *system){


	if(coupling->coast_model!=NULL){
		this->coast_model=system->my_comodel;
	}
	//set the fp-models
	for(int i=0; i< coupling->number_fp_models; i++){
		Hyd_Model_Floodplain *buffer;
		buffer=system->get_ptr_floodplain_model(coupling->floodplain_model[i]->Param_FP.get_floodplain_number());
		this->add_floodplainmodel_pointer(buffer);
	}
	//the lists
	this->list.clone_list(&coupling->list, system);

}
//Set up the list of fpl-section ids in the total merged lists
void Hyd_Coupling_FP2CO_Merged::set_up_fpl_section_ids_list(void){
	this->list.set_list_fpl_sec_id();
}
//Calculate and output final results of the maximum waterlevel of the coupling points to database
void Hyd_Coupling_FP2CO_Merged::calc_output_final_results(QSqlDatabase *ptr_database, const _sys_system_id id, const string id_break, const int bound_sc){

	int sec_id=0;
	int number_sec=0;
	_hyd_max_values h_max;
	h_max.maximum=0.0;
	h_max.time_point=0.0;
	int point_id_h_max=0;
	_hyd_max_values h_max2break;
	h_max2break.maximum=0.0;
	h_max2break.time_point=0.0;
	int point_id_h_max2break=0;
	//left list
	number_sec=this->list.get_ptr_fpl_section_ids()->count();
	for(int i=0; i< number_sec; i++){
		sec_id=this->list.get_ptr_fpl_section_ids()->at(i);
		this->list.get_max_h2fpl(sec_id, &h_max, &point_id_h_max);
		this->list.get_max_h2break(sec_id, &h_max2break, &point_id_h_max2break);
		Hyd_Coupling_RV2FP_Merged::output_final_results(ptr_database, id, id_break, bound_sc, sec_id, h_max, point_id_h_max, h_max2break, point_id_h_max2break);
	}	
}
//__________
//private
//Add a pointer of additional floodplain models
void Hyd_Coupling_FP2CO_Merged::add_floodplainmodel_pointer(Hyd_Model_Floodplain *pointer){

	if(pointer==NULL){
		return;
	}

	//check if it is the same
	for(int i=0; i< this->number_fp_models; i++){
		if(this->floodplain_model[i]==pointer){
			return;
		}
	}

	//allocate a buffer
	Hyd_Model_Floodplain **buffer=NULL;
	try{
		(buffer)=new Hyd_Model_Floodplain*[this->number_fp_models];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//copy it
	for(int i=0; i< this->number_fp_models; i++){
		buffer[i]=this->floodplain_model[i];
	}
	//delete the pointer
	if(this->floodplain_model!=NULL){
		delete []this->floodplain_model;
		this->floodplain_model=NULL;
	}
	//count up
	this->number_fp_models++;
	//allocate new
	try{
		(this->floodplain_model)=new Hyd_Model_Floodplain*[this->number_fp_models];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//recopy it
	for(int i=0; i< this->number_fp_models-1; i++){
		this->floodplain_model[i]=buffer[i];
	}
	//set the last pointer
	this->floodplain_model[this->number_fp_models-1]=pointer;

	//delete the buffer
	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}
}
//set the error
Error Hyd_Coupling_FP2CO_Merged::set_error(const int err_type){
	string place="Hyd_Coupling_FP2CO_Merged::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;
	switch (err_type){
		case 0://bad alloc
			place.append("add_floodplainmodel_pointer(Hyd_Model_Floodplain *pointer)");
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