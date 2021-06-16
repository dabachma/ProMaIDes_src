//#include "_Sys_Exception.h"
#include "Sys_Headers_Precompiled.h"



//constructor
_Sys_Exception::_Sys_Exception(void){
	this->type=0;
	this->counter_output=0;
	this->current_exception_number_module=0;
	this->module_type=_sys_system_modules::SYS_SYS;
}
//destructor  
_Sys_Exception::~_Sys_Exception(void){
}
//______________________________________
//public
//Append to string secondary informations
void _Sys_Exception::make_second_info(const string info){//put together the secondary information to the error/warning
	this->secundary_info.append(info); 
}
//(static) Insert into a string with line-breaks tabulators after the line-breaks
void _Sys_Exception::insert_tabs_after_endl(string *txt){
	if(txt->empty()==true){
		return;
	}
	//insert whitespaces at the beginning
	//txt->insert(0,"     ");
	//insert whitespaces after line-breaks
	int counter=0;
	int size=txt->size();
	char t;
	do{

		t=txt->at(counter);
		//insert after it whitespaces
		if(t=='\n'){
			txt->insert(counter+1,"                       ");
			size=txt->size();
		}
		counter++;
	}
	while(counter < size-1);

}
//______________________________________
//protected
///Set the exceptiontime
string _Sys_Exception::set_time(void){//set the time of exeption
	//fot the time
	string occ_time;
	time_t error_time;
	struct tm *timeinfo;
	//get time
	time(&error_time);
	timeinfo=localtime(&error_time);
	occ_time=asctime(timeinfo);
	return occ_time;
}
//Output the message to the corresponding module output display
void _Sys_Exception::output_msg2moduldisplay(string txt){
	switch (this->module_type){
		case _sys_system_modules::SYS_SYS:
			Sys_Common_Output::output_system->output_txt(txt,false);
			Sys_Common_Output::output_system->reset_userprefix();
			break;
		case _sys_system_modules::FPL_SYS:
			Sys_Common_Output::output_fpl->output_txt(txt,false);
			Sys_Common_Output::output_fpl->reset_userprefix();
			break;
		case _sys_system_modules::HYD_SYS:
			Sys_Common_Output::output_hyd->output_txt(txt,false);
			Sys_Common_Output::output_hyd->reset_userprefix();
			break;
		case _sys_system_modules::MADM_SYS:
			Sys_Common_Output::output_madm->output_txt(txt,false);
			Sys_Common_Output::output_madm->reset_userprefix();
			break;
		case _sys_system_modules::DAM_SYS:
			Sys_Common_Output::output_dam->output_txt(txt,false);
			Sys_Common_Output::output_dam->reset_userprefix();
			break;
		default:
			break;
	}

}