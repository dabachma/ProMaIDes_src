#pragma once
/**\class Fpl_Frc_Curve
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_FRC_CURVE_H
#define FPL_FRC_CURVE_H


///Data structure to store the values of a fragility curve  \ingroup fpl
/**This structure is used in the frc-simualtion for the automatic stepsize adaptation. 
Then just the mean-value of the failure probaility and the waterlevel are stored.
*/
struct _fpl_frc_values{
	///Mean of the failure probability
	double failure_prob;
	///Waterlevel, for which the monte carlo analysis is performed
	double waterlevel;
};

///Data structure to store the intervals of a fragility curve  \ingroup fpl
struct _fpl_frc_intervals{
	///Identifier string of the class (waterlevel with x.xx m)
	string id_str;
	///Class mid waterlevel as identifier
	double interval_mid;
	///Waterlevel of the lower interval boundary 
	_fpl_frc_values interval_low;
	///Waterlevel of the upper interval boundary
	_fpl_frc_values interval_up;
	///Breakheigth of the interval
	double break_waterlevel;
	///Number of occurence
	int number_occ;
};

///Structure for gathering the information a break in a fpl-section \ingroup fpl
struct _fpl_break_info{
	
	///Id of the fpl-section
	int id_fpl_sec;
	///Id of the frc-interval \see Fpl_Frc_Curve
	string id_interval;
	///Break height
	double break_height;

};

//forward declaration
struct _fpl_frc_result;
class Fpl_Section;

///Class for the handling of a FRC-curve, like loading from database, interval creation and interploation \ingroup fpl
/**

*/
class Fpl_Frc_Curve  
{
public:
	///Default constructor
	Fpl_Frc_Curve(void);
	///Default destructor
	~Fpl_Frc_Curve(void);

	//members

	///Pointer to the table for intervals of the frc-curve for the catchment area risk calculation
	static Tables *table_intervals;

	//methods

	///Set the database table for the frc-intervals: it sets the table name and the name of the columns and allocate them
	static void set_table_intervals(QSqlDatabase *ptr_database);
	///Create the database table for the frc-intervals
	static void create_table_intervals(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the frc-intervals
	static void delete_data_in_table_intervals(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the frc-intervals for the given specifier
	static void delete_data_in_table_intervals(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, const int sz_bound_id);
	///Delete all data in the database table for the frc-intervals for the given specifier
	static void delete_data_in_table_intervals(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Close and delete the database table for the frc-intervals
	static void close_table_intervals(void);
	///Get the break height form database table by the given specifier
	static double get_break_height(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, const int sz_bound_id, int interval_id);


	///Copy the results of a given system id to another one in database tables
	static void copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);


	///Set the curve per database
	void set_curve_data(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);

	

	///Transfer the data of the intervals to the database table
	void transfer_intervals2database(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, const int sz_bound_id);

	///Interpolate value
	_fpl_frc_result interpolate_value(const double waterlevel, const bool break_flag);

	///Initialize the intervals 
	void init_intervals(const double max_waterlevel);
	///Get the intervals identifier for a given random value 
	string get_interval_identifier(_Fpl_Random_Generator *ran_gen, const int number_try, const double simil_factor);
	///Get most probable interval not taken into account the nobreak interval
	string get_most_probable_nobreak_identifier(double *prob);
	///Get the number of intervals 
	static int get_number_intervals(void);
	///Set the number of intervals 
	static void set_number_intervals(const int number_intervals);
	///Get if the selected interval is the nobreak interval 
	bool get_in_nobreak_interval(void);
	///Set if this section is a nobreak section
	void set_in_nobreak_interval(const bool in_nobreak);
	///Get the probability of the nobreak-interval
	double get_prob_nobreak_interval(void);

	///Set if the section is checked
	void set_is_checked(const bool is_checked);
	///Get if the section is already checked
	bool get_is_checked(void);

	///Output the interval(s) to console/display
	void output_intervals(Fpl_Section *section);

	///Get the current evaluated waterlevel
	double get_current_waterlevel(void);
	///Get the current evaluated index of the intervall
	int get_current_index_intervall(void);
	///Get the current evaluated intervall as string
	string get_current_intervall_string(void);

	///Convert a interval string to an integer
	static int convert_interval_string2integer(const string interval_id);


private:

	//member
	///Number of points
	int number_points;
	///Points of the frc
	_fpl_frc_result *points;

	///Number of intervals (test)
	static int number_intervals;
	///The intervals of the frc-curve
	_fpl_frc_intervals *intervals;
	///Maximum waterlevel possible(test)
	double max_waterlevel;
	///Flag if the selected interval is the nobreak interval (test)
	bool in_nobreak_interval;

	///Current evaluated waterlevel
	double current_waterlevel;
	///Current evaluated index of the intervall
	int current_index_intervall;

	///Flag if the section is already checked
	bool section_is_checked;
	
	//method
	///Allocate the points
	void allocate_points(void);
	///Delete the points
	void delete_points(void);

	///Allocate the intervals (test)
	void allocate_intervals(void);
	///Delete the intervals (test)
	void delete_intervals(void);

	///Find the index of the intervals (test)
	int find_index_intervals(const double random_value, double *waterlevel);


	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif
