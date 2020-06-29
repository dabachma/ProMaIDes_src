#pragma once
/**\class Dam_Eco_Btype_Damage_Function
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef DAM_ECO_BTYPE_DAMAGE_FUNCTION_H
#define DAM_ECO_BTYPE_DAMAGE_FUNCTION_H


//dam-libs
#include "Dam_Impact_Values.h"
#include "_Dam_2value_Function.h"

///Class for a damage function of one biotope-type\ingroup dam
/**
In general, one biotope-type has three damage functions depending to the impact type (_dam_impact_type):
	- waterlevel (IMPACT_H)
	- flow velocity (IMPACT_V)
	- duration of flood (IMPACT_D)
The discret given values can treated as an continuous, it means a linear interpolation is applied for value inbetween, or 
purely discret (step function).
*/
class Dam_Eco_Btype_Damage_Function: public _Dam_2value_Function
{
//class for displaying the function in a widget
friend class DamGui_Eco_Btype_Func_Wid;
public:
	///Default constructor
	Dam_Eco_Btype_Damage_Function(void);
	///Default destructor
	~Dam_Eco_Btype_Damage_Function(void);

	//methods
	///Read in the biotope-type damage function per file
	void read_function(ifstream *ifile, int *line_counter);
	///Transfer the biotope-type damage function data to a database: the general settings of the functions as well as the function points
	void transfer_input_members2database(const int id_glob, QSqlTableModel *model, QSqlDatabase *ptr_database, const int biotope_id);
	///Input the biotope-type damage function data per database: the general settings of the functions as well as the function points
	void input_function_perdatabase(const QSqlTableModel *results, const int glob_index);

	///Get the impact type of the function
	_dam_impact_type get_impact_type(void);
	///Get the weight of the vulnerability values of this impact type
	double get_weight_vul_value(void);

	///Get the weighted cost factor by a given impact value
	double get_weighted_cost_factor(Dam_Impact_Values *impact);

	///Output the members of the damage function to display/console
	void output_member(void);

	///Create the database table for the general information of the biotope-type damage functions
	static void create_function_table(QSqlDatabase *ptr_database);
	///Set the database table for the general information of the biotope-type damage functions: it sets the table name and the name of the columns and allocate them
	static void set_function_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the general information of the biotope-type damage functions
	static void delete_data_in_function_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the general information of the biotope-type damage functions
	static void close_function_table(void);
	///Get the maximum value of the global index of the biotope-type function database table
	static int get_max_glob_id_biotope_function_table(QSqlDatabase *ptr_database);

	///Select and count the number of relevant biotope-type damage functions in a database table
	static int select_relevant_functions_database(QSqlTableModel *results, const int biotope_id, const bool with_output = true);

private:

	//members
	///Impact type for the impact values of the functions
	_dam_impact_type impact_type;
	///Weight of the vulnerability values of this impact type for merging the vunerability to a total vulnerability
	double weight;

	///Pointer to the table for the biotope-type damage function general information in a database
	static Tables *function_table;
	///Pointer to the table for the biotope-type damage function points in a database
	static Tables *point_table;

	//methods
	///Transfer the biotope-type damage function points to a database
	void transfer_function_point2database(QSqlDatabase *ptr_database, const int function_id);
	///Input the biotope-type damage function points per database
	void input_function_points_perdatabase(const QSqlTableModel *results, const int index);

	///Create the database table for the biotope-type damage functions points
	static void create_point_table(QSqlDatabase *ptr_database);
	///Set the database table for the biotope-type damage functions points: it sets the table name and the name of the columns and allocate them
	static void set_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the biotope-type damage functions points
	static void delete_data_in_point_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the biotope-type damage functions points
	static void close_point_table(void);

	///Select and count the number of relevant points for the biotope-type damage function in a database table
	static int select_relevant_function_points_database(QSqlTableModel *results, const int index);
	///Get the maximum value of the global index of the biotope-type function point database table
	static int get_max_glob_id_biotope_function_point_table(QSqlDatabase *ptr_database);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};

#endif
