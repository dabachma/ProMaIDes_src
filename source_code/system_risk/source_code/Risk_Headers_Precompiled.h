#pragma once
#ifndef RISK_HEADERS_PRECOMPILED_H
#define RISK_HEADERS_PRECOMPILED_H

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
#include "Common_Const.h"
//add this to the cpp-files

//main risk system
#include "Risk_System.h"

//break scenarios
#include "Risk_Break_Scenario_Management.h"
#include "Risk_Break_Scenario.h"
//list of break scenarios
#include "Risk_Catchment_Break_Scenario_List.h"

//dialog for asking the reliability of the required results
#include "RiskGui_Result_Reliability_Dia.h"
//dialog for combining a risk break scenario for the scenario risk approach
#include "RiskGui_Risk_Scenario_Dia.h"
//dialog for asking the break height
#include "RiskGui_Break_Height_Dia.h"
//dialog for selecting catchment risk states for calculation
#include "RiskGui_Catchment_Scenario_Dia.h"
//widget for displaying the catchment risk states 
#include "RiskGui_Catchment_Scenario_Table_Wid.h"
//widget for displaying the statistics of the catchment risk states 
#include "RiskGui_Catchment_Scenario_Statistic_Wid.h"
//widget for the deatiled risk results as summary widget
#include "RiskGui_Detailed_Risk_Summary_Wid.h"
//widget for displaying the generated scenarios of the catchment area risk approach in a table widget
#include "RiskGui_Result_Scenario_Wid.h"
//dialog for asking the calculation steps
#include "RiskGui_Calculate_All_Dia.h"

//widget for displaying the cumulated risk result
#include "RiskGui_Result_Cumulated_Wid.h"
//widget for displaying the total risk result
#include "RiskGui_Result_Wid.h"
//widget for displaying the detailed risk result
#include "RiskGui_Detailed_Result_Wid.h"
//summary widget for the fpl-setcion probabilities
#include "RiskGui_Probabilty_Fpl_Section_Wid.h"
//ask for the optimisation boundary values of the risk prediction
#include "RiskGui_Optimisation_Prediction_Dia.h"
//widget for dsipalying the general members of the risk calculation
#include "RiskGui_System_Member_Wid.h"

//class for the calculations of the hydraulic break scenarios
#include "Risk_Hydraulic_Break_Calculation.h"


//prediction of damages for the catchment area risk approach
#include "Risk_Damage_Predictor.h"
//storing the data of the damage results
#include "Risk_Dam_Results.h"
//generation of the catchment area risk scenarios and handling
#include "Risk_Catchment_Scenario_Tree.h"
//gathering and managing data of the hydraulic stress to the fpl-section
#include "Risk_Fpl_Stress.h"


//namespace
using namespace std;

#endif