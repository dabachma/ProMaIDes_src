#pragma once
#ifndef  DAM_HEADERS_PRECOMPILED_H
#define DAM_HEADERS_PRECOMPILED_H

//MS specific (Overloading of not deprecated Functions)
#ifdef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
  #undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#endif

#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
  #define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif 

#define _CRT_SECURE_NO_DEPRECATE 

#define _SCL_SECURE_NO_WARNINGS
// End of MS specific

//system class for all damage calculations
#include "Dam_Damage_System.h"

//general
//geometrical raster class
#include "Dam_Raster.h"
//class for the impact values for the damage calculation for one floodplain element
#include "Dam_Impact_Values.h"
//class for the impact values for the damage calculation for one floodplain 
#include "Dam_Impact_Value_Floodplain.h"
//base class of the elements
#include "_Dam_Element.h"
//base for a damage function (2-value)
#include "_Dam_2value_Function.h"
//base class for various systems
#include "_Dam_System.h"
//geometrical polygon for interception
#include "Dam_Polygon.h"

//raster class for a import of hydraulic results
#include "Dam_Hyd_Result_Raster.h" 
//element class for a import of hydraulic results used in the raster class
#include "Dam_Hydimport_Element.h"


//economical
//system for the economical damage calculation
#include "Dam_Ecn_System.h"
//damage function for the relative damage
#include "Dam_Ecn_Damage_Function.h"
//economical damage element
#include "Dam_Ecn_Element.h"
//economicla raster
#include "Dam_Ecn_Raster.h"

//ecological
//system for the ecological damage calculation
#include "Dam_Eco_System.h"

//biotope-type
#include "Dam_Eco_Btype.h"
//damage function for the biotope-type
#include "Dam_Eco_Btype_Damage_Function.h"
//geometrical raster for the biotope-damage calculation
#include "Dam_Eco_Btype_Raster.h"
//biotope-damage information in the element of the biotope-damage raster 
#include "Dam_Eco_Btype_Element.h"

//cost function for a soil-type
#include "Dam_Eco_Soil_Cost_Function.h"
//erosion function for a soil-type
#include "Dam_Eco_Soil_Erosion_Function.h"
//geometrical raster for the soil-damage calculation
#include "Dam_Eco_Soil_Raster.h"
//soil-damage information in the element of the soil-damage raster 
#include "Dam_Eco_Soil_Element.h"

//people
//damage function for the people2risk-type
#include "Dam_People_Damage_Function.h"
//people2risk information in the element of the peopl2risk-raster
#include "Dam_People_Element.h"
//geometrical raster for the people2risk calculation
#include "Dam_People_Raster.h"
//system for the people2risk calculation
#include "Dam_People_System.h"

//simple counting
//subcategories of the simple counting damage
#include "Dam_Sc_Subcategory.h"
//geometrical raster of the simple counting damage
#include "Dam_Sc_Point_Manager.h"
//managing system
#include "Dam_Sc_System.h"
//simple-counting information in a simple counting point
#include "Dam_Sc_Point.h"




//CI
//management of CI
#include "_Dam_CI_Element.h"
#include "Dam_CI_Element_List.h"
#include "Dam_CI_Point.h"
#include "Dam_CI_Polygon.h"
#include "Dam_CI_System.h"

//psycho-social
//managing system
#include "Dam_Pys_System.h"
//people density factor-function
#include "Dam_Pys_Density_Function.h"
//psycho-social information in the element of the psycho-social raster
#include "Dam_Pys_Element.h"
//psycho social score fuction
#include "Dam_Pys_Function.h"
//geometrical raster
#include "Dam_Pys_Raster.h"
//categories of pscho-social scores
#include "Dam_Pys_Category.h"

//display the damage results as summary
#include "DamGui_Result_Summary_Wid.h"
//display the damage results for each scenario
#include "DamGui_Result_Wid.h"
//ask for an multiple raster import
#include "DamGui_Number_Raster_Dia.h"

//widgets for displaying the data
#include "DamGui_Raster_Wid.h"
#include "DamGui_Pys_Categories_Wid.h"
#include "DamGui_Pop_Function_Wid.h"
#include "DamGui_Eco_Soil_Erosion_Wid.h"
#include "DamGui_Eco_Soil_Cost_Wid.h"
#include "DamGui_Eco_Btype_Wid.h"
#include "DamGui_Eco_Btype_Func_Wid.h"
#include "DamGui_Ecn_Func_Wid.h"
#include "DamGui_Sc_Point_Wid.h"
#include "DamGui_Sc_Subcategory_Wid.h"

#include "DamGui_Ci_Point_Wid.h"
#include "DamGui_Ci_Polygon_Wid.h"

//raster dialog class
#include "DamGui_Raster_Dia.h"


#endif