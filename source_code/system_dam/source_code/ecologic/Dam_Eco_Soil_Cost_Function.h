#pragma once
/**\class Dam_Eco_Soil_Cost_Function
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef DAM_ECO_SOIL_COST_FUNCTION_H
#define DAM_ECO_SOIL_COST_FUNCTION_H

//dam-libs
#include "_Dam_2value_Function.h"



///Class for evaluating the costs of a soil-type via a function.\ingroup dam
/**
The costs in this function depends on the erosion-grade.
\see Dam_Eco_Soil_Erosion_Function
*/
class Dam_Eco_Soil_Cost_Function : public _Dam_2value_Function
{
//class for displaying the data
friend class Sys_Data_Tree_Wid;
//class for displaying the data in a widget
friend class DamGui_Eco_Soil_Cost_Wid;

public:
	///Default constructor
	Dam_Eco_Soil_Cost_Function(void);
	///Default destructor
	~Dam_Eco_Soil_Cost_Function(void);


	//members
	///Pointer to the table for the soil-cost function general information in a database
	static Tables *function_table;

	//method
	///Read in the damage function per file
	void read_function(ifstream *ifile, int *line_counter);
	///Transfer the soil-cost function data to a database: the general settings of the functions as well as the function points
	void transfer_input_members2database(QSqlDatabase *ptr_database);
	///Add soil-type data to a database: the general settings of the functions (soil-type information) as well as the function points
	void add_input_members2database(QSqlDatabase *ptr_database);
	///Input the soil-cost function data per database: the general settings of the functions as well as the function points
	void input_function_perdatabase(const QSqlTableModel *results, const int glob_index);

	///Get the index of the soil-cost function
	int get_index(void);
	///Get the name of the soil-cost function
	string get_name(void);
	///Output the members of the soil-cost function to display/console
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

	///Select and count the number of relevant damage functions in a database table by a given function index
	static int select_relevant_functions_database(QSqlTableModel *results, const int index);

private:

	//members
	///Index of the soil-cost function
	int index;
	///Name of the soil type
	string name;


	///Pointer to the table for the soil-cost function points in a database
	static Tables *point_table;


	//methods
	///Transfer the soil-cost function points to a database
	void transfer_function_point2database(QSqlDatabase *ptr_database, const int id_func);
	///Input the soil-cost function points per database
	void input_function_points_perdatabase(const QSqlTableModel *results, const int index);

	///Create the database table for the damage functions points
	static void create_point_table(QSqlDatabase *ptr_database);
	///Set the database table for the damage functions points: it sets the table name and the name of the columns and allocate them
	static void set_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the damage functions points
	static void delete_data_in_point_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the damage functions points
	static void close_point_table(void);

	///Select and count the number of relevant points for the soil-cost function in a database table
	static int select_relevant_function_points_database(QSqlTableModel *results, const int index);

	///Check if a soil-cost-type id is already in use; it this is the case return a new soil-cost-type id
	int check_soil_cost_type_id_is_used(QSqlDatabase *ptr_database, const int id);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};

#endif