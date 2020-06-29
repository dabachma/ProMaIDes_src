#pragma once
/**\class _Fpl_Mechanism
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _FPL_MECHANISM_H
#define _FPL_MECHANISM_H
//qt-libs
#include <QtSql> 
//system fpl
#include "Fpl_Random_Variables.h" 
//system sys
#include "Common_Const.h"


///Base-class for the a mechanism for calculating the reliability of the fpl section  \ingroup fpl
/**
*/
class _Fpl_Mechanism
{
public:
	///Default constructor
	_Fpl_Mechanism(void);
	///Default destructor
	virtual ~_Fpl_Mechanism(void);
	
	//methods
	///Set the flag, if the mechanism should be calculated
	void set_2calc_flag(const bool flag);
	///Get the flag, if the mechanism should be calculated
	bool get_2calc_flag(void);

	///Set the flag, if the mechanism should be used in the failure tree
	void set_in_tree_flag(const bool flag);
	///Get the flag, if the mechanism should be used in the failure tree
	bool get_in_tree_flag(void);


	///Set the random variables of the result members
	virtual void set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id)=0;

	///Output members of the mechanism to display/console
	virtual void output_members(void)=0;
	
	///Check the statistic of the random variables of the mechanism
	virtual void check_statistic(void)=0;
	///Reset the statistic important values
	virtual void reset_statistics(void)=0;
	///Output the statistics of the random variables of the mechanism to display/console
	virtual void output_statistic(void)=0;
	///Output the results of a deterministic calculation to display/console
	virtual void output_determ_results(void)=0;

	///Sum up the counter of the single occurence probability by 1
	void sum_up_counter_single_prob(void);
	///Sum up the counter of the fault-tree occurence probability by 1
	void sum_up_counter_tree_prob(void);
	///Get the counter of the single occurence probability
	double get_counter_single_prob(void);
	///Get the counter of the fault-tree occurence probability
	double get_counter_tree_prob(void);
	///Reset the counter of the single occurence probability
	void reset_counter_single_prob(void);
	///Reset the counter of the fault-tree occurence probability
	void reset_counter_tree_prob(void);
	///Reset the counter of the fault-tree occurence probability and of the single occurence probability
	void reset_counter_prob(void);


protected:
	//members
	///Flag if the mechanism schould be calculated
	bool calculate_flag;
	///Flag if the mechanism is applied in the failure-tree calculation
	bool in_tree_flag;

	///Counter for the single occurence probability
	int counter_single_prob;
	///Counter for the fault-tree occurence probabillty
	int counter_tree_prob;

	///wave rduction to zero
	bool wave2zero;

	//method
	///Calculate the angle of wave attack, from which the wave height is reduced
	void calculate_wavedirection_reduction(const double wind_direction, const double section_direction, double *wave_height, double *wave_period);

	
};
#endif