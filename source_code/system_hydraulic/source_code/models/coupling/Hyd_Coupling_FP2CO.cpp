#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_FP2CO.h"

//constructor
Hyd_Coupling_FP2CO::Hyd_Coupling_FP2CO(void){
	this->coast_model=NULL;
	this->floodplain_model=NULL;
	this->is_merged_flag=false;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_FP2CO)-sizeof(Hyd_Coupling_Point_FP2CO_List), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coupling_FP2CO::~Hyd_Coupling_FP2CO(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_FP2CO)-sizeof(Hyd_Coupling_Point_FP2CO_List), _sys_system_modules::HYD_SYS);
}
//__________________
//public
//The pointer of the models for coupling are setted
void Hyd_Coupling_FP2CO::set_ptr_coupling(Hyd_Coast_Model *coast, Hyd_Model_Floodplain *floodplain){
	this->coast_model=coast;
	this->floodplain_model=floodplain;
	
}
//Get the pointer to the coast model
Hyd_Coast_Model *Hyd_Coupling_FP2CO::get_coastmodel_ptr(void){
	return this->coast_model;
}
//Get the pointer to the floodplain model
Hyd_Model_Floodplain *Hyd_Coupling_FP2CO::get_floodplainmodel_ptr(void){
	return this->floodplain_model;
}
//The couplings are initialized
void Hyd_Coupling_FP2CO::init_coupling(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "CO_FP> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Coupling between coast model ";
	cout << "and floodplain model " <<this->floodplain_model->Param_FP.get_floodplain_number() <<"..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	try{
		//set the coast line as relevant polysegment line
		this->list.set_defining_polysegment(&(this->coast_model->coastline_polysegment));
		//fill the list with the floodplain elements
		this->floodplain_model->raster.assign_elements2couplingpointlist(&this->list);

		//add the relevant points of the defining polysegment
		this->list.add_relevant_polysegment_points(&(this->floodplain_model->raster.geometrical_bound));
		


		//sort it along the defining line
		this->list.sort_distance_along_polysegment();
		this->list.calculate_alldistance_up_down();
		//transfer the infos to the coupling points
		this->list.transfer_informations2points();

	}
	catch(Error msg){
		ostringstream info;
		info<<"Coupling  between coast model and floodplain model " << this->floodplain_model->Param_FP.get_floodplain_number() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the header for the coupled model indizes (static)
void Hyd_Coupling_FP2CO::output_header_coupled_indices(ostringstream *cout){
		*cout << "Coast to floodplain..." << endl ;
		*cout << "No." << W(12) << "Id_CO" << W(12) << "Id_FP" << endl ;
		Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the indices of the coupled model
void Hyd_Coupling_FP2CO::output_index_coupled_models(ostringstream *cout, const int number){
	*cout << number << W(10);
	*cout << 0 << W(12);
	*cout << this->floodplain_model->Param_FP.get_floodplain_number()<<endl;
	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the setted coupling points in the list
void Hyd_Coupling_FP2CO::output_setted_coupling_points(void){
	ostringstream cout;
	ostringstream prefix;
	prefix << "FP_"<<this->floodplain_model->Param_FP.get_floodplain_number()<<"_CO> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout << "List of coupling points" << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	this->list.output_setted_members(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Set that the couplings are already merged
void Hyd_Coupling_FP2CO::set_is_merged(void){
	this->is_merged_flag=true;
}
//Get that the couplings are already merged
bool Hyd_Coupling_FP2CO::get_is_merged(void){
	return this->is_merged_flag;
}