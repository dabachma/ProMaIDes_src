#pragma once
/**\class Fpl_Distribution_Class_Mean
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_DISTRIBUTION_CLASS_MEAN_H
#define FPL_DISTRIBUTION_CLASS_MEAN_H

//system fpl
#include "_Fpl_Distribution_Class.h"

//enum
///Enumerator for the mean distribution types \ingroup fpl
enum _fpl_mean_distribution_type : short{
	///Mean distribution type: deterministic
	deterministic_type,
	///Mean distribution type: uniform
	uniform_type,
	///Mean distribution type: normal
	normal_type,
	///Mean distribution type: exponentiel
	exponentiel_type,
	///Mean distribution type: log-normal
	log_normal_type,
	///Mean distribution type: gumbel
	gumbel_type,
	///Mean distribution type: weibull
	weibull_type,
	///Unknown mean distribution type
	unknown_mean_type
};

///Data structure for the predefined default mean type distributions variables \ingroup fpl
struct _fpl_default_mean{
	///Name of the variable
	string variable_name;
	///Default name 
	string default_name;
	///Description 
	string description;
	///Mean value
	double mean;
	///Second moment
	double second_moment;
	///Constant
	double constant;
	///Mean type distribution type
	_fpl_mean_distribution_type type;
};

///Class for a mean type distribution \ingroup fpl
/**
A mean type distribution is characterizised by a mean value, a standard deviation and a constant.
Moreover a mean-distribution type specifies in detail which transformation is required. 
Following mean-distribution types are implemented:
	- deterministic
	- uniform
	- normal
	- exponentiel
	- log-normal
	- gumbel
	- weibull (attention here alpha:=mean and beta:=std_dev)

*/
class Fpl_Distribution_Class_Mean : public _Fpl_Distribution_Class  
{
///Classes, which combines two mean-type distributions
friend class Fpl_Distribution_Class_Mean_Mean;
///Classes, which combines a discrete with a mean-type distributions
friend class Fpl_Distribution_Class_Discret_Mean;

public:
	///Default constructor
	Fpl_Distribution_Class_Mean(void);
	///Constructor with all parameters given
	Fpl_Distribution_Class_Mean(const double var_mean , const double second_moment, const double var_constant, const bool calculated_mean, const _fpl_boundaries boundaries, _fpl_mean_distribution_type type);
	///Default destructor
	~Fpl_Distribution_Class_Mean(void);

	//members
	///Pointer to the table for storing the information about the mean distribution types (user-values)
	static Tables *table;
	///Pointer to the table for storing the information about the mean distribution types (default-values)
	static Tables *default_table;

	//methods
	///Set the database table for the parameters of the mean distribution types (user-values): it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Create the database table for the parameters of the mean distribution types (user-values)
	static void create_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the parameters of the mean distribution types (user-values)
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Delete data in the database table for the parameters of the mean distribution types (user-values) for a given random variable id
	static void delete_data_in_table(QSqlDatabase *ptr_database, const int rand_id);

	///Set the database table for the parameters of the mean distribution types (default-values): it sets the table name and the name of the columns and allocate them
	static void set_default_table(QSqlDatabase *ptr_database);
	///Create the database table for the parameters of the mean distribution types (default-values)
	static bool create_default_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the parameters of the mean distribution types (default-values) and restore predefined values
	static void delete_data_in_default_table(QSqlDatabase *ptr_database);
	///Prepare and write the predefined default variables to the database table
	static void write_default_variables2table(QSqlDatabase *ptr_database);

	///Close and delete the database table for the parameters of the mean distribution types the user-value-tables as well as the default table
	static void close_table(void);

	///Copy the parameters of the distribution type in the database table (static)
	static void copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id);

	///Set the parameter via console
	void set_input(void);
	///Set the input values per given parameters
	void set_input(const double var_mean , const double second_moment, const double var_constant, const bool calculated_mean, const _fpl_boundaries boundaries, _fpl_mean_distribution_type type);
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

	///Generate for the distribution a deterministic value (mean_value+constant)
	_fpl_result make_deterministic(void);
	///Generate for the distribution a random distributed value
	_fpl_result make_random(void);

	///Statistic of the generation process: output it to console/display
	void output_statistic(const bool calc_flag, bool detailed=true);

	///Set a new mean value of the distribution (dynamic mean-setting)
	void set_new_middle(const double new_mean);

	///Convert the mean distribution type (_fpl_mean_distribution_type) to a string
	static string convert_meandist_type2txt(_fpl_mean_distribution_type type);
	///Convert a string to the mean distribution type (_fpl_mean_distribution_type)
	static _fpl_mean_distribution_type convert_txt2meandist_type(const string txt);
	///Convert an integer to the mean distribution type (_fpl_mean_distribution_type)
	static _fpl_mean_distribution_type convert_int2meandist_type(const int type);

	///Get calculated exact probability for the distribution for the random generator check
	double get_calulculated_exact_prob(const double value, const double stepsize);

	///Get mean distribution type as text
	string get_mean_distribution_type(void);
	///Get the mean-value
	double get_mean_value(void);
	///Get the standard deviation
	double get_std_dev(void);
	///Get the variation coefficient
	double get_var_coeff(void);
	///Get constant
	double get_constant(void);
	
private:
	//member
	///Mean value of the distribution
	double mean;
	///Standard deviation
	double std_dev;
	///Variation coefficient; required for the dynamic mean-value setting
	double var_koef;
	///Constant of the distribution
	double constant;
	///Type of the mean distribution classes
	_fpl_mean_distribution_type type_distribution;

	//method

	///Check the parameter of the distribution type
	void check_input(bool calculated_mean);

	///Find the key-values for the file input of the distribution type
	void find_key_values_file(const string myline, int *must_found_counter, const bool calculated_flag, bool *second_moment_found);
	
	///Calculate the variation coefficient with the standard deviation; required for the dynamic mean-value setting
	void calculate_varkoef(void);
	///Calculate the standard deviation with the variation coefficient; required for the dynamic mean-value setting
	void calculate_stddev(void);

	///Transform the variable corresponding the distribution type: uniform distribution
	double uniformly_dist(void);
	///Transform the variable corresponding the distribution type: normal distribution with the Box-Mueller-algorithm
	double normal_dist(void);
	///Transform the variable corresponding the distribution type: exponential distribution
	double expo_dist(void);
	///Transform the variable corresponding the distribution type: log-normal distribution with the Box-Mueller-algorithm
	double lognormal_dist(void);
	///Transform the variable corresponding the distribution type: gumbel distribution
	double gumbel_dist(void);
	///Transform the variable corresponding the distribution type: weibull distribution
	double weibull_dist(void);

	///Direct calculation of the distribution type for checking the random- and transformation process: deterministic
	double direct_deterministic_dist(const double value, const double stepsize);
	///Direct calculation of the distribution type for checking the random- and transformation process: uniform distribution
	double direct_uniformly_dist(const double value);
	///Direct calculation of the distribution type for checking the random- and transformation process: normal distribution
	double direct_normal_dist(const double value);
	///Direct calculation of the distribution type for checking the random- and transformation process: exponential distribution
	double direct_expo_dist(const double value);
	///Direct calculation of the distribution type for checking the random- and transformation process: log-normal distribution 
	double direct_lognormal_dist(const double value);
	///Direct calculation of the distribution type for checking the random- and transformation process: gumbel distribution
	double direct_gumbel_dist(const double value);
	///Direct calculation of the distribution type for checking the random- and transformation process: weibull distribution
	double direct_weibull_dist(const double value);



	
	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);		
};
#endif
