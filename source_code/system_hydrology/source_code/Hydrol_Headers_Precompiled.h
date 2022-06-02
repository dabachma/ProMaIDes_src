#pragma once
#ifndef  HYDROL_HEADERS_PRECOMPILED_H
#define HYDROL_HEADERS_PRECOMPILED_H

//MS specific (Overloading of not deprecated Functions)
//#ifdef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
//  #undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
//#endif

//#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
//  #define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
//#endif

//#define _CRT_SECURE_NO_DEPRECATE

//#define _SCL_SECURE_NO_WARNINGS
// End of MS specific

//add headers here

//HYDRAULIC_SYS
//Class for managing multiple hydrologic systems
#include "Hydrol_Multiple_Hydrologic_Systems.h"
//Main class which manage the hydrologic systems
#include "Hydrol_Hydrologic_Systems.h"


//SYSTEM_SYS
//system_sys_ classes
#include "_Sys_Common_System.h"

//Geometrical classes
//base class for other segment classes
#include "Geo_Segment.h"
//base class for other points
#include "Geo_Point.h"
//simple polygon
#include "Geo_Simple_Polygon.h"
//polysegment
#include "Geo_Polysegment.h"
//list of points
#include "Geo_Point_List.h"


//Widget for displaying data
//#include "HydGui_Floodplain_Member_Wid.h"



//namespace
using namespace std;

#endif
