#pragma once
/**\class Hyd_Coupling_Model_List
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_MODEL_LIST_H
#define HYD_COUPLING_MODEL_LIST_H

//system_sys_ classes
#include "_Sys_Common_System.h"

///Structure where the indices of two coupled models are stored \ingroup hyd
struct _hyd_model_coupling{
	///Index of the first model (main-model)
	int index_first_model;
	///Index of the second model
	int index_second_model;
};

///List-class where the indices of coupled models are stored in a list \ingroup hyd
/**
In this class the indices of coupled models are stored in a list. The coupling of models is decided via geometrical interception
of the models. This class is needed to store if the models are intercepting each other. After filling this list
the coupling classes are allocated.
*/
class Hyd_Coupling_Model_List
{
public:
	///Default constructor
	Hyd_Coupling_Model_List(void);
	///Default destructor
	~Hyd_Coupling_Model_List(void);

	//methods
	///Get the number of couplings
	int get_number_couplings(void);
	///Get the total list as a pointer
	_hyd_model_coupling* get_total_list(void);
	///Get a specific _hyd_model_coupling with the given index
	_hyd_model_coupling get_model_coupling(const int index);
	///Get the first _hyd_model_coupling of the list
	_hyd_model_coupling get_first_model_coupling(void);
	///Get the last _hyd_model_coupling of the list
	_hyd_model_coupling get_last_model_coupling(void);

	///Output members
	void output_members(ostringstream *cout);

	///Set a new _hyd_model_coupling
	void set_new_model_coupling(_hyd_model_coupling *new_coupling);

	///Delete the list
	void delete_list(void);


private:

	//members
	//Number of points in list
	int number;
	///_hyd_model_coupling in list
	_hyd_model_coupling *couplings;


	//methods
	///Delete the allocated points
	void delete_model_coupling(void);
	///Allocate the points
	void allocate_model_coupling(void);

	///Set error(s)
	Error set_error(const int err_type);
};

#endif