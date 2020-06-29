#pragma once
/**\class Hyd_Coupling_FP2CO_Merged
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_FP2CO_MERGED_H
#define HYD_COUPLING_FP2CO_MERGED_H

//hyd-class
//Coupling to a coast model from floodplainmodel
#include "Hyd_Coast_Model.h"
//floodplain model class
#include "Hyd_Model_Floodplain.h"
//list of coupling points
#include "Hyd_Coupling_Point_FP2CO_List.h"
//the class to merge in this class
#include "Hyd_Coupling_FP2CO.h"
//for inserting river junctions
#include "Hyd_Coupling_RV2CO.h"

//system_sys_ classes
#include "_Sys_Common_System.h"


///Managing-class for a coupling between mulitple floodplain-model (FP) to the coast-model (CO) \ingroup hyd
/**
This class manages the coupling between mulitple floodplain-model (FP) and the coast-model (CO) via a list of coupling points.
The point lists of different couplings between one floodplain-model (FP) to the coast-model (CO) are merged here to one point list.
\see Hyd_Coupling_FP2CO, Hyd_Coupling_Point_FP2CO, Hyd_Coupling_Point_FP2CO_List
*/
class Hyd_Coupling_FP2CO_Merged
{
public:
	///Default constructor
	Hyd_Coupling_FP2CO_Merged(void);
	///Default destructor
	~Hyd_Coupling_FP2CO_Merged(void);

	//members
	///The relevant coupling points are stored here
	Hyd_Coupling_Point_FP2CO_List list;


	//methods
	///Get the number of coupled floodplains
	int get_number_coupled_fp(void);
	///Get the index of a coupled floodplain model by the given index (number of coupled floodplain models)
	int get_index_coupled_fp_models(const int index);

	///The pointer to the coastmodel is set
	void set_ptr_coupling_coast(Hyd_Coast_Model *coast);

	///Get the pointer to the coast model
	Hyd_Coast_Model *get_ptr_coast_model(void);

	///The pointer of the models for couplings are set and the lists are merged
	void set_ptr_coupling_with_merging(Hyd_Coupling_FP2CO *couplings);

	///Synchronise the models
	void synchronise_models(const double timepoint, const double delta_t, const bool time_check, const int internal_counter);
	///Get the maximum waterlevel gradient 
	double get_maximum_h_grad(void);


	///Output the header for the coupled model indizes
	static void output_header_coupled_indices(ostringstream *cout);
	///Output the indices of the coupled model
	void output_index_coupled_models(ostringstream *cout);
	///Output the setted coupling points in the list
	void output_setted_coupling_points(void);

	///Insert river junctions into the list
	void insert_river_junctions(Hyd_Coupling_RV2CO *rv2co_coupling, const int number);

	///Clone the coast to floodplain merged coupling
	void clone_couplings(Hyd_Coupling_FP2CO_Merged *coupling, Hyd_Hydraulic_System *system);

	///Set up the list of fpl-section ids in the total merged lists
	void set_up_fpl_section_ids_list(void);

	///Calculate and output final results of the maximum waterlevel of the coupling points to database
	void calc_output_final_results(QSqlDatabase *ptr_database, const _sys_system_id id, const string id_break, const int bound_sc);


private:

	//members
	///Pointer to the pointer of coupled floodplain models
	Hyd_Model_Floodplain **floodplain_model;
	///Number of coupled floodplain models
	int number_fp_models;

	///Pointer to the coupled coast model
	Hyd_Coast_Model *coast_model;

	
	//methods
	///Add a pointer of additional floodplain models
	void add_floodplainmodel_pointer(Hyd_Model_Floodplain *pointer);
	///set the error
	Error set_error(const int err_type);

};

#endif
