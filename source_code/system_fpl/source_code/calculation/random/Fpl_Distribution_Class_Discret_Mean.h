#pragma once
/**\class Fpl_Distribution_Class_Discret_Mean
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_DISTRIBUTION_CLASS_DISCRET_MEAN_H
#define FPL_DISTRIBUTION_CLASS_DISCRET_MEAN_H

//system fpl
#include "_Fpl_Distribution_Class.h"
#include "Fpl_Distribution_Class_Mean.h"
#include "Fpl_Distribution_Class_Discret.h"


//struct
///Data structure for gathering the parameter of the distrubutions used in the discrte2mean type distribution
struct _fpl_discret2mean_data{
	///The discrete points  of the leading discrete distribution
	_fpl_discrete_dist_point discret_point;
	///The mean value of the subordinate mean type distribution 
	double mean;
	///The second moment of the subordinate mean type distribution
	double second_moment;
	///The constant of the subordinate mean type distribution
	double constant;
	///Mean-distribution type of the subordinate mean type distribution
	_fpl_mean_distribution_type mean_type;
};

///Data structure for the predefined default discrete2mean type distributions variables \ingroup fpl
struct _fpl_default_discrete2mean{
	///Name of the variable
	string variable_name;
	///Default name 
	string default_name;
	///Description 
	string description;
	///Number of distributions
	int number;
	///Distribution and their discrete points
	_fpl_discret2mean_data *distributions;
};

///Class for a distribution combination of a discrete type distributions to multiple mean type distribution \ingroup fpl
/**
The first (leading) type distribution is a dicrete distribution. The evauluated 
discrete values decide the secondary (subordinated) mean type distribution.

\see Fpl_Distribution_Class_Mean, Fpl_Distribution_Class_Discret
*/
class Fpl_Distribution_Class_Discret_Mean : public _Fpl_Distribution_Class
{

public:
	///Default constructor
	Fpl_Distribution_Class_Discret_Mean(void);
	///Default destructor
	~Fpl_Distribution_Class_Discret_Mean(void);
	
	//members
	///Pointer to the table for storing the information about the discrete2mean distribution types (user-values)
	static Tables *table;
	///Pointer to the table for storing the information about the discrete2mean distribution types (default-values)
	static Tables *default_table;

	//methods
	///Set the database table for the parameters of the discrete2mean distribution types (user-values): it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Create the database table for the parameters of the discrete2mean distribution types (user-values)
	static void create_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the parameters of the discrete2mean distribution types (user-values)
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Delete data in the database table for the parameters of the discrete2mean distribution types (user-values) for a given random variable id
	static void delete_data_in_table(QSqlDatabase *ptr_database, const int rand_id);

	///Set the database table for the parameters of the discrete2mean distribution types (default-values): it sets the table name and the name of the columns and allocate them
	static void set_default_table(QSqlDatabase *ptr_database);
	///Create the database table for the parameters of the discrete2mean distribution types (default-values)
	static bool create_default_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the parameters of the discrete2mean distribution types (default-values)
	static void delete_data_in_default_table(QSqlDatabase *ptr_database);
	///Prepare and write the predefined default variables to the database table
	static void write_default_variables2table(QSqlDatabase *ptr_database);

	///Close and delete the database table for the parameters of the discrete2mean distribution types the user-value-tables as well as the default table
	static void close_table(void);

	///Copy the parameters of the distribution type in the database table (static)
	static void copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id);



	///Set the parameter via console
	void set_input(void);
	///Set the parameter with given parameter
	void set_input(const int number_dist, _fpl_discret2mean_data *data);
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

	///Generate for the distribution a deterministic value (weighted middle of the mean values + constant)
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
	void set_new_middle(const double new_middle);

	///Get the index of the result (relevant by discret and discret2mean distribution type)
	int get_result_index(void);

	///Get the number of points (relevant by discret and discret2mean distribution type)
	int get_number_point(void);
	///Get the value per given point index  (relevant by discret and discret2mean distribution type)
	double get_point_value(const int index);

private:
	//members
	///Number of discrete distribution
	int number_distribution;

	///Pointer to the discrete distribution
	Fpl_Distribution_Class_Discret *distribution;
	///Pointer to the mean type distribution, which consists in general of several distributions
	Fpl_Distribution_Class_Mean *distribution_array;

	///Index of the mean type distribution, which is hit for one generation step 
	int hit_number;

	
	//methods
	///Allocate the leading discrete distribution
	void allocate_discrete_distribution(void);
	///Delete the leading discrete distribution
	void delete_discrete_distribution(void);
	///Allocate the second mean type distribution(s)
	void allocate_mean_type_distributions(void);
	///Delete the second mean type distribution(s)
	void delete_mean_type_distributions(void);

	///Find the key-values for the file input of the distribution type
	void find_key_values_file(const string myline, int *must_found_counter, const bool calculated_flag);


	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif