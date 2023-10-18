#pragma once
/**\class HydTemp_Model
	\author Daniel Bachmann et al.
	\author produced by the AG-FRM University of applied sciences Magdeburg-Stendal
	\version 0.0.1                                                              
	\date 2023 
*/
#ifndef HYDTEMP_MODEL_H
#define HYDTEMP_MODEL_H


//system_hyd classes
//base class
#include "_Hyd_Model.h"

#include "HydTemp_Param.h"

//info class about the hydraulic boundary szenario data
#include "Hyd_Boundary_Szenario.h"
#include "HydTemp_Profile.h"








///Model class for the temperature model in a 1d-river model \ingroup hyd
/**

*/
class HydTemp_Model: public _Hyd_Model
{

//class for displaying the data
friend class Sys_Data_Tree_Wid;

public:
	///Default constructor
	HydTemp_Model(void);
	///Default destructor
	~HydTemp_Model(void);

	//members

	///Container object for the model members
	HydTemp_Param Param_Temp;
	///Member for storing the hydraulic boundary szenario information
	Hyd_Boundary_Szenario hyd_sz;

	//Pointer to river model
	Hyd_Model_River *ptr_river_model;

	///Pointer to the table for the general parameters in a database
	static Tables *general_temp_param_table;

	//method

	///Input the temperature model with the index per file with parser; the general settings as well as the profile data
	void input_members(const string global_file, const int index, const string global_path);
	///Input the temperature model with the index per file with parser; the general settings as well as the profile data
	void input_members(const HydTemp_Param param_temp);
	///Transfer the temperature model data to a database; the general settings as well as the profile data
	void transfer_input_members2database(QSqlDatabase *ptr_database);
	///Transfer a hydraulic boundary szenario from file to a database
	void transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database);
	///Input the river model with the index from a database selection of the relevant temperature models; the general settings as well as the profile data
	void input_members(const int index, const QSqlTableModel *query_result, QSqlDatabase *ptr_database, const bool just_profiles, const bool relevant, const bool output_flag=true);

	///Create the database table for the general parameters of the temperature model
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the general parameters of the temperature  model: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the temperature  model parameters as well as the temeratureprofile data 
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the general parameters of the temerature river model
	static void close_table(void);
	///Select and count the number of relevant temperature  models in a database table
	static int select_relevant_model_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output = true);
	///Select and count the number of relevant temperature  models in a database table with a given river number
	static int select_relevant_model_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int rv_no);
	///Check the number of relevant temperature  models in a database table
	static int check_relevant_model_database(QSqlDatabase *ptr_database);

	///Get a pointer to the temperature profile by a given index
	HydTemp_Profile* get_ptr_profile(const int index);


	///Clone the members of the temperature  model
	void clone_model(HydTemp_Model *temp);

	///Initialize the temperature  profiles
	void init_temp_model(QSqlDatabase *ptr_database, const int number_timestep, const double delta_t);
	///Initialize the temperature  geometry
	//void init_river_model_geometry(void);
	///Check the temperature model
	void check_temp_model(const bool output=true);

	///Compare the equality of two temperature models in terms of number of profiles
	void compare_models(HydTemp_Model *compare_model);

	///Set a new hydraulic boundary scenario id to the temperature  models
	void set_new_hyd_bound_sz_id(Hyd_Boundary_Szenario new_sz);
	///Set a pointer to the river model 
	void set_ptr2RV(Hyd_Model_River *RV_model);

	///Output the given members
	void output_members(void);
	///Output the setted and calculated members
	void output_setted_members(void);
	///Output the geometrie to paraview as 2d
	void output_geometrie2paraview_2d(void);

	///Output the result members for each riverprofile at every timestep
	void output_result_members_per_timestep(void);
	///Output the result members per timestep to csv as 1d
	void output_result2csv_1d(const double timepoint, const int timestep_number);
	///Output the result members per timestep to paraview as 2d
	void output_result2paraview_2d(const double timepoint, const int timestep_number);
	///Output the result members per timestep to database as 2d
	void output_result2database_2d(QSqlDatabase *ptr_database, const string break_sz, const double timepoint, const int timestep_number, const string time);


	///Output solver errors for one solver step
	void output_solver_errors(const double time_point, const int step_counter, const string timestring , const string realtime, const double diff_time, const int total_internal, const int internal_steps);
	///Output final statistics of the river model
	void output_final(void);
	///Output the maximum result members to csv
	void output_result_max2csv(void);
	///Output the maximum result members to paraview as 2d
	void output_result_max2paraview2d(void);
	///Output the maximum result members to a database table
	void output_result_max2database(QSqlDatabase *ptr_database, const string break_sz);

	///Reset all coupling values of the model; use it before syncronization
	void reset_coupling_values(void);

	///Reset the model to the state before the calculation after the preproccesing
	void reset_model(Hyd_Param_Global *global_params);

	///Clear the boundary condition, which are set; boundary conditions due to interception are not cleared
	void clear_boundary_condition(const bool with_output=true);
	///Set new boundary condition per database for an new hydraulic boundary scenario; the new boundary scenario has to be specified before
	void set_new_boundary_condition(const bool output_flag, QSqlDatabase *ptr_database);


	///Make the syncronisation between the models and the boundaries
	void make_syncronisation(const double time_point);

	///Calculate the balance and the maximum values of the model
	void make_hyd_balance_max(const double time_point);
//
	///Reset the solver of the model
	void reset_solver(void);
	///Solve the model
	void solve_model(const double next_time_point, const string system_id);
//
	///Close the output file for a tecplot output
	void close_tecplot_output(void);


	///Get the pointer to the profile by a given index
	HydTemp_Profile* get_ptr_river_profile(const int index);
	///Get the pointer to the profile by a given number
	HydTemp_Profile* get_ptr_river_profile_number(const int number);

	///Set the folder for the file-output in case of an database based calculation
	void set_output_folder(const string folder);



//private:	
	//members

	///Output file for the 2d tecplot output for the results
	ofstream tecplot_output_2d;
	///Output file for the 2d tecplot output for the geometrie
	ofstream tecplot_output_2d_geo;


	///Flag if the temperature model for this river is applied
	bool model_is_applied;

	///Temperature data of the river profiles
	HydTemp_Profile *profiles;

;


//	//methods

	///Allocate the river profile for temperature data 
	void allocate_temp_river_profiles(void);

	///Input the river profile temperature data per file
	void input_river_profiles_temp_data_perfile(void);

	///Transfer the temperature profile data to a database
	void transfer_profile_members2database(QSqlDatabase *ptr_database, const bool with_output=true);
	///Input the temperature profile data per database
	void input_profiles_perdatabase(const QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const bool output_flag);



	///Set function to solve to the solver
	void set_function2solver(void);
	///Set the init condition to the result vector
	void set_initcond2resultvector(void);


	///Connect the profiles to the instat boundary curves
	void connect_profiles2instat_boundarycurves(void);
	///Check instationary boundary curves are found and if the inflow- and the outflow instationary boundary curves (if they are used) are sufficient long for the modeltime
	void check_instationary_boundary_found(void);

	///Output the statistics of the solver
	void output_solver_statistics(void);


	///Output the maximum results
	void output_maximum_results(void);
//

//
	///Calculate the hydrological balance of the temperature  model
	void calculate_total_hydrological_balance(void);
	///Reset the total hydraological balance values
	void reset_total_hydrological_balance(void);
	///Output the hydrological balance of the temperature  model
	void output_hydrological_balance(void);
//
	///Get the model description
	string get_model_description(void);

//	
	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set errors
	Error set_error(const int err_type);
//
//	///Main function for 1D temperature modelling; it is solved by the solver
	#ifdef _WIN32
     friend int  __cdecl ftemp_equation2solve( realtype time, N_Vector results, N_Vector da_dt, void *river_data);
	#elif defined(__unix__) || defined(__unix)
	friend int  __attribute__((cdecl)) ftemp_equation2solve( realtype time, N_Vector results, N_Vector da_dt, void *river_data);
	#endif
};

#ifdef _WIN32
int __cdecl ftemp_equation2solve( realtype time, N_Vector results, N_Vector da_dt, void *river_data);
#elif defined(__unix__) || defined(__unix)
int __attribute__((cdecl)) ftemp_equation2solve( realtype time, N_Vector results, N_Vector da_dt, void *river_data);
#endif
	
#endif