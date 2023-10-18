#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_RV2FP.h"

//constructor
Hyd_Coupling_RV2FP::Hyd_Coupling_RV2FP(void){
	this->floodplain_model=NULL;
	this->river_model=NULL;
	this->list_left.set_leftriver_bank_line(true);
	this->list_right.set_leftriver_bank_line(false);
	this->is_merged_flag=false;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_RV2FP)-sizeof(Hyd_Coupling_Point_RV2FP_List)*2, _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coupling_RV2FP::~Hyd_Coupling_RV2FP(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_RV2FP)-sizeof(Hyd_Coupling_Point_RV2FP_List)*2, _sys_system_modules::HYD_SYS);
}
//__________________
//public
//The pointer of the models for coupling are setted
void Hyd_Coupling_RV2FP::set_ptr_coupling(Hyd_Model_Floodplain *floodplain, Hyd_Model_River *river){
	this->floodplain_model=floodplain;
	this->river_model=river;
}
//The couplings are initialized
void Hyd_Coupling_RV2FP::init_coupling(void){

	//set prefix for output
	ostringstream prefix;
	prefix << "RV_FP> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Coupling between floodplain model " << this->floodplain_model->Param_FP.get_floodplain_number();
	cout << " and river model " <<this->river_model->Param_RV.get_river_number() <<"..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	try{

		//set the river banks (left) as the relevant polysegment line
		this->list_left.set_defining_polysegment(&(this->river_model->river_leftline));
		//set the river banks (right) as the relevant polysegment line
		this->list_right.set_defining_polysegment(&(this->river_model->river_rightline));

		//fill the list with the floodplain elements
		this->floodplain_model->raster.assign_elements2couplingpointlist(&this->list_left);
		if (this->list_left.get_number_couplings() <= 1) {
			Error msg = this->set_error(0);
			ostringstream info;
			info << "1d-river model name and id: " << this->river_model->Param_RV.get_river_name()<<"   "<< this->river_model->Param_RV.get_river_number()<< endl;
			info << "2d-floodplain model name and id: " << this->floodplain_model->Param_FP.get_floodplain_name()<< "   " << this->floodplain_model->Param_FP.get_floodplain_number()<< endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		this->floodplain_model->raster.assign_elements2couplingpointlist(&this->list_right);
		if (this->list_right.get_number_couplings() <= 1) {
			Error msg = this->set_error(1);
			ostringstream info;
			info << "1d-river model name and id: " << this->river_model->Param_RV.get_river_name() << "   " << this->river_model->Param_RV.get_river_number() << endl;
			info << "2d-floodplain model name and id: " << this->floodplain_model->Param_FP.get_floodplain_name() << "   " << this->floodplain_model->Param_FP.get_floodplain_number() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//add the relevant points of the defining polysegment
		this->list_left.add_relevant_polysegment_points(&(this->floodplain_model->raster.geometrical_bound));
		this->list_right.add_relevant_polysegment_points(&(this->floodplain_model->raster.geometrical_bound));

		//sort it along the defining line
		this->list_left.sort_distance_along_polysegment();
		this->list_right.sort_distance_along_polysegment();
		//calculate the distances
		this->list_left.calculate_alldistance_up_down();
		this->list_right.calculate_alldistance_up_down();
		//set the profile pointer
		this->list_left.convert_profiles_index2pointer(this->river_model);
		this->list_right.convert_profiles_index2pointer(this->river_model);

		//transfer the infos to the coupling points
		this->list_left.transfer_informations2points();
		this->list_right.transfer_informations2points();


	}
	catch(Error msg){
		ostringstream info;
		info<<"Coupling between floodplain model " << this->floodplain_model->Param_FP.get_floodplain_number();
		info<< "  and river model " << this->river_model->Param_RV.get_river_number() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Output the header for the coupled model indizes
void Hyd_Coupling_RV2FP::output_header_coupled_indices(ostringstream *cout){
		*cout << "River to floodplain..." << endl ;
		*cout << "No." << W(12) << "Id_RV" << W(12) << "Id_FP" << endl ;
		Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the indexes of the coupled model
void Hyd_Coupling_RV2FP::output_index_coupled_models(ostringstream *cout, const int number){
	*cout << number << W(10);
	*cout << this->river_model->Param_RV.get_river_number() << W(12);
	*cout << this->floodplain_model->Param_FP.get_floodplain_number()<<endl;
	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the setted coupling points in the list
void Hyd_Coupling_RV2FP::output_setted_coupling_points(void){
	ostringstream cout;
	ostringstream prefix;
	prefix << "RV_" << this->river_model->Param_RV.get_river_number()<<"_FP_"<<this->floodplain_model->Param_FP.get_floodplain_number()<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout << "List of coupling points (left river bank)" << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	this->list_left.output_setted_members(&cout);
	cout << "List of coupling points (right river bank)" << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	this->list_right.output_setted_members(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Get a pointer to the coupled river model
Hyd_Model_River* Hyd_Coupling_RV2FP::get_river_model(void){
	return this->river_model;
}
//Get a pointer to the coupled floodplain model
Hyd_Model_Floodplain* Hyd_Coupling_RV2FP::get_floodplain_model(void){
	return this->floodplain_model;
}
//Get the coupled river index
int Hyd_Coupling_RV2FP::get_river_index(void){
	if(this->river_model!=NULL){
		return this->river_model->Param_RV.get_river_number();
	}
	else{
		return -1;
	}
}
//Get the coupled floodplain index
int Hyd_Coupling_RV2FP::get_floodplain_index(void){
	if(this->floodplain_model!=NULL){
		return this->floodplain_model->Param_FP.get_floodplain_number();
	}
	else{
		return -1;
	}
}
//Set that the couplings are already merged
void Hyd_Coupling_RV2FP::set_is_merged(void){
	this->is_merged_flag=true;
}
//Get that the couplings are already merged
bool Hyd_Coupling_RV2FP::get_is_merged(void){
	return this->is_merged_flag;
}
//____________
//private
//set the error
Error Hyd_Coupling_RV2FP::set_error(const int err_type) {
	string place = "Hyd_Coupling_RV2FP::";
	string help;
	string reason;
	int type = 0;
	bool fatal = false;
	stringstream info;
	Error msg;
	switch (err_type) {
	case 0://not enough interceptions
		place.append("init_coupling(void)");
		reason = "There are no or just 1 inteception point of the left river boudnary line with the 2d-raster elements";
		help = "The river should have at least 2 interceptions with the 2d-raster elements; extend the river model or reduce the 2d-element size";
		type = 16;
		break;
	case 1://not enough interceptions
		place.append("init_coupling(void)");
		reason = "There are no or just 1 inteception point of the right river boudnary line with the 2d-raster elements";
		help = "The river should have at least 2 interceptions with the 2d-raster elements; extend the river model or reduce the 2d-element size";
		type = 16;
		break;
	default:
		place.append("set_error(const int err_type)");
		reason = "Unknown flag!";
		help = "Check the flags";
		type = 6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}
