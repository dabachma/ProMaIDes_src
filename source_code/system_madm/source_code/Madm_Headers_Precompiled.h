#pragma once
#ifndef MADM_HEADERS_PRECOMPILED_H
#define MADM_HEADERS_PRECOMPILED_H

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


//decision matrix
#include "Madm_Decision_Matrix.h"
//madm analysis applying the madm-algorithms
#include "Madm_Analysis.h"
//parent of the madm-algorithms
#include "_Madm_Solver.h"
#include "Madm_Solver_Electre.h"
#include "Madm_Solver_Topsis.h"
#include "Madm_Solver_Saw.h"

#include "Madm_System.h"

//dialog for selction of weighting sets
#include "MadmGui_Weighting_Set_Dia.h"
//widget for the weighting sets
#include "MadmGui_Weight_Sets_Wid.h"
//widget for the ranking
#include "MadmGui_Rank_Wid.h"
//widget for the composed ranking
#include "MadmGui_Total_Rank_Wid.h"
//summary of the madm-matrix
#include "MadmGui_Matrix_Summary_Wid.h"



//namespace
using namespace std;

#endif