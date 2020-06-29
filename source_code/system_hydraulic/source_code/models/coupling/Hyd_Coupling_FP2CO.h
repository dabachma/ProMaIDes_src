#pragma once
/**\class Hyd_Coupling_FP2CO
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_FP2CO_H
#define HYD_COUPLING_FP2CO_H

//hyd-class
//Coupling to a coast model
#include "Hyd_Coast_Model.h"
//floodplain model class
#include "Hyd_Model_Floodplain.h"
//list of coupling points
#include "Hyd_Coupling_Point_FP2CO_List.h"

//system_sys_ classes
#include "_Sys_Common_System.h"

///Coupling management-class for a coupling between one floodplain-model (FP) to the coast-model (CO) \ingroup hyd
/**
This class manages the coupling between one floodplain-model (FP) and the coast-model (CO) via a list of coupling points.
This coupling is a linear coupling.
The flow between the models is over the defined coast-line. For flow-calculation the Poleni-formula is used.
In this class just the coupling
between one floodplain- to the coast-model is managed. Thus, it gets necessary to merge the lists after evaluation
all couplings of the coast-model to diverse floodplain-models. This is done in Hyd_Coupling_FP2CO_Merged.

\see Hyd_Coupling_Point_FP2CO, Hyd_Coupling_Point_FP2CO_List
*/
class Hyd_Coupling_FP2CO
{
public:
	///Default constructor
	Hyd_Coupling_FP2CO(void);
	///Default destructor
	~Hyd_Coupling_FP2CO(void);

	//members
	///The relevant coupling points are stored here
	Hyd_Coupling_Point_FP2CO_List list;

	//methods
	///The pointer of the models for coupling are se
	void set_ptr_coupling(Hyd_Coast_Model *coast, Hyd_Model_Floodplain *floodplain);
	///Get the pointer to the coast model
	Hyd_Coast_Model *get_coastmodel_ptr(void);
	///Get the pointer to the floodplain model
	Hyd_Model_Floodplain *get_floodplainmodel_ptr(void);

	///The couplings are initialized
	void init_coupling(void);
	///Output the header for the coupled model indizes
	static void output_header_coupled_indices(ostringstream *cout);
	///Output the indices of the coupled model
	void output_index_coupled_models(ostringstream *cout, const int number);
	///Output the setted coupling points in the list
	void output_setted_coupling_points(void);

	///Set that the couplings are already merged
	void set_is_merged(void);
	///Get that the couplings are already merged
	bool get_is_merged(void);

private:
	//members
	///Pointer to the coupled coast model
	Hyd_Coast_Model *coast_model;
	///Pointer to the coupled floodplain model
	Hyd_Model_Floodplain *floodplain_model;
	///Flag if the couplings are already merged
	bool is_merged_flag;


};
#endif
