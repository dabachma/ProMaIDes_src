#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_Model_List.h"

//constructor
Hyd_Coupling_Model_List::Hyd_Coupling_Model_List(void){
	this->number=0;
	this->couplings=NULL;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_Model_List), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coupling_Model_List::~Hyd_Coupling_Model_List(void){
	this->delete_model_coupling();

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_Model_List), _sys_system_modules::HYD_SYS);
}
//Get the number of couplings
int Hyd_Coupling_Model_List::get_number_couplings(void){
	return this->number;
}
//Get the total list as a pointer
_hyd_model_coupling* Hyd_Coupling_Model_List::get_total_list(void){
	return this->couplings;
}
//Get a specific _hyd_model_coupling with the given index
_hyd_model_coupling Hyd_Coupling_Model_List::get_model_coupling(const int index){
	if(index>=0 && index< this->number){
		return this->couplings[index];
	}
	else{
		_hyd_model_coupling buffer;
		buffer.index_first_model=-1;
		buffer.index_second_model=-1;
		return buffer;
	}
}
//Get the first _hyd_model_coupling of the list
_hyd_model_coupling Hyd_Coupling_Model_List::get_first_model_coupling(void){
	if(this->number>=0){
		return this->couplings[0];
	}
	else{
		_hyd_model_coupling buffer;
		buffer.index_first_model=-1;
		buffer.index_second_model=-1;
		return buffer;
	}
}
//Get the last _hyd_model_coupling of the list
_hyd_model_coupling Hyd_Coupling_Model_List::get_last_model_coupling(void){
	if(this->number>0){
		return this->couplings[number-1];
	}
	else{
		_hyd_model_coupling buffer;
		buffer.index_first_model=-1;
		buffer.index_second_model=-1;
		return buffer;
	}
}
//Output members
void Hyd_Coupling_Model_List::output_members(ostringstream *cout){
	*cout <<"Number of couplings " << this->number<< endl;
	*cout <<"No. " << W(12) << "index 1" <<W(12)<< "index 2" <<endl;
	Sys_Common_Output::output_hyd->output_txt(cout);

	for(int i=0; i<this->number; i++){
		*cout << i << W(12) << this->couplings[i].index_first_model;
		*cout << W(12) << this->couplings[i].index_second_model;
		*cout << endl;
		Sys_Common_Output::output_hyd->output_txt(cout);
	}
}
//Set a new _hyd_model_coupling
void Hyd_Coupling_Model_List::set_new_model_coupling(_hyd_model_coupling *new_coupling){

	//allocate a buffer
	_hyd_model_coupling *buffer=NULL;
	try{
		buffer=new _hyd_model_coupling[this->number];
	}
	catch(bad_alloc &){
		Error msg=this->set_error(0);
		throw msg;
	}
	//copy it
	for(int i=0; i< this->number;i++){
		buffer[i]=this->couplings[i];
	}
	//delete
	this->delete_model_coupling();
	//count it
	this->number++;

	try{
		//reallocate
		this->allocate_model_coupling();
	}
	catch(Error msg){
		throw msg;
	}

	//copy it back
	for(int i=0; i< this->number-1;i++){
		this->couplings[i]=buffer[i];
	}
	//set the new one
	this->couplings[this->number-1]=*new_coupling;


	//delete the buffer
	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}
	//reset it
	new_coupling->index_first_model=-1;
	new_coupling->index_second_model=-1;

}
//Delete the list
void Hyd_Coupling_Model_List::delete_list(void){
	this->delete_model_coupling();
	this->number=0;
}
//________________
//private
//Delete the allocated points
void Hyd_Coupling_Model_List::delete_model_coupling(void){
	if(this->couplings!=NULL){
		delete []this->couplings;
		this->couplings=NULL;
		//count the memory
		Sys_Memory_Count::self()->minus_mem(sizeof(_hyd_model_coupling)*this->number, _sys_system_modules::HYD_SYS);

	}
}
//Allocate the points
void Hyd_Coupling_Model_List::allocate_model_coupling(void){
	try{
		this->couplings=new _hyd_model_coupling[this->number];
	}
	catch(bad_alloc &){
		Error msg=this->set_error(0);
		throw msg;
	}
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(_hyd_model_coupling)*this->number, _sys_system_modules::HYD_SYS);
}
//Set error(s)
Error Hyd_Coupling_Model_List::set_error(const int err_type){
		string place="Hyd_Coupling_Model_List::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_model_coupling(void))/set_new_model_coupling(_hyd_model_coupling *new_coupling)");
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