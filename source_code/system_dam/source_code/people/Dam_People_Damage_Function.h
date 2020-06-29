#pragma once
/**\class Dam_People_Damage_Function
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef DAM_PEOPLE_DAMAGE_FUNCTION_H
#define DAM_PEOPLE_DAMAGE_FUNCTION_H


//sys-libs
#include "Sys_Plot_Wid.h"
//dam-libs
//base class



///Data structure for gathering the data of a damage function point for the risk2people calculation
struct _dam_people_function_point{
	///Impact value, like waterlevel or flow velocity (x-axis)
	double impact_value;
	///Corresponding relative damage [-] in zone 1 ds/dt<0.5 m/h (y-axis)
	double rel_damage_zone1;
	///Corresponding relative damage [-] in zone 2 ds/dt>=0.5 m/h (y-axis)
	double rel_damage_zone2;
	///Corresponding relative damage [-] in zone 3 v>=2 m/s and hv>=7m²/s (y-axis)
	double rel_damage_zone3;

};

///Class for a damage function of one people2risk-type \ingroup dam
/**

*/
class Dam_People_Damage_Function 
{
//class for displaying the data
friend class Sys_Data_Tree_Wid;
//class for displaying the data in a widget
friend class DamGui_Pop_Function_Wid;

public:
	///Default constructor
	Dam_People_Damage_Function(void);
	///Default destructor
	~Dam_People_Damage_Function(void);


	//member
	///Pointer to the table for the damage function general information in a database
	static Tables *function_table;

	//method
	///Read in the damage function per file
	void read_function(ifstream *ifile, int *line_counter); 
	///Transfer the damage function data to a database: the general settings of the functions (risk2people type) as well as the function points
	void transfer_input_members2database(QSqlDatabase *ptr_database);
	///Add damage function data to a database: the general settings of the functions (people2risk type) as well as the function points
	void add_input_members2database(QSqlDatabase *ptr_database);

	///Input the damage function data per database: the general settings of the functions (risk2people type) as well as the function points
	void input_function_perdatabase(const QSqlTableModel *results, const int glob_index);

	///Get the number of points 
	int get_number_points(void);
	///Get the index of the function
	int	get_index(void);
	///Get the name of the people2risk damage function
	string get_name(void);
	///Get the impact type
	_dam_impact_type get_impact_type(void);
	///Get the identifier of the zone, which was calculated
	int get_id_calculated_zone(void);

	///Interpolate the damage function for the given impact value
	double interpolate_function(const double impact_value, const double vh, const double velocity, const double waterlevel_rise);

	///Output the members of the damage function to display/console
	void output_member(void);

	///Create the database table for the general information of the damage functions
	static void create_function_table(QSqlDatabase *ptr_database);
	///Set the database table for the general information of the damage functions: it sets the table name and the name of the columns and allocate them
	static void set_function_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the general information of the damage functions
	static void delete_data_in_function_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the general information of the damage functions
	static void close_function_table(void);

	///Select and count the number of relevant damage functions in a database table
	static int select_relevant_functions_database(QSqlTableModel *results, const bool with_output = true);

	///Select and count the number of relevant damage functions in a database table by given identifier
	static int select_relevant_functions_database(QSqlTableModel *results, const int id);

	///Set the function to the plot widget
	void set_function2plot_widget(Sys_Plot_Wid *widget);


private:

	//members
	///Name of the peopel2risk function
	string name;
	///Index of the peopel2risk function. It represents the people2risk category.
	int index;
	///Impact type of the damage function
	_dam_impact_type impact_type;

	///Number of points
	int number_points;
	///Pointer to the points of the functions
	_dam_people_function_point *points;

	///vh-boundary for zone 1
	const double vh_bound_1;
	///v-boundary for zone 1
	const double v_bound_1;
	///ds/dt-boundary for zone 2
	const double dsdt_bound_2;
	///h-boundary for zone 2
	const double h_bound_2;

	///Identifier of the zone, which was calculated
	int id_zone;


	///Pointer to the table for the damage function points in a database
	static Tables *point_table;

	//methods
	///Transfer the damage function points to a database
	void transfer_function_point2database(QSqlDatabase *ptr_database, const int id_func);
	///Input the damage function points per database
	void input_function_points_perdatabase(const QSqlTableModel *results, const int index);

	///Allocate the damage function points
	void alloc_points(void);
	///Delete the damage function points
	void delete_points(void);

	///Check the damage function
	void check_function(void);

	///Create the database table for the damage functions points
	static void create_point_table(QSqlDatabase *ptr_database);
	///Set the database table for the damage functions points: it sets the table name and the name of the columns and allocate them
	static void set_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the damage functions points
	static void delete_data_in_point_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the damage functions points
	static void close_point_table(void);

	///Select and count the number of relevant points for the damage function in a database table
	static int select_relevant_function_points_database(QSqlTableModel *results, const int index);

	///Check if a category id is already in use; it this is the case return a new category id
	int check_category_id_is_used(QSqlDatabase *ptr_database, const int id);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif
