#pragma once
/**\class Fpl_Distribution_Class_Discret
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_DISTRIBUTION_CLASS_DISCRET_H
#define FPL_DISTRIBUTION_CLASS_DISCRET_H

//system fpl
#include "_Fpl_Distribution_Class.h"

//struct
///Data structure for storing the discrete values and their probabilities \ingroup fpl
struct _fpl_discrete_dist_point{
	///Absolute discrete value
	double abs_discrete_value;
	///Relative discrete values
	double rel_discrete_value;
	///Calculated probability
	double calc_prob;
	///Probability
	double probability;
};

///Data structure for the predefined default discrete type distributions variables
struct _fpl_default_discrete{
	///Name of the variable
	string variable_name;
	///Default name 
	string default_name;
	///Description 
	string description;
	///Number of discrete points
	int number;
	///Discrete points
	_fpl_discrete_dist_point *points;
};

//forward declaration
struct _fpl_discret2mean_data;

///Class for a discrete type distribution \ingroup fpl
/**
A discrete type distribution is characterizised by several discrete values and their corresponding
probabilities. The sum of probabilities have to be 100%. Each probability has to be between 0 and 100%.
*/
class Fpl_Distribution_Class_Discret : public _Fpl_Distribution_Class
{
///Classes, which combines a discrete with a mean-type distributions
friend class Fpl_Distribution_Class_Discret_Mean;


public:
	///Default constructor
	Fpl_Distribution_Class_Discret(void);
	///Constructor with all parameters given
	Fpl_Distribution_Class_Discret(const int number_points, _fpl_discrete_dist_point *points, const bool calculated_middle, const _fpl_boundaries boundaries);
	///Default destructor
	~Fpl_Distribution_Class_Discret(void);

	//member
	///Pointer to the table for storing the information about the discrete distribution types (user-values)
	static Tables *table;
	///Pointer to the table for storing the information about the discrete distribution types (default-values)
	static Tables *default_table;

	//methods
	///Set the database table for the parameters of the discrete distribution types (user-values): it sets the table name and the name of the columns and allocate them
	static void set_table( QSqlDatabase *ptr_database);
	///Create the database table for the parameters of the discrete distribution types (user-values)
	static void create_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the parameters of the discrete distribution types (user-values)
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Delete data in the database table for the parameters of the discrete distribution types (user-values) for a given random variable id
	static void delete_data_in_table(QSqlDatabase *ptr_database, const int rand_id);
	
	///Set the database table for the parameters of the discrete distribution types (default-values): it sets the table name and the name of the columns and allocate them
	static void set_default_table(QSqlDatabase *ptr_database);
	///Create the database table for the parameters of the discrete distribution types (default-values)
	static bool create_default_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the parameters of the discrete distribution types (default-values)
	static void delete_data_in_default_table(QSqlDatabase *ptr_database);
	///Prepare and write the predefined default variables to the database table
	static void write_default_variables2table(QSqlDatabase *ptr_database);

	///Close and delete the database table for the parameters of the discrete distribution types the user-value-tables as well as the default table
	static void close_table(void);

	///Copy the parameters of the distribution type in the database table (static)
	static void copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id);


	///Set the parameter via console
	void set_input(void);
	///Set the input values per given parameters
	void set_input(const int number_points, _fpl_discrete_dist_point *points, const bool calculated_middle, const _fpl_boundaries boundaries);
	///Set the input values per given parameters
	void set_input(const int number_points, _fpl_discret2mean_data *points, const bool calculated_middle, const _fpl_boundaries boundaries);
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

	///Generate for the distribution a deterministic value (mid_value+constant)
	_fpl_result make_deterministic(void);
	///Generate for the distribution a random distributed value
	_fpl_result make_random(void);

	///Statistic of the generation process: mean, standard deviation and minimum and maximum values are calculated for multiple random generations of the variable
	void check_statistic(void);
	///Statistic of the generation process: output it to console/display
	void output_statistic(const bool calc_flag, bool detailed=true);
	///Reset the values for checking the statistics
	void reset_statistic(void);
	
	///Set a new mid value of the distribution (dynamic mid-setting)
	void set_new_middle(const double new_middle);

	///Get calculated exact probability for the distribution for the random generator check
	double get_calulculated_exact_prob(const double value, const double stepsize);

	//Get the index of the result (relevant by discret and discret2mean distribution type)
	int get_result_index(void);

	///Get the number of points (relevant by discret and discret2mean distribution type)
	int get_number_point(void);
	///Get the value per given point index  (relevant by discret and discret2mean distribution type)
	double get_point_value(const int index);
	
private:
	//members
	///Number of discrete values
	int number_points;
	///Points of the discrete distribution
	_fpl_discrete_dist_point *discret_points;
	///Absolute mid value of the discrete distribution; required for the dynamic mid-value setting
	double absolut_middle;

	///Index of the result
	int result_index;

	//methods 

	///Allocate the discrete points
	void allocate_discrete_points(void);
	///Delete discrete points
	void delete_discrete_points(void);
	///Check the parameter of the distribution type
	void check_input(const bool calculate_middle);

	///Find the key-values for the file input of the distribution type
	void find_key_values_file(const string myline, int *must_found_counter, const bool calculated_flag);


	///Convert the values from relative values to absolute values; required for the dynamic mid-value setting
	void relativ_to_absolut(void);
	
	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif