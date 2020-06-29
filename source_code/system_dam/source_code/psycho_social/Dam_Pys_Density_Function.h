#pragma once
/**\class Dam_Pys_Density_Functio
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef DAM_PYS_DENSITY_FUNCTION_H
#define DAM_PYS_DENSITY_FUNCTION_H

//dam-libs
#include "_Dam_2value_Function.h"

///Class for the psycho-social population-density factor function \ingroup dam
/**

*/
class Dam_Pys_Density_Function : public _Dam_2value_Function
{
//class for displaying the data
friend class Sys_Data_Tree_Wid;

public:
	///Default constructor
	Dam_Pys_Density_Function(void);
	///Default destructor
	~Dam_Pys_Density_Function(void);

	//method
	///Read in the population-density function per file
	void read_function(ifstream *ifile, int *line_counter);
	///Transfer the density function data to a database: the function points
	void transfer_input_members2database(QSqlDatabase *ptr_database);
	///Input the density function data per database
	void input_function_perdatabase(QSqlDatabase *ptr_database);

	///Calculate and get the density factor
	double calculate_density_factor(const double pop_density);

	///Get the flag, if the function is set
	bool get_function_is_set(void);


	///Output the members of the population-density function to display/console
	void output_member(void);

	///Create the database table for the psycho-social density-functions points
	static void create_point_table(QSqlDatabase *ptr_database);
	///Set the database table for the psycho-social density-functions points: it sets the table name and the name of the columns and allocate them
	static void set_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the psycho-social density-functions points
	static void delete_data_in_point_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the psycho-social density-functions points
	static void close_point_table(void);

	///Select the points and count the number of points of the psycho-social density function in a database table
	static int select_relevant_function_points_database(QSqlTableModel *results, const bool with_output = true);


private:

	//member

	///Pointer to the table for the psycho-social density-function points in a database
	static Tables *point_table;

	///Density factor
	double density_factor;

	///Flag if the density function is set
	bool function_is_set;

	//methods
	///Set error(s)
	Error set_error(const int err_type);
	//Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif
