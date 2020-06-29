#pragma once
/**\class Dam_Ecn_Damage_Function
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef DAM_ECN_DAMAGE_FUNCTION_H
#define DAM_ECN_DAMAGE_FUNCTION_H


//sys-libs
#include "_Sys_Common_System.h"

//dam-lib
#include "Dam_Impact_Values.h"

///Data structure for gathering the data of a damage function point
struct _dam_ecn_function_point{
	///Impact value, like waterlevel or flow velocity (x-axis)
	double impact_value;
	///Corresponding relative damage [%] (y-axis)
	double rel_damage;
	///5%-quantile of the relative damage of this impact value
	double quant_5_rel_damage;
	///95%-quantile of the relative damage of this impact value
	double quant_95_rel_damage;
};
///Data structure for gathering the data of the land use information
struct _dam_ecn_landuse_info{
	///The id for specifying the land use category
	int id;
	///The name describing the land use category
	string name;
	///Default stock value for the land use category
	double default_stockvalue;
	///5%-quantile of the default stock value for the land use category
	double def_stock_5;
	///95%-quantile of the default stock value for the land use category
	double def_stock_95;
	
};

///Data structure for gathering the result data of the economical damage calculation
struct _dam_ecn_result{
	///Mid-value of the economical damage calculation result
    float mid_result;
	///5%quantile of the economical damage calculation result
    float quantile_5_result;
		///95%quantile of the economical damage calculation result
    float quantile_95_result;
};

///This class represents the damage functions for the calculation of the relative economical damage \ingroup dam
/**

*/
class Dam_Ecn_Damage_Function 
{

//class for displaying the data
friend class Sys_Data_Tree_Wid;
//class for displaying the data in a widget
friend class DamGui_Ecn_Func_Wid;

public:
	///Default constructor
	Dam_Ecn_Damage_Function(void);
	///Default destructor
	~Dam_Ecn_Damage_Function(void);

	//members
	///Pointer to the table for the damage function general information in a database
	static Tables *function_table;

	//methods
	///Read in the damage function per file
	void read_function(ifstream *ifile, int *line_counter, const int index);
	///Transfer the damage function data to a database: the general settings of the functions (land-use information) as well as the function points
	void transfer_input_members2database(QSqlDatabase *ptr_database);
	///Add damage function data to a database: the general settings of the functions (land-use information) as well as the function points
	void add_input_members2database(QSqlDatabase *ptr_database);
	///Input the damage function data per database: the general settings of the functions (land-use information) as well as the function points
	void input_function_perdatabase(const QSqlTableModel *results, const int glob_index);

	///Get the number of points 
	int get_number_points(void);
	///Get a pointer to the function points by a given index
	_dam_ecn_function_point* get_ptr_function_points(const int index);

	///Get the land-use information
	_dam_ecn_landuse_info get_land_use_info(void);
	///Get the impact type
	_dam_impact_type get_impact_type(void);

	///Interpolate the damage function for the given impact value
	_dam_ecn_function_point interpolate_function(const double impact_value);
	///Interpolate the damage function for the given impact value
	_dam_ecn_function_point interpolate_function(Dam_Impact_Values *impact);

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
	///Select and count the number of relevant damage functions in a database table by a given function id
	static int select_relevant_functions_database(QSqlTableModel *results, const int function_id);


	///Get the result per landuse id
	_dam_ecn_result get_result_landuse_id(void);

	///Calculate the damage per landuse id by using the calculated results in database
	void calculate_damage_landuse_id(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);

	///Output the sum over the land use id of the damage results to display/console
	void output_result_landuse_damage(ostringstream *cout);

private:

	//members
	///Information about the land-use category, e.g. name, id, and default stock value
	_dam_ecn_landuse_info landuse_info;
	///Number of points representing the damage function
	int no_points;
	///Type of the impact value (x-axis)
	_dam_impact_type impact_type;
	///Pointer to the points representing the damage function
	_dam_ecn_function_point *points;
	///Index of the damage function
	int index;

	///Result per landuse id
	_dam_ecn_result result;


	///Pointer to the table for the damage function points in a database
	static Tables *point_table;



	//method
	///Transfer the damage function points to a database
	void transfer_function_point2database(QSqlDatabase *ptr_database, const int id_func);
	///Input the damage function points per database
	void input_function_points_perdatabase(const QSqlTableModel *results, const int index);

	///Allocate the necessary damage function points, consisting of a value (corresponding to the impact value, e.g. waterlevel) and a relative damage [%]
	void alloc_points(void);
	///Delete the damage function points
	void delete_points(void);

	///Check the damage function
	void check_function(void);

	///Check the quantiles of the damage function points
	void check_quantiles(_dam_ecn_function_point *point);

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

	///Check if a land use id is already in use; it this is the case return a new land-use id
	int check_landuse_id_is_used(QSqlDatabase *ptr_database, const int id);


	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif
