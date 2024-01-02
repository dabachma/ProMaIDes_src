#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_Management.h"

//constructor
Hyd_Coupling_Management::Hyd_Coupling_Management(void){
	
	this->total_number=0;
	this->number_rv2co=0;
	this->number_fp2fp=0;
	this->number_rv2fp=0;
	this->number_merged_rv2fp=0;
	this->number_fp2co=0;
	this->number_merged_fp2co=0;
	this->number_rv2rv=0;
	this->number_rv2rv_diversion=0;
	this->number_rv2fp_structure=0;	
	
	
	this->number_rv2fp_direct_out=0;
	this->number_rv2fp_direct_in=0;
	this->total_number_coupling=0;

	this->number_rv2fp_dikebreak=0;
	this->number_rv2fp_dikebreak_fpl=0;

	this->number_fp2co_dikebreak=0;
	this->number_fp2co_dikebreak_fpl=0;


	this->coupling_rv2co=NULL;
	this->coupling_rv2fp=NULL;
	this->coupling_merged_rv2fp=NULL;
	this->coupling_fp2co=NULL;
	this->coupling_merged_fp2co=NULL;
	this->coupling_fp2fp=NULL;
	this->coupling_rv2rv=NULL;
	this->coupling_1d_diversion=NULL;
	this->coupling_rv2fp_structure=NULL;

	this->coupling_rv2fp_dikebreak=NULL;
	this->coupling_rv2fp_dikebreak_fpl=NULL;

	this->coupling_fp2co_dikebreak=NULL;
	this->coupling_fp2co_dikebreak_fpl=NULL;

	this->old_time_point=0.0;
	this->delta_t=0.0;
	this->ptr_output_flags = NULL;


	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_Management), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coupling_Management::~Hyd_Coupling_Management(void){
	this->delete_coupling_class_rv2co();
	this->delete_coupling_class_rv2fp();
	this->delete_coupling_class_fp2co();
	this->delete_coupling_class_fp2fp();
	this->delete_coupling_class_rv2rv();
	this->delete_coupling_class_rv2rv_diversion();
	this->delete_coupling_class_rv2fp_structure();
	this->delete_coupling_class_rv2fp_dikebreak();
	this->delete_coupling_class_fp2co_merged();
	this->delete_coupling_class_rv2fp_merged();
	this->delete_coupling_class_rv2fp_dikebreak_fpl();
	this->delete_coupling_class_fp2co_dikebreak();
	this->delete_coupling_class_fp2co_dikebreak_fpl();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_Management), _sys_system_modules::HYD_SYS);
}
//_____________
//public
//Output the members
void Hyd_Coupling_Management::output_members(void){
	ostringstream cout;
	//set prefix for output
	ostringstream prefix;
	prefix << "COUP> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	//structure
	for(int i=0; i<this->number_rv2fp_structure; i++){
		this->coupling_rv2fp_structure[i].output_members();
	}
	//diversion channels
	for(int i=0; i<this->number_rv2rv_diversion; i++){
		this->coupling_1d_diversion[i].output_members();
	}
	//dikebreak rv2fp
	for(int i=0; i<this->number_rv2fp_dikebreak;i++){
		this->coupling_rv2fp_dikebreak[i].output_members();
	}
	//dikebreak fp2co
	for(int i=0; i<this->number_fp2co_dikebreak;i++){
		this->coupling_fp2co_dikebreak[i].output_members();
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the set members
void Hyd_Coupling_Management::output_setted_members(QSqlDatabase *ptr_database){
	ostringstream cout;
	//set prefix for output
	ostringstream prefix;
	prefix << "COUP> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout << "Automatic coupling between the models..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	
	//river to river
	if(this->number_rv2rv>0){
		Hyd_Coupling_RV2RV::output_header_coupled_indices(&cout);
		for(int i=0; i< this->number_rv2rv; i++){
			this->coupling_rv2rv[i].output_index_coupled_models(&cout, i);
		}
		Sys_Common_Output::output_hyd->reset_prefix_was_outputed();
	}


	//river to floodplain
	if(this->number_merged_rv2fp>0){
		for(int i=0; i< this->number_merged_rv2fp; i++){
			Hyd_Coupling_RV2FP_Merged::output_header_coupled_indices(&cout);
			this->coupling_merged_rv2fp[i].output_index_coupled_models(&cout,i);
			this->coupling_merged_rv2fp[i].output_setted_coupling_points();

		}
	Sys_Common_Output::output_hyd->reset_prefix_was_outputed();
	}
	
	//river to coast
	if(this->number_rv2co>0){
		Hyd_Coupling_RV2CO::output_header_coupled_indices(&cout);
		for(int i=0; i< this->number_rv2co; i++){
			this->coupling_rv2co[i].output_index_coupled_models(&cout, i);
		}
		Sys_Common_Output::output_hyd->reset_prefix_was_outputed();
	}
	//floodplain to coast	
	if(this->number_merged_fp2co>0){
		for(int i=0; i< this->number_merged_fp2co; i++){
			Hyd_Coupling_FP2CO_Merged::output_header_coupled_indices(&cout);
			this->coupling_merged_fp2co[i].output_index_coupled_models(&cout);
			this->coupling_merged_fp2co[i].output_setted_coupling_points();
		}
		Sys_Common_Output::output_hyd->reset_prefix_was_outputed();
	}

	//floodplain to floodplain
	if(this->number_fp2fp>0){
		for(int i=0; i< this->number_fp2fp; i++){
			Hyd_Coupling_FP2FP::output_header_coupled_indices(&cout);
			this->coupling_fp2fp[i].output_index_coupled_models(&cout,i);
			this->coupling_fp2fp[i].output_setted_coupling_points();
		}
		Sys_Common_Output::output_hyd->reset_prefix_was_outputed();
	}
	
	cout << "Setted coupling between the models..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	//river to river as diversion channel
	if(this->number_rv2rv_diversion>0){
		Hyd_Coupling_RV2RV_Diversion::output_header_coupled_indices(&cout);
		for(int i=0; i< this->number_rv2rv_diversion; i++){
			this->coupling_1d_diversion[i].output_index_coupled_models(&cout,i);
		}
		Sys_Common_Output::output_hyd->reset_prefix_was_outputed();
	}
	//river to floodplain as structure coupling
	if(this->number_rv2fp_structure>0){
		Hyd_Coupling_RV2FP_Structure::output_header_coupled_indices(&cout);
		for(int i=0; i< this->number_rv2fp_structure; i++){
			this->coupling_rv2fp_structure[i].output_index_coupled_models(&cout,i);
			this->coupling_rv2fp_structure[i].output_point2database(ptr_database);
		}
		Sys_Common_Output::output_hyd->reset_prefix_was_outputed();
	}
	//river to floodplain as dikebreak coupling
	if(this->number_rv2fp_dikebreak>0){
		Hyd_Coupling_RV2FP_Dikebreak::output_header_coupled_indices(&cout);
		for(int i=0; i< this->number_rv2fp_dikebreak; i++){
			this->coupling_rv2fp_dikebreak[i].output_index_coupled_models(&cout,i);
			this->coupling_rv2fp_dikebreak[i].output_point2database(ptr_database);
		}
		Sys_Common_Output::output_hyd->reset_prefix_was_outputed();
	}

	//coast to floodplain as dikebreak coupling
	if(this->number_fp2co_dikebreak>0){
		Hyd_Coupling_FP2CO_Dikebreak::output_header_coupled_indices(&cout);
		for(int i=0; i< this->number_fp2co_dikebreak; i++){
			this->coupling_fp2co_dikebreak[i].output_index_coupled_models(&cout,i);
			this->coupling_fp2co_dikebreak[i].output_point2database(ptr_database);
		}
		Sys_Common_Output::output_hyd->reset_prefix_was_outputed();
	}

	//output the coupling statistic
	this->output_coupling_statistic();
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output final result members
void Hyd_Coupling_Management::output_final_results(void){
	ostringstream cout;
	//set prefix for output
	ostringstream prefix;
	prefix << "COUP> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	//structure
	for(int i=0; i<this->number_rv2fp_structure; i++){
		this->coupling_rv2fp_structure[i].output_final_results();
	}
	//dikebreak river to floodplain
	for(int i=0; i<this->number_rv2fp_dikebreak;i++){
		this->coupling_rv2fp_dikebreak[i].output_final_results();
	}
	for(int i=0; i<this->number_rv2fp_dikebreak_fpl;i++){
		this->coupling_rv2fp_dikebreak_fpl[i].output_final_results();
	}

	//dikebreak coast to floodplain
	for(int i=0; i<this->number_fp2co_dikebreak;i++){
		this->coupling_fp2co_dikebreak[i].output_final_results();
	}
	for(int i=0; i<this->number_fp2co_dikebreak_fpl;i++){
		this->coupling_fp2co_dikebreak_fpl[i].output_final_results();
	}
	//output the discharge lists (just for testing)
	//for(int i=0; i< this->number_merged_fp2co; i++){
	//	this->coupling_merged_fp2co[i].list.output_discharge_lists();
	//}
	//for(int i=0; i< this->number_fp2fp; i++){
	//	this->coupling_fp2fp[i].list.output_discharge_lists();
	//}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output final result members to database
void Hyd_Coupling_Management::output_final_results(QSqlDatabase *ptr_database, const int hyd_bound_sz, const string break_sc){

	ostringstream prefix;
	prefix << "COUP> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;


	try{
		//first delete old results in table
		if (this->number_rv2fp_dikebreak > 0 || this->number_rv2fp_dikebreak_fpl > 0 || this->number_fp2co_dikebreak > 0|| this->number_fp2co_dikebreak_fpl > 0) {
			_Hyd_Coupling_Dikebreak::delete_data_in_result_table(ptr_database, this->system_id, break_sc, hyd_bound_sz);
		}

		//dikebreak river to floodplain
		if(this->number_rv2fp_dikebreak>0){
			cout << "Output final results of the river to floodplain break coupling to database..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);		
		}
		for(int i=0; i<this->number_rv2fp_dikebreak;i++){
			this->coupling_rv2fp_dikebreak[i].output_final_results(ptr_database, break_sc, hyd_bound_sz);
		}

		if(this->number_rv2fp_dikebreak_fpl>0){
			cout << "Output final results of the river to floodplain break coupling initiated by the FPL-system to database..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
		}
		for(int i=0; i<this->number_rv2fp_dikebreak_fpl;i++){
			this->coupling_rv2fp_dikebreak_fpl[i].output_final_results(ptr_database, break_sc, hyd_bound_sz);
		}


		//dikebreak coast to floodplain
		if(this->number_fp2co_dikebreak>0){
			cout << "Output final results of the coast to floodplain break coupling to database..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
		}
		for(int i=0; i<this->number_fp2co_dikebreak;i++){
			this->coupling_fp2co_dikebreak[i].output_final_results(ptr_database, break_sc, hyd_bound_sz);
		}

		if(this->number_fp2co_dikebreak_fpl>0){
			cout << "Output final results of the coast to floodplain break coupling initiated by the FPL-system to database..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout); 
		}
		for(int i=0; i<this->number_fp2co_dikebreak_fpl;i++){
			this->coupling_fp2co_dikebreak_fpl[i].output_final_results(ptr_database, break_sc, hyd_bound_sz);
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Output final results of the coupling points concerning the maximum waterlevel for the coupling to the fpl-sections
void Hyd_Coupling_Management::output_final_results_max_waterlevel(QSqlDatabase *ptr_database, const int hyd_bound_sz, const string break_sc){
	ostringstream prefix;
	prefix << "COUP> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;


	try{
		//dikebreak
		if(this->number_merged_rv2fp>0){
			cout << "Output final results (maximum waterlevel) of the river to floodplain coupling points to database..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			Hyd_Coupling_RV2FP_Merged::delete_data_max_h_table(ptr_database, this->system_id, hyd_bound_sz, break_sc);  
		}
		for(int i=0; i<this->number_merged_rv2fp;i++){
			this->coupling_merged_rv2fp[i].calc_output_final_results(ptr_database,this->system_id, break_sc, hyd_bound_sz);
		}

		if(this->number_merged_fp2co>0){
			cout << "Output final results (maximum waterlevel) of the coast to floodplain coupling points to database..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			if(this->number_merged_rv2fp==0){
				Hyd_Coupling_RV2FP_Merged::delete_data_max_h_table(ptr_database, this->system_id, hyd_bound_sz, break_sc);  
			}

			this->coupling_merged_fp2co[0].calc_output_final_results(ptr_database,this->system_id, break_sc, hyd_bound_sz);
		}

	}
	catch(Error msg){
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Add number of river to coast coupling
void Hyd_Coupling_Management::add_rv2co(const int number){
	this->number_rv2co=	this->number_rv2co+number;
}
//Get the number of river to coast coupling
int Hyd_Coupling_Management::get_rv2co(void){
	return this->number_rv2co;
}
//Add number of floodplain to coast coupling
void Hyd_Coupling_Management::add_fp2co(const int number){
	this->number_fp2co=this->number_fp2co+number;
}
//Get the number of floodplain to coast coupling
int Hyd_Coupling_Management::get_fp2co(void){
	return this->number_fp2co;
}
//Add number of floodplain to floodplain coupling
void Hyd_Coupling_Management::add_fp2fp(const int number){
	this->number_fp2fp=this->number_fp2fp+number;
}
//Get the number of floodplain to floodplain coupling
int Hyd_Coupling_Management::get_fp2fp(void){
	return this->number_fp2fp;
}
//Add number of river to floodplain coupling
void Hyd_Coupling_Management::add_rv2fp(const int number){
	this->number_rv2fp=this->number_rv2fp+number;
}
//Get the number of river to floodplain coupling
int Hyd_Coupling_Management::get_rv2fp(void){
	return this->number_rv2fp;
}
//Add number of river to river coupling
void Hyd_Coupling_Management::add_rv2rv(const int number){
	this->number_rv2rv=this->number_rv2rv+number;
}
//Get the number of river to river coupling
int Hyd_Coupling_Management::get_rv2rv(void){
	return this->number_rv2rv;
}
//Set the number of dikebreak coupling, which are automatically set via the fpl-system; they are also allocated here (river model to floodplain model)
void Hyd_Coupling_Management::set_number_rv2fp_dikebreak_fpl_alloc(const int number){
	this->number_rv2fp_dikebreak_fpl=number;
	try{
		this->allocate_coupling_class_rv2fp_dikebreak_fpl();
	}
	catch(Error msg){
		throw msg;
	}
}
//Get the number of dikebreak coupling, which are automatically set via the fpl-system (river model to floodplain model)
int Hyd_Coupling_Management::get_number_rv2fp_dikebreak_fpl(void){
	return this->number_rv2fp_dikebreak_fpl;
}
//Set the breach start-parameters for the allocated coupling classes (river model to floodplain model)
void Hyd_Coupling_Management::set_start_rv2fp_dikebreak_fpl(const int index, const _hyd_breach_start_parameters params, const bool left_flag){
	if(index<0 || index >= this->number_rv2fp_dikebreak_fpl){
		return;
	}
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	int index_breach=0;
	index_breach=this->generate_breachindex_rv2fp_break_coupling();
	this->coupling_rv2fp_dikebreak_fpl[index].input_parameter(index_breach, params, left_flag);
}
//Initialise the dikebreak coupling, which are automatically set via the fpl-system (scenario based risk approach) (river model to floodplain model)
void Hyd_Coupling_Management::init_rv2fp_dikebreak_fpl(const int index, const int number_involved_profiles, _Hyd_River_Profile **involved_profiles, const int section_id_fpl){
	if(index<0 || index >= this->number_rv2fp_dikebreak_fpl){
		return;
	}
	ostringstream cout;
	cout << "Automatic coupling (SC-approach) no. " << index <<" between rivermodel and floodplainmodel by the FPL-section " << section_id_fpl << " with generated breach-index " <<this->coupling_rv2fp_dikebreak_fpl[index].get_breach_index()<<"..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	
	this->coupling_rv2fp_dikebreak_fpl[index].init_coupling(this->coupling_merged_rv2fp, this->number_merged_rv2fp, involved_profiles, number_involved_profiles, section_id_fpl, this->ptr_output_flags);
}
//Initialise the dikebreak coupling, which are automatically set via the fpl-system; the coupling point index, where the break should begin is directly given (catchment area risk approach) (river model to floodplain model)
void Hyd_Coupling_Management::init_rv2fp_dikebreak_fpl(const int index, const int section_id_fpl, const int point_id){
	if(index<0 || index >= this->number_rv2fp_dikebreak_fpl){
		return;
	}
	if(point_id<0){
		return;
	}
	ostringstream cout;
	cout << "Automatic coupling (CA-approach) no. " << index <<" between rivermodel and floodplainmodel by the FPL-section " << section_id_fpl << " with generated breach-index " <<this->coupling_rv2fp_dikebreak_fpl[index].get_breach_index()<<"..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	
	this->coupling_rv2fp_dikebreak_fpl[index].init_coupling(this->coupling_merged_rv2fp, this->number_merged_rv2fp, section_id_fpl, point_id, this->ptr_output_flags);
}
//Clear the dikebreak coupling, which are automatically set via the fpl-system (river model to floodplain model)
void Hyd_Coupling_Management::clear_rv2fp_dikebreak_fpl(void){
	this->delete_coupling_class_rv2fp_dikebreak_fpl();
	this->number_rv2fp_dikebreak_fpl=0;
}
//Set the number of dikebreak coupling, which are automatically set via the fpl-system; they are also allocated here (coast model to floodplain model)
void Hyd_Coupling_Management::set_number_fp2co_dikebreak_fpl_alloc(const int number){
	this->number_fp2co_dikebreak_fpl=number;
	try{
		this->allocate_coupling_class_fp2co_dikebreak_fpl();
	}
	catch(Error msg){
		throw msg;
	}

}
//Get the number of dikebreak coupling, which are automatically set via the fpl-system (coast model to floodplain model)
int Hyd_Coupling_Management::get_number_fp2co_dikebreak_fpl(void){
	return this->number_fp2co_dikebreak_fpl;
}
//Set the breach start-parameters for the allocated coupling classes (coast model to floodplain model)
void Hyd_Coupling_Management::set_start_fp2co_dikebreak_fpl(const int index, const _hyd_breach_start_parameters params){
	if(index<0 || index >= this->number_fp2co_dikebreak_fpl){
		return;
	}
	_hyd_fp2co_breach_start_parameters buffer;
	buffer.distance2related=params.distance2related;
	buffer.id_related_point=params.id_related_station;
	buffer.max_breach_width=params.max_breach_width;
	buffer.start_height=params.start_height;

	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	int index_breach=0;
	index_breach=this->generate_breachindex_fp2co_break_coupling();
	this->coupling_fp2co_dikebreak_fpl[index].input_parameter(index_breach, buffer);

}
//Initialise the dikebreak coupling, which are automatically set via the fpl-system (scenario based risk approach) (coast model to floodplain model)
void Hyd_Coupling_Management::init_fp2co_dikebreak_fpl(const int index, const int number_involved_points, Hyd_Floodplain_Dikeline_Point **involved_points, const int section_id_fpl){

	if(index<0 || index >= this->number_fp2co_dikebreak_fpl){
		return;
	}
	ostringstream cout;
	cout << "Automatic coupling (SC-approach) no. " << index <<" between coastmodel and floodplainmodel by the FPL-section " << section_id_fpl << " with generated breach-index " <<this->coupling_fp2co_dikebreak_fpl[index].get_breach_index()<<"..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	
	this->coupling_fp2co_dikebreak_fpl[index].init_coupling(this->coupling_merged_fp2co, involved_points, number_involved_points, section_id_fpl, this->ptr_output_flags);
	
}
//Initialise the dikebreak coupling, which are automatically set via the fpl-system; the coupling point index, where the break should begin is directly given (catchment area risk approach) (coast model to floodplain model)
void Hyd_Coupling_Management::init_fp2co_dikebreak_fpl(const int index, const int section_id_fpl, const int point_id){
	if(index<0 || index >= this->number_fp2co_dikebreak_fpl){
		return;
	}
	if(point_id<0){
		return;
	}
	ostringstream cout;
	cout << "Automatic coupling (CA-approach) no. " << index <<" between coast-and floodplainmodel by the FPL-section " << section_id_fpl << " with generated breach-index " <<this->coupling_fp2co_dikebreak_fpl[index].get_breach_index()<<"..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	
	this->coupling_fp2co_dikebreak_fpl[index].init_coupling(this->coupling_merged_fp2co, section_id_fpl, point_id,  this->ptr_output_flags);


}
//Clear the dikebreak coupling, which are automatically set via the fpl-system (coast model to floodplain model)
void Hyd_Coupling_Management::clear_fp2co_dikebreak_fpl(void){
	this->delete_coupling_class_fp2co_dikebreak_fpl();
	this->number_fp2co_dikebreak_fpl=0;
}
//Get if the breach for a coupling of a coast model/river model to a floodplain model has started for a specific section id of the fpl-system
bool Hyd_Coupling_Management::get_fpl_breach_has_started(const int sec_id){

	for(int i=0; i< this->number_rv2fp_dikebreak_fpl; i++){
		if(this->coupling_rv2fp_dikebreak_fpl[i].get_index_fpl_section()==sec_id){
			if(this->coupling_rv2fp_dikebreak_fpl[i].get_breach_has_started()==true){
				return true;
			}
			else{
				return false;
			}
		}
	}

	for(int i=0; i< this->number_fp2co_dikebreak_fpl; i++){
		if(this->coupling_fp2co_dikebreak_fpl[i].get_index_fpl_section()==sec_id){
			if(this->coupling_fp2co_dikebreak_fpl[i].get_breach_has_started()==true){
				return true;
			}
			else{
				return false;
			}
		}
	}

	return false;
}
//Get the observed maximum waterlevel for a breach development
double Hyd_Coupling_Management::get_max_observed_waterlevel_breach(const int sec_id){

	for(int i=0; i< this->number_rv2fp_dikebreak_fpl; i++){
		if(this->coupling_rv2fp_dikebreak_fpl[i].get_index_fpl_section()==sec_id){
			return this->coupling_rv2fp_dikebreak_fpl[i].get_max_observed_waterlevel();
		}
	}

	for(int i=0; i< this->number_fp2co_dikebreak_fpl; i++){
		if(this->coupling_fp2co_dikebreak_fpl[i].get_index_fpl_section()==sec_id){
			return this->coupling_fp2co_dikebreak_fpl[i].get_max_observed_waterlevel();
		}
	}
	return 0.0;
}
//Set number of river to river coupling via diversion; here they are also allocated
void Hyd_Coupling_Management::set_rv2rv_diversion(const int number){
	this->number_rv2rv_diversion=number;
	try{
		this->allocate_coupling_class_rv2rv_diversion();
	}
	catch(Error msg){
		throw msg;
	}
}
//Input the parameters for all river coupling via a diversion channel per file
void Hyd_Coupling_Management::read_rv2rv_diversion_per_file(const string file) {
	try {
		for (int i = 0; i < this->number_rv2rv_diversion; i++) {
			this->coupling_1d_diversion[i].input_parameter_per_file(i, file);
		}
	}
	catch (Error msg) {
		throw msg;
	}
}
//Input the parameters for all river coupling via a diversion channel from a selection of a database table
void Hyd_Coupling_Management::read_rv2rv_diversion_per_database(const QSqlTableModel *results, const _sys_system_id id) {
	try {
		for (int i = 0; i < this->number_rv2rv_diversion; i++) {
			this->coupling_1d_diversion[i].set_systemid(id);
			this->coupling_1d_diversion[i].input_parameter_per_database(results, i, true);
		}
	}
	catch (Error msg) {
		throw msg;
	}
}
//Transfer the parameters for a river coupling via a diversion channel to a database
void Hyd_Coupling_Management::transfer_rv2rv_diversion2database(QSqlDatabase *ptr_database) {
	try {
		for (int i = 0; i < this->number_rv2rv_diversion; i++) {
			this->coupling_1d_diversion[i].transfer_diversion_channel2database_table(ptr_database);
		}
	}
	catch (Error msg) {
		throw msg;
	}

}
//Convert the indices of the coupled models into pointer for all river coupling via a diversion channel
void Hyd_Coupling_Management::set_pointer_rv2rv_diversion(Hyd_Model_River *river_models, const int number_rv_models) {
	try {
		for (int i = 0; i < this->number_rv2rv_diversion; i++) {
			this->coupling_1d_diversion[i].convert_indices2pointer(river_models, number_rv_models);
		}
	}
	catch (Error msg) {
		throw msg;
	}
}
//Get the number of river to river coupling via diversion
int Hyd_Coupling_Management::get_rv2rv_diversion(void) {
	return this->number_rv2rv_diversion;
}
//Set number of river to floodplain coupling via a hydraulic structure; here they are also allocated
void Hyd_Coupling_Management::set_rv2fp_structure(const int number) {
	this->number_rv2fp_structure = number;
	try {
		this->allocate_coupling_class_rv2fp_structure();
	}
	catch (Error msg) {
		throw msg;
	}
}
//Input the parameters for a river to floodplain coupling via a hydraulic structure per file
void Hyd_Coupling_Management::read_rv2fp_structure_per_file(const string file) {
	try {
		for (int i = 0; i < this->number_rv2fp_structure; i++) {
			this->coupling_rv2fp_structure[i].input_parameter_per_file(i, file);
		}
	}
	catch (Error msg) {
		throw msg;
	}
}
//Input the parameters for a river to floodplain coupling via a hydraulic structure from a selection of a database table
void Hyd_Coupling_Management::read_rv2fp_structure_per_database(const QSqlQueryModel *results, const _sys_system_id id) {
	try {
		for (int i = 0; i < this->number_rv2fp_structure; i++) {
			this->coupling_rv2fp_structure[i].set_systemid(id);
			this->coupling_rv2fp_structure[i].input_parameter_per_database(results, i, true);
		}
	}
	catch (Error msg) {
		throw msg;
	}
}
//Transfer the parameters for a river to floodplain coupling via a hydraulic structure to a database
void Hyd_Coupling_Management::transfer_rv2fp_structure2database(QSqlDatabase *ptr_database) {
	try {
		for (int i = 0; i < this->number_rv2fp_structure; i++) {
			this->coupling_rv2fp_structure[i].transfer_coupling_structure2database_table(ptr_database);
		}
	}
	catch (Error msg) {
		throw msg;
	}
}
//Get the number of river to floodplain coupling via a hydraulic structure
int Hyd_Coupling_Management::get_rv2fp_structure(void) {
	return this->number_rv2fp_structure;
}
//Set number of river to floodplain coupling via a dikebreak; here they are also allocated
void Hyd_Coupling_Management::set_rv2fp_dikebreak(const int number) {
	this->number_rv2fp_dikebreak = number;
	if (number == 0) {
		return;
	}


	try{
		this->allocate_coupling_class_rv2fp_dikebreak();
	}
	catch(Error msg){
		throw msg;
	}

}
//Input the parameters for a river to floodplain coupling via a dikebreak per file
void Hyd_Coupling_Management::read_rv2fp_dikebreak_per_file(const string file){
	try{
		for(int i=0; i<this->number_rv2fp_dikebreak; i++){
			this->coupling_rv2fp_dikebreak[i].input_parameter_per_file(i, file);
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Input the parameters for a river to floodplain coupling via a dikebreak from a selection of a database table
void Hyd_Coupling_Management::read_rv2fp_dikebreak_per_database(const QSqlQueryModel *results, const _sys_system_id id){
	try{
		for(int i=0; i<this->number_rv2fp_dikebreak; i++){
			this->coupling_rv2fp_dikebreak[i].set_systemid(id);
			this->coupling_rv2fp_dikebreak[i].input_parameter_per_database(results, i, true);
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Transfer the parameters for a river to floodplain coupling via a dikebreak to a database
void Hyd_Coupling_Management::transfer_rv2fp_dikebreak2database(QSqlDatabase *ptr_database){
	try{
		for(int i=0; i<this->number_rv2fp_dikebreak; i++){
			this->coupling_rv2fp_dikebreak[i].transfer_break_data2database_table(ptr_database);
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Get the number of river to floodplain coupling via a dikebreak
int Hyd_Coupling_Management::get_rv2fp_dikebreak(void){
	return this->number_rv2fp_dikebreak;
}
//Set number of coast to floodplain coupling via a dikebreak; here they are also allocated
void Hyd_Coupling_Management::set_fp2co_dikebreak(const int number){
	this->number_fp2co_dikebreak=number;
	if (number == 0) {
		return;
	}
	try{
		this->allocate_coupling_class_fp2co_dikebreak();
	}
	catch(Error msg){
		throw msg;
	}
}
//Input the parameters for a coast to floodplain coupling via a dikebreak per file
void Hyd_Coupling_Management::read_fp2co_dikebreak_per_file(const string file){
	try{
		for(int i=0; i<this->number_fp2co_dikebreak; i++){
			this->coupling_fp2co_dikebreak[i].input_parameter_per_file(i, file);
		}
	}
	catch(Error msg){
		throw msg;
	}

}
//Input the parameters for a coast to floodplain coupling via a dikebreak from a selection of a database table
void Hyd_Coupling_Management::read_fp2co_dikebreak_per_database(const QSqlQueryModel *results, const _sys_system_id id){
	try{
		for(int i=0; i<this->number_fp2co_dikebreak; i++){
			this->coupling_fp2co_dikebreak[i].set_systemid(id);
			this->coupling_fp2co_dikebreak[i].input_parameter_per_database(results, i, true);
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Transfer the parameters for a coast to floodplain coupling via a dikebreak to a database
void Hyd_Coupling_Management::transfer_fp2co_dikebreak2database(QSqlDatabase *ptr_database){
	try{
		for(int i=0; i<this->number_fp2co_dikebreak; i++){
			this->coupling_fp2co_dikebreak[i].transfer_break_data2database_table(ptr_database);
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Get the number of coast to floodplain coupling via a dikebreak
int Hyd_Coupling_Management::get_fp2co_dikebreak(void){
	return this->number_fp2co_dikebreak;
}
//Allocate the coupling classes for river to coast coupling
void Hyd_Coupling_Management::allocate_coupling_class_rv2co(void){
	this->delete_coupling_class_rv2co();
	//river to coast
	if(this->coupling_rv2co==NULL){
		try{
			this->coupling_rv2co=new Hyd_Coupling_RV2CO[this->number_rv2co];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Allocate the coupling classes for river to floodplain coupling
void Hyd_Coupling_Management::allocate_coupling_class_rv2fp(void){
	this->delete_coupling_class_rv2fp();
	if(this->coupling_rv2fp==NULL){
		try{
			this->coupling_rv2fp=new Hyd_Coupling_RV2FP[this->number_rv2fp];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Allocate the coupling classes for floodplain to coast coupling
void Hyd_Coupling_Management::allocate_coupling_class_fp2co(void){
	this->delete_coupling_class_fp2co();
	if(this->coupling_fp2co==NULL){
		try{
			this->coupling_fp2co=new Hyd_Coupling_FP2CO[this->number_fp2co];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Allocate the coupling classes for floodplain to floodplain coupling
void Hyd_Coupling_Management::allocate_coupling_class_fp2fp(void){
	this->delete_coupling_class_fp2fp();
	if(this->coupling_fp2fp==NULL){
		try{
			this->coupling_fp2fp=new Hyd_Coupling_FP2FP[this->number_fp2fp];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

}
//Allocate the coupling classes for river to river coupling
void Hyd_Coupling_Management::allocate_coupling_class_rv2rv(void){
	this->delete_coupling_class_rv2rv();
	if(this->coupling_rv2rv==NULL){
		try{
			this->coupling_rv2rv=new Hyd_Coupling_RV2RV[this->number_rv2rv];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Initialize the couplings
void Hyd_Coupling_Management::init_couplings(void){

	ostringstream cout;

	//river to river as a diversion channel
	if(this->number_rv2rv_diversion>0){
		cout << "Setted coupling between river- and rivermodel as diversion channel..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_rv2rv_diversion; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_1d_diversion[i].init_coupling();
		}
	}
	//river to coast
	if(this->number_rv2co>0){
		cout << "Automatic coupling between rivermodels and coastmodel..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_rv2co; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_rv2co[i].init_coupling();
		}
	}
	//river to river
	if(this->number_rv2rv>0){
		cout << "Automatic coupling between river- and rivermodel..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_rv2rv; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_rv2rv[i].init_coupling();
		}
	}
	//river to floodplain
	if(this->number_rv2fp>0){
		cout << "Automatic coupling between river- and floodplainmodel..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_rv2fp; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_rv2fp[i].init_coupling();
		}
	
		cout << "Merge river- and floodplainmodel couplings..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//merge the coupling classes rv to floodplains
		this->init_coupling_class_rv2fp_merged();
		cout << "Merging river- and floodplainmodel couplings is finished!" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//insert special points to the list of RV2FP_coupling points (e.g. river junctions etc.)
		try{
			this->insert_river_junctions2RV2FPlist();
		}
		catch(Error msg){
			ostringstream info;
			info << "Insert river junction coupling points (RV2FP)"<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		try{
			//insert dikebreak points
			this->insert_breaks2RV2FPlist();
		}
		catch(Error msg){
			ostringstream info;
			info << "Insert break coupling points (RV2FP)"<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		try{
			//add additional points
			this->insert_add_coupling_point2RV2FPlist();
		}
		catch(Error msg){
			ostringstream info;
			info << "Add the additional coupling points (RV2FP)"<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

	//floodplain to coast
	if(this->number_fp2co>0){
		cout << "Automatic coupling between floodplainmodels and coastmodel..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_fp2co; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_fp2co[i].init_coupling();
		}
	
		cout << "Merge coastmodel and floodplainmodel couplings..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//floodplain to coast to merged
		this->init_coupling_class_fp2co_merged();
		cout << "Merging coastmodel and floodplainmodel couplings is finished!" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);

		try{
			//Insert special points to the FP2CO point list (e.g. river junctions)
			this->insert_river_junctions2FP2COlist();
		}
		catch(Error msg){
			ostringstream info;
			info << "Insert river junction coupling points (FP2CO)"<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		try{
			//insert dikebreak points
			this->insert_breaks2FP2COlist();
		}
		catch(Error msg){
			ostringstream info;
			info << "Insert break coupling points (FP2CO)"<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		try{		
			//add additional points
			this->insert_add_coupling_point2FP2COlist();
		}
		catch(Error msg){
			ostringstream info;
			info << "Add the additional coupling points (FP2CO)"<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}


	//floodplain to floodplain
	if(this->number_fp2fp>0){
		cout << "Automatic coupling between floodplainmodels..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_fp2fp; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_fp2fp[i].init_coupling();
		}
	}
	//setted coupling river to floodplain via a hydraulic structure (must be after the automatic coupling of floodplain to river model)
	if(this->number_rv2fp_structure>0){
		cout << "Setted coupling between river- and floodplain-model via a hydraulic structure..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_rv2fp_structure; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_rv2fp_structure[i].init_coupling(this->coupling_merged_rv2fp, this->number_merged_rv2fp);
		}
	}
	try{

		//setted coupling river to floodplain via a dikebreak (must be after the automatic coupling of floodplain to river model)
		if(this->number_rv2fp_dikebreak>0){
			cout << "Setted coupling between river- and floodplain-model via a dikebreak..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			for(int i=0; i< this->number_rv2fp_dikebreak; i++){
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
				this->coupling_rv2fp_dikebreak[i].init_coupling(this->coupling_merged_rv2fp, this->number_merged_rv2fp);
			}
		}
		//setted coupling coast to floodplain via a dikebreak (must be after the automatic coupling of floodplain to river model)
		if(this->number_fp2co_dikebreak>0){
			cout << "Setted coupling between coast- and floodplain-model via a dikebreak..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			for(int i=0; i< this->number_fp2co_dikebreak; i++){
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
				this->coupling_fp2co_dikebreak[i].init_coupling(this->coupling_merged_fp2co);
			}
		}


		//check if the river outflow profile is in a floodplain model and is not coupeld to other river models=> outflow goes to the floodplain
		if(this->number_rv2fp>0){
			cout << "Check if river outflow profiles are not coupled and in a floodplain model ..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			for(int i=0; i< this->number_merged_rv2fp; i++){
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
				this->coupling_merged_rv2fp[i].check_river_outflow2floodplain();
			}
		}
		//check if the river inflow profile is in a floodplain model and is not coupeld to other river models=> outflow goes to the floodplain
		if(this->number_rv2fp>0){
			cout << "Check if river inflow profiles are not coupled and in a floodplain model ..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			for(int i=0; i< this->number_merged_rv2fp; i++){
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
				this->coupling_merged_rv2fp[i].check_river_inflow2floodplain();
			}
		}
		//Check if there there are RV2FP point in the lists, which are coupled beyond river midlines; there a coupling is not possible; do it after the merged lists are completly setted
		this->check_RV2FPpoints_beyond_rivers();

		//delete not needed coupling classes
		this->delete_coupling_class_fp2co();
		this->delete_coupling_class_rv2fp();
	}
	catch(Error msg){
		throw msg;
	}
}
//Synchronise the couplings
void Hyd_Coupling_Management::synchronise_couplings(const double timepoint, const double delta_t, const bool time_check, const int internal_counter, Profiler* profiler){
	//first reset the discharges
	this->reset_coupling_discharges();

	this->delta_t=delta_t;

	if(time_check==false){
		this->old_time_point=timepoint;
	}


	//river to river
	profiler->profile("river2river", Profiler::profilerFlags::START_PROFILING);
	for(int i=0; i< this->number_rv2rv; i++){
		this->coupling_rv2rv[i].synchronise_models();
	}
	profiler->profile("river2river", Profiler::profilerFlags::END_PROFILING);
	//river to river as diversion channel
	for(int i=0; i< number_rv2rv_diversion; i++){
		this->coupling_1d_diversion[i].synchronise_models();
	}
	
	//river to floodplain via a dikebreak
	for(int i=0; i< this->number_rv2fp_dikebreak; i++){
		this->coupling_rv2fp_dikebreak[i].synchronise_models(timepoint, this->delta_t, time_check);
	}
	//river to floodplain via a dikebreak set by the fpl-system
	for(int i=0; i< this->number_rv2fp_dikebreak_fpl; i++){
		this->coupling_rv2fp_dikebreak_fpl[i].synchronise_models(timepoint,this->delta_t, time_check);
	}

	
	//river to floodplain (after dikebreak coupling)
	for(int i=0; i< this->number_merged_rv2fp; i++){
		this->coupling_merged_rv2fp[i].synchronise_models(timepoint,this->delta_t, time_check, internal_counter);
	}
	


	//river to floodplain via a hydraulic structure
	profiler->profile("river2fp", Profiler::profilerFlags::START_PROFILING);
	for(int i=0; i< this->number_rv2fp_structure; i++){
		this->coupling_rv2fp_structure[i].synchronise_models(timepoint, this->delta_t, time_check);
	}
	profiler->profile("river2fp", Profiler::profilerFlags::END_PROFILING);
	//river to coast
	for(int i=0; i< this->number_rv2co; i++){
		this->coupling_rv2co[i].synchronise_models();
	}
	

	//floodplain to floodplain
	profiler->profile("fp2fp", Profiler::profilerFlags::START_PROFILING);
	for(int i=0; i< this->number_fp2fp; i++){
		this->coupling_fp2fp[i].synchronise_models(timepoint, this->delta_t, time_check, internal_counter);
	}
	profiler->profile("fp2fp", Profiler::profilerFlags::END_PROFILING);

	//coast to floodplain via a dikebreak
	for(int i=0; i< this->number_fp2co_dikebreak; i++){
		this->coupling_fp2co_dikebreak[i].synchronise_models(timepoint,this->delta_t, time_check);
	}
	//coast to floodplain via a dikebreak set by the fpl-system
	for(int i=0; i< this->number_fp2co_dikebreak_fpl; i++){
		this->coupling_fp2co_dikebreak_fpl[i].synchronise_models(timepoint,this->delta_t, time_check);
	}
	//floodplain to coast (after dikebreak)
	for(int i=0; i< this->number_merged_fp2co; i++){
		this->coupling_merged_fp2co[i].synchronise_models(timepoint,this->delta_t, time_check, internal_counter);
	}


}
//Output the couplings (e.g. dikebreak development) per calculation step
void Hyd_Coupling_Management::output_coupling_calculation_steps2file(const double timepoint){
	
	if (this->ptr_output_flags->tecplot_1d_required == true) {
		//river to floodplain
		for (int i = 0; i < this->number_rv2fp_dikebreak; i++) {
			this->coupling_rv2fp_dikebreak[i].output_results2file_tecplot(timepoint);
		}
		for (int i = 0; i < this->number_rv2fp_dikebreak_fpl; i++) {
			this->coupling_rv2fp_dikebreak_fpl[i].output_results2file_tecplot(timepoint);
		}
		//coast to floodplain
		for (int i = 0; i < this->number_fp2co_dikebreak; i++) {
			this->coupling_fp2co_dikebreak[i].output_results2file_tecplot(timepoint);
		}
		for (int i = 0; i < this->number_fp2co_dikebreak_fpl; i++) {
			this->coupling_fp2co_dikebreak_fpl[i].output_results2file_tecplot(timepoint);
		}
	}
	if (this->ptr_output_flags->paraview_1d_required == true) {
		//river to floodplain
		for (int i = 0; i < this->number_rv2fp_dikebreak; i++) {
			this->coupling_rv2fp_dikebreak[i].output_results2file_csv(timepoint);
		}
		for (int i = 0; i < this->number_rv2fp_dikebreak_fpl; i++) {
			this->coupling_rv2fp_dikebreak_fpl[i].output_results2file_csv(timepoint);
		}
		//coast to floodplain
		for (int i = 0; i < this->number_fp2co_dikebreak; i++) {
			this->coupling_fp2co_dikebreak[i].output_results2file_csv(timepoint);
		}
		for (int i = 0; i < this->number_fp2co_dikebreak_fpl; i++) {
			this->coupling_fp2co_dikebreak_fpl[i].output_results2file_csv(timepoint);
		}
	}
	
	//clear lists
	//river to floodplain
	for (int i = 0; i < this->number_rv2fp_dikebreak; i++) {
		this->coupling_rv2fp_dikebreak[i].clear_break_list();
	}
	for (int i = 0; i < this->number_rv2fp_dikebreak_fpl; i++) {
		this->coupling_rv2fp_dikebreak_fpl[i].clear_break_list();
	}
	//coast to floodplain
	for (int i = 0; i < this->number_fp2co_dikebreak; i++) {
		this->coupling_fp2co_dikebreak[i].clear_break_list();
	}
	for (int i = 0; i < this->number_fp2co_dikebreak_fpl; i++) {
		this->coupling_fp2co_dikebreak_fpl[i].clear_break_list();
	}


}
//Reset the coupling for a new hydraulic simulation without deleting the objects
void Hyd_Coupling_Management::reset_couplings(void){



	//river to floodplain
	for(int i=0; i< this->number_merged_rv2fp; i++){
		this->coupling_merged_rv2fp[i].list_left.reset_points();
		this->coupling_merged_rv2fp[i].list_right.reset_points();
	}


	//river to floodplain via a hydraulic structure
	for(int i=0; i< this->number_rv2fp_structure; i++){
		this->coupling_rv2fp_structure[i].reset_hydro_balance_max_values();
		this->coupling_rv2fp_structure[i].reset_smoothing();
	}
	//river to floodplain via a dikebreak
	for(int i=0; i< this->number_rv2fp_dikebreak; i++){
		this->coupling_rv2fp_dikebreak[i].reset_hydro_balance_max_values();
		this->coupling_rv2fp_dikebreak[i].reset_smoothing();
	}

	//coast to floodplain via a dikebreak
	for(int i=0; i< this->number_fp2co_dikebreak; i++){
		this->coupling_fp2co_dikebreak[i].reset_hydro_balance_max_values();
		this->coupling_fp2co_dikebreak[i].reset_smoothing();
	}

	//floodplain to coast
	for(int i=0; i< this->number_merged_fp2co; i++){
		this->coupling_merged_fp2co[i].list.reset_points();
	}
	//floodplain to floodplain
	for(int i=0; i< this->number_fp2fp; i++){
		this->coupling_fp2fp[i].list.reset_points();
	}

	this->old_time_point=0.0;
	this->delta_t=0.0;

}
//Total reset of the couplings
void Hyd_Coupling_Management::total_reset(void){
	this->delete_coupling_class_rv2co();
	this->delete_coupling_class_rv2fp();
	this->delete_coupling_class_fp2co();
	this->delete_coupling_class_fp2fp();
	this->delete_coupling_class_rv2rv();
	this->delete_coupling_class_rv2rv_diversion();
	this->delete_coupling_class_rv2fp_structure();
	this->delete_coupling_class_rv2fp_dikebreak();
	this->delete_coupling_class_fp2co_dikebreak();
	this->delete_coupling_class_fp2co_merged();
	this->delete_coupling_class_rv2fp_merged();
	this->delete_coupling_class_rv2fp_dikebreak_fpl();
	this->delete_coupling_class_fp2co_dikebreak_fpl();
	this->total_number=0;
	this->number_rv2co=0;
	this->number_fp2fp=0;
	this->number_rv2fp=0;
	this->number_merged_rv2fp=0;
	this->number_fp2co=0;
	this->number_merged_fp2co=0;
	this->number_rv2rv=0;
	this->number_rv2rv_diversion=0;
	this->number_rv2fp_structure=0;	
	this->number_rv2fp_dikebreak=0;
	this->number_fp2co_dikebreak=0;
	this->number_rv2fp_direct_out=0;
	this->number_rv2fp_direct_in=0;
	this->number_rv2fp_dikebreak_fpl=0;
	this->number_fp2co_dikebreak_fpl=0;
	this->total_number_coupling=0;
	this->old_time_point=0.0;
	this->delta_t=0.0;

}
//Clone the couplings
void Hyd_Coupling_Management::clone_couplings(Hyd_Coupling_Management *coupling, Hyd_Hydraulic_System *system){
	ostringstream cout;
	this->total_number=coupling->total_number;
	
	this->number_rv2co=coupling->number_rv2co;
	if(this->number_rv2co>0){
		this->allocate_coupling_class_rv2co();
		cout << "Clone "<<this->number_rv2co<<" river to coast coupling(s)..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_rv2co; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_rv2co[i].clone_couplings(&coupling->coupling_rv2co[i], system);
		}
	}
	this->number_fp2fp=coupling->number_fp2fp;
	if(this->number_fp2fp>0){
		this->allocate_coupling_class_fp2fp();
		cout << "Clone "<<this->number_fp2fp<<" floodplain to floodplain coupling(s)..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_fp2fp; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_fp2fp[i].clone_couplings(&coupling->coupling_fp2fp[i], system);
		}
	}
	this->number_rv2fp=coupling->number_rv2fp;
	this->number_merged_rv2fp=coupling->number_merged_rv2fp;
	if(this->number_merged_rv2fp>0){
		this->allocate_coupling_class_rv2fp_merged();
		cout << "Clone "<<this->number_merged_rv2fp<<" river to floodplain merged coupling(s)..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_merged_rv2fp; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_merged_rv2fp[i].clone_couplings(&coupling->coupling_merged_rv2fp[i], system);
		}
	}
	this->number_fp2co=coupling->number_fp2co;
	this->number_merged_fp2co=coupling->number_merged_fp2co;
	if(this->number_merged_fp2co>0){
		this->allocate_coupling_class_fp2co_merged();
		cout << "Clone "<<this->number_merged_fp2co<<" coast to floodplain merged coupling(s)..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_merged_fp2co; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_merged_fp2co[i].clone_couplings(&coupling->coupling_merged_fp2co[i], system);
		}
	}


	this->number_rv2rv=coupling->number_rv2rv;
	if(this->number_rv2rv>0){
		this->allocate_coupling_class_rv2rv();
		cout << "Clone "<<this->number_rv2rv<<" river to river coupling(s)..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_rv2rv; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_rv2rv[i].clone_couplings(&coupling->coupling_rv2rv[i], system);
		}
	}
	
	this->number_rv2rv_diversion=coupling->number_rv2rv_diversion;
	if(this->number_rv2rv_diversion>0){
		this->allocate_coupling_class_rv2rv_diversion();
		cout << "Clone "<<this->number_rv2rv_diversion<<" river to river as diversion channel coupling(s)..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_rv2rv_diversion; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_1d_diversion[i].clone_couplings(&coupling->coupling_1d_diversion[i], system);
		}
	}

	this->number_rv2fp_structure=coupling->number_rv2fp_structure;
	if(this->number_rv2fp_structure>0){
		this->allocate_coupling_class_rv2fp_structure();
		cout << "Clone "<<this->number_rv2fp_structure<<" river to floodplain as structure coupling(s)..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_rv2fp_structure; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_rv2fp_structure[i].clone_couplings(&coupling->coupling_rv2fp_structure[i], system);
		}
	}


	this->number_rv2fp_dikebreak=coupling->number_rv2fp_dikebreak;
	if(this->number_rv2fp_dikebreak>0){
		this->allocate_coupling_class_rv2fp_dikebreak();
		cout << "Clone "<<this->number_rv2fp_dikebreak<<" river to floodplain as dikebreak coupling(s)..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_rv2fp_dikebreak; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_rv2fp_dikebreak[i].clone_couplings(&coupling->coupling_rv2fp_dikebreak[i], this->coupling_merged_rv2fp, this->number_merged_rv2fp);
		}
	}
	this->number_fp2co_dikebreak=coupling->number_fp2co_dikebreak;
	if(this->number_fp2co_dikebreak>0){
		this->allocate_coupling_class_fp2co_dikebreak();
		cout << "Clone "<<this->number_fp2co_dikebreak<<" coast to floodplain as dikebreak coupling(s)..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for(int i=0; i< this->number_fp2co_dikebreak; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_fp2co_dikebreak[i].clone_couplings(&coupling->coupling_fp2co_dikebreak[i], this->coupling_merged_fp2co);
		}
	}

	this->number_rv2fp_direct_out=coupling->number_rv2fp_direct_out;
	this->number_rv2fp_direct_in=coupling->number_rv2fp_direct_in;
	this->total_number_coupling=coupling->total_number_coupling;
	this->number_rv2fp_dikebreak_fpl=coupling->number_rv2fp_dikebreak_fpl;
	this->number_fp2co_dikebreak_fpl=coupling->number_fp2co_dikebreak_fpl;
}
//Initialize output files, e.g. dike break
void Hyd_Coupling_Management::init_output_files(void){
	for (int i = 0; i < this->number_rv2fp_dikebreak; i++) {
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		if (this->ptr_output_flags->tecplot_1d_required == true) {
			this->coupling_rv2fp_dikebreak[i].init_output2file_tecplot();
		}
		if (this->ptr_output_flags->paraview_1d_required == true) {
			this->coupling_rv2fp_dikebreak[i].init_output2file_csv();
		}


	}

	for(int i=0; i< this->number_fp2co_dikebreak; i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		if (this->ptr_output_flags->tecplot_1d_required == true) {
			this->coupling_fp2co_dikebreak[i].init_output2file_tecplot();
		}
		if (this->ptr_output_flags->paraview_1d_required == true) {
			this->coupling_fp2co_dikebreak[i].init_output2file_csv();
		}
	}
}
//Set the pointer to the output flags
void Hyd_Coupling_Management::set_ptr2outputflags(_hyd_output_flags *flags) {
	this->ptr_output_flags = flags;
}
//__________________
//private
//Insert special points to the merged RV2FP point list (e.g. river junctions, diversion channels)
void Hyd_Coupling_Management::insert_river_junctions2RV2FPlist(void){
	//diversion channels
	for(int i=0; i <this->number_rv2rv_diversion;i++){
		for(int j=0; j< this->number_merged_rv2fp;j++){
			this->coupling_1d_diversion[i].insert_junction_inflow_point2RV2FPlist(&(this->coupling_merged_rv2fp[j].list_left), this->coupling_merged_rv2fp[j].get_river_index() );
			this->coupling_1d_diversion[i].insert_junction_inflow_point2RV2FPlist(&(this->coupling_merged_rv2fp[j].list_right),this->coupling_merged_rv2fp[j].get_river_index());
			this->coupling_1d_diversion[i].insert_junction_outflow_point2RV2FPlist(&(this->coupling_merged_rv2fp[j].list_left), this->coupling_merged_rv2fp[j].get_river_index() );
			this->coupling_1d_diversion[i].insert_junction_outflow_point2RV2FPlist(&(this->coupling_merged_rv2fp[j].list_right),this->coupling_merged_rv2fp[j].get_river_index());
		}
	}
	
	//river to river
	for(int i=0; i<this->number_rv2rv; i++){
		for(int j=0; j< this->number_merged_rv2fp;j++){
			this->coupling_rv2rv[i].insert_junction_inflow_point2RV2FPlist(&(this->coupling_merged_rv2fp[j].list_left), this->coupling_merged_rv2fp[j].get_river_index() );
			this->coupling_rv2rv[i].insert_junction_inflow_point2RV2FPlist(&(this->coupling_merged_rv2fp[j].list_right),this->coupling_merged_rv2fp[j].get_river_index());
		}
	}
}
//Insert special points to the merged RV2FP point list (e.g. dikebreak)
void Hyd_Coupling_Management::insert_breaks2RV2FPlist(void){
	if(this->number_merged_rv2fp>0){
		for(int i=0; i<this->number_rv2fp_dikebreak; i++){
			for(int j=0; j< this->number_merged_rv2fp;j++){
				this->coupling_rv2fp_dikebreak[i].set_additional_coupling_point(&(this->coupling_merged_rv2fp[j]));
			}
		}
	}
}
//Insert additional coupling points for a finer coupling discretisation to the merged RV2FP point list
void Hyd_Coupling_Management::insert_add_coupling_point2RV2FPlist(void){
	
	for(int j=0; j< this->number_merged_rv2fp;j++){
		try{
			this->coupling_merged_rv2fp[j].list_left.add_additional_coupling_points();
			this->coupling_merged_rv2fp[j].list_right.add_additional_coupling_points();
		}
		catch(Error msg){
			ostringstream info;
			info << "Merging list : " << j <<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Insert special points to the merged FP2CO point list (e.g. river junctions)
void Hyd_Coupling_Management::insert_river_junctions2FP2COlist(void){

	if(this->number_rv2co==0){
		return;
	}
	for(int i=0; i<this->number_merged_fp2co; i++){
		this->coupling_merged_fp2co[i].insert_river_junctions(this->coupling_rv2co, this->number_rv2co);
	}
}
//Insert special points to the merged FP2CO point list (e.g. dikebreak)
void Hyd_Coupling_Management::insert_breaks2FP2COlist(void){
	if(this->number_merged_fp2co>0){
		for (int i=0; i< this->number_fp2co_dikebreak; i++){
			this->coupling_fp2co_dikebreak[i].set_additional_coupling_point(&(this->coupling_merged_fp2co[0]));
		}
		//TODO
		if (this->number_fp2co_dikebreak > 0) {
			this->coupling_merged_fp2co[0].list.sort_distance_along_polysegment();
			this->coupling_merged_fp2co[0].list.calculate_alldistance_up_down();
			this->coupling_merged_fp2co[0].list.transfer_informations2points();
		}
	}
}
//Insert additional coupling points for a finer coupling discretisation to the merged FP2CO point list
void Hyd_Coupling_Management::insert_add_coupling_point2FP2COlist(void){
	for(int j=0; j< this->number_merged_fp2co;j++){
		this->coupling_merged_fp2co[j].list.add_additional_coupling_points();
	}
}
//Check if there there are RV2FP point in the lists, which are coupled beyond river; there a coupling is not possible; do it after the merged lists are completly setted
void Hyd_Coupling_Management::check_RV2FPpoints_beyond_rivers(void){

	for(int i=0; i< this->number_merged_rv2fp; i++){
		//make a check for itself
		this->coupling_merged_rv2fp[i].check_coupling2_other_river_side();
		//check with other rivers
		for(int j=0; j < this->number_merged_rv2fp; j++){
			if(j!=i){
				this->coupling_merged_rv2fp[i].check_coupling2_other_river_side(&this->coupling_merged_rv2fp[j]);
			}
		
		}
	}
}
//Reset coupling discharges between the model (use it before syncronisation)
void Hyd_Coupling_Management::reset_coupling_discharges(void){
	//river to river
	for(int i=0; i< this->number_rv2rv; i++){
		this->coupling_rv2rv[i].reset_coupling_discharges();
	}
	//river to river as diversion channel
	for(int i=0; i< this->number_rv2rv_diversion; i++){
		this->coupling_1d_diversion[i].reset_coupling_discharges();
	}
	//river to floodplain
	for(int i=0; i< this->number_merged_rv2fp; i++){
		this->coupling_merged_rv2fp[i].reset_coupling_discharges();
	}
	//river to floodplain via a hydraulic structure
	for(int i=0; i< this->number_rv2fp_structure; i++){
		this->coupling_rv2fp_structure[i].reset_coupling_discharge();
	}
	//river to floodplain via a dikebreak
	for(int i=0; i< this->number_rv2fp_dikebreak; i++){
		this->coupling_rv2fp_dikebreak[i].reset_coupling_discharge();
	}
	//coast to floodplain via a dikebreak
	for(int i=0; i< this->number_fp2co_dikebreak; i++){
		this->coupling_fp2co_dikebreak[i].reset_coupling_discharge();
	}
	//floodplain to coast
	for(int i=0; i< this->number_merged_fp2co; i++){
		this->coupling_merged_fp2co[i].list.reset_coupling_discharge();
	}
	//floodplain to floodplain
	for(int i=0; i< this->number_fp2fp; i++){
		this->coupling_fp2fp[i].list.reset_coupling_discharge();
	}
	//river to floodplain via a dikebreak set by the fpl-system
	for(int i=0; i< this->number_rv2fp_dikebreak_fpl; i++){
		this->coupling_rv2fp_dikebreak_fpl[i].reset_coupling_discharge();
	}
	//coast to floodplain via a dikebreak set by the fpl-system
	for(int i=0; i< this->number_fp2co_dikebreak_fpl; i++){
		this->coupling_fp2co_dikebreak_fpl[i].reset_coupling_discharge();
	}
}
//Delete the coupling classes for river to coast coupling
void Hyd_Coupling_Management::delete_coupling_class_rv2co(void){
	if(this->coupling_rv2co!=NULL){
		delete []this->coupling_rv2co;
		this->coupling_rv2co=NULL;
	}
}
//Delete the coupling classes for river to floodplain coupling
void Hyd_Coupling_Management::delete_coupling_class_rv2fp(void){
	if(this->coupling_rv2fp!=NULL){
		delete []this->coupling_rv2fp;
		this->coupling_rv2fp=NULL;
	}

}
//Delete the coupling classes for floodplain to coast coupling
void Hyd_Coupling_Management::delete_coupling_class_fp2co(void){
	if(this->coupling_fp2co!=NULL){
		delete []this->coupling_fp2co;
		this->coupling_fp2co=NULL;
	}
}
//Delete the coupling classes for floodplain to floodplain coupling
void Hyd_Coupling_Management::delete_coupling_class_fp2fp(void){
	if(this->coupling_fp2fp!=NULL){
		delete []this->coupling_fp2fp;
		this->coupling_fp2fp=NULL;
	}
}
//Delete the coupling classes for river to river coupling
void Hyd_Coupling_Management::delete_coupling_class_rv2rv(void){
	if(this->coupling_rv2rv!=NULL){
		delete []this->coupling_rv2rv;
		this->coupling_rv2rv=NULL;
	}
}
//Init the coupling classes for river to multiple floodplains (merged)
void Hyd_Coupling_Management::init_coupling_class_rv2fp_merged(void){
	if(this->number_rv2fp>0){
		int *counter=NULL;
		//count the number of needed merged objects
		try{
			counter=new int[this->number_rv2fp];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		//init it 
		for(int i=0; i< this->number_rv2fp;i++){
			counter[i]=-1;
		}
		//set the indices to the counter
		int counter_models=0;
		for(int i=0; i< this->number_rv2fp;i++){
			int index=this->coupling_rv2fp[i].get_river_index();
			for(int j=0; j<this->number_rv2fp;j++){
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
				if(counter[j]>=0 && counter[j]==index){
					break;
				}
				else if(counter[j]<0 && counter[j]!=index){
					counter[j]=index;
					counter_models++;
					break;
				}
			}
		}
		//set the number to the merged class
		this->number_merged_rv2fp=counter_models;
		try{
			this->allocate_coupling_class_rv2fp_merged();
		}
		catch(Error msg){
			throw msg;
		}
		ostringstream cout;
		//set the river models pointer to the merging class
	
		for(int i=0; i< this->number_merged_rv2fp;i++){
			for(int j=0; j< this->number_rv2fp; j++){
				cout << "Merging river " << i << " to floodplain coupling "<< j<<"..." << endl ;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
				this->coupling_merged_rv2fp[i].set_ptr_coupling_with_merging(&(this->coupling_rv2fp[j]));
			}
		}

		//set up list of the involved fpl-section ids
		for(int i=0; i< this->number_merged_rv2fp; i++){
			this->coupling_merged_rv2fp[i].set_up_fpl_section_ids_list();

		}

		//delete it
		if(counter!=NULL){
			delete []counter;
			counter=NULL;
		}

	}
}
//Init the coupling classes for multiple floodplains to one coast model (merged)
void Hyd_Coupling_Management::init_coupling_class_fp2co_merged(void){
	if(this->number_fp2co>0){
		//there is just one doupling class
		this->number_merged_fp2co=1;
		this->allocate_coupling_class_fp2co_merged();
		for(int i=0; i<this->number_fp2co;i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->coupling_merged_fp2co[0].set_ptr_coupling_with_merging(&this->coupling_fp2co[i]);
		}

		//set up list of the involved fpl-section ids
		this->coupling_merged_fp2co[0].set_up_fpl_section_ids_list();
	}
}

//Allocate the coupling classes for river to multiple floodplains (merged)
void Hyd_Coupling_Management::allocate_coupling_class_rv2fp_merged(void){
	this->delete_coupling_class_rv2fp_merged();
	if(this->number_merged_rv2fp==0){
		return;
	}
	if(this->coupling_merged_rv2fp==NULL){
		try{
			this->coupling_merged_rv2fp=new Hyd_Coupling_RV2FP_Merged[this->number_merged_rv2fp];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Allocate the coupling classes for multiple floodplains to one coast model (merged)
void Hyd_Coupling_Management::allocate_coupling_class_fp2co_merged(void){
	this->delete_coupling_class_fp2co_merged();
	if(this->number_merged_fp2co==0){
		return;
	}
	if(this->coupling_merged_fp2co==NULL){
		try{
			this->coupling_merged_fp2co=new Hyd_Coupling_FP2CO_Merged[this->number_merged_fp2co];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Delete the coupling classes for river to multiple floodplains (merged)
void Hyd_Coupling_Management::delete_coupling_class_rv2fp_merged(void){
	if(this->coupling_merged_rv2fp!=NULL){
		delete []this->coupling_merged_rv2fp;
		this->coupling_merged_rv2fp=NULL;
	}
}
//Delete the coupling classes for multiple floodplains to one coast model (merged)
void Hyd_Coupling_Management::delete_coupling_class_fp2co_merged(void){
	if(this->coupling_merged_fp2co!=NULL){
		delete []this->coupling_merged_fp2co;
		this->coupling_merged_fp2co=NULL;
	}
}
//Allocate the coupling classes for river to river coupling via diversion
void Hyd_Coupling_Management::allocate_coupling_class_rv2rv_diversion(void){
	this->delete_coupling_class_rv2rv_diversion();
	if(this->number_rv2rv_diversion==0){
		return;
	}
	if(this->coupling_1d_diversion==NULL){
		try{
			this->coupling_1d_diversion=new Hyd_Coupling_RV2RV_Diversion[this->number_rv2rv_diversion];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		for(int i=0; i< this->number_rv2rv_diversion; i++){
			this->coupling_1d_diversion[i].set_systemid(this->system_id);
		}
	}
}
//Delete the coupling classes for river to river coupling via diversion
void Hyd_Coupling_Management::delete_coupling_class_rv2rv_diversion(void){
	if(this->coupling_1d_diversion!=NULL){
		delete []this->coupling_1d_diversion;
		this->coupling_1d_diversion=NULL;
	}
}
//Allocate the coupling classes for river to floodplain coupling via a hydraulic structure
void Hyd_Coupling_Management::allocate_coupling_class_rv2fp_structure(void){
	this->delete_coupling_class_rv2fp_structure();
	if(this->coupling_rv2fp_structure==NULL){
		try{
			this->coupling_rv2fp_structure=new Hyd_Coupling_RV2FP_Structure[this->number_rv2fp_structure];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		for(int i=0; i< this->number_rv2fp_structure; i++){
			coupling_rv2fp_structure[i].set_systemid(this->system_id);
		}
	}
}
//Delete the coupling classes for river to floodplain coupling via a hydraulic structure
void Hyd_Coupling_Management::delete_coupling_class_rv2fp_structure(void){
	if(this->coupling_rv2fp_structure!=NULL){
		delete []this->coupling_rv2fp_structure;
		this->coupling_rv2fp_structure=NULL;
	}
}
//Allocate the coupling classes for river to floodplain coupling via a dikebreak
void Hyd_Coupling_Management::allocate_coupling_class_rv2fp_dikebreak(void){
	this->delete_coupling_class_rv2fp_dikebreak();
	if(this->coupling_rv2fp_dikebreak==NULL){
		try{
			this->coupling_rv2fp_dikebreak=new Hyd_Coupling_RV2FP_Dikebreak[this->number_rv2fp_dikebreak];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		for(int i=0; i< this->number_rv2fp_dikebreak; i++){
			this->coupling_rv2fp_dikebreak[i].set_systemid(this->system_id);
			this->coupling_rv2fp_dikebreak[i].set_flag_user_defined(true);
		}
	}
}
//Delete the coupling classes for river to floodplain coupling via a dikebreak
void Hyd_Coupling_Management::delete_coupling_class_rv2fp_dikebreak(void){
	if(this->coupling_rv2fp_dikebreak!=NULL){
		delete []this->coupling_rv2fp_dikebreak;
		this->coupling_rv2fp_dikebreak=NULL;
	}
}
//Allocate the coupling classes for river to floodplain coupling via a dikebreak auotmatically set by the fpl-system
void Hyd_Coupling_Management::allocate_coupling_class_rv2fp_dikebreak_fpl(void){
	this->delete_coupling_class_rv2fp_dikebreak_fpl();
	if(this->coupling_rv2fp_dikebreak_fpl==NULL){
		try{
			this->coupling_rv2fp_dikebreak_fpl=new Hyd_Coupling_RV2FP_Dikebreak[this->number_rv2fp_dikebreak_fpl];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		for(int i=0; i< this->number_rv2fp_dikebreak_fpl; i++){
			this->coupling_rv2fp_dikebreak_fpl[i].set_systemid(this->system_id);
			this->coupling_rv2fp_dikebreak_fpl[i].set_flag_user_defined(false);
		}
	}
}
//Delete the coupling classes for river to floodplain coupling via a dikebreak auotmatically set by the fpl-system
void Hyd_Coupling_Management::delete_coupling_class_rv2fp_dikebreak_fpl(void){
	if(this->coupling_rv2fp_dikebreak_fpl!=NULL){
		delete []this->coupling_rv2fp_dikebreak_fpl;
		this->coupling_rv2fp_dikebreak_fpl=NULL;
	}
}
//Allocate the coupling classes for coast to floodplain coupling via a dikebreak
void Hyd_Coupling_Management::allocate_coupling_class_fp2co_dikebreak(void){
	this->delete_coupling_class_fp2co_dikebreak();
	if(this->coupling_fp2co_dikebreak==NULL){
		try{
			this->coupling_fp2co_dikebreak=new Hyd_Coupling_FP2CO_Dikebreak[this->number_fp2co_dikebreak];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		for(int i=0; i< this->number_fp2co_dikebreak; i++){
			this->coupling_fp2co_dikebreak[i].set_systemid(this->system_id);
			this->coupling_fp2co_dikebreak[i].set_flag_user_defined(true);
		}
	}

}
//Delete the coupling classes for coast to floodplain coupling via a dikebreak
void Hyd_Coupling_Management::delete_coupling_class_fp2co_dikebreak(void){
	if(this->coupling_fp2co_dikebreak!=NULL){
		delete []this->coupling_fp2co_dikebreak;
		this->coupling_fp2co_dikebreak=NULL;
	}
}
//Allocate the coupling classes for coast to floodplain coupling via a dikebreak auotmatically set by the fpl-system
void Hyd_Coupling_Management::allocate_coupling_class_fp2co_dikebreak_fpl(void){
	this->delete_coupling_class_fp2co_dikebreak_fpl();
	if(this->coupling_fp2co_dikebreak_fpl==NULL){
		try{
			this->coupling_fp2co_dikebreak_fpl=new Hyd_Coupling_FP2CO_Dikebreak[this->number_fp2co_dikebreak_fpl];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		for(int i=0; i< this->number_fp2co_dikebreak_fpl; i++){
			this->coupling_fp2co_dikebreak_fpl[i].set_systemid(this->system_id);
			this->coupling_fp2co_dikebreak_fpl[i].set_flag_user_defined(false);
		}
	}
}
//Delete the coupling classes for coast to floodplain coupling via a dikebreak auotmatically set by the fpl-system
void Hyd_Coupling_Management::delete_coupling_class_fp2co_dikebreak_fpl(void){
	if(this->coupling_fp2co_dikebreak_fpl!=NULL){
		delete []this->coupling_fp2co_dikebreak_fpl;
		this->coupling_fp2co_dikebreak_fpl=NULL;
	}
}
//Output the coupling statistics
void Hyd_Coupling_Management::output_coupling_statistic(void){
	this->calculate_total_number_coupling();
	if(this->total_number>0){
		ostringstream cout;	
		cout << "SET COUPLINGS " << endl;
		cout << " Number of rv2rv                   :"<<W(15)<< this->number_rv2rv<< endl;
		cout << " Number of rv2rv as diversion      :"<<W(15)<< this->number_rv2rv_diversion<< endl;
		cout << " Number of merged rv2fp            :"<<W(15)<< this->number_merged_rv2fp<<W(3) <<"("<<this->number_rv2fp<<")"<< endl;
		cout << " Number of rv2fp as structure      :"<<W(15)<< this->number_rv2fp_structure<< endl;
		cout << " Number of rv2fp direct out        :"<<W(15)<< this->number_rv2fp_direct_out<< endl;
		cout << " Number of rv2fp direct in         :"<<W(15)<< this->number_rv2fp_direct_in<< endl;
		cout << " Number of rv2fp as dikebreak      :"<<W(15)<< this->number_rv2fp_dikebreak<< endl;
		cout << " Number of rv2co                   :"<<W(15)<< this->number_rv2co<< endl;
		cout << " Number of merged fp2fp            :"<<W(15)<< this->number_fp2fp<< endl;
		cout << " Number of merged fp2co            :"<<W(15)<< this->number_merged_fp2co<<W(3) <<"("<<this->number_fp2co<<")"<< endl;
		cout << " Number of fp2co as dikebreak      :"<<W(15)<< this->number_fp2co_dikebreak << endl;
		cout << " Total number                      :"<<W(15)<< this->total_number<< endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
}
//Calculated total number of couplings
void Hyd_Coupling_Management::calculate_total_number_coupling(void){
	this->number_rv2fp_direct_out=0;
	this->number_rv2fp_direct_in=0;
	//count the rv2fp direct couplings (outflow)
	for(int i=0; i<this->number_merged_rv2fp; i++){
		if(this->coupling_merged_rv2fp[i].get_direct_coupling_flag_out()==true){
			this->number_rv2fp_direct_out++;
		}
	}
	//count the rv2fp direct couplings (inflow)
	for(int i=0; i<this->number_merged_rv2fp; i++){
		if(this->coupling_merged_rv2fp[i].get_direct_coupling_flag_in()==true){
			this->number_rv2fp_direct_in++;
		}
	}

	//make the statistic
	this->total_number=0;
	this->total_number=this->number_merged_fp2co+
		this->number_rv2co+this->number_merged_rv2fp+this->number_rv2rv+
		this->number_fp2fp+
		this->number_rv2rv_diversion+this->number_rv2fp_structure+
		this->number_fp2co_dikebreak+this->number_rv2fp_dikebreak+
		this->number_rv2fp_direct_out+this->number_rv2fp_direct_in;
}
//Generate a breach-index for the break-coupling between river - and floodplain-models
int Hyd_Coupling_Management::generate_breachindex_rv2fp_break_coupling(void){


	int index=0;
	bool failure=false;
	bool changed=false;

	do{
		changed=false;
		for(int i=0; i< this->number_rv2fp_dikebreak; i++){
			if(index==this->coupling_rv2fp_dikebreak[i].get_breach_index()){
				i=0;
				index++;
				if(index>1000000){
					failure=true;
					break;

				}
			}
		}
		for(int i=0; i< this->number_rv2fp_dikebreak_fpl; i++){
			if(index==this->coupling_rv2fp_dikebreak_fpl[i].get_breach_index()){
				i=0;
				index++;
				changed=true;
				if(index>1000000){
					failure=true;
					break;
				}
			}
		}
	}
	while(changed==true);

	if(failure==true){
		Error msg=this->set_error(1);
		throw msg;
	}

	return index;
}
//Generate a breach-index for the break-coupling between coast - and floodplain-models
int Hyd_Coupling_Management::generate_breachindex_fp2co_break_coupling(void){


	int index=0;
	bool failure=false;
	bool changed=false;

	do{
		changed=false;
		for(int i=0; i< this->number_fp2co_dikebreak; i++){
			if(index==this->coupling_fp2co_dikebreak[i].get_breach_index()){
				i=0;
				index++;
				if(index>1000000){
					failure=true;
					break;

				}
			}
		}
		for(int i=0; i< this->number_fp2co_dikebreak_fpl; i++){
			if(index==this->coupling_fp2co_dikebreak_fpl[i].get_breach_index()){
				i=0;
				index++;
				changed=true;
				if(index>1000000){
					failure=true;
					break;
				}
			}
		}
	}
	while(changed==true);

	if(failure==true){
		Error msg=this->set_error(2);
		throw msg;
	}

	return index;
}
//set the error
Error Hyd_Coupling_Management::set_error(const int err_type){
	string place="Hyd_Coupling_Management::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;
	switch (err_type){
		case 0://bad alloc
			place.append("allocate_coupling_class_...(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://gernerate indices
			place.append("generate_breachindex_rv2fp_break_coupling(void)");
			reason="Can not find a suitable breach index";
			help="Check the code";
			type=6;
			break;
		case 2://gernerate indices
			place.append("generate_breachindex_fp2co_break_coupling(void)");
			reason="Can not find a suitable breach index";
			help="Check the code";
			type=6;
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