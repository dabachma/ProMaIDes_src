#pragma once
/**\class Fpl_Distribution_Class_Triangle
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_DISTRIBUTION_CLASS_TRIANGLE_H
#define FPL_DISTRIBUTION_CLASS_TRIANGLE_H

//system fpl
#include "_Fpl_Distribution_Class.h"

///Data structure for the predefined default triangle type distributions variables \ingroup fpl
struct _fpl_default_triangle{
	///Name of the variable
	string variable_name;
	///Default name 
	string default_name;
	///Description 
	string description;
	///Mid value
	double mid;
	///Minimum
	double min;
	///Maximum
	double max;
	///Constant
	double constant;
};

///Class for a triangle type distribution \ingroup fpl
/**
A triangle type distribution is characterizised by a minmum, a middle, a maximum and a constant.
*/
class Fpl_Distribution_Class_Triangle : public _Fpl_Distribution_Class
{

public:
	///Default constructor
	Fpl_Distribution_Class_Triangle(void);
	///Constructor with all parameters given
	Fpl_Distribution_Class_Triangle(const double var_min , const double var_mid, const double var_max, const double var_constant, const _fpl_boundaries boundaries, const bool calculated_mid);
	///Default destructor
	~Fpl_Distribution_Class_Triangle(void);
	
	//member
	///Pointer to the table for storing the information about the triangle distribution types (user-values)
	static Tables *table;
	///Pointer to the table for storing the information about the triangle distribution types (default-values)
	static Tables *default_table; 
	
	//methods
	///Set the database table for the parameters of the triangle distribution types (user-values): it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Create the database table for the parameters of the triangle distribution types (user-values)
	static void create_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the parameters of the triangle distribution types (user-values)
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Delete data in the database table for the parameters of the triangle distribution types (user-values) for a given random variable id
	static void delete_data_in_table(QSqlDatabase *ptr_database, const int rand_id);


	///Set the database table for the parameters of the triangle distribution types (default-values): it sets the table name and the name of the columns and allocate them
	static void set_default_table(QSqlDatabase *ptr_database);
	///Create the database table for the parameters of the triangle distribution types (default-values)
	static bool create_default_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the parameters of the triangle distribution types (default-values) and restore the predefined values
	static void delete_data_in_default_table(QSqlDatabase *ptr_database);
	///Prepare and write the predefined default variables to the database table
	static void write_default_variables2table(QSqlDatabase *ptr_database);

	///Close and delete the database table for the parameters of the triangle distribution types the user-value-tables as well as the default table
	static void close_table(void);

	///Copy the parameters of the distribution type in the database table (static)
	static void copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id);


	///Set the parameter via console
	void set_input(void);
	///Set the parameter via database table for a user defined variable
	void set_input(const int position, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries);
	///Set the parameter via database table for a default variable
	void set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries);
	///Set the parameter via file for a user defined variable
	void set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries);

	///Transfer the data of the distribution type to database table
	void transfer_disttype2database(const int position, QSqlDatabase *ptr_database, const bool calculated_mean);

	///Set the input of the distribution type via dialog for checking the random generator and the transformation
	bool set_input_check(void);

	///Output the member of the distribution type to console/display
	void output_member(bool detailed=true);

	///Generate for the distribution a deterministic value (mid_value+constant)
	_fpl_result make_deterministic(void);
	///Generate for the distribution a random distributed value
	_fpl_result make_random(void);

	///Statistic of the generation process: output it to console/display
	void output_statistic(const bool calc_flag, bool detailed=true);

	///Set a new mid value of the distribution (dynamic mid-setting)
	void set_new_middle(const double new_mean);

	///Get calculated exact probability for the distribution for the random generator check
	double get_calulculated_exact_prob(const double value, const double stepsize);

private:
	//member
	///Mid-value of the triangle-distribution
	double mid;
	///Absolute minimum-value of the triangle-distribution
	double min;
	///Absolute maximum-value of the triangle-distribution
	double max;
	///Relative maximum-value of the triangle-distribution; required for the dynamic mid-value setting
	double max_rel;
	///Relative minimum-value of the triangle-distribution; required for the dynamic mid-value setting
	double min_rel;
	///Constant of the distribution
	double constant;
	

	//methods

	///Check the parameter of the distribution type
	void check_input(void);
	///Find the key-values for the file input of the distribution type
	void find_key_values_file(const string myline, int *must_found_counter, const bool calculated_flag);
	
	
	///Convert the absolute values (max/min) to relative values (max/min); required for the dynamic mid-value setting
	void absolut_to_relativ(void);
	///Convert the relative value (max/min) to absolute values (max/min); required for the dynamic mid-value setting
	void relativ_to_absolut(void);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);	
};

#endif