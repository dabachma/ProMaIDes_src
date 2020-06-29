#pragma once
/**\class Hyd_Coupling_RV2CO
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_RV2CO_H
#define HYD_COUPLING_RV2CO_H

//hyd-class
//Coupling to a coast model of the last profile of a river model
#include "Hyd_Coast_Model.h"
//river model class
#include "Hyd_Model_River.h"

//system_sys_ classes
#include "_Sys_Common_System.h"

//forward
class Hyd_Hydraulic_System;

///Coupling management-class for the coupling between a river- (RV) and the coast-model (CO) \ingroup hyd
/**
The coupling between a river- and the coast-model is a punctuell coupling. If the outflow profile's mid-point of the 
river-model (Hyd_River_Profile_Outflow) lies inside the coast-model polygon (Hyd_Coast_Model) than a coupling
is established. The waterlevel of the coast-model is set directly as a boundary-condition to the river-model's outflow profile.
The discharge to the coast-model is not further considered.
*/
class Hyd_Coupling_RV2CO
{
public:
	///Default constructor
	Hyd_Coupling_RV2CO(void);
	///Default destructor
	~Hyd_Coupling_RV2CO(void);

	//methods
	///The pointer of the models for coupling are set
	void set_ptr_coupling(Hyd_Coast_Model *coast, Hyd_Model_River *river);
	///Get the pointer to the river model
	 Hyd_Model_River *get_ptr_rivermodel(void);

	///The couplings are initialized
	void init_coupling(void);
	///Synchronise the models
	void synchronise_models(void);
	///Output the header for the coupled model indizes
	static void output_header_coupled_indices(ostringstream *cout);
	///Output the indexes of the coupled model
	void output_index_coupled_models(ostringstream *cout, const int number);
	///Clone the river to coast couplings
	void clone_couplings(Hyd_Coupling_RV2CO *coupling, Hyd_Hydraulic_System *system);

private:
	//members
	///Pointer to the coupled coast model
	Hyd_Coast_Model *coast_model;
	///Pointer to the coupled river model
	Hyd_Model_River *river_model;

	

};
#endif
