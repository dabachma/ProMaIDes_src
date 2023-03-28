#pragma once
#ifndef  HYD_HEADERS_PRECOMPILED_H
#define HYD_HEADERS_PRECOMPILED_H

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
//Class for managing multiple hyraulic systems
#include "Hyd_Multiple_Hydraulic_Systems.h"
//Main class which manage the hydraulic system
#include "Hyd_Hydraulic_System.h"
//base class for the boundary szenarios
#include "Hyd_Boundary_Szenario.h"
//List class for the boundary szenarios
#include "Hyd_Boundary_Scenario_List.h"
//managment class for the boundary szenarios
#include "Hyd_Boundary_Szenario_Management.h"
//observation points
#include "Hyd_Observation_Point.h"
//managment class of observation points
#include "Hyd_Observation_Point_Manager.h"

//model classes
//Base class of hydraulic models
#include "_Hyd_Model.h"
//Class of a river model
#include "Hyd_Model_River.h"
//Class of a floodplain model
#include "Hyd_Model_Floodplain.h"
//class for the coast model
#include "Hyd_Coast_Model.h"

//class for the groundwater model
#include "Hyd_Model_Groundwater.h"

//classes for the temperature model
//class for the temperature model
#include "HydTemp_Model.h"
//class for the parameters of the temperature model
#include "HydTemp_Param.h"
//class for the parser of the parameters of the temperature model
#include "HydTemp_Parse.h"
//class for the profiles of the temperature model
#include "HydTemp_Profile.h"

//classes of 1d calculation
//base clase of the river profiles
#include "_Hyd_River_Profile.h"
//standard profile class
#include "Hyd_River_Profile_Connection_Standard.h"
//upwards profile is inflow
#include "Hyd_River_Profile_Connection_Inflow.h"
//downwards profile is outflow
#include "Hyd_River_Profile_Connection_Outflow.h"
//Base class of the profile types
#include "_Hyd_River_Profile_Type.h"
//class for printing the profile in a plot
#include "HydGui_Profile_Plot.h"
//Standard profile type (river)
#include "Hyd_River_Profile_Type_Standard.h"
//Weir profile type (river)
#include "Hyd_River_Profile_Type_Weir.h"
//Bridge profile type (river)
#include "Hyd_River_Profile_Type_Bridge.h"
//Table class for the profile characteristics
#include "Hyd_Tables.h"
//Point class of the river profile
#include "Hyd_River_Profile_Point.h"
//Segment of a river profile
#include "Hyd_Profile_Segment.h"

//classes of 2d calculation
//A polygon of the floodplain (geometrical boundary, noflow-polygon etc.)
#include "Hyd_Floodplain_Polygon.h"
//A polysegement of the floodplain like dikelines or riverlines
#include "Hyd_Floodplain_Polysegment.h"
//The numerical raster of the floodplain
#include "Hyd_Floodplain_Raster.h"
//Raster converter class
#include "Hyd_Gis2Promraster_Converter.h"
//A raster polygon of the numerical raster of the floodplain
#include "Hyd_Floodplainraster_Polygon.h"
//A segment of the raster polygon (numerical raster of the floodplain)
#include "Hyd_Floodplainraster_Segment.h"
//A point of the raster (numerical raster of the floodplain)
#include "Hyd_Floodplainraster_Point.h"
//A list of raster points
#include "Hyd_Floodplainraster_Point_List.h"
//Dikeline points
#include "Hyd_Floodplain_Dikeline_Point.h"
//List class for dikeline points
#include "Hyd_Floodplain_Dikeline_Point_List.h"
//boundary members
#include "Hyd_Element_Bound_Members.h"
//coupling members
#include "Hyd_Element_Coup_Members.h"

//Elements of the floodplain model
#include "Hyd_Element_Floodplain.h"
//Base class for the element types
#include "_Hyd_Element_Floodplain_Type.h"
//Standard element
#include "Hyd_Element_Floodplain_Type_Standard.h"
//Noflow element (floodplain-model is intercepted by a noflow-polygon or is a no-info-element)
#include "Hyd_Element_Floodplain_Type_Noflow.h"
//Coastal element (floodplain-model is intercepted by a coast-model)
#include "Hyd_Element_Floodplain_Type_Coast.h"
//River element (floodplain-model is intercepted by a river-model)
#include "Hyd_Element_Floodplain_Type_River.h"
//Floodplain element which is just use in cause of an DAM-project
#include "Hyd_Element_Floodplain_Type_Dam.h"

//class for coupling
//List where indices of coupled models are stored
#include "Hyd_Coupling_Model_List.h"
//Managament classes for the model couplings
#include "Hyd_Coupling_Management.h"
//Coupling class for a river-model coupled with a coast-model
#include "Hyd_Coupling_RV2CO.h"
//Coupling class for river-models coupled with a floodplain-model
#include "Hyd_Coupling_RV2FP.h"
//Merged coupling class one river-models coupled with multiple floodplain-model
#include "Hyd_Coupling_RV2FP_Merged.h"
//Coupling class for a floodplain-model coupled with a coast-model
#include "Hyd_Coupling_FP2CO.h"
//Coupling class for mulitple floodplain-model coupled with one coast-model
#include "Hyd_Coupling_FP2CO_Merged.h"
//Coupling class for a floodplain-model coupled with a floodplain-model
#include "Hyd_Coupling_FP2FP.h"
//Coupling class for a river-model coupled with a river-model
#include "Hyd_Coupling_RV2RV.h"
//Coupling class for a river-model coupled with a river-model via a diversion channel
#include "Hyd_Coupling_RV2RV_Diversion.h"
//Coupling class for a river-model coupled with a floodplain-model via a hydraulic structure
#include "Hyd_Coupling_RV2FP_Structure.h"
//base class for break coupling
#include "_Hyd_Coupling_Dikebreak.h"
//Coupling class for a river-model coupled with a floodplain-model via a dikebreak
#include "Hyd_Coupling_RV2FP_Dikebreak.h"
//Coupling class for a coast-model coupled with a floodplain-model via a dikebreak
#include "Hyd_Coupling_FP2CO_Dikebreak.h"
//Base class of the coupling points
#include "_Hyd_Coupling_Point.h"
//Base class of a list for the management of coupling points
#include "_Hyd_Coupling_Point_List.h"
#include "Hyd_Coupling_Point_RV2FP.h"
#include "Hyd_Coupling_Point_RV2FP_List.h"
#include "Hyd_Coupling_Point_FP2CO.h"
#include "Hyd_Coupling_Point_FP2CO_List.h"
#include "Hyd_Coupling_Point_FP2FP.h"
#include "Hyd_Coupling_Point_FP2FP_List.h"

//Base class for the coupling structures
#include "_Hyd_Coupling_Structure.h"
//Weir coupling class
#include "Hyd_Coupling_Structure_Weir.h"
//Gate coupling class
#include "Hyd_Coupling_Structure_Gate.h"

//Parameter classes
//container class for the river parameters
#include "Hyd_Param_RV.h" 
//container class for the floodplain parameters
#include "Hyd_Param_FP.h"			
//container class the of global parameters
#include "Hyd_Param_Global.h" 
//cointainer class for the material parameters
#include "Hyd_Param_Material.h"

//Parser classes
//Base class for the input per file
#include "_Hyd_Parse_IO.h"
//class parsing the global inputs
#include "Hyd_Parse_Glob.h"
//class parsing the river models
#include "Hyd_Parse_RV.h"
//class parsing the floodplain models
#include "Hyd_Parse_FP.h"
//class parsing the coast models
#include "Hyd_Parse_CO.h"
//class parsing the data for the diversion channel coupling
#include "Hyd_Parse_Coupling_Diversion.h"
//class parsing the data for a river- floodplain model coupling via a hydraulic structure (weir, gate)
#include "Hyd_Parse_Coupling_RV2FP_Structure.h"
//class parsing the data for a river- floodplain model coupling via a dikebreak
#include "Hyd_Parse_Coupling_RV2FP_Dikebreak.h"
//class parsing the data for a coast- floodplain model coupling via a dikebreak
#include "Hyd_Parse_Coupling_FP2CO_Dikebreak.h"


//Class of instationary boundary conditions
#include "Hyd_Instationary_Boundary.h"

//Dialog for choosing one hydraulic file for importing it into a database
#include "HydGui_System_Filechooser_Dia.h"
//Dialog-class for choosing multiple hydraulic boundary scenarios, which should be calculated or checked
#include "HydGui_Boundary_Scenario_Dia.h"
//dialog class for the hydraulic system parameters
#include "HydGui_System_State_Dia.h"
//Profile converter class to DGM-W
#include "Hyd_Profil2Dgm_Converter.h"


//CVODE SOLVER
//definitions of types realtype (set to double) and integertype (set to int), and the constant FALSE 
#include "sundials_types.h" 
//prototypes for CVodeMalloc, CVode, and CVodeFree,  constants OPT_SIZE, BDF, NEWTON, SV, SUCCESS, NST, NFE, NSETUPS, NNI, NCFN, NETF               
#include "cvode.h"  
//definitions of type N_Vector and macro NV_Ith_S, prototypes for N_VNew, N_VFree 
#include "nvector_serial.h"  
//definitions of type DenseMat, macro DENSE_ELEM 
#include "cvode_dense.h"             
#include "sundials_math.h"
//Preconditioner Includes
//contains the enum for types of preconditioning  
#include "sundials_iterative.h"  
//use CVSPGMR linear solver each internal step
#include "cvode_spgmr.h"   
//band preconditioner function prototypes 
#include "cvode_bandpre.h"       


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
#include "HydGui_Floodplain_Member_Wid.h"
#include "HydGui_River_Member_Wid.h"
#include "HydGui_System_Member_Wid.h"
#include "HydGui_Rv_Profile_General_Wid.h"
#include "HydGui_Bound_Scenario_Table_Wid.h"
#include "HydGui_Bound_Scenario_Data_Dia.h"
#include "HydGui_Coupling_Structure_Wid.h"
#include "HydGui_Diversion_Channel_Wid.h"
#include "HydGui_River_Geometrics_Table.h"
#include "HydGui_Rv_Profile_Overflow_Params_Wid.h"
#include "HydGui_Rv_Profile_General_Wid.h"
#include "HydGui_Rv_Profile_Break_Params_Wid.h"
#include "HydGui_Rv_Profile_Boundary_Point_Params_Wid.h"
#include "HydGui_Rv_Profile_Boundary_Lateral_Params_Wid.h"
#include "HydGui_Profil2Dgm_Converter_Dia.h"


//namespace
using namespace std;

#endif
