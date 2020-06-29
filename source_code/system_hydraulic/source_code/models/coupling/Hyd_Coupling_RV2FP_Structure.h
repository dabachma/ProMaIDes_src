#pragma once
/**\class Hyd_Coupling_RV2FP_Structure
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_RV2FP_STRUCTURE_H
#define HYD_COUPLING_RV2FP_STRUCTURE_H

//hyd_system class
//Base class of the coupling structures
#include "_Hyd_Coupling_Structure.h"
//Types of coupling structures
#include "Hyd_Coupling_Structure_Weir.h"
#include "Hyd_Coupling_Structure_Gate.h"
//Parser the input per file
#include "Hyd_Parse_Coupling_RV2FP_Structure.h"
#include "Hyd_Coupling_RV2FP.h"
#include "Hyd_Coupling_Point_RV2FP.h"

///Coupling-class for a coupling between a river-model and a floodplain-model via a hydraulic structure \ingroup hyd
/**
This class represents a special type of coupling between a river- and a floodplain-model. 
It is a punctuell coupling. It is defined by the user not via geometrical interception.
\see _Hyd_Coupling_Structure, Hyd_Coupling_Structure_Weir, Hyd_Coupling_Structure_Gate 
*/
class Hyd_Coupling_RV2FP_Structure : public _Hyd_Coupling_Point, public _Sys_Common_System
{
public:
	///Default constructor
	Hyd_Coupling_RV2FP_Structure(void);
	///Default destructor
	~Hyd_Coupling_RV2FP_Structure(void);



	//methods

	///Input of the structure coupling parameters per file via a parser (Hyd_Parse_Coupling_Structure)
	void input_parameter_per_file(const int index, const string file);
	///Input of the structure coupling parameters from a database selection
	void input_parameter_per_database(const QSqlQueryModel *results, const int index, const bool output_flag);

	///Create the database table for the coupling structure data
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the coupling structure data: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the coupling structure data
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the coupling structure data
	static void close_table(void);
	///Select and count the number of relevant structure couplings in a database table
	static int select_relevant_structures_database(QSqlDatabase *ptr_database, QSqlQueryModel *results, const _sys_system_id id, const bool with_output = true);


	///Transfer the coupling structure data of a file into a database table
	void transfer_coupling_structure2database_table(QSqlDatabase *ptr_database);

	///The couplings are initialized with the already performed RV2FP couplings
	void init_coupling(Hyd_Coupling_RV2FP_Merged *couplings, const int number);
	///Synchronise the models
	void synchronise_models(const double timepoint, const double delta_t, const bool time_check);
	///Reset the coupling discharges
	void reset_coupling_discharge(void);

	///Output the members
	void output_members(void);
	///Output the header for the coupled model indizes
	static void output_header_coupled_indices(ostringstream *cout);
	///Output the indizes of the coupled model
	void output_index_coupled_models(ostringstream *cout, const int number);

	///Output final result members
	void output_final_results(void);

	///Reset the hydrological balance values and the maximum calculated values
	void reset_hydro_balance_max_values(void);

	///Clone the structure coupling
	void clone_couplings(Hyd_Coupling_RV2FP_Structure *coupling, Hyd_Hydraulic_System *system);

	///Ouput the point to database as Geo_point data
	void output_point2database(QSqlDatabase *ptr_database);

	///Reset the smoothing calculation members
	void reset_smoothing(void);

private:
	//members

	///Pointer to the table for the the dikebreak/wallbreak data in a database
	static Tables *coupling_structure_table;

	///Index of the coupling
	int index;
	///Index of the river model
	int index_rv_model;
	///Index of the floodplain model
	int index_fp_model;
	///Direction of coupling in flow direction (true:= left; false:=right)
	bool left_flag;
	///Index of the river profile related to the given distance
	int index_related_profile;
	///Distance of the breach begin related to a profile
	double distance_start_related;


	///Specifies the type of structure
	_hyd_1d_outflow_types inflow_type;
	///Pointer to the coupling structure; it will be decided which type is needed (weir/gate)
	_Hyd_Coupling_Structure *coupling_struct;


	///Pointer to the river profile downwards
	_Hyd_River_Profile *river_profile_down;
	///Pointer to the river profile upwards
	_Hyd_River_Profile *river_profile_up;
	///Pointer to the floodplain element
	Hyd_Element_Floodplain *floodplain_elem;

	///Maximum of the mid of height of the two profiles (global) and the floodplain element height 
    double mid_height;
	///Factor (upwards) for the mid-value calculation of the mid_height and the mid_waterlevel depending on the distances
	double mid_fac_up;
	///Factor (downwards) for the mid-value calculation of the mid_height and the mid_waterlevel depending on the distances
	double mid_fac_down;
	///Mid of waterlevel of the two profiles
	double mid_waterlevel;

	///Flag for a horizontal backwater from downstream
	bool horizontal_backwater_flag;
	///Flag for a horizontal backwater from upstream
	bool horizontal_backwater_flag_upstream;

	///Current gradient of q
	double grad_q_current;
	///Gradient one syncronisation step before
	double grad_q_before;
	///Counter for the oscilations
	double oscilation_counter;
	///Counter if there are no oscilation
	double no_osci_counter;


	//methods

	///Allocate the given coupling structure type
	void allocate_coupling_structure(void);

	///Set the structure coupling member with a structure where the parser has gathered the informations (_hyd_rv2fp_struct_coupling_params)
	void set_structure_coupling_params(_hyd_rv2fp_struct_coupling_params parameter);

	///Set the mid-values (mid_height, mid_fac_up, mid_fac_down) for a coupling over the left bank
	void set_midvalues_leftbank(void);
	///Set the mid-values (mid_height, mid_fac_up, mid_fac_down) for a coupling over the right bank
	void set_midvalues_rightbank(void);

	///Calculate the current mid-waterlevel via the factors
	void calculate_mid_waterlevel(void);


	//methods
	///Smooth the coupling discharge with the coupling discharge calculated one internal timestep before
	double smooth_coupling_discharge(const double q_current, double *old_q);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif
