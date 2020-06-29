#pragma once
/**\class Fpl_Frc_Sim
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_FRC_SIM_H
#define FPL_FRC_SIM_H

//system fpl
#include "Fpl_Mc_Sim.h"
#include "Sys_Database_Browser.h" //kann weg
#include "Fpl_Frc_Curve.h"


///Class for a fragility curve simulation \ingroup fpl
/**
This class performs a fragility curve simulation for a complete
FloodProtectionLine section.
For given waterlevels a monte-carlo simulation is applied. This waterlevels are 
dynamically adapted to the variation of the curve from one curve point to the following.
\see Fpl_Mc_Sim
*/
class Fpl_Frc_Sim
{
//widget for displaying the parameter
friend class FplGui_System_Member_Wid; 

public:
	///Default constructor
	Fpl_Frc_Sim(void);
	///Constructor with a given control parameter-set for a fragility curve simulation
	Fpl_Frc_Sim(const double min_stepsize, const double max_stepsize, const int calc_max, const double start_step, const double step_factor, const double boundary_reduce
				 , const double boundary_expand, const double value_start);
	///Default destructor
	~Fpl_Frc_Sim(void);

	//members
	///Pointer to a monte carlo simulation
	Fpl_Mc_Sim *mc_sim_frc;


	//methods

	///Input the control parameters per database table
	void set_input(QSqlDatabase *ptr_database, const bool output=true);

	///Initialize the fragility curve calculation
	void init_frc_calculation(void);

	///Output the control parameters of the fragility curve simualtion to display/console
	void output_members(void);
	
	///Perform a fragility curve calculation for an given fpl-section
	void make_frc_sim(Fpl_Section *section_to_calc,  QSqlDatabase *ptr_database);

private:
	//members

	///Applied control parameter: minimal stepsize [m] of fragility curve simulations
	double stepsize_min;
	///Appliedt control parameter: maximal stepsize [m] of fragility curve simulations
	double stepsize_max;
	///Applied control parameter: maximal number of steps for fragility curve simulations
	int max_calculation;
	///Applied control parameter: Starting stepsize [m] for fragility curve simulations
	double starting_stepsize;
	///Applied control parameter: Stepsize factor for fragility curve simulations
	double stepsize_factor;
	///Applied control parameter: Boundary value for a stepsize reduction for fragility curve simulations
	double quotient_boundary_reduce;
	///Applied control parameter: Boundary value for a stepsize expansion for fragility curve simulations
	double quotient_boundary_expand;
	///Applied control parameter: Starting waterlevel [m] for fragility curve simulations
	double start_value;

	///Current stepsize
	double stepsize;

	///Pair of variates of the fragility curve: current calculation step
	_fpl_frc_values frc_value_actuel;
	_fpl_frc_values frc_value_mid_1;
	_fpl_frc_values frc_value_mid_2;
	_fpl_frc_values frc_value_last;

	///Current gradient of the fragility curve
	double gradient;
	///Gradient of the fragility curve one calculation step before
	double gradient_old;
	///Quotient of the old and the current gradient
	double quotient_grad;

	///Flag if the calculation is aborted due to the abortion criterion
	bool abort_flag;
	///Counter how often the abortion criterion is met
	int abort_counter;

	///Counter of calculation steps
	int counter_calc;
	///Counter of monte carlo simulation
	int counter_mc;
	
	///Pair of variates of the fragility curve: 0 is the current calculation step (i), 1 is calculation step i-1 etc.
	/**Just probability and waterlevel are stored here.
	The purpose is the check if the waterlevel was already calculated.
	*/
	_fpl_frc_values *frc_value;
	///Number of stored frc-values
	int number_frc_value;

	///The results of one monte-carlo simulation for one waterlevel
	_fpl_frc_result result_of_mc;

	///Time difference between start and end of a simulation
	time_t time_diff;


	//methods
	///Allocate the monte carlo simulation
	void allocate_mc_sim(void);
	///Delete the monte carlo simulation
	void delete_mc_sim(void);

	///Allocate pair of variates of the fragility curve to avoid double calculation of a waterlevel
	void allocate_frc_value(void);
	///Delete the pair of variates of the fragility curve to avoid double calculation of a waterlevel
	void delete_frc_value(void);
	///Reset the pair of variates of the fragility curve to avoid double calculation of a waterlevel 
	void reset_frc_value(void);

	///Check the control parameters for a fragility curve simulation
	void check_frc_param(void);

	///Reset the start values of the fragility curve simulation
	void reset_start_value(void);


	///Check the criterion for abortion of the calculation and update the stepsize
	void update_stepsize_waterlevel(const int number_of_calc);
	///Calculate the gradient and the quotient of the gradient of the curve
	void calc_grad_quo(void);

	///The stored frc-values are copied for a new calculation step
	void copy_frc_value(void);
	///Check if the waterlevel to calculate is already calculated and stored in frc_value
	bool check_calculated(void);

	///Output the statistics of the fragility curve calculation to display/console
	void output_statistics(void);
	///Output the results of the fragility curve calculation to display/console
	void output_result(void);
	///Output a text header for the fragility curve simulation corresponding the given type to display/console
	void out_text_frcsampling(const int type);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);

};
#endif
