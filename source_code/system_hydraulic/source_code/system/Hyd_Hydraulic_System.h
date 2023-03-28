#pragma once
/**\class Hyd_Hydraulic_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_HYDRAULIC_SYSTEM_H
#define HYD_HYDRAULIC_SYSTEM_H

//qt-libs
#include <QDir>

//system hyd libs
//Class of a river model
#include "Hyd_Model_River.h" 
//Class of a floodplain model
#include "Hyd_Model_Floodplain.h"
//Class of the coast model
#include "Hyd_Coast_Model.h"
//Class of the temperature model
#include "HydTemp_Model.h"
//container class the of global parameters
#include "Hyd_Param_Global.h"
//cointainer class for the material parameters
#include "Hyd_Param_Material.h"
//class for coupling
#include "Hyd_Coupling_Management.h"
//class for counting the couplings
#include "Hyd_Coupling_Model_List.h"
//managing class for the hydraulic boundary szenarios
#include "Hyd_Boundary_Szenario_Management.h"

//Observation points in the models
#include "Hyd_Observation_Point_Manager.h"

//system_sys_ classes
#include "_Sys_Common_System.h"
#include "Sys_Project.h"




///Main class of the hydraulic module; the hydraulic system consists of the hydraulic models (e.g. river, floodplain etc.) and the couplings between the models \ingroup hyd
/**

*/
class Hyd_Hydraulic_System: public QThread , public _Sys_Common_System
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT 

public:
	///Default constructor
	Hyd_Hydraulic_System(void);
	///Default destructor
	~Hyd_Hydraulic_System(void);

	//members

	//development static output file
	//static ofstream test;

	///Pointer to the river models
	Hyd_Model_River *my_rvmodels;
	///Pointer to the floodplainmodels
	Hyd_Model_Floodplain *my_fpmodels;
	///Pointer to the coast model 
	Hyd_Coast_Model *my_comodel;
	///Pointer to the temp model
	HydTemp_Model *my_temp_model; 
	///Container of global parameters of the system
	Hyd_Param_Global global_parameters;


	///Container of material parameters 
	Hyd_Param_Material material_params;
	///Member for storing the hydraulic boundary szenario information
	Hyd_Boundary_Szenario hyd_sz;

	///Flag (static) if a multithreading via Qtthreads is applied
	static bool qt_thread_applied;

	///Mangement class of the model couplings
	Hyd_Coupling_Management coupling_managment;
	///Mangement class of observation points
	Hyd_Observation_Point_Manager obs_point_managment;

	
	//method

	///Ask the global file name per console
	static string ask_global_file_name(void);

	///Read in the hydraulic system- and modelparameters from the global file
	void set_system_per_file(const string global_file); 

	///Read in one river model per file
	void set_river_model_per_file(const Hyd_Param_RV param_rv);
	
	///Read in the hydraulic system- and modelparameters from a database
	void set_system_per_database(const bool modul_extern_startet=false);
	///Read in the hydraulic temperature system- and modelparameters from a database
	void set_temp_system_per_database(const bool modul_extern_startet = false);
	///Read in the hydraulic system- and modelparameters from a database just for the floodplain models
	void set_2Dsystem_per_database(const bool modul_extern_startet=false);

	///Read in the the river-model(s) with their profile etc. and the coast model from database
	void set_river_coast_model_per_database(const bool with_output=true);

	///Read in the base hydraulic system per file and set this data to a database; all existing hydraulic data in database will be deleted
	void import_basesystem_file2db(const string global_file);
	///Add new hydraulic boundary scenarios to the base hydraulic system into the database; the whole hydraulic system must be set before, e.g. read in per file
	void import_new_hydraulic_boundary_sz2database(void);
	///Create a whole set of hydraulic database tables
	void create_hyd_database_tables(void);
	///Check all hydraulic database tables, for their existence in the database and their declaration in the database table-file
	void check_hyd_database_tables(void);
	///Delete the data of all hydraulic database tables
	void delete_data_hyd_database_tables(void);
	///Close all hydraulic database tables
	static void close_hyd_database_tables(void);

	///Switch applied flag of the hydraulic results for a given system id
	static void switch_applied_flags_results(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch applied flag of the hydraulic results for a given system id and given hydraulic boundary scenario id
	static void switch_applied_flags_results(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int hyd_sc);

	///Copy the results of a given system id to another one in database tables
	static void copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);

	///Check if hydraulic results are calculated
	static bool check_hyd_results_calculated(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag=false);
	///Check if instationary hydraulic results are calculated
	static bool check_hyd_instat_results_calculated(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag = false);


	///Set a new hydraulic boundary scenario id to the hydraulic system and all appendent models
	void set_new_hyd_bound_sz_id(Hyd_Boundary_Szenario new_sz);
	///Get a pointer to the currently used hydraulic boundary scenario
	Hyd_Boundary_Szenario* get_ptr_hyd_bound_scenario(void);


	///Output the models and global parameters etc.
	void output_glob_models(void);

	///Compare the equality of two hydraulic systems in terms of model numbers; further the models are compared
	void compare_hydraulic_system(Hyd_Hydraulic_System *compare_system);

	///Compare the equality of two hydraulic systems in terms of model numbers; further the models are compared
	void transfer_glob_elem_id(Hyd_Hydraulic_System *to_system);
	

	///Clone the hydraulic system after initialisation
	void clone_system(Hyd_Hydraulic_System *system);


	///Initialize the models
	void init_models(const bool modul_extern_startet=false);

	///Initialize the temperature models
	void init_temp_models(const bool modul_extern_startet = false);

	///Output setted and calculated members
	void output_setted_members(void);

	///Output setted and calculated temperature members
	void output_setted_temp_members(void);

	///Initialize the solver for the models; all not needed data is here deleted
	void init_solver(void);

	///Initialize the solver for the temperature models; all not needed data is here deleted
	void init_temp_solver(void);

	///Make the calculation of the external loop (output time steps)
	void make_calculation(void);

	///Make the temperature calculation of the external loop (output time steps)
	void make_temp_calculation(void);

	///Set the thread number (0 first thread/1 second thread...); used for the output of the calculation class; use it just before the method run(void)
	void set_thread_number(const int thread);
	///Set the system number by multi system calculations
	void set_system_number(const int calcnumber);
	///Get the system number 
	int get_system_number(void);

	///Run the calculation; it is identically to the methods make_calculation(void); just use this for multithreading
	void run(void);

	///Output final statistics of the models
	void output_final_model_statistics(const bool all_output=true);

	///Get the number of errors
	int get_number_errors(void);
	///Get the number of warnings
	int get_number_warnings(void);


	///Increase the error number by 1
	void increase_error_number(void);

	///Convert the model type (_hyd_model_type) into a text string
	static string convert_model_type2txt(const _hyd_model_type type);
	///Convert a string into a model type (_hyd_model_type)
	static _hyd_model_type convert_txt2model_type(const string text);

	///Set the system-id (_sys_system_id) of the object
	void set_systemid(const _sys_system_id actuel_id);

	///Reset the system to the state before the calculation after the preproccesing
	void reset_system(void);

	///Clear the boundary condition of the models representing the system
	void clear_boundary_conditions(void);

	///Set a new set of boundary conditions to the system's models; the new boundary scenario has to be specified with set_new_hyd_bound_sz_id(Hyd_Boundary_Szenario new_sz) before
	void set_new_boundary_condition(void);

	///Set the flag if a file output is required
	void set_file_output_flag(const bool flag);

	///Set identifier string for the break scenario
	void set_string_break_sc(const string break_sc);
	///Get identifier string for the break scenario
	string get_string_break_sc(void);

	///Set a pointer to the database and copy it
	void set_ptr2database(QSqlDatabase *ptr_database, const int thread_number);
	///Set a the database without making a copy of the database connection
	void set_ptr2database(QSqlDatabase database, const int thread_number);

	///Total reset of the hydraulic system
	void total_reset(void);
	///Set flag that the preprocessing was successful/not successful
	void set_preproc_flag(const bool flag);
	///Get the flag if the preprocessing was successful/not successful
	bool get_preproc_flag(void);

	///Set output folder name in case of a calculation via database by a given scenario name
	void set_folder_name(const string sc_name, const bool creation_flag);
	///Set output folder name in case of file calculation output
	void set_folder_name_file(void);
	///Set identifier string
	void set_identifier_string(const string identifier);
	///Get the identifier prefix for the output to display/console
	string get_identifier_prefix(const bool as_prefix=true);

	///Get the pointer to a floodplain model by a given model number
	Hyd_Model_Floodplain* get_ptr_floodplain_model(const int number);
	///Get the pointer to a floodplain model by a given index
	Hyd_Model_Floodplain* get_ptr_floodplain_model_index(const int index);

	///Get the pointer to a river model by a given model number
	Hyd_Model_River* get_ptr_river_model(const int number);
	///Get the pointer to a river model by a given index
	Hyd_Model_River* get_ptr_river_model_index(const int index);

	///Output the final number of error(s)/warning(s)
	void output_final_error_warning_number(void);

	///Set the total riverline to no-overflow
	void set_total_river2no_overflow(void);

	///Get if the output to display is running
	bool get_output_is_running(void);
	///Get if the output to display is required
	bool get_output_is_required(void);
	///Set that an output is allowed
	void set_output_is_allowed(void);

	///Input just the river models per database (used in the Alt_System)
	void input_just_river_models(QSqlDatabase *ptr_database, const bool with_output=true);


	///Initialize and connect the data of the river models
	void connect_rivers(Hyd_Param_Material *mat_param);

	///Initialize and connect the data of the temperature models
	void connect_temperature_model(void);

	///Set river inflow boundary condition to true for all river models (used in Alt-System)
	void set_rv_inflow_flag(void);

	///Set if the temperature calculation is applied
	void set_temp_calc_apply(const bool flag);

signals:

	///Emit that an output is required
	void output_required(int thread_no);




private:
	//members
	
	///Output time
	double output_time;
	///Actual internal time
	double internal_time;
	///Next internal timestep
	double next_internal_time;
	///Total number of internal time steps
	int total_internal_timestep;
	///Differences of real time
	double diff_real_time;


	///Counter of the internal timesteps
	int timestep_counter;
	///Counter of the external timesteps
	int timestep_internal_counter;
	///Current internal timestep
	double internal_timestep_current;
	///Base internal timestep
	double internal_timestep_base;
	///Minimal internal timesteps
	double internal_timestep_min;

	///String of the model time
	string model_time_str;

	///Time classes for the real time calculation (start time point)
	time_t start_time;
	///Time classes for the real time calculation (actual time point)
	time_t actual_time;
	///String of the real time
	string real_time_str;

	///Total number of errors for the run 
	int error_number;
	///Total number of warnings for the run
	int warning_number;
	///Number of warnings before the evaluation
	int warning_number_start;

	///Number of thread (0 first thread/1 second thread); used for the output of the calculation methods
	int thread_number;
	///Number of multi system calculation in general the hydraulic boundary scenario id
	int calc_number;
	///Identifer string 
	string identifier_str;
	

	///Output to display is on the way
	bool output_is_running;
	///Output to display is allowed
	bool output_is_allowed;
	///Output to display is required
	bool output_is_required;

	///Flag, if a file output is required
	bool file_output_required;

	///Identifier string for the break scenario
	string break_sz;

	///Copy of the database
	QSqlDatabase database;
	///Flag if the database is set, the calculation is done via database
	bool database_is_set;

	///Flag if the preprocessing was succesful
	bool preproc_success_flag;

	///String for the folder, where the file output is stored in case of a database calculation
	string file_output_folder;

	//Flag if the temperature calculation is applied;
	bool temp_calc;

	//methods 

	///Output final statistics of the system
	void output_final_system_statistics(void);
	///Output final statistics of the temperature system
	void output_final_temp_system_statistics(void);
	///Allocate the floodplain models
	void allocate_floodplain_models(void);
	///Read in the floodplain models of the system with Hyd_Parse_FP
	void input_floodplains_models(const string global_file);
	///Transfer the data of the floodplain models to the database
	void transfer_floodplainmodel_data2database(QSqlDatabase *ptr_database);
	///Read in the floodplain models of the system from a database
	void input_floodplains_models(const QSqlTableModel *query_result, QSqlDatabase *ptr_database, const bool just_elems);
	///Initialize and connect the data of the floodplain models
	void connect_floodplains(void);

	///Allocate the river models
	void allocate_river_models(void);
	///Read in the river models of the system with Hyd_Parse_RV
	void input_rivers_models(const string global_file);
	///Read in the river models of the system with given parameters
	void input_rivers_models(const Hyd_Param_RV param_rv);
	///Transfer the data of the river models to the database
	void transfer_rivermodel_data2database(QSqlDatabase *ptr_database);
	///Read in the river models of the system from a database
	void input_rivers_models(const QSqlTableModel *query_result, QSqlDatabase *ptr_database, const bool with_output=true);


	///Allocate the coast model
	void allocate_coast_model(void);
	///Read in the coast model of the system with Hyd_Parse_CO
	void input_coast_model(const string global_file);
	///Read in the coast model of the system from selection of a database
	void input_coast_model(const QSqlQueryModel *query_result, QSqlDatabase *ptr_database , const bool with_output=true);


	///Allocate the temperature model
	void allocate_temp_model(void);
	///Read in the temperature model of the system with Hyd_Parse_CO
	void input_temp_model(const string global_file);
	///Transfer the data of the river models to the database
	void transfer_tempmodel_data2database(QSqlDatabase *ptr_database);
	///Read in the temperature model of the system from selection of a database
	void input_temp_model(const QSqlTableModel *query_result, QSqlDatabase *ptr_database, const bool with_output = true);


	///Make the geometrical interceptions between the models
	void make_geometrical_interception(void);
	///Make the geometrical interceptions between a coast model and floodplain models
	void make_geometrical_interception_co2fp(void);
	///Make the geometrical interceptions between floodplain and floodplain models
	void make_geometrical_interception_fp2fp(void);
	///Make the geometrical interceptions between floodplain and river models
	void make_geometrical_interception_fp2rv(void);
	///Make the geometrical interceptions between coast model and river models
	void make_geometrical_interception_rv2co(void);
	///Make the geometrical interceptions between river- and river models
	void make_geometrical_interception_rv2rv(void);

	///Initialize the automatic coupling
	void init_automatic_coupling(void);

	///Check the models
	void check_models(void);
	
	///Make the calculation for the internal loop
	void make_calculation_internal(void);
	///Make the temperature calculation for the internal loop
	void make_temp_calculation_internal(void);
	///Reset the solver-tolerances of each model
	void reset_solver_tolerances(void);
	///Reset all coupling discharges of the river models
	void reset_coupling_discharge_river_models(void);
	///Make the syncronisation of the river models for each internal step
	void make_syncron_rivermodel(void);
	///Make the syncronisation of the temperature models for each internal step
	void make_syncron_tempmodel(void);
	///Get the maximum change in a element of a river model as well as the maximum change of the explicitly velocity head
	void get_max_changes_rivermodel(double *max_change_h, double *max_change_v, const bool timecheck, const double timestep);
	///Calculate the hydrological balance and the maximum values of the river models for each internal step
	void make_hyd_balance_max_rivermodel(void);
	///Calculate the hydrological balance and the maximum values of the temperature models for each internal step
	void make_hyd_balance_max_tempmodel(void);
	///Reset the solver of the river models
	void reset_solver_rv_models(void);
	///Make the calculation of the river models for each internal step
	void make_calculation_rivermodel(void);
	///Make the calculation of the temperature models for each internal step
	void make_calculation_tempmodel(void);

	///Make the syncronisation of the floodplain models for each internal step
	void make_syncron_floodplainmodel(void);
	///Get the maximum change in a element of a floodplain model
	void get_max_changes_floodplainmodel(double *max_change_h, const bool timecheck);
	///Calculate the hydrological balance and the maximum values of the floodplain models for each internal step
	void make_hyd_balance_max_floodplainmodel(void);
	///Reset the solver of the floodplain models
	void reset_solver_fp_models(void);
	///Make the calculation of the floodplain models for each internal step
	void make_calculation_floodplainmodel(void);

	///Output the results of the calculation steps of the river models to file
	void output_calculation_steps_rivermodel2file(const double timestep);
	///Output the calculation steps (time, solversteps etc) of the river models to display/console
	void output_calculation_steps_rivermodel2display(const double timestep);
	///Output the calculation steps (time, solversteps etc) of the river models to databse
	void output_calculation_steps_rivermodel2database(const double timestep, const string time);


	///Output the results of the calculation steps of the temperature models to file
	void output_calculation_steps_tempmodel2file(const double timestep);
	///Output the calculation steps (time, solversteps etc) of the temperature models to display/console
	void output_calculation_steps_tempmodel2display(const double timestep);
	///Output the calculation steps (time, solversteps etc) of the temperature models to databse
	void output_calculation_steps_tempmodel2database(const double timestep, const string time);

	///Output the results of the calculation steps of the floodplain models to file
	void output_calculation_steps_floodplainmodel2file(const double timestep);
	///Output the results of the calculation steps of the floodplain models to display/console
	void output_calculation_steps_floodplainmodel2display(const double timestep);
	///Output the results of the calculation steps of the floodplain models to database
	void output_calculation_steps_floodplainmodel2database(const double timestep, const string time);

	///Clear all not needed data of the models before the solver is initialized
	void clear_models(void);

	///Set the starting number of warning(s) from the exception_class (sys)
	void set_start_warning_number(void);
	///Set the final number of warning(s) from the exception_class (sys)
	void set_final_warning_number(void);


	///Calculate the approximate memory requirement for the floodplain models
	string calculate_approx_memory_fpmodels(void);
	///Calculate the total was-wetted area for the floodplain models
	double calculate_was_wetted_area_fp(double *area_without_coast);

	///Wait loop for the output of the calculation to display/console (for multi threading)
	void waitloop_output_calculation2display(void);	


	///Check the internal time steps
	double check_internal_timestep(void);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif
