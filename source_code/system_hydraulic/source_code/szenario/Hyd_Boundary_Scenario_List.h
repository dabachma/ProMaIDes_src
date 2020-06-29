#pragma once
/**\class Hyd_Boundary_Scenario_List
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_BOUNDARY_SZENARIO_LIST_H
#define HYD_BOUNDARY_SZENARIO_LIST_H

//hyd-system
#include "Hyd_Boundary_Szenario.h"


///List-class for handling of boundary scenarios for one hydraulic system \ingroup hyd
/**

\see Hyd_Boundary_Szenario, Hyd_Boundary_Management

*/
class Hyd_Boundary_Scenario_List
{
public:
	///Default constructor
	Hyd_Boundary_Scenario_List(void);
	///Default destructor
	~Hyd_Boundary_Scenario_List(void);

	///Add scenario to list
	void add_scenario2list(Hyd_Boundary_Szenario *add);

	///Clear the list
	void clear_list(void);

	///Get the number of list members
	int get_number_in_list(void);
	///Get a pointer to the total list
	Hyd_Boundary_Szenario *get_total_list(void);
	///Get a pointer to a list member specified by the given index
	Hyd_Boundary_Szenario *get_ptr_scenario(const int index);





private:

	//members
	///Number of boundary scenarios in list
	int number;
	///List of boundary scenarios
	Hyd_Boundary_Szenario *bound_sc;


	//method
	///Allocate the hydraulic boundary scenarios
	void allocate_list(void);
	///Delete the list
	void delete_list(void);

	///Set error(s)
	Error set_error(const int err_type);


};
#endif
