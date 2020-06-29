#pragma once
/**\class Fpl_Random_Variables
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_RANDOM_VARIABLES_H
#define FPL_RANDOM_VARIABLES_H

//qt lib
#include <QtSql> 

//system fpl
//for log, norm, expo, weibull etc: type, mean, std_dev, constant
#include "Fpl_Distribution_Class_Mean.h"
//for triangle: type (7), min, mid, max, constant
#include "Fpl_Distribution_Class_Triangle.h"
//for discrete: type (8)
#include "Fpl_Distribution_Class_Discret.h"
//for a combination of the distribution types (9) mean_type to mean_type
#include "Fpl_Distribution_Class_Mean_Mean.h"
//for a combination of or (10) discret to mean_types
#include "Fpl_Distribution_Class_Discret_Mean.h"

//system sys
#include "Common_Const.h"
#include "Sys_Project.h"


///Class for a random distributed variables for a reliability calculation  \ingroup fpl
/**
All variables which are randomly distributed are from this class (e.g. waterlevel, soil parameters etc).
The objective is to get an random value corresponding to the statistical moments and the type of distribution.
The random generator ranmar produces just uniformly distributed numbers between 0 and 1. To transform this numbers 
transformation methods are needed. They are depending on the distribution type. 
Following distribution types are implemented:
	- a mean type distribution, e.g. uniform, log-normal, normal etc. (Fpl_Distribution_Class_Mean)
	- a triangle distribution (Fpl_Distribution_Class_Triangle)
	- a discret	distribution (Fpl_Distribution_Class_Discret)
	- a combination of a mean type with a mean type distribution (Fpl_Distribution_Class_Mean_Mean)
	- a	combination of a discret with mean type distributions (Fpl_Distribution_Class_Discret_Mean)

This class decides which distribution type is chosen to generate the transformed output.
*/
class Fpl_Random_Variables
{

public:
	///Default constructor
	Fpl_Random_Variables(void);
	///Default destructor
	~Fpl_Random_Variables(void);

	//Members
	///Pointer to the distribution type
	_Fpl_Distribution_Class *distribution;
	///Pointer to the table for storing the information about the random variables
	static Tables *table;

	//methods
	///Set the database table for the parameters of the random variables: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Create the database table for the parameters of the random variables
	static void create_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the parameters of the random variables
	static void delete_data_in_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const int sec_id);
	///Delete data in the database table for the parameters of the random variables for a specific fpl-section
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the parameters of the random variables
	static void close_table(void);
	///Select and count the number of existing random variables in database table
	static int select_variables_database(QSqlQueryModel *results, const _sys_system_id id_sys, const int section_id, const string name_var, QSqlDatabase *ptr_database, const string mechanism, const string section_type);
	///Select and count the number of existing random variables in database table
	static int select_variables_database(QSqlQueryModel *results, const _sys_system_id id_sys, const int section_id, const string name_var, QSqlDatabase *ptr_database, const string mechanism, const string section_type, const int zone_id);


	///Switch the applied-flag for the results of the random variables in the database table for a defined system state
	static void switch_applied_flag_variable_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the results of the random variables in the database table for a defined system state and id
	static void switch_applied_flag_variable_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id);

	///Copy the random-variable of a fpl-section of a given system id to another one in database tables
	static void copy_fpl_random_variables(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id);

	
	///Set the input for the random variable and the connected distribution type per console
	void set_input(void);
	///Set the input for the random variable and the appending distribution type per file
	void set_input(QFile *ifile, int *line_counter, const string name, const bool calculated, const string end_block_str, const bool must_zone_found=false);
	///Check if a random variable is gievn in a file file
	bool check_variable_in_file(QFile *ifile, int *line_counter, const string name, const string end_block_str);

	///Set the input via a dialog for random generator and transformation checking
	bool set_input_check(_fpl_distribution_type dist_type);
	///Check if there is a random variable in database
	bool check_input(const _sys_system_id id_sys, const int section_id, const string name_var, const bool calculated, QSqlDatabase *ptr_database, const string mechanism, const string section_type, const int zone_number);
	///Set the input for the random variable per database table
	void set_input(const _sys_system_id id_sys, const int section_id, const string name_var, const bool calculated, QSqlDatabase *ptr_database, const string mechanism, const string section_type, const int zone_number, const bool with_output=false);
	///Set the input for the distribution type per database table; the parameter of the random variable has to be set before 
	void set_distribution_types(QSqlDatabase *ptr_database);
	///Set the input for the distribution type per file; the parameter of the random variable has to be set before 
	void set_distribution_types(QFile *ifile, int *line_counter);

	///Transfer the data of the random variables to database
	void transfer_rand2database(QSqlDatabase *ptr_database, const _sys_system_id id_sys, const int section_id, const string mechanism, const string section_type);

	///Set a deterministic variable, where just the mid value is dynamically change during calculation
	void set_determinstic_calculated_type(void);
	
	///Reset the flag that the vvariable was randomly generated
	void reset_random_generated(void);
	///Get the flag if the variable was randomly generated
	bool get_random_generated(void);

	///Get calculated mean value of the distribution
	double get_calculated_mean(void);

	///Set the flag if the mean value is calculated for each calculation step (dynamic mean)
	void set_calculated_flag(const bool flag);
	///Set the name of the variable
	void set_name(const string name);

	///Set the boundaries for the result of the variable
	void set_boundaries(const double min, const double max);
	///Set the maximum boundary for the result of the variable
	void set_max_boundary(const double max);

	///Output the members to display/console
	void output_member(bool detailed=true);

	///Calculate the result of the randomly distributed variable (randomly:=true, deterministically:=false)
	_fpl_result calculate_variables(const bool random_flag=false);

	///Check the statistics of the random variables generation
	void check_statistic(void); 
	///Reset the statistic important values
	void reset_statistics(void);
	///Output the statistics of the randomly generated variable to display/console
	void output_statistic(bool detailed=true);
	///Output a deterministic result to display/console
	void output_deterministic(void);

	///Set a new middle for the distribution classes: mean, mid (triangle), middle (discret), mean_1 (mean_mean), middle (discret_mean)
	void set_new_middle(const double new_middle);

	///Get the results directly from the distribution type without any generation
	_fpl_result get_result(void);

	///Get the distribution type of the random variable
	_fpl_distribution_type get_distribution_type(void);

	///Get the name of the default distribution type; if no default distribution type is used "given" is set 
	string get_default_name(void);

	///Get the determinstic mid-value (mean, middle etc.) of the distribution
	double get_deterministic_mid(void);

	///Get the zone-id
	int get_zone_id(void);

	///Set the position in database table of the distribution type
	void set_dist_type_position(const int pos);
	///Set the default name
	void set_default_name(const string def_name);
	///Set the boundaries
	void set_boundaries(const _fpl_boundaries bound);
	///Set the distribution type
	void set_distribution_type(const _fpl_distribution_type dist_type);
	///Set zone id
	void set_zone_id(const int id);

	///Check given boundaries
	void check_given_boundaries(const double max, const double min);
	///Check for a positive variable
	void check_positiv_variable(void);
	///Check for a positive or zero variable
	void check_positiv_zero_variable(void);

	///set the unit of the random variable
	void set_unit(const string unit);

private:
	//members
	///Name of the variable
	string name;
	///Name of the default distribution type; if no default distribution type is used "given" is set 
	string default_name;
	///Type of the distribution class
	_fpl_distribution_type distribution_type;
	///Zone-id, if it has no specific zone it is 0
	int zone_id;


	///Flag if the mean value is calculated for each calculation step (dynamic mean)
	bool flag_calculated;
	///Boundaries of the random variable
	_fpl_boundaries boundaries;
	///Counter how often the boundary are met during calculation
	int boundary_counter;
	///Flag if the variable is already generated
	bool flag_was_generated;
	///Position of the distribution of the random variable in the database tables of the distribution types;
	int dist_typ_position;

	///String for the unit of the random variable
	string unit;
	
	//methods

	///Allocate the distribution type
	void allocate_dist_type(void);
	///Delete the distribution type
	void delete_dist_type(void);

	///Generate a random distributed variable
	_fpl_result make_random(void);
	///Generate a determinstic variable (mean, mid or a middle of the discret values)
	_fpl_result make_deterministic(void);

	///Mark the variable, that it was is randomly generated
	void set_random_generated(void);

	///Delete the distribution type data in database table of the given random variable
	static void delete_distribution_type_database(QSqlDatabase *ptr_database, QSqlQueryModel *results);

	///Find the key-values for the file input of the random variable data
	bool find_key_values_file(const string myline, int *must_found_counter, bool *name_found, const bool must_zone_found=false);


	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif