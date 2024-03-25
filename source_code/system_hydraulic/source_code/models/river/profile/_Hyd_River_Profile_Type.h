#pragma once
/**\class _Hyd_River_Profile_Type
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _HYD_RIVER_PROFILE_TYPE_H
#define _HYD_RIVER_PROFILE_TYPE_H

//system_sys_ classes
#include "_Sys_Common_System.h"
//system_hyd classes
#include "Hyd_Tables.h"
#include "Hyd_Profile_Segment.h"
#include "HydGui_Profile_Plot.h"
#include "Hyd_River_Profile_Point.h"
//cointainer class for the material parameters
#include "Hyd_Param_Material.h"

///Data structure for summarizing a maximum value of a results varaible and the corresponding point in time \ingroup hyd
struct _hyd_max_values{
	///Maximum value of the result variable, e.g. waterlevel
    float maximum;
	///Corresponding point in time, when the maximum value occurs
    float time_point;
};

///Data structure, which defines data for a bridge profile \ingroup hyd
struct _hyd_bridge_values{
	///The size of the bridge body [m]
	double bridgebody_size;
	///The local bridge heigth to the river bed (z min-value) [m]
	double local_bridge_height;
};

///Data structure, which defines flags for user-defined calculation flags
struct _hyd_profile_calc_setting{
	///Flag if the explicit velocity-approach for the calculation of the gradient ist used
	bool explict_v_is_used;
	///Flag for a interface approach for composed profiles: false:= no interface is taken between right/left bank and main channel; true:=interface is taken from the main channel
	bool use_interface;

};

//forward declaration
class _Hyd_River_Profile;

///Base class for all river profile types (e.g. river, weir) \ingroup hyd
/** 
This is the base class of all river profile types. Each river profile (_Hyd_River_Profile) has a specific type.
There are three types implemented:
	- river: the standard type; the flow between the profiles is calculated via the Manning-formula
	- weir: the flow to the neighbouring profiles is calculated via the Poleni-formula
	- bridge: the flow to the neighbouring profiles is calculated via Manning (no backwater), via pressure flow (backwater by the bridge body)
	or a combination of pressure flow and the Poleni-formula by also overtopping the bridge body 

This class consists also of the tables (Hyd_Tables) where the geometrical properties of the profile is stored.

*/
class _Hyd_River_Profile_Type : public _Sys_Common_System
{
public:
	///Default constructor
	_Hyd_River_Profile_Type(void);
	///Default destructor
	virtual ~_Hyd_River_Profile_Type(void);

	//members
	///The points of which the profile consists
	Hyd_River_Profile_Point *points;

	///Global x and y-coordinates of the midpoint of the river profile
	Geo_Point mid_point_global_x_y;
	///Global x and y-coordinates of the minpoint of the river profile
	Geo_Point min_point_global_x_y;

	///Table in which the area is stored dependent from the global waterlevel (s)
	Hyd_Tables area_table;
	///Table in which the global waterlevel (s) is stored dependent from the area
	Hyd_Tables h_table;
	//Table in which the width is stored dependent from the area
	Hyd_Tables width_table;

	///Pointer to the table for the profile point data in a database
	static Tables *profile_point_table;


	//method
	///Set number of points for builduing the profile
	void set_number_points(const int number);
	///Set a profile specific value (needed for the bridge type)
	virtual void set_profile_specific_value(const _hyd_bridge_values data);
	///Input the members per file of the profile type
	void input_members(QFile *profile_file, int *line_counter, const string name, const int number);
	///Input the river profile point data from a selection of a database table
	void input_members(const QSqlQueryModel *query_results, const string name, const int number);

	///Transfer the river profile point data to a database
	void transfer_profile_points2database(QSqlDatabase *ptr_database, const int prof_id);

	///Set the database table for the profile points data: it sets the table name and the name of the columns and allocate them
	static void set_profile_point_table(QSqlDatabase *ptr_database);
	///Create the database table for the profile points data
	static void create_profile_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database of the profile points data 
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database of the profile points data
	static void close_table(void);
	///Select and count the river profiles points in the database table and return the number
	static int select_profile_points_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const int glob_id);
	///Delete the point data in the database table for a given global profile id
	static void delete_points_by_rv_profile(QSqlDatabase *ptr_database, const int id);

	///Get the number of profile points
	int get_number_points(void);

	///Get the points of the left bank
	int get_points_left_bank(double **x, double **z);
	///Get the points of the main channel
	int get_points_main_channel(double **x, double **z);
	///Get the of the right bank
	int get_points_right_bank(double **x, double **z);

	///Get local zmax value
	double get_local_zmax(void);

	///Get the first point of the profile
	Hyd_River_Profile_Point * get_first_point(void);
	///Get the last point of the profile
	Hyd_River_Profile_Point * get_last_point(void);

	///Get the global x-coordinate of a profile point
	double get_global_x(const int index);
	///Get the global y-coordinate of a profile point
	double get_global_y(const int index);

	///Get the minimal z-coordinate (global_z_min) 
	double get_global_z_min(void);

	///Get the Init-condition as an area by a given local waterlevel
	double get_init_condition_value(const double init_value_h);

	///Set the waterlevel to the init-condition
	void set_waterlevels2init_condition(const double h_init);

	///Get the actual global waterlevel (s_value)
	double get_actual_global_waterlevel(void);

	///Get the waterlevel by a given area from table 
	virtual double get_waterlevel_from_table(const double area)=0;
	///Get the actual local waterlevel (h_value)
	double get_actual_local_waterlevel_h(void);
	///Get the actual local flowvelocity
	double get_actual_flow_velocity(void);
	///Get the solved area
	double get_solved_area(void);
	///Get the area for used for the calculation
	double get_area2calc(void);
	///Get a maximum waterlevel related to a profile point specified by the given point index; ifthe point do not exists 0.0 m will be returned
	double get_hmax2related_profile_point(const int profile_point_index);

	///Get a array of the profile points (x_gloabl, y_global), the number is returned
	int get_points_x_y_glob(Geo_Point **my_points);
	///Get a string for inserting a polyline data of the profile to a sql-database
	string get_polyline2sql_string(void);

	///Get the convenyance factor for the mid channel
	virtual double get_c_mid_channel(void)=0;
	///Get the convenyance factor
	virtual double get_c_value(void)=0;

	///Output the members of profile type
	virtual void output_members(ostringstream *cout);

	///Calculate the maximum values and the corresponding point in time 
	void calculate_maximum_values(const double time_point);

	///Output the result members each internal timestep (controll reason)
	void output_result_members_timestep(ostringstream *cout);

	///Output the result members each internal timestep to tecplot file
	void output_result_members_timestep(ofstream *file);
	///Output the result members each internal timestep to csv file
	void output_result_members_timestep2csv(ofstream *file);


	///Output the header for the maximum result output to console/display 
	static void output_header_max_results(ostringstream *cout);
	///Output the maximum result output to console/display 
	void output_max_results(ostringstream *cout);
	///Output the maximum result output to a given file 
	void output_max_results2file(ofstream *file);
	///Output the maximum result output to a csv file 
	void output_max_results2csvfile(ofstream *file);
	///Set the maximum result values of profile type to a string for the transfer into a database table (_Hyd_River_Profile)
	string set_maximum_value2string(void);

	///Set the instationary result values of profile type to a string for the transfer into a database table (_Hyd_River_Profile)
	string set_instat_value2string(void);


	///Set the actuel flow specific values with a given area from tables
	virtual void set_actuel_profilevalues_by_table(const double area)=0;

	///Set the actuel flow specific values with a given waterlevel from tables
	virtual void set_actuel_profilevalues_by_waterlevel(const double global_waterlevel)=0;

	///Set the actuel flow velocity by a given discharge and the corresponding froude-number
	void set_actuel_flowvelocity(const double discharge);

	///Calculate the actual discharge through this profile by a given upstream and downstream profile
	virtual double calculate_actual_discharge(_Hyd_River_Profile *upstream_profile,  _Hyd_River_Profile *downstream_profile, const double distance_upstream)=0;

	///Calculate the velocity energy head per syncronisation step
	void calculate_velocity_energy_head(const double time_point, _Hyd_River_Profile *downstream);
	///Get the velocity energy head per syncronisation step
	double get_velocity_energy_head(void);

	///Set the time point of the syncronisation step before
	void set_old_time_point(const double time_point);
	///Get the time point of the syncronisation step before
	double get_old_time_point(void);


	///Output the segments of the profile
	virtual void output_area_segments(void)=0;

	///Output the generated tables
	virtual void output_tables(void)=0;

	///Calculate and allocate the tables
	virtual void calc_alloc_tables(const double delta_x, Hyd_Param_Material *material_table)=0;

	///Calculate the stabilization discharge
	void calculate_stabilization_discharge(void);
	///Get the stabilization discharge
	double get_stabilization_discharge(void);

	///Get the flag if an outflow reduction is applied due to a setted outflow but no watervolume is left in the profile
	bool get_outflow_reduction_flag(void);

	///Reset the maximum values and the result
	virtual void reset_result_max_values(void);

	///Get the width of the profile start and end point 
	double get_width_end_start_profile(void);

	///Get a starting value in cause of: a hole before, this profile is dry, the waterlevel before is greater than this global z_min value
	virtual double get_starting_value(void)=0;

	///Get the froude-number for this profile
	double get_froude_number(void);

	///Clone profile type
	virtual void clone_profile_type(_Hyd_River_Profile_Type *prof_type);

	///Channel type to text
	static string channel_type2text(const int type);

	///Set the user flags for the calculation
	void set_user_flags(const _hyd_profile_calc_setting flags);
	///Get the user flags for the calculation
	_hyd_profile_calc_setting get_user_flags(void);

	///Get h_max-value
	_hyd_max_values get_max_h(void);
	///Get width_max-value
	_hyd_max_values get_max_width(void);
	///Get s_max-value
	_hyd_max_values get_max_s(void);
	///Get max vtot-value
	_hyd_max_values get_max_vtot(void);

	///Get h-value
	double get_h(void);
	///Get width-value
	double get_width(void);
	///Get s-value
	double get_s(void);
	///Get vtot-value
	double get_vtot(void);


protected:

	//members
	///Number of profile points
	int number_points;
	///The minimal z-coordinate (height)
	double global_z_min;
	///The x-coordinate to the minimal z-coordinate (height)
	double global_x_zmin;
	///The y-coordinate to the minimal z-coordinate (height)
	double global_y_zmin;
	///The maximal z-coordinate (height)
	double global_z_max;
	///The x-coordinate to the maximal z-coordinate (height)
	double global_x_zmax;
	///The y-coordinate to the maximal z-coordinate (height)
	double global_y_zmax;
	///The index to the point of the minimal z-coordinate (height)
	int zmin_marker;
	///The index to the point of the maximal z-coordinate (height)
	int zmax_marker;
	///Specifies the type of material, which is needed for this class
	_hyd_mat_coefficient_types material_type;

	//result variables per time step
	///Actual local waterlevel for this profile per solver step
	double h_value;
	///Velocity energy head for per syncronisation step
	double v_energy;
	///Velocity energy head for one syncronisation step before
	double v_energy_old;
	///Velocity energy head for one syncronisation two step before
	double v_energy_old_old;
	///Delta of time for calculate a new velocity energy head
	double delta_t_v_energy;
	///The calculated gradient between the profiles
	double gradient;
	///Actual global waterlevel for this profile per solver step
	double s_value;
	///The maximum global waterlevel for this profile during calculation and the corresponding point in time
	_hyd_max_values s_value_max;
	///The maximum local waterlevel for this profile during calculation and the corresponding point in time
	_hyd_max_values h_value_max;
	///The flow velocitiy
	double v_value;
	///The maximum flow velocity for this profile during calculation and the corresponding point in time
	_hyd_max_values v_value_max;
	///Flag if the profile was dry
	bool was_dry_flag;
	///Duration how long the profile was dry
	double dry_duration;
	///Duration how long the profile was dry
	double wet_duration;

	///Width of the water surface
	double width;
	///The maximum width for this profile during calculation and the corresponding point in time
	_hyd_max_values width_value_max;

	///The Froude-number for this profile; the calculation is done approximatly for a rectangular profile (fr=v/(g*h)^0.5)
	double froude_no;
	
	///The time point one syncronisation step before
	double old_time_point;
	///The time point for the energy calculation one step before
	double old_energy_time;




	///Flag if an outflow reduction is applied due to a setted outflow but no watervolume is left in the profile
	bool zero_outflow_reduction_flag;

	///The area which is calculated by the solver
	double solved_area;
	///The area which is taken for calculation
	double area2calc;

	///Stabilization discharge
	double stab_discharge;

	///Plot member for illustarting the profile in a diagram
	HydGui_Profile_Plot *plot;

	///Number of points needed in the table
	int number_table_points;
	///Constant for increment the maximal height for the table calculation
	const double increment_max_height;

	///Line of waterlevel for the profile intersection with the waterlevel => table values
	Geo_Straight_Line *waterlevel_line;

	///User flags for the calculation
	_hyd_profile_calc_setting user_setting;

	//methods
	///Generates a plot of the profile
	void generate_plot(QWidget *parent=NULL);

	///Set the actual local Waterlevel (h) and the global waterlevel (s) with a given area from tables
	void set_actual_waterlevel_by_table(const double area);

	///Set the waterlevels of the profile directly without the use of a table
	void set_waterlevel_direct(double const global_waterlevel);

	///Check the points and the areatypes of consistency
	virtual void check_points(const string name, const int number);

	///Check if there are unphysical waterlevel calculated by the solver
	void check_unphysical_waterlevels(void);

private:
	//members
	///The width between the first and the last point of the profile
	double profile_width;

	//methods
	///Set number of profile points
	void allocate_points(void);
	///Set the coordinates of minimal z-coordinate (height);
	void set_zmin_coordinates(void);
	///Set the coordinates of maximal z-coordinate (height);;
	void set_zmax_coordinates(void);
	///Calculate the local z-coordinates (height) related to the minimal z-coordinate and the local x-coordinate related to the first point
	void calculate_local_coordinates(void);
	///Calculate the global midpoint
	void calculate_global_midpoint_x_y(void);
	///Calculate the global minpoint
	void calculate_global_minpoint_x_y(void);
	
	///Set warnings
	Warning set_warning(const int warn_type);
	///set errors
	Error set_error(const int err_type);


};
#endif
