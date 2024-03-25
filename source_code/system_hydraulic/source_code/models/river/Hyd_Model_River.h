#pragma once
/**\class Hyd_Model_River
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_MODEL_RIVER_H
#define HYD_MODEL_RIVER_H


//system_hyd classes
//base class
#include "_Hyd_Model.h"
//standard profile class
#include "Hyd_River_Profile_Connection_Standard.h"
//upwards profile is inflow
#include "Hyd_River_Profile_Connection_Inflow.h"
//downwards profile is outflow
#include "Hyd_River_Profile_Connection_Outflow.h"
//parameter container
#include "Hyd_Param_RV.h"
//Make the river boundaries as a polygon
#include "Hyd_Floodplain_Polygon.h"
//Make the midpoints of the river as a river line
#include "Hyd_Floodplain_Polysegment.h"
//info class about the hydraulic boundary szenario data
#include "Hyd_Boundary_Szenario.h"

//forward declaration
struct _hyd_breach_start_parameters;




///Model class for a river model, where the one dimensional diffusive wave approach is applied \ingroup hyd
/**

*/
class Hyd_Model_River: public _Hyd_Model
{

//class for displaying the data
friend class Sys_Data_Tree_Wid;
friend class HydTemp_Model;

public:
	///Default constructor
	Hyd_Model_River(void);
	///Default destructor
	~Hyd_Model_River(void);

	//members

	///Container object for the model members
	Hyd_Param_RV Param_RV;
	///The midline of the river (the mid of each profile)
	Hyd_Floodplain_Polysegment river_midline;
	///The minline of the river (the lowest point in each profile)
	Hyd_Floodplain_Polysegment river_minline;
	///The outer left bank line of the river (in flow direction)
	Hyd_Floodplain_Polysegment river_leftline;
	///The outer right bank line of the river (in flow direction)
	Hyd_Floodplain_Polysegment river_rightline;
	///The boundary of a river as a polygon
	Hyd_Floodplain_Polygon river_polygon;
	///Profile in between the inflow- and the outflow profile 			
	Hyd_River_Profile_Connection_Standard *river_profiles;
	///The first profile upwards is always an inflow profile
	Hyd_River_Profile_Connection_Inflow inflow_river_profile;
	///The last profile downwards is always an outflow profile
	Hyd_River_Profile_Connection_Outflow outflow_river_profile;
	///Member for storing the hydraulic boundary szenario information
	Hyd_Boundary_Szenario hyd_sz;

	///Pointer to the table for the general parameters in a database
	static Tables *general_param_table;


	//method

	///Input the river model with the index per file with parser; the general settings as well as the profile data
	void input_members(const string global_file, const int index, const string global_path);
	///Input the river model with the index per file with parser; the general settings as well as the profile data
	void input_members(const Hyd_Param_RV param_rv);
	///Transfer the river model data to a database; the general settings as well as the profile data
	void transfer_input_members2database(QSqlDatabase *ptr_database);
	///Transfer a hydraulic boundary szenario from file to a database
	void transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database);
	///Input the river model with the index from a database selection of the relevant river models; the general settings as well as the profile data
	void input_members(const int index, const QSqlTableModel *query_result, QSqlDatabase *ptr_database, const bool just_profiles, const bool relevant, const bool output_flag=true);

	///Create the database table for the general parameters of the river model
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the general parameters of the river model: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the river model parameters as well as the river profile data 
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the general parameters of the river model
	static void close_table(void);
	///Select and count the number of relevant river models in a database table
	static int select_relevant_model_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output = true);
	///Select and count the number of relevant river models in a database table with a given river number
	static int select_relevant_model_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int rv_no);
	///Check the number of relevant river models in a database table
	static int check_relevant_model_database(QSqlDatabase *ptr_database);


	///Clone the members of the river model
	void clone_model(Hyd_Model_River *river);

	///Initialize the river profiles
	void init_river_model(Hyd_Param_Material *material_table);
	///Initialize the river geometry
	void init_river_model_geometry(void);
	///Check the river model
	void check_river_model(const bool output=true);

	///Compare the equality of two river models in terms of number of profiles and the river station of each profile
	void compare_models(Hyd_Model_River *compare_model);

	///Set a new hydraulic boundary scenario id to the river models
	void set_new_hyd_bound_sz_id(Hyd_Boundary_Szenario new_sz);

	///Get the diversion channel flag
	bool get_diversion_channel_flag(void);
	///Set the diversion channel flag
	void set_diversion_channel_flag(const bool flag);

	///Get the minimum river width
	double get_min_width(void);
	///Get the maximum river width
	double get_max_width(void);
	///Set minimum river width
	void set_min_width(const double width);
	///Set maximum river width
	void set_max_width(const double width);

	///Get a polygon for the river section (profile to profile i+1) a polygon with the given index
	Hyd_Floodplain_Polygon* get_river_section_polygon(const int section_index);
	///Get one segment of the river midline for one section for the given section index
	Geo_Segment* get_river_midline_section(const int section_index);
	///Get the first segment of the river midline
	Geo_Segment* get_river_midline_first_section(void);
	///Get the last segment of the river midline
	Geo_Segment* get_river_midline_last_section(void);


	///Output the given members
	void output_members(void);
	///Output the setted and calculated members
	void output_setted_members(void);
	///Output the geometrie to tecplot as 2d
	void output_geometrie2tecplot_2d(void);
	///Output the geometrie to paraview as 2d
	void output_geometrie2paraview_2d(void);

	///Output the result members for each riverprofile at every timestep
	void output_result_members_per_timestep(void);
	///Output the result members per timestep to tecplot as 1d
	void output_result2tecplot_1d(const double timepoint, const int timestep_number);
	///Output the result members per timestep to tecplot as 2d
	void output_result2tecplot_2d(const double timepoint, const int timestep_number);
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
	///Output the maximum result members to tecplot
	void output_result_max2tecplot(void);
	///Output the maximum result members to csv
	void output_result_max2csv(void);
	///Output the maximum result members to paraview as 2d
	void output_result_max2paraview2d(void);
	///Output the maximum result members to a database table
	void output_result_max2database(QSqlDatabase *ptr_database, const string break_sz);

	///Intercept the model with a coast model (Hyd_Coast_Model)
	bool intercept_coast_model(Hyd_Floodplain_Polygon *coastpolygon);
	///Intercept the model with an other river model (Hyd_Model_River)
	bool intercept_river_model(Hyd_Model_River *outflow_river);
	///Check the river models one another by interception
	void check_river_modelsby_interception(Hyd_Model_River *check_river);

	///Find the river section index were a given point is inside; -1 the point is in no river section
	int find_river_section_index_inside(Geo_Point *point);

	//Find the river section index (polygon) were a given point, the second point of the segment is inside; -1 the second point is in no river section; if the point is at the boundary than it is decide with the helb of the segment, which polygon is taken
	int find_river_section_index_inside(const bool begin_flag, Geo_Segment *segment);

	///Reset all coupling discharges of the model; use it before syncronization
	void reset_coupling_discharge(void);

	///Reset the model to the state before the calculation after the preproccesing
	void reset_model(Hyd_Param_Global *global_params);

	///Clear the boundary condition, which are set; boundary conditions due to interception are not cleared
	void clear_boundary_condition(const bool with_output=true);
	///Set new boundary condition per database for an new hydraulic boundary scenario; the new boundary scenario has to be specified before
	void set_new_boundary_condition(const bool output_flag, QSqlDatabase *ptr_database);

	///Make the syncronisation between the models and the boundaries
	void make_syncronisation(const double time_point);
	///Get the maximum change in a river section
	void get_max_changes_section(double *max_change_h, double *max_change_v, const bool timecheck, const double timestep);
	///Calculate the hydrological balance and the maximum values of the model
	void make_hyd_balance_max(const double time_point);

	///Reset the solver of the model
	void reset_solver(void);
	///Solve the model
	void solve_model(const double next_time_point, const string system_id);

	///Close the output file for a tecplot output
	void close_tecplot_output(void);

	///Set the folder for the file-output in case of an database based calculation
	void set_output_folder(const string folder);

	///Find the pointer to the river profiles upstream and downstream to a given riverstation
	bool find_ptr_upstream_downstream_prof(const double riverstation, _Hyd_River_Profile **up, _Hyd_River_Profile **down);

	///Get a polyline of the river profile(s), which are represented by an fpl-section (Fpl_Section)
	void get_polyline_fpl_section(const bool left_flag, Geo_Polysegment *polyline, _Hyd_River_Profile *prof_start, _Hyd_River_Profile *prof_end);

	///Idealise the river profile(s), which are represented by an fpl-section, with the data of the fpl-section (Fpl_Section)
	void idealise_profile_by_fpl(const bool left_flag, _Hyd_River_Profile *prof_start, _Hyd_River_Profile *prof_end, _hyd_idealisation_parameters ideal_params, _hyd_breach_start_parameters *start_params, const int section_id);

	///Get the number of involved profiles to a fpl-section
	int get_number_involved_profiles(void);
	///Get a pointer to the involved profiles by a given index
	_Hyd_River_Profile* get_ptr_involved_profile(const int index);

	///Get the pointer to the profile by a given index
	_Hyd_River_Profile* get_ptr_river_profile(const int index);
	///Get the pointer to the profile by a given number
	_Hyd_River_Profile* get_ptr_river_profile_number(const int number);


	///Set the total river bank to no overflow
	void set_total_river2no_overflow(void);

	///Find the index of a river profile by a given point
	int find_river_profile_id_by_point(Geo_Point *point);

	///Transfer the river profile data to a database, delete the boundary condition and copy the existing boundary condition 
	void transfer_profile_members2database(QSqlDatabase *ptr_database, Hyd_Model_River *source, const _sys_system_id base);


private:	
	//members

	///Total river lenght form inflow to outflow
	double delta_distance_outflow_inflow;
	///Total geodetic height loss form inflow to outflow
	double delta_height_outflow_inflow;
	///Number of the profiles which lies between inflow- and outflow-profile
	int number_inbetween_profiles;
	///Output file for the 2d tecplot output for the results
	ofstream tecplot_output_2d;
	///Output file for the 2d tecplot output for the geometrie
	ofstream tecplot_output_2d_geo;


	///Total hydrological balance of the lateral boundary discharges to the river segment up to the profile
	_hyd_hydrological_balance total_lateral_boundary_volume;
	///Total hydrological balance of the point boundary discharges to the river segment up to the profile
	_hyd_hydrological_balance total_point_boundary_volume;
	///Total hydrological balance of the coupling discharges due to overflow over the left bank (RV2FP)
	_hyd_hydrological_balance total_coupling_left_bank_volume;
	///Total hydrological balance of the coupling discharges due to overflow over the right bank (RV2FP)
	_hyd_hydrological_balance total_coupling_right_bank_volume;
	///Total hydrological balance of the coupling discharges due to coupling with other river models (RV2RV)
	_hyd_hydrological_balance total_coupling_1d_volume;
	///Total hydrological balance of the coupling discharges due to coupling to floodplain models via hydraulic structure (RV2FP)
	_hyd_hydrological_balance total_structure_coupling_volume;
	///Total hydrological balance of the coupling discharges due to coupling to floodplain models via a dikebreak at the left bank (RV2FP)
	_hyd_hydrological_balance total_left_dikebreak_coupling_volume;
	///Total hydrological balance of the coupling discharges due to coupling to floodplain models via a dikebreak at the right bank (RV2FP)
	_hyd_hydrological_balance total_right_dikebreak_coupling_volume;
	///Total volume error due to outflow of a profile, if there is no more watervolume left in the element
	double total_volume_error_zero_outflow;

	///Flag if this river model is a diversion channel
	bool diversion_channel_flag;

	///Number of river profiles
	int number_river_profiles;
	///Number of weir profiles
	int number_weir_profiles;
	///Number of bridge rofiles
	int number_bridge_profiles;

	///Minimum river width
	double min_width;
	///Maximum river width
	double max_width;

	///Polygon for the river sections (profile to profile i+1)
	Hyd_Floodplain_Polygon river_section_polygon;

	///A list of pointer to a part of the river profiles
	_Hyd_River_Profile **list_part_profiles;
	///Number of the profiles stored in the list list_part_profiles
	int number_part_profiles;

	//methods

	///Allocate the river profile with number of profiles
	void allocate_river_profiles(void);
	///Set a polygon for the river section (profile to profile i+1) a polygon with the given index
	void set_river_section_polygon(const int section_index);
	///Set a river polygon for storing the maximum result values of the inflow profile
	void set_river_section_first_profile(void);


	///Allocate the list of the pointer of a part of the river profiles
	void allocate_part_profile_list(void);
	///Delete the list of pointer of a part of the river profiles
	void delete_part_profile_list(void);
	///Set the list of the pointer of a part of the river profiles by two given profiles
	void set_part_profile_list(_Hyd_River_Profile *up, _Hyd_River_Profile *down);

	///Input the river profile data per file
	void input_river_profiles_perfile(void);
	///Transfer the river profile data to a database
	void transfer_profile_members2database(QSqlDatabase *ptr_database, const bool with_output=true);
	///Input the river profile data per database
	void input_river_profiles_perdatabase(const QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const bool output_flag);

	///Calculate the distance between the profiles
	void calculate_distance_of_profiles(void);
	///Calculate the geodetic height loss form inflow to outflow (delta_height_outflow_inflow)
	void calculate_deltaz_in2out(void);
	///Calculate the maximum / minimum width of the river
	void calculate_river_width(void);
	///Initialize the river geometrie as a midline and a polygon
	void init_river_geometrie(void);

	///Set function to solve to the solver
	void set_function2solver(void);
	///Set the init condition to the result vector
	void set_initcond2resultvector(void);

	///Allocate the instationary boundary curve 
	void allocate_instat_boundarycurves(void);
	///Read the instationary boundary curve from file
	void read_instat_boundarycurves(void);
	///Read in the instationary boundary curves from a database
	void read_instat_boundary_curves(const bool output_flag, QSqlDatabase *ptr_database);
	///Connect the profiles to the instat boundary curves
	void connect_profiles2instat_boundarycurves(void);
	///Check instationary boundary curves are found and if the inflow- and the outflow instationary boundary curves (if they are used) are sufficient long for the modeltime
	void check_instationary_boundary_found(void);

	///Output the statistics of the solver
	void output_solver_statistics(void);
	///Initialize the output file for a tecplot output 
	void init_tecplot_output(void);

	///Initialize and output to the output file for tecplot the maximum results (use it after the results per timesteps)
	void output_maximum_results2tecplot(void);
	///Output the maximum results
	void output_maximum_results(void);

	///Count element types
	void count_profile_types(void);

	///Calculate the hydrological balance of the river model
	void calculate_total_hydrological_balance(void);
	///Reset the total hydrological balance values
	void reset_total_hydrological_balance(void);
	///Output the hydrological balance of the river model
	void output_hydrological_balance(void);

	///Get the model description
	string get_model_description(void);

	///Check the coast model with the river model
	void check_coast_modelby_interception(Hyd_Floodplain_Polygon *coastpolygon);

	///Set the pointer to the upstream/downstream profile of the river profiles
	void set_upstream_downstream_profiles(void);
	
	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set errors
	Error set_error(const int err_type);

	///Main function for 1D diffusive wave modelling; it is solved by the solver
	#ifdef _WIN32
    friend int  __cdecl f1D_equation2solve( realtype time, N_Vector results, N_Vector da_dt, void *river_data);
	#elif defined(__unix__) || defined(__unix) 
    friend int  __attribute__((cdecl)) f1D_equation2solve( realtype time, N_Vector results, N_Vector da_dt, void *river_data);
	#endif
};
#ifdef _WIN32
int __cdecl f1D_equation2solve( realtype time, N_Vector results, N_Vector da_dt, void *river_data);
#elif defined(__unix__) || defined(__unix)
int __attribute__((cdecl)) f1D_equation2solve( realtype time, N_Vector results, N_Vector da_dt, void *river_data);
#endif

#endif