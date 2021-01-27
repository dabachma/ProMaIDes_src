#pragma once
/**\class _Dam_System
	\author produced by AG FRM HS-M
	\version 0.0.1
	\date 2021
*/

#ifndef DAM_CI_SYSTEM
#define DAM_CI_SYSTEM



//sys libs
#include "_Sys_Common_System.h"
#include "Sys_Multi_Filechooser_Dia.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Diverse_Text_Dia.h"
#include "Geo_Point.h"

//dam-libs
#include "Dam_CI_Element_List.h"

//Class for managing the CI-damage calculation  \ingroup dam
/**
This class manages the input, the calculation and the output of the CI-damage system.
*/
class Dam_CI_System
{
public:
	///Default constructor
	Dam_CI_System(void);
	///Default destructor
	~Dam_CI_System(void);


	//method

private:
	///Set error(s)
};
#endif


