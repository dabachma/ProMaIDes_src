#pragma once
#ifndef ALT_HEADERS_PRECOMPILED_H
#define ALT_HEADERS_PRECOMPILED_H

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


#include "Alt_System.h"

//dialog for general measure data
#include "AltGui_Alternative_Info_Dia.h"
//dialog for detailed measure data
#include "AltGui_Alternative_Type_Dia.h"

//one measure by file replacing widget
#include "AltGui_File_Replacing_Wid.h"
//multiple measure by file replacing dialog
#include "AltGui_Measure_Replacing_File_Dia.h"
//dialog for selecting a base measure state
#include "AltGui_Alternative_Base_On_Dia.h"

#include "AltGui_Dam_Resettlement_Dia.h"
#include "AltGui_Dam_Change_Function_Dia.h"

#include "AltGui_Id_Replacing_Wid.h"

#include "AltGui_Measure_Dia.h"

#include "AltGui_Alternative_Info_Wid.h"


//namespace
using namespace std;

#endif