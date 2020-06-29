#pragma once
/**\class Fpl_Check_Distribution
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_CHECK_DISTRIBUTION_H
#define FPL_CHECK_DISTRIBUTION_H

//sys-libs
#include "Sys_Plot_Wid.h"

//fpl-libs
#include "Fpl_Random_Variables.h"
#include "FplGui_Random_Check_Dia.h"


///Data structure for an interval for sorting the random results \ingroup fpl
struct _fpl_check_interval{
	///Mid value of the class
	double class_mid;
	///Probability of occurence randomly generated of the class
	double prob_occurence_gen;
	///Probability of occurence calculated of the class
	double prob_occurence_calc;
};

///Class for checking the random-generator and the transformation of the probabilistic variables \ingroup fpl
/**


\see _Fpl_Distribution_Class, Fpl_Random_Variables
*/
class Fpl_Check_Distribution
{
public:
	///Default constructor
	Fpl_Check_Distribution(void);
	///Default destructor
	~Fpl_Check_Distribution(void);

	//members


	//methods

	///Set the pointer to the random variables, which should be checked
	void set_ptr_random_variables(Fpl_Random_Variables *variable2check);
	///Set the number of generation runs
	int set_number_generation_runs(const int number);
	///Set the result value for one generation run
	void set_result_one_run(const double result);
	///Get the number of simulation runs
	int get_number_sim_runs(void);

	///Ask the checking varaibles per dialog
	bool ask_check_setting_per_dia(void);

	///Perform the analysis after the random generation process
	void perform_end_analysis(void);

	///Get results as plot widget
	void get_results2plot(QWidget *parent=NULL);
	///Get the error of the calculation
	double get_error(void);

	///Get a pointer to the dialog for the checking settings
	FplGui_Random_Check_Dia* get_ptr_check_dia(void);

private:
	//member
	///Pointer to the array of generated random results
	double *gen_result;
	///Number of generation runs
	int number_gen_run;
	///Stepsize of the intervals
	double step_interval;
	///Number of intervals
	int number_intervals;
	///Pointer to the classes for sorting the probabilities
	_fpl_check_interval *check_classes;

	///Index of the last result, which was set
	int counter_setting;

	///Maximum number of values which are stored for the checking
	const int max_storing;

	///Maximum value of the generated occurence probability
	double max_occurence_prob;

	///Pointer to the random variable, which is checked
	Fpl_Random_Variables *variable2check;
	///Dialog for setting the variables for the checking simulation run 
	FplGui_Random_Check_Dia check_dia;



	///Error of the calculation
	double error;

	///method
	///Analyse the randomly generated results
	void analyse_random_results(void);
	///Analyse by calculation of the results
	void analyse_calculated_results(void);
	///Calculate the error of the analysis
	void calculate_error(void);

	///Allocate the array of randomly generated results
	void allocate_result_array(void);
	///Delete the array of randomly generated results
	void delete_result_array(void);

	///Allocate the classes for sorting the results
	void allocate_check_classes(void);
	///Delete the classes for sorting the results
	void delete_check_classes(void);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif
