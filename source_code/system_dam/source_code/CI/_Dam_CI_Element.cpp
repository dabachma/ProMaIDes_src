#include "source_code\Dam_Headers_Precompiled.h"
#include "_Dam_CI_Element.h"

//Default constructor
_Dam_CI_Element::_Dam_CI_Element(void){

	//Boundary value (waterlevel), when the CI element fails
	this->boundary_value = -9999;

	//Index of the floodplain, to which the CI element is connected
	this->index_fp=-1;
	//Index of the floodplain element, to which the CI element is connected
	this->index_fp_elem=-1;
	//Flag if CI element is connected to the hydraulic
	this->is_connected= false;
	//Global index of CI element in the database
	this->global_index = -1;
	//Incomíngs CI elements
	this->incomings=NULL;
	//Outgoings CI elements
	this->outgoing=NULL;
}

//Default destructor
_Dam_CI_Element::~_Dam_CI_Element(void){
}


//__________
//public




//____________
//private