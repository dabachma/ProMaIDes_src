#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_RV2CO.h"

//constructor
Hyd_Coupling_RV2CO::Hyd_Coupling_RV2CO(void){
	this->coast_model=NULL;
	this->river_model=NULL;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_RV2CO), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coupling_RV2CO::~Hyd_Coupling_RV2CO(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_RV2CO), _sys_system_modules::HYD_SYS);
}
//__________________
//public
//The pointer of the models for coupling are setted
void Hyd_Coupling_RV2CO::set_ptr_coupling(Hyd_Coast_Model *coast, Hyd_Model_River *river){
	this->coast_model=coast;
	this->river_model=river;
}
	///Get the pointer to the river model
Hyd_Model_River *Hyd_Coupling_RV2CO::get_ptr_rivermodel(void){
	return this->river_model;
}
//The couplings are initialized
void Hyd_Coupling_RV2CO::init_coupling(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "CO_RV> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Coupling between coast model";
	cout << " and river model " <<this->river_model->Param_RV.get_river_number() <<"..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	try{
		this->river_model->outflow_river_profile.set_h_outflow_is_given();
	}
	catch(Error msg){
		ostringstream info;
		info<<"Coupling  between coast model and river model " << this->river_model->Param_RV.get_river_number() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Synchronise the models
void Hyd_Coupling_RV2CO::synchronise_models(void){
	try{
		this->river_model->outflow_river_profile.set_h_outflow_global(this->coast_model->get_global_sea_h());
	}
	catch(Error msg){
		ostringstream info;
		info<<"Synchronisation between coast model and river model " << this->river_model->Param_RV.get_river_number() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}

//Output the header for the coupled model indizes (static)
void Hyd_Coupling_RV2CO::output_header_coupled_indices(ostringstream *cout){
		*cout << "Coast to river..." << endl ;
		*cout << "No." << W(12) << "Id_CO" << W(12) << "Id_RV" << endl ;
		Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the indices of the coupled model
void Hyd_Coupling_RV2CO::output_index_coupled_models(ostringstream *cout, const int number){
	*cout << number << W(10);
	*cout << 0 << W(12);
	*cout << this->river_model->Param_RV.get_river_number()<<endl;
	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Clone the river to coast couplings
void Hyd_Coupling_RV2CO::clone_couplings(Hyd_Coupling_RV2CO *coupling, Hyd_Hydraulic_System *system){
	if(coupling->coast_model!=NULL){
		this->coast_model=system->my_comodel;
	}
	if(coupling->river_model!=NULL){
		this->river_model=system->get_ptr_river_model(coupling->river_model->Param_RV.get_river_number());
	}
}