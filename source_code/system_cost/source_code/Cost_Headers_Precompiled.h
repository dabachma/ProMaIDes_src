#pragma once
#ifndef COST_HEADERS_PRECOMPILED_H
#define COST_HEADERS_PRECOMPILED_H

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


#include "Cost_System.h"

//economical cost system
#include "Cost_Ecn_Cost_System.h"
//dialog for set economical cost-data by hand
#include "CostGui_Alternative_Ecn_Cost_Dia.h"


//namespace
using namespace std;

#endif