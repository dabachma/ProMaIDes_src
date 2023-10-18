#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Dike_Segment.h"

//Default constructor
Fpl_Dike_Segment::Fpl_Dike_Segment(void){
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Dike_Segment), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Dike_Segment::~Fpl_Dike_Segment(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Dike_Segment), _sys_system_modules::FPL_SYS);//count the memory
}
//__________
//public