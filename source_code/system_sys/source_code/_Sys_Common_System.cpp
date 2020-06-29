//#include "_Sys_Common_System.h"
#include "Sys_Headers_Precompiled.h"

//constructor
_Sys_Common_System::_Sys_Common_System(void){
	this->system_id.area_state=0;
	this->system_id.measure_nr=0;
}
//destructor
_Sys_Common_System::~_Sys_Common_System(void){
}
//______________________
//pulic
//Set the system-id (_sys_system_id) of the object
void _Sys_Common_System::set_systemid(const _sys_system_id actuel_id){
	this->system_id.area_state=actuel_id.area_state;
	this->system_id.measure_nr=actuel_id.measure_nr;
}
//Get the system id (_sys_system_id);
_sys_system_id _Sys_Common_System::get_sys_system_id(void){
	return this->system_id;
}
///Get a pointer to the system id (_sys_system_id);
_sys_system_id* _Sys_Common_System::get_ptr_sys_system_id(void){
	return &this->system_id;
}
