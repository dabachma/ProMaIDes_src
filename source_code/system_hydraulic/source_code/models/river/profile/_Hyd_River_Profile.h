#pragma once
/**\class _Hyd_River_Profile
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/
#ifndef _HYD_RIVER_PROFILE_H
#define _HYD_RIVER_PROFILE_H

//system_sys_ classes
#include "_Sys_Common_System.h"
//hydsystem classes
#include "Hyd_River_Profile_Type_Standard.h"
#include "Hyd_River_Profile_Type_Weir.h"
#include "Hyd_River_Profile_Type_Bridge.h"
#include "Hyd_Instationary_Boundary.h"
#include "Hyd_Tables.h"
#include "_Hyd_Parse_IO.h"
//info class about the hydraulic boundary szenario data
#include "Hyd_Boundary_Szenario.h"
#include "_Hyd_Model.h"

//enum
///Enumerater to specify the type of the river profile \ingroup hyd
enum _hyd_profile_types{
	///Type for normal river profile
	RIVER_TYPE,
	///Type for weir profile
	WEIR_TYPE,
	///Type for bridge profile
	BRIDGE_TYPE
};
///Enumerator to specify the connection type of each river profile \ingroup hyd
enum _hyd_connection_types{
	///Standard connection type for the inbetween profiles
	STANDARD_CONN,
	///Inflow connection type for the inflow (first) profile (just one possible per river model)
	INFLOW_CONN,
	///Outflow connection type for the outflow (last) profile (just one possible per river model)
	OUTFLOW_CONN
};

//struct
///Structure for the break-parameters of the floodprotection line along the river \ingroup hyd
struct _hyd_break_parameters{
	///Resistance of the line (dikes:=critical velocity; walls/mobile walls:=resistance factor)
	double resistance;
	//Flag if the construction fails abrupt or continually
	bool abrupt_fails_flag;
	///Abrupt breach opening (dikes:=not used; walls/mobile walls:= the abrupt breach opening if the resistance is exceeded)
	double abrupt_opening;
};
///Structure for an idealisation of the river profile(s) by the fpl-section in case of a combined calculation of risk for the HYD- and the FPL-system \ingroup hyd
struct _hyd_idealisation_parameters{
	///Name of the fpl-section for information reason
	string name;
	///Index of the fpl-section for information reason
	int index;
	///Break-parameters of the FPL-section
	_hyd_break_parameters break_params;
	///Structure-height above base-point of the FPL-section
	double struct_height;
};
///Base class for all river profiles. The river-model consists of these river profiles. Each river profile has a specific profile type \ingroup hyd
/**
The base class for all river profiles. There are three possible forms of river profiles:
	- inflow: the first river profile of the river model in flow direction (see Hyd_River_Profile_Connection_Inflow)
	- standard: the inbetween profiles of the river-model are standard forms (see Hyd_River_Profile_Connection_Standard)
	- outflow: the last river profile of the river model in flow direction (see Hyd_River_Profile_Connection_Outflow)

Every river profile has an specific profile type.
\see _Hyd_River_Profile_Type
*/
class _Hyd_River_Profile : public _Sys_Common_System
{
//class for displaying the data
friend class HydGui_Rv_Profile_General_Wid;

public:
	///Default constructor
	_Hyd_River_Profile(void);
	///Default destructor
	virtual ~_Hyd_River_Profile(void);

	//members

	///Pointer to the profile type
	_Hyd_River_Profile_Type *typ_of_profile;
	///Pointer to the table for the results of an hydraulic simulation for the river elements
	/**This pointer is allocated with set_erg_table(QSqlDatabase *ptr_database) and deleted with close_erg_table(void) */
	static Tables *erg_table;
	///Pointer to the table for the instationary results of an hydraulic simulation for the river elements
	/**This pointer is allocated with set_erg_table(QSqlDatabase *ptr_database) and deleted with close_erg_table(void) */
	static Tables *erg_instat_table;
	///Pointer to the table for the profile data in a database
	/**This pointer is allocated with set_profile_table(QSqlDatabase *ptr_database) and deleted with close_erg_table(void) */
	static Tables *profile_table;

	///Member for storing the hydraulic boundary szenario information
	Hyd_Boundary_Szenario hyd_sz;

	//methods

	///Input the members of the profile per file
	void input_members(QFile *profile_file, const int profile_number, string my_line, int *line_counter);
	///Transfer the river profile data to a database
	virtual void transfer_profile_members2database(const int glob_prof_id, QSqlQuery *model, QSqlDatabase *ptr_database, const int rv_number)=0;
	///Transfer the river profile boundary data of an hydraulc boundary scenario to a database
	void transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database, const int rv_number);
	///Input the river profile data with the index from database selection
	virtual void input_members_per_database(const int index, const QSqlQueryModel *query_result, QSqlDatabase *ptr_database)=0;

	///Select and count the relevant river profiles in the database table and return the number
	static int select_relevant_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number, const bool with_output=true);
	///Select and count the relevant river profiles in the database table and return the number just for the given system_id
	static int select_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id);
	///Select and count the relevant river profiles in the database table and return the number just for the given system_id and river number
	static int select_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number);

	///Select the relevant river profiles in the database table for a given connection type
	static int select_relevant_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number, _hyd_connection_types conn_type, const bool with_output=true );
	///Select and count the relevant river profiles in the database table with a given global profile number
	static int select_relevant_profile_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number, const int glob_prof_no);

	///Get the maximum discharge of the outflow profile
	static double get_maximum_outflow_discharge(QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number,const int bound_sz, const string break_sz);

	///Set the database table for the profile data of the river model: it sets the table name and the name of the columns and allocate them
	static void set_profile_table(QSqlDatabase *ptr_database);
	///Set the database table for the boundary profile data: it sets the table name and the name of the columns and allocate them
	static void set_profile_boundary_table(QSqlDatabase *ptr_database);
	///Set the database table for the wallbreak profile data: it sets the table name and the name of the columns and allocate them
	static void set_profile_wallbreak_table(QSqlDatabase *ptr_database);
	///Set the database table for the dikebreak profile data: it sets the table name and the name of the columns and allocate them
	static void set_profile_dikebreak_table(QSqlDatabase *ptr_database);
	///Set the database table for the bridge profile data: it sets the table name and the name of the columns and allocate them
	static void set_profile_bridge_table(QSqlDatabase *ptr_database);

	///Create the database table for the profile data of the river model
	static void create_profile_table(QSqlDatabase *ptr_database);
	///Get the maximum value of the global index of the profiles in database table (static)
	static int get_max_glob_id_prof_table(QSqlDatabase *ptr_database);
	///Create the database table for the boundary profile data
	static void create_profile_boundary_table(QSqlDatabase *ptr_database);
	///Create the database view for the connection of boundary ids and theriver profiles (static)
	static void create_bound2profile_view(QSqlDatabase *ptr_database);
	///Check if the view exists already 
	static bool check_bound2profile_view_exists(QSqlDatabase *ptr_database);

	///Create the database table for the wallbreak profile data
	static void create_profile_wallbreak_table(QSqlDatabase *ptr_database);
	///Create the database table for the dikebreak profile data
	static void create_profile_dikebreak_table(QSqlDatabase *ptr_database);
	///Create the database table for the bridge profile data
	static void create_profile_bridge_table(QSqlDatabase *ptr_database);

	///Create the database table for the maximum results of an hydraulic simulation for the river profiles
	static void create_erg_table(QSqlDatabase *ptr_database);
	///Set the database table for the maximum results of an hydraulic simulation for the river profiles: it sets the table name and the name of the columns and allocate them
	static void set_erg_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Close and delete the database table for the results of an hydraulic simulation the river profiles
	static void close_erg_table(void);

	///Create the database table for the instationary results of an hydraulic simulation for the river profiles
	static void create_erg_instat_table(QSqlDatabase *ptr_database);
	///Set the database table for the instationary results of an hydraulic simulation for the river profiles: it sets the table name and the name of the columns and allocate them
	static void set_erg_instat_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Close and delete the database table for the instationary results of an hydraulic simulation the river profiles
	static void close_erg_instat_table(void);


	///Delete the results data in the maximum result database table for a given boundary scenario
	static void delete_results_by_scenario(QSqlDatabase *ptr_database, const int sc_id);
	///Delete the results data in the maximum result database table for a given system state
	static void delete_results_by_system_state(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete the results data in the maximum result database table for specific parameters
	static void delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag=false);
	///Delete the results data in the maximum result database table for specific parameters
	static void delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz,const string break_sz, const bool like_flag=false);
	
	///Delete the instationary results data in the maximum result database table for a given boundary scenario
	static void delete_instat_results_by_scenario(QSqlDatabase *ptr_database, const int sc_id);
	///Delete the instationary results data in the maximum result database table for a given system state
	static void delete_instat_results_by_system_state(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete the instationary results data in the maximum result database table for specific parameters
	static void delete_instat_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag = false);
	///Delete the instationaryresults data in the maximum result database table for specific parameters
	static void delete_instat_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag = false);
	
	///Delete the profiles and appending data by a system id
	static void delete_profile_in_table(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Delete the boundary condition data in the database table for a given boundary scenario
	static void delete_boundary_cond_by_scenario(QSqlDatabase *ptr_database, const int sc_id);
	///Delete the boundary condition data in the database table for a given system state
	static void delete_boundary_cond_by_system_state(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Delete the boundary condition data in the database table for a given global profile id
	static void delete_boundary_cond_by_rv_profile(QSqlDatabase *ptr_database, const int id);
	///Delete the wallbreak data in the database table for a given global profile id
	static void delete_wall_break_by_rv_profile(QSqlDatabase *ptr_database, const int id);
	///Delete the dikebreak data in the database table for a given global profile id
	static void delete_dike_break_by_rv_profile(QSqlDatabase *ptr_database, const int id);
	///Delete the bridge data in the database table for a given global profile id
	static void delete_bridge_by_rv_profile(QSqlDatabase *ptr_database, const int id);



	///Select the data in the database table for the maximum results of the river profiles specified by the system state and the scenario-ids
	static int select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag=false);
	///Select the data in the database table for the maximum results of the river profiles specified by the system state, the scenario-ids, the river number and a specific profile
	static int select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int rv_no, const int prof_no);

	///Select the data in the database table for the instationary results of the river profiles specified by the system state and the scenario-ids
	static int select_data_in_instat_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag = false);
	///Select the data in the database table for the instationary results of the river profiles specified by the system state and the scenario-ids
	static int select_data_in_instat_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int rv_no, const bool like_flag = false);


	///Delete all data in the database of all tables for the river profile data
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database of all tables for the river profile data
	static void close_table(void);

	///Switch the applied-flag for the 1-d river profiles in the database table for a defined system state
	static void switch_applied_flag_profile_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the 1-d river profiles in the database table for a defined system state and river number
	static void switch_applied_flag_profile_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_no, const bool flag);
	///Switch the applied-flag for the 1-d results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the 1-d results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);

	///Switch the applied-flag for the 1-d instationary results in the database table for a defined system state
	static void switch_applied_flag_erg_instat_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the 1-d instationbary results in the database table for a defined system state
	static void switch_applied_flag_erg_instat_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);

	///Switch the applied-flag for the boundary condition in the database table for a defined system state
	static void switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the boundary condition in the database table for a defined system state
	static void switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);

	///Copy the results of a given system id to another one in database table
	static void copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);

	///Copy the instationary results of a given system id to another one in database table
	static void copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);

	///Copy the boundary condition from the source global profile id to the target
	static void copy_boundary_condition(QSqlDatabase *ptr_database, const _sys_system_id base, const int src, const int targ);

	///Initialize the profile like calculation of the tables of the values h, a, c etc.
	void init_profile(Hyd_Param_Material *material_table, const _hyd_profile_calc_setting user_setting);

	///Set the initial condition to the profiles
	virtual void set_init_condition(void);

	///Get the Init-condition as an area from the class _Hyd_River_Profile_Type
	double get_init_condition_value(void);

	///Clone river profile
	void clone_profile(_Hyd_River_Profile *profile);

	///Get the connection type of the profile
	_hyd_connection_types get_connection_type(void);
	///Get the profile type of the profile (river, weir etc.)
	_hyd_profile_types get_profile_type(void);
	///Get the river_station of the profile
	double get_river_station(void);
	///Get the distance to the upstream profile (distance2upstream)
	double get_distance2upstream(void);
	///Set the distance to the upstream profile (distance2upstream)
	virtual void set_distance2upstream(const double riverstation_upstream);
	///Get the distance to the downstream profile (distance2downstream)
	double get_distance2downstream(void);
	///Set the distance to the downstream profile (distance2downstream)
	void set_distance2downstream(const double distance);
	///Get the profile number
	int get_profile_number(void);
	///Get the profile name
	string get_profile_name(void);

	///Get the width of the river
	double get_river_width(void);

	///Get if an overflow over the left river bank into the floodplain is given
	virtual bool get_overflow_flag_left(void);
	///Get the poleni-factor for an overflow over the left river bank into the floodplain
	virtual double get_overflow_poleni_left(void);
	///Get if an overflow over the right river bank into the floodplain is given
	virtual bool get_overflow_flag_right(void);
	///Get the poleni-factor for an overflow over the right river bank into the floodplain
	virtual double get_overflow_poleni_right(void);

	///Get the flag if a break of the left floodprotection line is possible
	bool get_break_flag_left(void);
	///Get the index of the left basepoint
	int get_left_basepoint_index(void);
	///Get a pointer to the left basepoint
	Hyd_River_Profile_Point* get_left_basepoint(void);
	///Get the flag if a break of the right floodprotection line is possible
	bool get_break_flag_right(void);
	///Get the index of the right basepoint
	int get_right_basepoint_index(void);
	///Get a pointer to the right basepoint
	Hyd_River_Profile_Point* get_right_basepoint(void);

	///Get the data structure for the break-parameters of the left bank
	_hyd_break_parameters* get_break_parameter_left(void);
	///Get the data structure for the break-parameters of the right bank
	_hyd_break_parameters* get_break_parameter_right(void);

	///Get the absolute height related to the left base point
	double get_height_left_bank_abs(void);
	///Get the absolute height related to the right base point
	double get_height_right_bank_abs(void);

	///Get the actual global waterlevel (s_value)
	double get_actual_global_waterlevel(void);

	///Get the actual discharge through this profile
	virtual double get_actual_river_discharge(void)=0;

	///Get the watervolume of the init-conditions in the river segment up of the profiles
	double get_watervolume_init(void);
	///Get the watervolume in the river segment up of the profiles
	virtual double get_watervolume_element(void);

	///Make the syncronistation of the elements to a given time point
	virtual void make_syncronisation(const double time_point, _Hyd_River_Profile *downstream)=0;

	///Calculate the values for a hydrological balance of the corresponding river segment (upstream) for each syncronisation step
	virtual void calculate_hydrological_balance(const double time_point);

	///Reset all coupling discharges; use it before syncronization
	virtual void reset_coupling_discharge(void);

	///Add a discharge value to the coupling discharge over the left bank [m³/(s)] in flow direction (coupling RV2FP)
	void add_coupling_discharge_left_bank(const double value);
	///Add a discharge value to the coupling discharge over the right bank [m³/(s)] in flow direction (coupling RV2FP)
	void add_coupling_discharge_right_bank(const double value);
	///Reset the discharge value to the coupling discharge over the left bank [m³/(s)] in flow direction (coupling RV2FP)
	void reset_coupling_discharge_left_bank(void);
	///Reset the discharge value to the coupling discharge over the right bank [m³/(s)] in flow direction (coupling RV2FP)
	void reset_coupling_discharge_right_bank(void);
	///Get the discharge value due to the coupling discharge over the left bank in flow direction (coupling RV2FP)
	double get_coupling_discharge_left_bank(void);
	///Get the discharge value due to the coupling discharge over the right bank in flow direction (coupling RV2FP)
	double get_coupling_discharge_right_bank(void);
	///Add a discharge value to the 1d-coupling discharge (coupling RV2RV)
	void add_coupling_discharge_1d(const double value);
	///Reset the discharge value due to the 1d-coupling discharge (coupling RV2RV)
	void reset_coupling_discharge_1d(void);
	///Get the discharge value due to the 1-coupling discharge (coupling RV2RV)
	double get_coupling_discharge_1d(void);
	///Add a discharge value to the coupling discharge through a hydraulic structure (coupling RV2FP via structure)
	void add_structure_coupling_discharge(const double value);
	///Reset a discharge value to the coupling discharge through a hydraulic structure (coupling RV2FP via structure)
	void reset_structure_coupling_discharge(void);
	///Get a discharge value due to the coupling discharge through a hydraulic structure (coupling RV2FP via structure)
	double get_structure_coupling_discharge(void);
	///Add a discharge value to the coupling discharge due to a dikebreak at the left bank (coupling RV2FP via dikebreak)
	void add_dikebreak_coupling_discharge_left_bank(const double value);
	///Reset a discharge value to the coupling discharge due to a dikebreak at the left bank (coupling RV2FP via dikebreak)
	void reset_dikebreak_coupling_discharge_left_bank(void);
	///Get a value of the coupling discharge due to a dikebreak at the left bank (coupling RV2FP via dikebreak)
	double get_dikebreak_coupling_discharge_left_bank(void);
	///Add a discharge value to the coupling discharge due to a dikebreak at the right bank (coupling RV2FP via dikebreak)
	void add_dikebreak_coupling_discharge_right_bank(const double value);
	///Reset a discharge value to the coupling discharge due to a dikebreak at the right bank (coupling RV2FP via dikebreak)
	void reset_dikebreak_coupling_discharge_right_bank(void);
	///Get a value of the coupling discharge due to a dikebreak at the right bank (coupling RV2FP via dikebreak)
	double get_dikebreak_coupling_discharge_right_bank(void);

	///Add the hydrological balance value of the coupling condition discharges for the left bank overflow to the given pointer
	void add_hydro_balance_leftbank_coupling(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling condition discharges for the right bank overflow to the given pointer
	void add_hydro_balance_rightbank_coupling(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling condition discharges to other river-models to the given pointer
	void add_hydro_balance_coupling_1d(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling condition discharges to floodplain models via a hydraulic structure to the given pointer
	void add_hydro_balance_structure_coupling(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling condition discharges to floodplain models via a dikebreak at the left bank to the given pointer
	void add_hydro_balance_dikebreak_left_coupling(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling condition discharges to floodplain models via a dikebreak at the right bank to the given pointer
	void add_hydro_balance_dikebreak_right_coupling(_hyd_hydrological_balance *given);

	///Output the members
	virtual void output_members(void)=0;
	///Output the setted and calculated members (e.g. tables, boundary conditions etc.)
	virtual void output_setted_members(void)=0;

	///Output the header for the output of the result members to console/display (normal use for development)
	static void output_result_members_per_timestep_header(void);
	///Output the result members to console/display (normal use for development)
	void output_result_members_per_timestep(ostringstream *cout);

	///Output the result members for each riverprofiletype at every timestep to file
	void output_result_members_per_timestep(ofstream *file);
	///Output the result members for each riverprofiletype at every timestep to csv file
	void output_result_members_per_timestep2csv(ofstream *file);

	///Output the header for the maximum result output to console/display
	static void output_header_max_results(void);
	///Output the maximum result output to console/display
	void output_max_results(void);
	///Output the maximum result output to a given tecplot file
	void output_max_results2file(ofstream *file);
	///Output the maximum result output to a csv file
	void output_max_results2csvfile(ofstream *file);
	///Output the maximum calculated results to the database table (erg_table)
	void output_max_results(QSqlDatabase *ptr_database, const int rv_no, const string polygon_string, int *glob_id, const string break_sc);
	///Output the instationary calculated results to the database table (erg_table)
	void output_instat_results(QSqlDatabase *ptr_database, const int rv_no, const string polygon_string, int *glob_id, const string break_sc, const string time);


	///Check the profiles
	void check_profiles(void);

	///Get the stabilization discharge
	virtual double get_stabilization_discharge(void);

	///Calculate the stabilization discharge
	virtual void calculate_stabilization_discharge(void);

	///Reset the hydrological balance value and the maximum values
	virtual void reset_hydrobalance_maxvalues(void);

	///Set the pointer to the upstream and downstream profiles
	void set_ptr_downstream_upstream_prof(_Hyd_River_Profile *down, _Hyd_River_Profile *up);
	///Get the pointer to the downstream profile
	_Hyd_River_Profile *get_ptr_downstream_prof(void);
	///Get the pointer to the upstream profile
	_Hyd_River_Profile *get_ptr_upstream_prof(void);

	///Idealise the profile with given parameters of a fpl-section (Fpl_Section)
	void idealise_profile_fpl(const bool left_flag, _hyd_idealisation_parameters ideal_param, const _hyd_connection_types fpl_section_connection, const int section_id);

	///Set the waterlevel difference between river profile and connected floodplain element for a break and calculate the maximum one
	void set_delta_waterlevel_rv2fp2break(const double waterlevel, const double delta_h, const bool left_flag);

	///Convert string to enum _hyd_profile_types
	static _hyd_profile_types convert_string2profiletype(const string str_profiletype);
	///Convert a enum _hyd_profile_types to a string
	static string convert_profiletype2string(const _hyd_profile_types profiletype);
	///Convert string to enum _hyd_connection_types
	static _hyd_connection_types convert_string2connectiontype(const string str_connectiontype);
	///Convert enum _hyd_connection_types to string
	static string convert_connectiontype2string(const _hyd_connection_types type);

	///Get the fpl-section id of the left bank
	int get_fpl_section_id_left(void);
	///Get the fpl-section id of the right bank
	int get_fpl_section_id_right(void);

	///Get the global profile id in database
	int get_prof_glob_id(void);

	///Append a string to the profile name
	void append_str2name(const string append);

	///Get q-value
	virtual double get_Q(void) = 0;

	///Get boundary point condition is applied
	virtual bool boundary_point_is_applied(void);

	///Get boundary point value
	virtual double get_boundary_point_value(const double time);

protected:
	//members

	///Pointer to the table for the boundary data in a database
	/**This pointer is allocated with set_profile_boundary_table(QSqlDatabase *ptr_database) and deleted with close_erg_table(void) */
	static Tables *boundary_table;
	///Pointer to the table for the wall break data in a database
	/**This pointer is allocated with set_profile_wallbreak_table(QSqlDatabase *ptr_database) and deleted with close_erg_table(void) */
	static Tables *wallbreak_table;
	///Pointer to the table for the dike break data in a database
	/**This pointer is allocated with set_profile_dikebreak_table(QSqlDatabase *ptr_database) and deleted with close_erg_table(void) */
	static Tables *dikebreak_table;
	///Pointer to the table for the bridge data in a database
	/**This pointer is allocated with set_profile_bridge_table(QSqlDatabase *ptr_database) and deleted with close_erg_table(void) */
	static Tables *bridge_table;

	///Name of profile
	string name;
	///Number of profile
	int profile_number;
	///Global profile id in database
	int glob_prof_id;
	///Connection type of profile
	_hyd_connection_types connection;

	///Stationing of the profile for distance calculation
	double river_station;
	///Distances for the table dicretization
	double delta_x_table;
	///Specifies the profile type
	_hyd_profile_types type;
	///Initial waterlevel relative to the bottom of the profile
	double init_condition;
	///Distance to the upstream profile
	double distance2upstream;
	///Distance to the downstrem profile
	double distance2downstream;
	///River width
	double river_width;

	///Pointer to the upstream profile
	_Hyd_River_Profile *ptr_upstream_prof;
	///Pointer to the downstream profile
	_Hyd_River_Profile *ptr_downstream_prof;

	//members for break-coupling
	///Pointer to the parameters of a break development at the left bank; it is not required for an outflow profile
	_hyd_break_parameters *left_break_params;
	///Pointer to the parameters of a break development at the right bank; it is not required for an outflow profile
	_hyd_break_parameters *right_break_params;

	///Flag if a break (e.g. dikebreak) of the river section at the left bank is possible; it is set to true when a base point left is set
	bool break_flag_left;
	///Number of the left base point for a break (e.g. dikebreak) of the river section at the left bank
	int index_basepoint_left;
	///Flag if a break (e.g. dikebreak) of the river section at the right bank is possible; it is set to true when a base point right is set
	bool break_flag_right;
	///Number of the left base point for a break (e.g. dikebreak) of the river section at the right bank
	int index_basepoint_right;
	///Maximum waterlevel above the right base point; the time point is corresponding to the time point of the maximum waterlevel of the profile (_Hyd_River_Profile_Type)
	double h_max_right_base;
	///Maximum waterlevel above the left base point; the time point is corresponding to the time point of the maximum waterlevel of the profile (_Hyd_River_Profile_Type)
	double h_max_left_base;

	///Section Id of a FPL-section of the left bank
	int fpl_sec_id_left;
	///Section Id of a FPL-section of the right bank
	int fpl_sec_id_right;

	///Flag if the left river bank is connected to a fpl-section (Fpl_Section)
	bool left_bank2fpl;
	///Flag if the right river bank is connected to a fpl-section (Fpl_Section)
	bool right_bank2fpl;
	///Height of the left river bank [m]
	double height_left_bank;
	///Height of the right river bank
	double height_right_bank;

	///Flag for a coupling via overflow over the left bank in flow direction (coupling RV2FP)
	bool overflow_left_flag;
	///Poleni factor for a coupling via overflow over the left bank in flow direction (coupling RV2FP)
	double poleni_factor_left_overflow;
	///Flag for a coupling via overflow over the right bank in flow direction (coupling RV2FP)
	bool overflow_right_flag;
	///Poleni factor for a coupling via overflow over the right bank in flow direction (coupling RV2FP)
	double poleni_factor_right_overflow;

	///They are read in (per file/database table) in this class -if the profile type is a bridge type- and transferred to the Hyd_River_Profile_Type_Bridge
	_hyd_bridge_values *bridge_specific_value;

	//members for a hydrological balance
	///The watervolume of the init-conditions in the river segment up of the profiles
	double watervolume_init;
	///The watervolume in the river segment up of the profiles
	double watervolume;
	///Defines the point in time of the last time syncronisation step (for a hydrological balance calculation)
	double old_time_point;
	///Defines the time between two syncronisation time steps;
	double delta_time;

	///The maximum value of the discharge through this profile
	_hyd_max_values q_value_max;

	///Actual discharge due to overflow to the floodplain over the left bank [m³/(s)] in flow direction (coupling RV2FP)
	double q_left_bank;
	///Actual discharge due to overflow to the floodplain over the right bank [m³/(s)] in flow direction (coupling RV2FP)
	double q_right_bank;
	///Actual discharge due to a coupling to another river model 1-coupling
	double q_1d_coupling;
	///Actual discharge due to a coupling to floodplain model via a hydraulic structure
	double q_structure_coupling;
	///Actual discharge due to a coupling to floodplain model via a dikebreak at the left bank
	double q_dikebreak_coupling_left;
	///Actual discharge due to a coupling to floodplain model via a dikebreak at the right bank
	double q_dikebreak_coupling_right;

	///Defines the values for a hydrological balance for the lateral boundary condition
	_hyd_hydrological_balance lateral_boundary_volume;
	///Defines the values for a hydrological balance for the pointwise boundary condition
	_hyd_hydrological_balance point_boundary_volume;
	///Defines the values for a hydrological balance for the overflow volume over the left bank (in flow direction)
	_hyd_hydrological_balance left_bank_overflow_volume;
	///Defines the values for a hydrological balance for the overflow volume over the right bank (in flow direction)
	_hyd_hydrological_balance right_bank_overflow_volume;
	///Defines the values for a hydrological balance for the coupling to other river models
	_hyd_hydrological_balance coupling_1d_volume;
	///Defines the values for a hydrological balance for the coupling to a floodplain model via a hydraulic structure
	_hyd_hydrological_balance structure_coupling_volume;
	///Defines the values for a hydrological balance for the coupling to a floodplain model via a dikebreak at the left bank
	_hyd_hydrological_balance left_dikebreak_coupling_volume;
	///Defines the values for a hydrological balance for the coupling to a floodplain model via a dikebreak at the right bank
	_hyd_hydrological_balance right_dikebreak_coupling_volume;

	///Number of members which have to be found in the file
	int must_found_number;

	///Maximum waterlevel at the maximum waterlevel difference between river and floodplain; it is set by the coupling points (left bank)
	double max_h_2break_left;
	///Maximum difference between river and floodplain related to the breakheight; it is set by the coupling points (left bank)
	double max_delta_h_2break_left;
	///Maximum waterlevel at the maximum waterlevel difference between river and floodplain; it is set by the coupling points (right bank)
	double max_h_2break_right;
	///Maximum difference between river and floodplain related to the breakheight; it is set by the coupling points (right bank)
	double max_delta_h_2break_right;

	//methods

	///Allocate the data structure for the break-parameters of the left bank; it is not required for an outflow profile
	void allocate_break_parameter_left(void);
	///Delete the break-parameters of the left bank; it is not required for an outflow profile
	void delete_break_parameter_left(void);
	///Allocate the data structure for the break-parameters of the right bank; it is not required for an outflow profile
	void allocate_break_parameter_right(void);
	///Delete the break-parameters of the right bank; it is not required for an outflow profile
	void delete_break_parameter_right(void);

	///Allocate the bridge specific data for input
	void allocate_bridge_data(void);
	///Decide and allocate the profile type
	void decide_alloc_profile_type(const int number_points);
	///Delete the profile type
	void delete_profile_type(void);
	///Extract the keyvalues from a given stream
	void get_keyvalues_file(stringstream *stream, string buffer);
	///Decide which key value is read and give the read-in value to the corresponding member
	virtual void decide_keyvalues_file(const string key, string buffer, int *found_counter);

	///Input the river profile point data from a database table
	void input_profile_points_per_database(QSqlDatabase *ptr_database, const int glob_prof_id);

	///Check the connection profile type
	virtual void check_connection_type_profile(void)=0;

	///Calculate the watervolume in the element
	virtual void calculate_watervolume_element(void);

	///Calculate the maximum waterlevel above the left/right basepoints of a profile
	void calculate_max_h2basepoint(void);

	///Transfer the river profile boundary data to a database
	virtual void transfer_boundary_data2database(QSqlDatabase *ptr_database, const int glob_prof_id)=0;

	///Check the given basepoints and set the height of the river banks
	void check_set_base_points(void);

	///Get boundary condition is applied
	virtual bool boundary_is_applied(void)=0;




private:
	//methods

	///Delete the bridge specific data after transferring it to the profile type
	void delete_bridge_data(void);
	///Calculate the river width
	void calculate_river_width(void);

	///Generate string for output max-results to database
	void generate_max_str2database(ostringstream *query_string, const int rv_no, const string polygon_string, int *glob_id, const string break_sc);

	///Delete all data in the database table for the river profile data
	static void delete_data_in_profile_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the river profile data
	static void close_profile_table(void);
	///Delete all data in the database table for the boundary profile data
	static void delete_data_in_boundary_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the boundary profile data
	static void close_boundary_table(void);
	///Delete all data in the database table for the wallbreak profile data
	static void delete_data_in_wallbreak_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the wallbreak profile data
	static void close_wallbreak_table(void);
	///Delete all data in the database table for the dikebreak profile data
	static void delete_data_in_dikebreak_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the dikebreak profile data
	static void close_dikebreak_table(void);
	///Delete all data in the database table for the bridge profile data
	static void delete_data_in_bridge_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the bridge profile data
	static void close_bridge_table(void);
	///Delete all data in the database table for the results of an hydraulic simulation the river profiles
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database);


	///Delete all data in the database table for the instationary results of an hydraulic simulation the river profiles
	static void delete_data_in_erg_instat_table(QSqlDatabase *ptr_database);


	///Delete the maximum result row in a database table for this profile
	void delete_max_result_row_in_table(QSqlDatabase *ptr_database, const int rv_no, const string break_sc);


	///Set error(s)
	Error set_error(const int err_type);
	///Set warnings
	Warning set_warning(const int warn_type);
};
#endif