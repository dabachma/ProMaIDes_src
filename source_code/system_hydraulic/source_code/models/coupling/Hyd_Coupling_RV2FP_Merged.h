#pragma once
/**\class Hyd_Coupling_RV2FP_Merged
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_RV2FP_MERGED_H
#define HYD_COUPLING_RV2FP_MERGED_H

//hyd-class
//Coupling to a coast model of the last profile of a river model
#include "Hyd_Model_Floodplain.h"
//river model class
#include "Hyd_Model_River.h"
//list of coupling points
#include "Hyd_Coupling_Point_RV2FP_List.h"
//the lists of this object are merged here
#include "Hyd_Coupling_RV2FP.h"

//system_sys_ classes
#include "_Sys_Common_System.h"




///Managing-class for a coupling between mulitple floodplain-model (FP) to the coast-model (CO) \ingroup hyd
/**
This class manages the coupling between mulitple floodplain-model (FP) to one river-model (RV) via lists of coupling points.
There are two list for the left river-bank line and the right river-bank line (in flow direction).
The point lists of different couplings between one floodplain-model (FP) to one river-model (RV) are merged here to one point list.
In this class it is also checked if a direct coupling, it means a coupling of the inflow river profile or
the outflow river profile to a floodplain, is established. Some condition are required:
 - no boundary condition is set to the river profile
 - the profiles are not coupled to another rivermodel or a coast-model
 - the mid-points of the profile are inside the floodplain-model
\see Hyd_Coupling_RV2FP, Hyd_Coupling_Point_RV2FP, Hyd_Coupling_Point_RV2FP_List
*/
class Hyd_Coupling_RV2FP_Merged
{
public:
	///Default constructor
	Hyd_Coupling_RV2FP_Merged(void);
	///Default destructor
	~Hyd_Coupling_RV2FP_Merged(void);

	//members
	///The relevant coupling points are stored here for the left river bank (in flow direction)
	Hyd_Coupling_Point_RV2FP_List list_left;
	///The relevant coupling points are stored here for the right river bank (in flow direction)
	Hyd_Coupling_Point_RV2FP_List list_right;

	///Pointer to the table for the maximum waterlevels of the coupling points for each fpl-section
	/**This pointer is allocated with set_erg_table(QSqlDatabase *ptr_database) and deleted with close_erg_table(void) */
	static Tables *max_h_table;


	//methods

	///Set the database table for the maximum waterlevels of the coupling points for each fpl-section: it sets the table name and the name of the columns and allocate them
	static void set_max_h_table(QSqlDatabase *ptr_database);
	///Create the database table for the maximum waterlevels of the coupling points for each fpl-section
	static void create_max_h_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the maximum waterlevels of the coupling points for each fpl-section
	static void delete_data_max_h_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the maximum waterlevels of the coupling points for each fpl-section
	static void delete_data_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete all data in the database table for the maximum waterlevels of the coupling points for each fpl-section for the given identifier
	static void delete_data_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag);
	///Delete all data in the database table for the maximum waterlevels of the coupling points for each fpl-section for the given identifier
	static void delete_data_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_bound_sz, const string break_sc);
	///Close and delete the database table for the maximum waterlevels of the coupling points for each fpl-section
	static void close_max_h_table(void);
	///Switch the applied-flag in the database table for the maximum waterlevels of the coupling points for each fpl-section for the given identifier 
	static void switch_applied_flag_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag in the database table for the maximum waterlevels of the coupling points for each fpl-section for the given hydraulic boundary identifier 
	static void switch_applied_flag_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);
	///Set the point id and the maximum waterlevel in the database table for the maximum waterlevels of the coupling points for each fpl-section for the given identifier
	static int set_hmax_in_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const string break_sc, const int fpl_sec, int *point_id, double *max_h);
	///Select the point id and the maximum waterlevel in the database table for the maximum waterlevels for a break between river- and floodplainmodel
	static int set_hmax2break_in_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const string break_sc, const int fpl_sec, int *point_id, double *max_h2break);
	///Select all maximum waterlevel in the database table for the maximum waterlevels for a break between river- and floodplainmodel for a given scenario
	static int set_hmax2break_in_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const string break_sc, QSqlQueryModel *query);


	///Copy the results of a given system id to another one in database table
	static void copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);
	///Output final results of the maximum waterlevel of the coupling points to database 
	static void output_final_results(QSqlDatabase *ptr_database, const _sys_system_id id, const string id_break, const int bound_sc, const int fpl_sec_id, const _hyd_max_values max_h, const int point_id_h_max, const _hyd_max_values max_h2break, const int point_id_h_max2break);

	///The pointer of the models for couplings are set and the lists are merged
	void set_ptr_coupling_with_merging(Hyd_Coupling_RV2FP *couplings);

	///Set up the list of fpl-section ids in the total merged lists (left/right)
	void set_up_fpl_section_ids_list(void);

	///Get the coupled river index
	int get_river_index(void);
	///Get a pointer to the coupled river model
	Hyd_Model_River* get_river_model(void);

	///Get the flag if the river model is already set
	bool get_river_model_is_setted(void);

	///Get the number of coupled floodplains
	int get_number_coupled_fp(void);
	///Get the index of a coupled floodplain model by the given index (number of coupled floodplain models)
	int get_index_coupled_fp_models(const int index);

	///Synchronise the models
	void synchronise_models(const double timepoint, const double delta_t, const bool time_check, const int internal_counter);
	///Get the maximum waterlevel gradient 
	double get_maximum_h_grad(void);

	///Reset the coupling discharges
	void reset_coupling_discharges(void);
	///Output the header for the coupled model indizes
	static void output_header_coupled_indices(ostringstream *cout);
	///Output the indices of the coupled model
	void output_index_coupled_models(ostringstream *cout, const int number);
	///Output the set coupling points in the list
	void output_setted_coupling_points(void);
	///Calculate and output final results of the maximum waterlevel of the coupling points to database
	void calc_output_final_results(QSqlDatabase *ptr_database, const _sys_system_id id, const string id_break, const int bound_sc);


	///Check and set if the river outflow profile is in a floodplain model and is not coupeld to other river models=> outflow goes to the floodplain
	void check_river_outflow2floodplain(void);
	///Get the flag, if direct RV2FP-coupling out of the outflow-profile is set
	bool get_direct_coupling_flag_out(void);

	///Check and set if the river inflow profile is in a floodplain model and is not coupeld to other river models=> inflow comes from the floodplain
	void check_river_inflow2floodplain(void);
	///Get the flag, if direct RV2FP-coupling into a inflow profile is set
	bool get_direct_coupling_flag_in(void);

	///Check coupling from one bank line to the other side of the river; the coupling is than not possible
	void check_coupling2_other_river_side(void);

	///Check coupling from one bank line to the other side of other rivers; the coupling is than not possible
	void check_coupling2_other_river_side(Hyd_Coupling_RV2FP_Merged *other_river);

	///Clone the river to floodplain merged coupling
	void clone_couplings(Hyd_Coupling_RV2FP_Merged *coupling, Hyd_Hydraulic_System *system);

private:
	
	//members
	///Pointer to the pointer of coupled floodplain models
	Hyd_Model_Floodplain **floodplain_model;
	///Pointer to the coupled river model
	Hyd_Model_River *river_model;
	///Flag if the pointer to the river model is set
	bool river_model_is_set;

	///Flag if a direct RV2FP-coupling out of the outflow-profile is set
	bool direct_coupling_flag_out;
	///Pointer to the floodplain element, which is directly coupled to the river (direct outflow profile coupling)
	Hyd_Element_Floodplain *direct_coupled_fpelem_out;
	///Index of the direct coupled element (direct outflow profile coupling)
	int direct_coupled_elem_index_out;
	///Pointer to the direct coupled floodplain (outflow)
	Hyd_Model_Floodplain *direct_coupled_fp_out;

	///Flag if a direct RV2FP-coupling into a inflow profile is set
	bool direct_coupling_flag_in;
	///Pointer to the floodplain element, which is directly coupled to the river (direct inflow profile coupling)
	Hyd_Element_Floodplain *direct_coupled_fpelem_in;
	///Index of the direct coupled element (direct inflow profile coupling)
	int direct_coupled_elem_index_in;
	///Pointer to the direct coupled floodplain (inflow)
	Hyd_Model_Floodplain *direct_coupled_fp_in;

	///Number of coupled floodplain models
	int number_fp_models;

	//methods
	///Initialize the direct RV2FP-coupling out of the outflow-profile
	void init_direct_coupling_out(void);
	///Synchronise the direct RV2FP-coupling out of the outflow-profile
	void synchronise_direct_coupling_out(void);
	///Reset the direct RV2FP-coupling out of the outflow-profile
	void reset_direct_coupling_discharges_out(void);
	///Check the direct coupling
	void check_direct_coupling_out(void);

	///Initialize the direct RV2FP-coupling into a inflow-profile
	void init_direct_coupling_in(void);
	///Synchronise the direct RV2FP-coupling into a inflow-profile
	void synchronise_direct_coupling_in(void);
	///Reset the direct RV2FP-coupling into a inflow-profile
	void reset_direct_coupling_discharges_in(void);
	///Check the direct coupling into a inflow-profile
	void check_direct_coupling_in(void);
	///Add a pointer of additional floodplain models
	void add_floodplainmodel_pointer(Hyd_Model_Floodplain *pointer);

	
	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif
