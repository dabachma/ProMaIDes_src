#pragma once
/**\class HydTemp_Profile
	\author Daniel Bachmann et al.
	\author produced by the AG-FRM University of applied sciences Magdeburg-Stendal
	\version 0.0.1                                                               
*/
#ifndef HYDTEMP_PROFILE_H
#define HYDTEMP_PROFILE_H



//system_sys_ classes
#include "_Sys_Common_System.h"
//hydsystem classes
#include "Hyd_Instationary_Boundary.h"
#include "_Hyd_Parse_IO.h"
//info class about the hydraulic boundary szenario data
#include "Hyd_Boundary_Szenario.h"
#include "_Hyd_Model.h"


///Data structure for the tmperature data of the river profile \ingroup hyd
struct _hyd_temp_data{
	//Flag if it is applied
	bool applied_flag;
	///flag if stationary (true) or instationary (false)
	bool stat_flag;
	///Value or curve index
	double index_value;
	///Pointer to the curve
	Hyd_Instationary_Boundary *ptr_curve;
	//Current value
	double current_value;

};


///Class for the temperature data of the river profiles. The river-model and the temperature model consists of these river profiles.  \ingroup hyd
/**


*/
class HydTemp_Profile : public _Sys_Common_System
{
//class for displaying the data
//friend class HydGui_Rv_Profile_General_Wid;
friend class HydTemp_Model;

public:
	///Default constructor
	HydTemp_Profile(void);
	///Default destructor
	 ~HydTemp_Profile(void);

	//members
	
	

	///Pointer to the table for the results of an hydraulic simulation for the river elements
	/**This pointer is allocated with set_erg_table(QSqlDatabase *ptr_database) and deleted with close_erg_table(void) */
	static Tables *erg_table;
	///Pointer to the table for the instationary results of an hydraulic simulation for the river elements
	/**This pointer is allocated with set_erg_table(QSqlDatabase *ptr_database) and deleted with close_erg_table(void) */
	static Tables *erg_instat_table;
	///Pointer to the table for the profile data in a database
	/**This pointer is allocated with set_profile_table(QSqlDatabase *ptr_database) and deleted with close_erg_table(void) */
	static Tables *profile_table;
	/**This pointer is allocated with set_profile_boundary_table(QSqlDatabase *ptr_database) and deleted with close_erg_table(void) */
	static Tables *boundary_table;

	///Member for storing the hydraulic boundary szenario information
	Hyd_Boundary_Szenario hyd_sz;

	///Boundary curve for the water depth
	Hyd_Instationary_Boundary bound_water_depth;
	///Boundary curve for the discharge
	Hyd_Instationary_Boundary bound_discharge;
	///Boundary curve for the velocity
	Hyd_Instationary_Boundary bound_velocity;

	//methods

	///Input the members of the profile per file
	void input_members(QFile *profile_file, const int profile_number, string my_line, int *line_counter);
	///Transfer the river profile temperature  data to a database
	void transfer_profile_members2database(const int glob_prof_id, QSqlQuery *model, QSqlDatabase *ptr_database, const int rv_number);
	///Transfer the river profile temperature boundary data of an hydraulc boundary scenario to a database
	void transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database, const int rv_number);
	///Input the river profile temperature data with the index from database selection
	void input_members_per_database(const int index, const QSqlQueryModel *query_result, QSqlDatabase *ptr_database);

	///Select and count the relevant temperature profiles in the database table and return the number
	static int select_relevant_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number, const bool with_output=true);
	///Select and count the relevant temperature  profiles in the database table and return the number just for the given system_id
	static int select_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id);
	///Select and count the relevant temperature  profiles in the database table and return the number just for the given system_id and river number
	static int select_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number);

	///Select the relevant temperature  profiles in the database table for a given connection type
	static int select_relevant_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number, _hyd_connection_types conn_type, const bool with_output=true );
	///Select and count the relevant temperature  profiles in the database table with a given global profile number
	static int select_relevant_profile_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number, const int glob_prof_no);

	///Set the database table for the temperature profile data of the river model: it sets the table name and the name of the columns and allocate them
	static void set_profile_table(QSqlDatabase *ptr_database);
	///Set the database table for the temperature boundary profile data: it sets the table name and the name of the columns and allocate them
	static void set_profile_boundary_table(QSqlDatabase *ptr_database);


	///Create the database table for the temperature profile data of the river model
	static void create_profile_table(QSqlDatabase *ptr_database);
	///Get the maximum value of the global index of the temperature profiles in database table (static)
	static int get_max_glob_id_prof_table(QSqlDatabase *ptr_database);
	///Create the database table for the temperature boundary profile data
	static void create_profile_boundary_table(QSqlDatabase *ptr_database);
	///Create the database view for the connection of temperature boundary ids and the river profiles (static)
	static void create_bound2profile_view(QSqlDatabase *ptr_database);
	///Check if the view exists already 
	static bool check_bound2profile_view_exists(QSqlDatabase *ptr_database);



	///Create the database table for the maximum results of an temperature  simulation for the river profiles
	static void create_erg_table(QSqlDatabase *ptr_database);
	///Set the database table for the maximum results of an temperature  simulation for the river profiles: it sets the table name and the name of the columns and allocate them
	static void set_erg_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Close and delete the database table for the results of an temperature  simulation the river profiles
	static void close_erg_table(void);

	///Create the database table for the instationary results of an temperature  simulation for the river profiles
	static void create_erg_instat_table(QSqlDatabase *ptr_database);
	///Set the database table for the instationary results of an temperature  simulation for the river profiles: it sets the table name and the name of the columns and allocate them
	static void set_erg_instat_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Close and delete the database table for the instationary results of an temperature  simulation the river profiles
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
	static void delete_boundary_cond_by_profile(QSqlDatabase *ptr_database, const int id);


	///Select the data in the database table for the maximum results of the river profiles specified by the system state and the scenario-ids
	static int select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag=false);
	///Select the data in the database table for the maximum results of the river profiles specified by the system state, the scenario-ids, the river number and a specific profile
	static int select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int rv_no, const int prof_no);

	///Select the data in the database table for the instationary results of the river profiles specified by the system state and the scenario-ids
	static int select_data_in_instat_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag = false);
	

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




	///Get the Init-condition as an area from the class HydTemp_Profile_Type
	double get_init_condition_value(void);
	///Set the init-condition to the current value
	void set_init_condition(void);

	///Clone river profile
	void clone_profile(HydTemp_Profile *profile);


	///Get the profile number
	int get_profile_number(void);
	///Get the profile name
	string get_profile_name(void);

	///Get the actual water temeperature 
	double get_actual_temperature(void);
	//Set the actuel temperature from solver
	void set_actuel_temp_from_solver(const double temp);
	///Get if the water temperature boundary is applied 
	bool get_water_temperature_applied(void);
	///Get if the inlet temperature boundary is applied 
	bool get_inlet_temperature_applied(void);



	///Make the syncronistation of the elements to a given time point
	void make_syncronisation(const int index, const double time_point, HydTemp_Param *params, HydTemp_Profile *upstream, HydTemp_Profile *downstream, Hyd_River_Profile_Connection_Standard *rv_profile);

	///Calculate the values for a temperature balance of the corresponding river segment (upstream) for each syncronisation step
	void calculate_temp_balance(const double time_point);

	///Reset all coupling discharges; use it before syncronization
	//void reset_coupling_discharge(void);


	///Output the members
	void output_members(void);
	///Output the setted and calculated members (e.g. tables, boundary conditions etc.)
	void output_setted_members(void);

	///Output the header for the output of the result members to console/display (normal use for development)
	static void output_result_members_per_timestep_header(void);
	///Output the result members to console/display (normal use for development)
	void output_result_members_per_timestep(ostringstream *cout, const double rv_station);

	///Output the result members for each riverprofiletype at every timestep to file
	void output_result_members_per_timestep(ofstream *file, const double rv_station);
	///Output the result members for each riverprofiletype at every timestep to csv file
	void output_result_members_per_timestep2csv(ofstream *file, const double rv_station, const double z_min);

	///Output the header for the maximum result output to console/display
	static void output_header_max_results(void);
	///Output the maximum result output to console/display
	void output_max_results(const double rv_station);
	///Output the maximum result output to a given tecplot file
	void output_max_results2file(ofstream *file, const double rv_station);
	///Output the maximum result output to a csv file
	void output_max_results2csvfile(ofstream *file, const double rv_station, const double z_min);
	///Output the maximum calculated results to the database table (erg_table)
	void output_max_results(QSqlDatabase *ptr_database, const int rv_no, const string polygon_string, int *glob_id, const string break_sc);
	///Output the instationary calculated results to the database table (erg_table)
	void output_instat_results(QSqlDatabase *ptr_database, const int rv_no, const string polygon_string, int *glob_id, const string break_sc, const string time);


	///Check the temperature data of the profiles
	void check_profiles(Hyd_River_Profile_Connection_Standard *rv_profile);


	///Reset the hydrological balance value and the maximum values
	void reset_hydrobalance_maxvalues(void);


	///Get the global profile id in database
	int get_prof_glob_id(void);

	///Connect instationary boundary curve to the temperature profiles
	void connect_instat_boundarycurve(Hyd_Instationary_Boundary *bound_curve, const int number);

	///Get the instationary found flag
	bool get_instat_boundarycurve_found(void);
	///Reset the boundary data of the temperature profile
	void reset_boundary_data(void);

	///Get the temperature differnce due to heat flows
	double get_delta_temp(void);

	///Get the distance to upstream profile
	double get_distance2up(void);

	///Get the current flow velocity
	double get_flow_velocity_current(void);

	///Set the distance to upstream profile
	void set_distance2up(const double distance);


private:
	//members



	///Name of profile
	string name;
	///Number of profile
	int profile_number;
	///Global profile id in database
	int glob_prof_id;

	///Initial waterlevel relative to the bottom of the profile
	double init_condition;

	///Bool if all relevant boundary czrves are found
	bool boundary_found;


	///Distance to upstream profile
	double distance2up;


	///The maximum value of the temperature in the profile
	_hyd_max_values temp_value_max;

	///The minimum value of the temperature in the profile
	_hyd_max_values temp_value_min;

	//The average temperature
	double temp_average;
	///The current temperature
	double temp_current;
	//Counter for the temperature average
	int count_average;

	///Temperature data: Air temperature [K]
	_hyd_temp_data air_temp;
	///Temperature data: Solar radiation [W/m²]
	_hyd_temp_data solar_rad;
	///Temperature data: Humiditiy [-]
	_hyd_temp_data humid;
	///Temperature data: Wind [m/s]
	_hyd_temp_data wind;
	///Temperature data: cloudness [-]
	_hyd_temp_data cloud;
	///Temperature data: shadow [-]
	_hyd_temp_data shadow;
	///Temperature data: inlet temperature [-]
	_hyd_temp_data inlet_temp;
	///Temperature data: Water temperature [K] as a fixed bounary condition; if this is set nothing else is required!
	_hyd_temp_data water_bound_temp;

	///Current discharge
	double discharge_current;
	///Current waterdepth
	double waterdepth_current;
	///Current flow velocity
	double flow_velocity_current;

	//Heat flow phi(total)
	double phi_total;
	///Heat flow phi(bed)
	double phi_bed;
	///Heat flow phi(conductivitiy)
	double phi_c;
	///Heat flow phi(evaporation)
	double phi_eva;
	///Heat flow phi(long wave radiation)
	double phi_lw;
	///Heat flow phi(solar radiation)
	double phi_solar;

	///Delta tmperature by heat flow
	double delta_temp_flow;


	

	///Defines the values for a hydrological balance for the lateral boundary condition
	//_hyd_hydrological_balance lateral_boundary_volume;


	///Number of members which have to be found in the file
	int must_found_number;



	//methods

	///Calculate heat flow bed
	void calc_phi_bed(HydTemp_Param *params);
	///Calculate heat flow conductivity
	void calc_phi_c(HydTemp_Param *params);
	///Calculate heat flow evaporation
	void calc_phi_eva(HydTemp_Param *params);
	///Calculate heat flow longwave radiation
	void calc_phi_lw(HydTemp_Param *params);
	///Calculate heat flow solar radiation
	void calc_phi_solar(HydTemp_Param *params);

	///Reset the temperature data structure
	void reset_temp_data_struct(_hyd_temp_data *to_reset);


	///Decide and fill the different keyvalues in the temperature data profile file
	void decide_keyvalues_file(QString line, _hyd_temp_data *to_reset, int *found_counter);


	///Transfer the river profile boundary data to a database
	void transfer_boundary_data2database(QSqlDatabase *ptr_database, const int glob_prof_id);

	///Input the temperature profile boundary data from a database
	void input_boundary_data_per_database(const int index, QSqlDatabase *ptr_database, const int glob_prof_id);


	///Get boundary condition is applied
	bool boundary_is_applied(void);


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