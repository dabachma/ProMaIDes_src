#pragma once
/**\class _Dam_System
	\author produced by AG FRM HS-M
	\version 0.0.1
	\date 2021
*/

#ifndef _DAM_CI_ELEMENT
#define _DAM_CI_ELEMENT



//sys libs
#include "_Sys_Common_System.h"
#include "Sys_Multi_Filechooser_Dia.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Diverse_Text_Dia.h"

//dam-libs
//#include "Dam_CI_Element_List.h"
//forward declaration
class Dam_CI_Element_List;

//Base class for the CI elements points and polygons which are used in the CI-damage calculation  \ingroup dam
/**

*/
class _Dam_CI_Element
{


public:
	///Default constructor
	_Dam_CI_Element(void);
	///Default destructor
	virtual ~_Dam_CI_Element(void);


	//method

protected:
	//members
	///Boundary value (waterlevel), when the CI element fails
	double boundary_value;

	///Index of the floodplain, to which the CI element is connected
	int index_fp;
	///Index of the floodplain element, to which the CI element is connected
	int index_fp_elem;
	///Flag if CI element is connected to the hydraulic
	bool is_connected;
	///Global index of CI element in the database
	int global_index;

	///Incommings CI elements
	Dam_CI_Element_List *incomings;
	///Outgoings CI elements
	Dam_CI_Element_List *outgoing;


private:

	///Set error(s)


};
#endif
