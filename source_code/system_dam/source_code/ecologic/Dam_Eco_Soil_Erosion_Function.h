#pragma once
/**\class Dam_Eco_Soil_Erosion_Function
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef DAM_ECO_SOIL_EROSION_FUNCTION_H
#define DAM_ECO_SOIL_EROSION_FUNCTION_H

//dam-libs
#include "_Dam_2value_Function.h"

///Class for the evaluation of the erosion grade via a function\ingroup dam
/**
In general, on
*/
class Dam_Eco_Soil_Erosion_Function : public _Dam_2value_Function
{
//class for displaying the data
friend class Sys_Data_Tree_Wid;
//class for displaying the data in a widget
friend class DamGui_Eco_Soil_Erosion_Wid;

public:
	///Default constructor
	Dam_Eco_Soil_Erosion_Function(void);
	///Default destructor
	~Dam_Eco_Soil_Erosion_Function(void);


	//method
	///Read in the soil-erosion function per file
	void read_function(ifstream *ifile, int *line_counter);
	///Transfer the soil-erosiont function data to a database: the general settings of the functions as well as the function points
	void transfer_input_members2database(QSqlDatabase *ptr_database);
	///Input the soil-erosion function data per database: the general settings of the functions as well as the function points
	void input_function_perdatabase(const QSqlTableModel *results, const int glob_index);

	///Output the members of the soil-erosion function to display/console
	void output_member(void);

	///Get the impact type of the soil-erosion function
	_dam_impact_type get_impact_type(void);

	///Get the index of the soil-erosion function
	int get_index(void);
	///Get the name of the soil-erosion function
	string get_name(void);

	///Create the database table for the general information of the soil-erosion functions
	static void create_function_table(QSqlDatabase *ptr_database);
	///Set the database table for the general information of the soil-erosion functions: it sets the table name and the name of the columns and allocate them
	static void set_function_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the general information of the soil-erosion functions
	static void delete_data_in_function_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the general information of the soil-erosion functions
	static void close_function_table(void);

	///Select and count the number of relevant soil-erosion functions in a database table
	static int select_relevant_functions_database(QSqlTableModel *results, const bool with_output = true);

	///Select and count the number of relevant soil-erosion functions in a database table by a given function id
	static int select_relevant_functions_database(QSqlTableModel *results, const int index);


private:

	///Index of the soil-erosion function
	int index;
	///Name of the soil erosion
	string name;
	///Impact type for the impact values of the functions
	_dam_impact_type impact_type;

	///Pointer to the table for the soil-erosion function general information in a database
	static Tables *function_table;
	///Pointer to the table for the soil-erosion function points in a database
	static Tables *point_table;

	//methods
	///Transfer the soil-erosion function points to a database
	void transfer_function_point2database(QSqlDatabase *ptr_database, const int id_func);
	///Input the soil-erosion function points per database
	void input_function_points_perdatabase(const QSqlTableModel *results, const int index);

	///Create the database table for the soil-erosion functions points
	static void create_point_table(QSqlDatabase *ptr_database);
	///Set the database table for the soil-erosion functions points: it sets the table name and the name of the columns and allocate them
	static void set_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the soil-erosion functions points
	static void delete_data_in_point_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the soil-erosion functions points
	static void close_point_table(void);

	///Select and count the number of relevant points for the soil-cost function in a database table
	static int select_relevant_function_points_database(QSqlTableModel *results, const int index);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif
