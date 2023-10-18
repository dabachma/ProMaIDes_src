#include "Risk_Headers_Precompiled.h"
//#include "Risk_Fpl_Stress.h"

//Default constructor
Risk_Fpl_Stress::Risk_Fpl_Stress(void){

	this->id_break_sc="CA";
	this->number_fpl_sec=0;
	this->section_stress=NULL;
	this->calculated_info=true;

	Sys_Memory_Count::self()->add_mem(sizeof(Risk_Fpl_Stress),_sys_system_modules::RISK_SYS);//count the memory
}
//Default destructor
Risk_Fpl_Stress::~Risk_Fpl_Stress(void){

	this->delete_section_stress();
	Sys_Memory_Count::self()->minus_mem(sizeof(Risk_Fpl_Stress),_sys_system_modules::RISK_SYS);//count the memory

}
//_________
//public
//Get the identifier string of the break scenario
string Risk_Fpl_Stress::get_id_string_break_sc(void){
	return this->id_break_sc;
}
//Set the identifier string of the break scenario
void Risk_Fpl_Stress::set_id_string_break_sc(string id_break_sc){
	this->id_break_sc=id_break_sc;
}
//Append a string to the identifier string of the break scenario
void Risk_Fpl_Stress::append2id_string_break_sc(string append_str){
	//do not append the "CA_"
	for(unsigned int i=2; i<append_str.size(); i++){
		this->id_break_sc.append(1, append_str.at(i));

	}
	
}
//Get number of relevant FPL-section(s)
int Risk_Fpl_Stress::get_number_fpl_section(void){
	return this->number_fpl_sec;
}
//Set number of relevant FPL-section(s) and allocate the _risk_section_stress data structure
void Risk_Fpl_Stress::set_number_fpl_section(const int number_fpl_sec){
	this->delete_section_stress();
	this->number_fpl_sec=number_fpl_sec;
	this->allocate_section_stress();
}
//Get the pointer to the individuel section stress
_risk_section_stress *Risk_Fpl_Stress::get_ptr_section_stress(void){
	return this->section_stress;
}
//Get the a pointer to the individuel section stress for a specific section
_risk_section_stress *Risk_Fpl_Stress::get_section_stress(const int index){
	if(this->number_fpl_sec==0){
		return NULL;
	}
	if(index<0 || index>=this->number_fpl_sec){
		return NULL;
	}
	else{

		return &this->section_stress[index];

	}
}
//Set the information of the individuel section stress for a specific section
void Risk_Fpl_Stress::set_section_stress(const int index, _risk_section_stress *stress){
	if(this->number_fpl_sec==0){
		return ;
	}
	if(index<0 || index>=this->number_fpl_sec){
		return ;
	}
	else{

		this->section_stress[index]=*stress;

	}

}
//Copy the object information from a given source object
void Risk_Fpl_Stress::copy_infos(Risk_Fpl_Stress *src){
	if(src==NULL){
		return;
	}
	this->id_break_sc=src->id_break_sc;
	this->calculated_info=src->calculated_info;
	if(this->number_fpl_sec!=src->number_fpl_sec){
		this->set_number_fpl_section(src->number_fpl_sec);
	}
	for(int i=0; i< this->number_fpl_sec;i++){
		this->section_stress[i]=src->section_stress[i];
	}
}
//Set if the hydraulic stress are calculated (:=true) or predicted one (:=false)
void Risk_Fpl_Stress::set_hyd_is_predicted(const bool flag){
	this->calculated_info=flag;
}
//Get if the hydraulic stress are calculated (:=true) or predicted one (:=false)
bool Risk_Fpl_Stress::get_hyd_is_predicted(void){
	return this->calculated_info;
}
//Predict new hydraulic stress out of different stress for this object
void Risk_Fpl_Stress::predict_values(Risk_Fpl_Stress *base, const int break_id, Risk_Fpl_Stress *part_new_info){
	this->calculated_info=false;
	double percentage=1.0;
	double delta=0.0;
	double h_base=0.0;
	double h_current=0.0;
	double h_part=0.0;

	//for the break section
	h_base=Risk_Fpl_Stress::get_maximum_waterlevel(base,break_id);
	h_current=Risk_Fpl_Stress::get_maximum_waterlevel(this,break_id);
	if(h_base!=0.0){
		percentage=h_current/h_base;
	}
	
	if(percentage>1.0){
		percentage=1.0;
	}

	//predict values
	for(int i=0; i< this->number_fpl_sec; i++){
		h_base=Risk_Fpl_Stress::get_maximum_waterlevel(base,this->section_stress[i].id_section);
		//h_current=current->get_section_stress(this->section_stress[i].used_index)->waterlevel;
		h_current=this->section_stress[i].waterlevel;
		h_part=Risk_Fpl_Stress::get_maximum_waterlevel(part_new_info,this->section_stress[i].id_section);

		delta=h_base-h_part;
		if(delta<0.0){
			delta=0.0;
		}
		this->section_stress[i].waterlevel=this->section_stress[i].waterlevel-delta*percentage;
	}
}
//Get the maximum waterlevel for a given section id of the hydraulic information (static)
double Risk_Fpl_Stress::get_maximum_waterlevel(Risk_Fpl_Stress *results, const int section_id){

	for(int i=0; i< results->get_number_fpl_section(); i++){
		if(results->get_section_stress(i)->id_section==section_id){
			return results->get_section_stress(i)->waterlevel;
		}

	}

	return -1.0;
}
//_________
//private
//Allocate the data structure for each individuel FPL-section stress (_risk_section_stress)
void Risk_Fpl_Stress::allocate_section_stress(void){
	//allocate the infos for each section
	try{
		this->section_stress = new _risk_section_stress[this->number_fpl_sec];
		Sys_Memory_Count::self()->add_mem(this->number_fpl_sec*sizeof(_risk_section_stress),_sys_system_modules::RISK_SYS);//count the memory

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//init the structure
	for(int i=0; i<this->number_fpl_sec; i++){

		this->section_stress[i].time=0.0;
		this->section_stress[i].rand_time=0.0;
		this->section_stress[i].waterlevel=0.0;
		this->section_stress[i].id_section=-1;
		this->section_stress[i].used_index=-1;
	}
}
//Delete the data structure for each individuel FPL-section stress (_risk_section_stress)
void Risk_Fpl_Stress::delete_section_stress(void){
	if(this->section_stress!=NULL){
		delete []this->section_stress;
		this->section_stress=NULL;
		Sys_Memory_Count::self()->minus_mem(this->number_fpl_sec*sizeof(_risk_section_stress),_sys_system_modules::RISK_SYS);//count the memory

	}
}
//Set error(s)
Error Risk_Fpl_Stress::set_error(const int err_type){
	string place="Risk_Fpl_Stress::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_section_stress(void)");
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
