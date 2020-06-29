#pragma once
/**\class Fpl_Distribution_Class_Mean_Mean
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_DISTRIBUTION_CLASS_MEAN_MEAN_H
#define FPL_DISTRIBUTION_CLASS_MEAN_MEAN_H

//system fpl
#include "_Fpl_Distribution_Class.h"
#include "Fpl_Distribution_Class_Mean.h"

///Data structure for the predefined default mean2mean type distributions variables \ingroup fpl
struct _fpl_default_mean2mean{
	///Name of the variable
	string variable_name;
	///Default name 
	string default_name;
	///Description 
	string description;
	///Mean value of first (leading) distribution
	double mean_1;
	///Second moment of first (leading) distribution
	double second_moment_1;
	///Constant of first (leading) distribution
	double constant_1;
	///Mean type distribution type of first (leading) distribution
	_fpl_mean_distribution_type type_1;
	///Variation coefficient of second (subordinated) distribution
	double var_coef_2;
	///Constant of second (subordinated) distribution
	double constant_2;
	///Mean type distribution type of second (subordinated) distribution
	_fpl_mean_distribution_type type_2;
};

///Class for a distribution combination of two mean type distributions \ingroup fpl
/**
The first (leading) mean-type distribution is evaluated, this result give the new mean 
value of the sceond (subordinated) mean type distribution (dynamic mean setting).
\see Fpl_Distribution_Class_Mean
*/
class Fpl_Distribution_Class_Mean_Mean : public _Fpl_Distribution_Class
{

public:
	///Default constructor
	Fpl_Distribution_Class_Mean_Mean(void);
	///Constructor with all parameters given
	Fpl_Distribution_Class_Mean_Mean(_fpl_mean_distribution_type type1, const double mean1, const double second_moment1, const double constant1, const bool calculated_mean1, _fpl_mean_distribution_type type2, const double var_koef_2, const double constant2, const _fpl_boundaries boundaries);
	///Default destructor
	~Fpl_Distribution_Class_Mean_Mean(void);

	//members
	///Pointer to the table for storing the information about the mean2mean distribution types (user-values)
	static Tables *table;
	///Pointer to the table for storing the information about the mean2mean distribution types (default-values)
	static Tables *default_table;

	//methods
	///Set the database table for the parameters of the mean2mean distribution types (user-values): it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Create the database table for the parameters of the mean2mean distribution types (user-values)
	static void create_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the parameters of the mean2mean distribution types (user-values)
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Delete data in the database table for the parameters of the mean2mean distribution types (user-values) for a given random variable id
	static void delete_data_in_table(QSqlDatabase *ptr_database, const int rand_id);

	///Set the database table for the parameters of the mean2mean distribution types (default-values): it sets the table name and the name of the columns and allocate them
	static void set_default_table(QSqlDatabase *ptr_database);
	///Create the database table for the parameters of the mean2mean distribution types (default-values)
	static bool create_default_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the parameters of the mean2mean distribution types (default-values)
	static void delete_data_in_default_table(QSqlDatabase *ptr_database);
	///Prepare and write the predefined default variables to the database table
	static void write_default_variables2table(QSqlDatabase *ptr_database);

	///Close and delete the database table for the parameters of the mean2mean distribution types the user-value-tables as well as the default table
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

	///Output the member of the distribution type to console/display
	void output_member(bool detailed=true);

	///Generate for the distribution a deterministic value (mean_value+constant of the first distribution)
	_fpl_result make_deterministic(void);
	///Generate for the distribution a random distributed value
	_fpl_result make_random(void);
	

	///Statistic of the generation process: mean, standard deviation and minimum and maximum values are calculated for multiple random generations of the variable
	void check_statistic(void);
	///Statistic of the generation process: output it to console/display
	void output_statistic(const bool calc_flag, bool detailed=true);
	///Reset the values for checking the statistics
	void reset_statistic(void);

	///Set a new mean value of the distribution (dynamic mean-setting of the first distribution)
	void set_new_middle(const double new_mean1);


private:
	//members
	///Pointer to the first (leading) mean type distribution
	Fpl_Distribution_Class_Mean *distribution1;
	///Pointer to the second (subordinated) mean type distribution
	Fpl_Distribution_Class_Mean *distribution2;
	

	//methods
	///Allocate the first (leading) distribution
	void allocate_first_distribution(void);
	///Allocate the first (leading) distribution with all parameters given
	void allocate_first_distribution(_fpl_mean_distribution_type type1, const double mean1, const double second_moment1, const double constant1, const bool calculated_mean1, const _fpl_boundaries boundaries);
	///Delete the first (leading) distribution
	void delete_first_distribution(void);

	///Allocate the second (subordinated) distribution
	void allocate_second_distribution(void);
	///Allocate the second (subordinated) distribution with all parameters given
	void allocate_second_distribution(const double mean1, _fpl_mean_distribution_type type2, const double var_koef_2, const double constant2, const _fpl_boundaries boundaries);
	///Delete the second (subordinated) distribution
	void delete_second_distribution(void);

	///Find the key-values for the file input of the distribution type
	void find_key_values_file(const string myline, int *must_found_counter, const bool calculated_flag, _fpl_default_mean2mean *fill_it);


	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif