#pragma once
/**\class Hyd_Coupling_FP2FP
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_FP2FP_H
#define HYD_COUPLING_FP2FP_H

//hyd-class
//floodplain model class
#include "Hyd_Model_Floodplain.h"
//list of coupling points
#include "Hyd_Coupling_Point_FP2FP_List.h"

//system_sys_ classes
#include "_Sys_Common_System.h"
//forward
class Hyd_Hydraulic_System;

///Managing-class for a coupling between one floodplain-model (FP) to one floodplain-model (FP) \ingroup hyd
/**
This class manages the coupling between one floodplain-model (FP) and one floodplain-model (FP) via a list of coupling points.
The flow between the models is over the their boundaries. For flow-calculation the Manning-formula is used.
\see Hyd_Coupling_Point_FP2FP, Hyd_Coupling_Point_FP2FP_List
*/
class Hyd_Coupling_FP2FP
{
public:
	///Default constructor
	Hyd_Coupling_FP2FP(void);
	///Default destructor
	~Hyd_Coupling_FP2FP(void);

	//members
	///The relevant coupling points are stored here
	Hyd_Coupling_Point_FP2FP_List list;

	//methods
	///The pointer of the models for coupling are set
	void set_ptr_coupling(Hyd_Model_Floodplain *floodplain_1, Hyd_Model_Floodplain *floodplain_2);
	///The couplings are initialized
	void init_coupling(void);
	///Synchronise the models
	void synchronise_models(const double timepoint, const double delta_t, const bool time_check, const int internal_counter);

	///Output the header for the coupled model indizes
	static void output_header_coupled_indices(ostringstream *cout);
	///Output the indices of the coupled model
	void output_index_coupled_models(ostringstream *cout, const int number);
	//Output the setted coupling points in the list
	void output_setted_coupling_points(void);

	///Clone the floodplain to floodplain couplings
	void clone_couplings(Hyd_Coupling_FP2FP *coupling, Hyd_Hydraulic_System *system);


private:
	//members
	///Pointer to the floodplain model no. 1
	Hyd_Model_Floodplain *floodplain_model_1;
	///Pointer to the floodplain model no. 2
	Hyd_Model_Floodplain *floodplain_model_2;
	///Defining polysegment is the boundary of the first floodplain model
	Hyd_Floodplain_Polysegment defining_polysegment;

	//methods
	///Set the defining polysegment with the boundary of the first floodplain model
	void set_defining_polysegment(void);


};
#endif
