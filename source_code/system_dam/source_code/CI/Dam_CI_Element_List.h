#pragma once
/**\class _Dam_System
	\author produced by AG FRM HS-M
	\version 0.0.1
	\date 2021
*/

#ifndef DAM_CI_ELEMENT_LIST
#define DAM_CI_ELEMENT_LIST



//sys libs
#include "_Sys_Common_System.h"
#include "Sys_Multi_Filechooser_Dia.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Diverse_Text_Dia.h"


//dam-libs
#include "Dam_CI_Point.h"
#include "Dam_CI_Polygon.h"

///Class for the points as CI elements points which are used in the CI-damage calculation  \ingroup dam
/**
The list connects the _Dam_CI_Elements with each other. In- and outcomming list are available.

*/
class Dam_CI_Element_List 
{
public:
	///Default constructor
	Dam_CI_Element_List(void);
	///Default destructor
	~Dam_CI_Element_List(void);

	//method

private:
	///Set error(s)
};
#endif

