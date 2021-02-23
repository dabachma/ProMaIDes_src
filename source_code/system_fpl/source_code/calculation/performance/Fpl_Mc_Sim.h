#pragma once
/**\class Fpl_Mc_Sim
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_MC_SIM_H
#define FPL_MC_SIM_H

//system sys
#include "Common_Const.h"
 
//system fpl
#include "Fpl_Random_Variables.h" 
#include "Fpl_Section.h"
#include "Fpl_Check_Distribution.h"




///Data structure to store the results of a fragility curve calculation \ingroup fpl
/**
\see Frc_Sim
*/
struct _fpl_frc_result{
	///Waterlevel, for which the monte carlo analysis is performed
	double waterlevel;
	///Mean of the failure probability
	double failure_prob;
	///Lower boundary of the failure probability
	double low_failure_prob;
	///Upper boundary of the failure probability
	double up_failure_prob;
	///Number of monte-carlo simulation needed
	int number_mc;
	///Computation time needed for the monte-carlo simulation
	double time_mc;
};


///Namespace for the keystring in a database table of the control parameters and the default paramaters for a monte carlo- or fragility curve simulation \ingroup fpl
namespace fpl_control_param{
	//mc
	///Keystring for maximal number of a monte-carlo simulations 
	const string mc_max("MC_MAX");
	///Keystring for minimal number of a monte-carlo simulations before the abort criterion is checked
	const string mc_min("MC_MIN");
	///Keystring for absolute accurancy-criteria for the abort criterion of a monte-carlo simulations
	const string mc_error_abs("MC_ERROR_ABS");
	///Keystring for relative accurancy-criteria for the abort criterion of a monte-carlo simulations
	const string mc_error_rel("MC_ERROR_REL_[%]");
	///Keystring for the confidence interval for the abort criterion of a monte-carlo simulations
	const string mc_confident("MC_CONFIDENT_INT_[%]");
	///Keystring for the random generator type
	const string mc_rand_generator("MC_RANDOM_GENERATOR");

	///Default control parameter: maximal number of monte-carlo simulations 
	const int mc_max_def=100000;
	///Default control parameter: minimal number of monte-carlo simulations, before the abort criterion is used
	const int mc_min_def=10000;
	///Default control parameter: absolute accurancy-criteria for the abort criterion
	const double epsilon_absolut_def=0.003; 
	///Default control parameter: relative accurancy-criteria for the abort criterion
	const double epsilon_relativ_def=3.0/100.0;
	///Default control parameter: confidence interval
	const double confidence_def=95.0/100.0;
	///Default random generator type
	const _fpl_random_generator_type random_def=_fpl_random_generator_type::gen_type_ranmar;

	//frc
	///Keystring for maximal number of fragility curve steps 
	const string frc_max("FRC_MAX");
	///Keystring for the minimal stepsize [m] of fragility curve calculation
	const string frc_step_min("FRC_STEP_MIN_[m]");
	///Keystring for the maximal stepsize [m] of fragility curve calculation
	const string frc_step_max("FRC_STEP_MAX_[m]");
	///Keystring for the starting stepsize [m] of fragility curve calculation
	const string frc_step_start("FRC_STEP_START_[m]");
	///Keystring for the starting waterlevel [m] of fragility curve calculation
	const string frc_height_start("FRC_START_HEIGHT[m]");
	///Keystring for the boundary value for a stepsize reduction for fragility curve simulations 
	const string frc_step_reduce("FRC_STEP_REDUCE");
	///Keystring for the boundary value for a stepsize expansion for fragility curve simulations 
	const string frc_step_expand("FRC_STEP_EXPAND");
	///Keystring for the step-size factor for fragility curve simulations 
	const string frc_step_factor("FRC_STEP_FACTOR");

	///Default control parameter: minimal stepsize [m] of fragility curve simulations 
	const double stepsize_min_def=0.01;
	///Default control parameter: maximal stepsize [m] of fragility curve simulations 
	const double stepsize_max_def=0.3;
	///Default control parameter: maximal number of steps for fragility curve simulations
	const int max_calculation_def=5000;
	///Default control parameter: Starting stepsize [m] for fragility curve simulations
	const double starting_stepsize_def=0.05;
	///Default control parameter: Stepsize factor for fragility curve simulations
	const double stepsize_factor_def=2.0;
	///Default control parameter: Boundary value for a stepsize reduction for fragility curve simulations
	const double quotient_boundary_reduce_def=1.7;
	///Default control parameter: Boundary value for a stepsize expansion for fragility curve simulations
	const double quotient_boundary_expand_def=1.3;
	///Default control parameter: Starting waterlevel [m] for fragility curve simulations
	const double start_value_def=0.0;

	//output
	///Keystring for tecplot output 
	const string output_tecplot("OUTPUT_TECPLOT");
	///Keystring for paraview output 
	const string output_paraview("OUTPUT_PARAVIEW");
	///Keystring for excel output 
	const string output_excel("OUTPUT_EXCEL");

	///Default control parameter: tecplot output 1 := true ; 0 := false 
	const int output_tecplot_def = 0;
	///Default control parameter: paraview output 1 := true ; 0 := false 
	const int output_paraview_def = 1;
	///Default control parameter: excel output 1 := true ; 0 := false 
	const int output_excel_def = 1;


};

///Class for a Monte-Carlo simulation \ingroup fpl
/**
This class performs a Monte-Carlo simulation for one random variable or for a complete
FloodProtectionLine section.
This simualtion is also applied during a fragility cruve simulation for a specific waterlevel.
\see Frc_Sim
*/
class Fpl_Mc_Sim
{
//widget for displaying the parameter
friend class FplGui_System_Member_Wid; 

public:
	///Default constructor
	Fpl_Mc_Sim(void);
	///Constructor with a given control parameter-set for a monte-carlo simulation
	Fpl_Mc_Sim(const double mc_max_var, const double mc_min_var, const double error_abs, const double error_rel_perc, const double confid_perc);
	///Default destructor
	~Fpl_Mc_Sim(void);


	//members

	///Pointer to the table for storing the relevant control parameters for the monte-carlo/ fragility curve calculation
	/**This table is also used by a fragility curve calculation.
	\see Frc_Sim
	*/
	static Tables *table;
	///Pointer to the table for storing the results of a reliability calculation (Mc/Frc)
	/**This table is also used by a fragility curve calculation.
	\see Frc_Sim
	*/
	static Tables *result_table;
	
	//methods

	///Create the database table for the control parameters
	static bool create_table(QSqlDatabase *ptr_database);
	///Reset all data in the database table for the controll parameters to the default values
	static void reset_data_in_table(QSqlDatabase *ptr_database);
	///Set the database table for the control parameters: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the controll data
	static void close_table(void);
	///Write the default value of the control parameters into the database table
	static void set_predefined_data2control_table(QSqlDatabase *ptr_database);

	///Get output control parameters from table
	static void get_output_control_from_table(QSqlDatabase *ptr_database, output_control *control_flags);
	///Set output control parameters to table
	static void set_output_control2table(QSqlDatabase *ptr_database, output_control control_flags);
 

	///Create the database table for the results of a reliability calculation (Mc/Frc)
	static void create_result_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the results of a reliability calculation (Mc/Frc)
	static void delete_data_in_result_table(QSqlDatabase *ptr_database);
	///Delete a data set in the database table for the results of a reliability calculation for a specified system state
	static void delete_data_in_result_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete a data set in the database table for the results of a reliability calculation for a specified section (Mc/Frc)
	static void delete_data_in_result_table(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const bool mc_flag);
	///Delete a data set in the database table for the results of a reliability calculation (Mc/Frc)
	static void delete_data_in_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool mc_flag, const bool just_generated);

	///Set the database table for the results of a reliability calculation (Mc/Frc): it sets the table name and the name of the columns and allocate them
	static void set_result_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the results of a reliability calculation (Mc/Frc)
	static void close_result_table(void);
	///Select results of given fpl section in a database table
	static int select_results_in_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id, const bool mc_flag);
	///Check if there are results available for a specific section-id
	static void check_results_available_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id, bool *mc_available, bool *frc_available);

	///Switch the applied-flag for the reliability results in the database table for a defined system state
	static void switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the reliability results in the database table for a defined system state and id
	static void switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id);

	///Copy the results of a given system id to another one in database tables
	static void copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id);


	///Set result data to the database result table
	void set_result2result_table(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const bool mc_flag);
	///Set a result to the database result table directly
	void set_result2resulttable_direct(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const bool mc_flag, const _fpl_frc_result result); 


	///Input the control parameters per database table
	void set_input(QSqlDatabase *ptr_database, const bool output=true);

	
	///Output the control parameters of the monte-carlo simulation to display/console
	void output_members(void);

	///Perform a monte-carlo simulation started by an fragility curve simualtion (Frc_Sim) for an given fpl-section
	_fpl_frc_result make_mc_sim(const double waterlevel_frc, Fpl_Section *section_to_calc, QSqlDatabase *ptr_database);
	///Perform a pure monte-carlo simulation for a given fpl-section
	_fpl_frc_result make_mc_sim(Fpl_Section *section_to_calc, QSqlDatabase *ptr_database);
	///Test the random generator and the probabilistic ditribution type with a monte-carlo analysis
	void make_mc_test_random(Fpl_Check_Distribution *random_checker, Fpl_Random_Variables *random2check);

	///Convert a simulation type (_fpl_simulation_type) into a text string
	static string convert_sim_type2text(_fpl_simulation_type type);
	///Convert a text string into a simulation type (_fpl_simulation_type)
	static _fpl_simulation_type convert_text2sim_type(const string buffer);

	///Convert a random generator type (_fpl_random_generator_type) into a text string
	static string convert_generator_type2text(_fpl_random_generator_type type);
	///Convert a text string into a random generator type (_fpl_random_generator_type)
	static _fpl_random_generator_type convert_text2generator_type(const string buffer);

	///Get the type of the random generator
	static _fpl_random_generator_type get_generator_type(void);

private:
	//members
	///Applied control parameter: maximal number of monte-carlo simulations 
	int mc_max;
	///Applied control parameter: minimal number of monte-carlo simulations, before the abort criterion is used
	int mc_min;
	///Applied control parameter: absolute accurancy-criteria for the abort criterion
	double epsilon_absolut; 
	///Applied control parameter: relative accurancy-criteria for the abort criterion
	double epsilon_relativ;
	///Applied control parameter: confidence interval
	double confidence;

	///Alpha is determined from the given confidence intervall
	double alpha;
	///Absolute calculated error
	double epsilon_calc_absolut;
	///Relative calculated error
	double epsilon_calc_relativ;
	///Lower boundary of the confidence intervall of the calculated mean-value
	double lower_interval_bound;
	///Upper boundary of the confidence intervall of the calculated mean-value
	double upper_interval_bound;
	///Flag if the abort criterion is reached
	bool abort_flag;

	///The mean of the monte-carlo simulation
	double mean_calc;
	///The standard deviation of the mean of the monte-carlo simulation
	double std_dev_calc;
	///The variance (standard deviation to the power of two) of the mean of the monte-carlo simulation
	double varianz_calc;
	///Counter of the monte-carlo simulations
	int counter_calc;
	///The mean value one simulation step before for updating the mean value
	double mean_calc_old;

	///Type of the random generator
	static _fpl_random_generator_type generator;

	///The result of an fault tree simulation is 1 (failure) or zero non-failure of the fpl-section (binominal distribution)
	int failure;
	///This result is used by testing the random generator and the distribution types (make_mc_test_random)
	double result;
	///Complete package of the result of a monte carlo simulation
	_fpl_frc_result result_complete;

	///Time difference between start and end of a simulation
	time_t time_diff;

	///Flag if the statistics was output
	bool stat_output;


	//methods
	///Determine the alpha-factor from the given conconfidence intervall (e.g. 95%) 
	void calculate_alpha(void);
	///Check the control parameter of the monte carlo simulation
	void check_mc_param(void);

	///Update the mean value and the standard deviation (statistics) per simulation run
	void check_statistic(void);
	///Output to the statistics of the monte carlo simulation to display/console
	void output_statistic(void);
	///Output to the statistics of the monte carlo simulation for random variable checking to display/console
	void output_statistic_checking(const double error_check);

	///Reset the start values of the monte carlo simulation, which are used for the abort criterion
	void reset_start_value(void);
	///Check the abort criterion for abort the simulation
	void abort_criterion(void);
	
	///Output the advance of the monte carlo simualtion to display/console
	void output_advance(const int counter_calc,  double& counter_output, time_t start_time);
	///Output a text header for the monte carlo simulation corresponding the given type to display/console
	void out_text_mcsampling(const int type);
	///Output the members for the checking of the random genertaion and the transformation via a monte carlo simualtion to display/console
	void output_check_members(void);
	
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif
