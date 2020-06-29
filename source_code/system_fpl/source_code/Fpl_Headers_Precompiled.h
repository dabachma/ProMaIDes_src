#pragma once
#ifndef FPL_HEADERS_PRECOMPILED_H
#define FPL_HEADERS_PRECOMPILED_H

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

//add this to the cpp-files
//#include "Fpl_Headers_Precompiled.h"

//System class of the fpl_system
#include "Fpl_Calculation.h"

//base class
#include "_Fpl_Random_Generator.h"
//random generator ranmar
#include "Fpl_Random_Generator_Ranmar.h"
//random generator zikurat
#include "Fpl_Random_Generator_Zikurat.h"
//random generator mother-of-all
#include "Fpl_Random_Generator_MofAll.h"
//random generator sfmt
#include "Fpl_Random_Generator_Sfmt.h"

//Base class of the probabilistic distribution
#include "_Fpl_Distribution_Class.h"
//discret distribution
#include "Fpl_Distribution_Class_Discret.h"
//discret distribution in combination with mean distributions
#include "Fpl_Distribution_Class_Discret_Mean.h"
//mean distribution
#include "Fpl_Distribution_Class_Mean.h"
//mean distribution in combination with mean distribution
#include "Fpl_Distribution_Class_Mean_Mean.h"
//triangle distribution
#include "Fpl_Distribution_Class_Triangle.h"
//distribution checking class
#include "Fpl_Check_Distribution.h"

//class for the random variables
#include "Fpl_Random_Variables.h"

//base class for the mechanism
#include "_Fpl_Mechanism.h"
//base class of the failure mechanisms
#include "_Fpl_Failure_Mechanism.h"
//failure mechanism for an overflow failure of the highground section type
#include "Fpl_Fail_Overflow_HiGround.h"
//failure mechanism for an overflow failure of the dike section type
#include "Fpl_Mech_Overflow_Dike.h"
//mechanism for the wavegeneration
#include "Fpl_Mech_Wave_Generation.h"
//paramater for the windwave-generation
#include "Fpl_Waveparam_Calculation.h"
//mechanism for the waverunup for a dike
#include "Fpl_Mech_Waverunup_Dike.h"
//piping after sellmeijer
#include "Fpl_Mech_Piping_Sell_Dike.h"
//piping after lane
#include "Fpl_Mech_Piping_Lane_Dike.h"
//piping after schmertmann
#include "Fpl_Mech_Piping_Schmertmann_Dike.h"
//bursting after DIN19712
#include "Fpl_Mech_Bursting_Din_Dike.h"
//slope stability
#include "Fpl_Mech_Slope_Stability_Dike.h"
//land side resistance class
#include "Fpl_Mech_Landside_Erosion.h"
//dune erosion failure
#include "Fpl_Mech_Erosion_Dune_van_Gent.h"
//dune wave runup
#include "Fpl_Mech_Waverunup_Dune.h"
//stability of waterside dike slope against wave impact
#include "Fpl_Mech_Wave_Impact_Waterside_Dike.h"
//waterside erosion of dike slope
#include "Fpl_Mech_Waterside_Erosion.h"
//start of the erosion
#include "Fpl_Mech_Waterside_Erosion_Start.h"

//the fpl section class
#include "Fpl_Section.h"
//points of the section
#include "Fpl_Section_Points.h"

//gui dialog for the section
#include "FplGui_Section_Dia.h"
//dialog for checking the random distribution
#include "FplGui_Random_Check_Dia.h"
//dialog checking a triangle distribution
#include "FplGui_Check_Disttype_Triangle_Dia.h"
//dialog checking a mean distribution
#include "FplGui_Check_Disttype_Mean_Dia.h"

//widget class for displaying the probabilistic results of a fpl-section
#include "FplGui_Prob_Result_Wid.h"
//widget class for displaying the detailed results of a fpl-section
#include "FplGui_Detailed_Result_Wid.h"
//dialog for setting fragility curve values per dialog
#include "FplGui_Frc_Values_Dia.h"

//widget faulttree dune setcion
#include "FplGui_Faulttree_Dune_Wid.h"
//widget faulttree dike setcion
#include "FplGui_Faulttree_Dike_Wid.h"
//widget faulttree gate setcion
#include "FplGui_Faulttree_Gate_Wid.h"

//base class of the section types
#include "_Fpl_Section_Type.h"
//class of the dike section type
#include "Fpl_Sec_Type_Dike.h"
//class of the highground section type
#include "Fpl_Sec_Type_Highground.h"
//class of the dune section type
#include "Fpl_Sec_Type_Dune.h"
//class of the gate section type
#include "Fpl_Sec_Type_Gate.h"

//class for the frc calculation
#include "Fpl_Frc_Sim.h"
//class for the mc-calculation
#include "Fpl_Mc_Sim.h"

//geometrical class for the dike crest
#include "Fpl_Cub_Dike_Crest.h"
//geometrical class for the dike waterside
#include "Fpl_Cub_Dike_Waterside.h"
//geometrical class for the dike landside
#include "Fpl_Cub_Dike_Landside.h"

//Segments of the dike waterside
#include "Fpl_Cub_Dike_Segment_Waterside.h"
//Segments of the dike landside
#include "Fpl_Cub_Dike_Segment_Landside.h"

//Segment of the hinterland and the foreland
#include "Fpl_Dike_Segment.h"
//Geometry of the dike foreland
#include "Fpl_Dike_Geo_Foreland.h"
//Geometry of the dike hinterland
#include "Fpl_Dike_Geo_Hinterland.h"

//geometrie of the windfetches
#include "Fpl_Wind_Fetch.h"
//local fetch (sections) for the wind-wave calculation via the krylow-approach
#include "Fpl_Local_Wind_Fetch.h"

//dike materialzones of body and underground
#include "Fpl_Dike_Geo_Materialzone.h"
//connection class
#include "Fpl_Dike_Geo_Materialzone_Connector.h"
//table to store the wetted material zone areas of a slope slice
#include "Fpl_Dike_Geo_Wet_Materialzone_Table.h"
#include "Fpl_Dike_Geo_Wet_Materialzone_Table_Point.h"

//waterside cubature material zones
#include "Fpl_Cub_Waterside_Materialzone.h"

//relevant slope material variables of the dike zones
#include "Fpl_Dike_Var_Materialzone.h"

//slope stability class
#include "Fpl_Slip_Circle.h"
#include "Fpl_Slope_Slices.h"

#include "Fpl_Bursting_Slice.h"

//class for handling a frc-curve
#include "Fpl_Frc_Curve.h"

//widget for displaying the data
#include "FplGui_System_Member_Wid.h"


//classes for the seepage line
#include "Fpl_Seepage_Line_Point_List.h"
#include "Fpl_Seepage_Line_Point.h"

#include "Fpl_Seepage_Calculator_Dike.h"

#include "Fpl_Slope_External_Force.h"

//namespace
using namespace std;

#endif