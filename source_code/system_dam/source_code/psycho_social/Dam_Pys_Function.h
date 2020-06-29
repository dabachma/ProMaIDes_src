#pragma once
/**\class Dam_Pys_Function
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef DAM_PYS_FUNCTION_H
#define DAM_PYS_FUNCTION_H


//dam-libs
#include "_Dam_2value_Function.h"

///Enumerator for specifiying the pscho-social criteria
enum _dam_pys_criteria{
	///Criteria for the age class 50 to 59 years
	crit_age_50_59,
	///Criteria for the age class greater 80 years
	crit_age_80,
	///Criteria for the percentage of female gender
	crit_female,
	///Criteria for the percentage of home owners
	crit_owner,
};



///Class of the psycho-social score functions \ingroup dam
/**

*/
class Dam_Pys_Function : public _Dam_2value_Function
{
public:
	///Default constructor
	Dam_Pys_Function(void);
	///Default destructor
	~Dam_Pys_Function(void);


	//members
	///Pointer to the table for the psycho-social score-function general information in a database
	static Tables *function_table;


	//method
	//Read in the score function per file
	void read_function(ifstream *ifile, int *line_counter);
	///Transfer the pscho-social score function data to a database: the general settings of the functions as well as the function points
	void transfer_input_members2database(const int id_glob, QSqlTableModel *model, QSqlDatabase *ptr_database, const int category_id);
	///Input the the pscho-social score function data per database: the general settings of the functions as well as the function points
	void input_function_perdatabase(const QSqlTableModel *results, const int glob_index);


	//Output the members of the score-function to display/console
	void output_member(void);

	///Create the database table for the general information of the psycho-social score-functions
	static void create_function_table(QSqlDatabase *ptr_database);
	///Set the database table for the general information of the psycho-social score-functions: it sets the table name and the name of the columns and allocate them
	static void set_function_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the general information of the psycho-social score-functions
	static void delete_data_in_function_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the general information of the psycho-social score-functions
	static void close_function_table(void);

	///Get the maximum value of the global index of the psycho-social category function database table
	static int get_max_glob_id_category_function_table(QSqlDatabase *ptr_database);

	///Select and count the number of relevant psycho-social categorye functions in a database table
	static int select_relevant_functions_database(QSqlTableModel *results, const int category_id, const bool with_output = true);

	///Convert a string into a psycho-social criteria (_dam_pys_criteria)
	static _dam_pys_criteria convert_txt2criteria(const string txt);
	///Convert a psycho-social criteria (_dam_pys_criteria) into a string 
	static string convert_criteria2txt(const _dam_pys_criteria criteria);

	///Get the criteria type of the function
	_dam_pys_criteria get_criteria_type(void);


private:

	//member
	///Gloabl index in the database of the function
	int global_index;


	///Pointer to the table for the psycho-social score-function points in a database
	static Tables *point_table;

	///The criteria type of the function
	_dam_pys_criteria criteria;

	//methods

	///Transfer the psycho-social criteria function points to a database
	void transfer_function_point2database(QSqlDatabase *ptr_database, const int function_id);
	///Input the psycho-social criteria function points per database
	void input_function_points_perdatabase(const QSqlTableModel *results, const int index);

	///Create the database table for the psycho-social score-functions points
	static void create_point_table(QSqlDatabase *ptr_database);
	///Set the database table for the psycho-social score-functions points: it sets the table name and the name of the columns and allocate them
	static void set_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the psycho-social score-functions points
	static void delete_data_in_point_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the psycho-social score-functions points
	static void close_point_table(void);

	///Select and count the number of relevant points for the psycho-social criteria function in a database table
	static int select_relevant_function_points_database(QSqlTableModel *results, const int index);
	///Get the maximum value of the global index of the psycho-social criteria function point database table
	static int get_max_glob_id_criteria_function_point_table(QSqlDatabase *ptr_database);

	///Check the function points
	void check_function(void);

	///Set error(s)
	Error set_error(const int err_type);
	//Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif
