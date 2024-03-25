#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Boundary_Scenario_List.h"

//Default constructor
Hyd_Boundary_Scenario_List::Hyd_Boundary_Scenario_List(void){

	this->number=0;
	this->bound_sc=NULL;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Boundary_Scenario_List), _sys_system_modules::HYD_SYS);
}
//Default destructor
Hyd_Boundary_Scenario_List::~Hyd_Boundary_Scenario_List(void){
	this->delete_list();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Boundary_Scenario_List), _sys_system_modules::HYD_SYS);
}
//_________
//public
//Add scenario to list
void Hyd_Boundary_Scenario_List::add_scenario2list(Hyd_Boundary_Szenario *add){

	//check if it is already in list
	for(int i=0; i< this->number; i++){
		if(this->bound_sc[i].get_id()==add->get_id() && 
			this->bound_sc[i].get_sys_system_id().area_state==add->get_sys_system_id().area_state &&
			this->bound_sc[i].get_sys_system_id().measure_nr==add->get_sys_system_id().measure_nr){
			return;
		}
	}

	Hyd_Boundary_Szenario *buffer=NULL;
	try{
		buffer=new Hyd_Boundary_Szenario[this->number];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//copy it
	for(int i=0; i< this->number; i++){
		buffer[i]=this->bound_sc[i];
	}

	this->number++;

	try{
		this->allocate_list();
		//recopy
		for(int i=0; i< this->number-1; i++){
			this->bound_sc[i]=buffer[i];
		}
		this->bound_sc[this->number-1]=*add;
	}
	catch(Error msg){
		if(buffer!=NULL){
			delete []buffer;
			buffer=NULL;
		}
		throw msg;
	}

	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}

}
//Clear the list
void Hyd_Boundary_Scenario_List::clear_list(void){
	this->number=0;
	this->delete_list();
}
//Get the number of list members
int Hyd_Boundary_Scenario_List::get_number_in_list(void){
	return this->number;
}
//Get a pointer to the total list
Hyd_Boundary_Szenario *Hyd_Boundary_Scenario_List::get_total_list(void){
	return this->bound_sc;
}
//Get a pointer to a list member specified by the given index
Hyd_Boundary_Szenario *Hyd_Boundary_Scenario_List::get_ptr_scenario(const int index){
	if(index<0 || index>=number){
		return NULL;
	}
	else{
		return &this->bound_sc[index];
	}
}
//________
//private
//Allocate the hydraulic boundary scenarios
void Hyd_Boundary_Scenario_List::allocate_list(void){
	this->delete_list();
	try{
		this->bound_sc=new Hyd_Boundary_Szenario[this->number];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the list
void Hyd_Boundary_Scenario_List::delete_list(void){
	if(this->bound_sc!=NULL){
		delete []this->bound_sc;
		this->bound_sc=NULL;
	}
}
//Set error(s)
Error Hyd_Boundary_Scenario_List::set_error(const int err_type){
	string place="Hyd_Boundary_Scenario_List::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){

		case 0://bad alloc
			place.append("allocate_list(void)");
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