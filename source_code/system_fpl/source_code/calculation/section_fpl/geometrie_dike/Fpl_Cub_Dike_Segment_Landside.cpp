#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Cub_Dike_Segment_Landside.h"


//Default constructor
Fpl_Cub_Dike_Segment_Landside::Fpl_Cub_Dike_Segment_Landside(void){

Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Cub_Dike_Segment_Landside)-sizeof(Geo_Segment), _sys_system_modules::FPL_SYS);//count the memory

}
//Default destructor
Fpl_Cub_Dike_Segment_Landside::~Fpl_Cub_Dike_Segment_Landside(void){

Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Cub_Dike_Segment_Landside)-sizeof(Geo_Segment), _sys_system_modules::FPL_SYS);//count the memory

}

//_________
//public
