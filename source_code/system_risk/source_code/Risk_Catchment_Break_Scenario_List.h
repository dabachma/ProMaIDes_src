#pragma once
/**\class Risk_Catchment_Break_Scenario_List
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef RISK_CATCHMENT_BREAK_SCENARIO_LIST_H
#define RISK_CATCHMENT_BREAK_SCENARIO_LIST_H

//system risk
#include "Risk_Break_Scenario.h"


///Data structure for gathering statistical information
struct _risk_statistic_info_break_sc{
	///Pointer to the hydraulic boundary scenario
	Hyd_Boundary_Szenario *ptr_hyd_sc;
	///Total number to calculate
	int total_number2calc;
	///Number already calculated
	int number_calculated;
	///Sum of probabiliy
	double sum_probability;
	///Sum of probability already calculated
	double sum_probability_calc;

};

//forward declaration
struct _risk_ca_scenario_control;

///List class for the break scenarios of the catchment area risk approach \ingroup risk
/**

\see Risk_Break_Scenario

*/

class Risk_Catchment_Break_Scenario_List
{
public:
	///Default constructor
	Risk_Catchment_Break_Scenario_List(void);
	///Default destructor
	~Risk_Catchment_Break_Scenario_List(void);

	///Add scenario to list
	void add_scenario2list(Risk_Break_Scenario *add);

	///Delete a scenario in the list
	void delete_scenario_from_list(Risk_Break_Scenario *delete_sc);
	///Delete a list of scenario in the list
	void delete_scenario_from_list(Risk_Catchment_Break_Scenario_List *delete_list);

	///Clear the list
	void clear_list(void);

	///Get the number of list members
	int get_number_in_list(void);
	///Get a pointer to the total list
	Risk_Break_Scenario *get_total_list(void);
	///Get a pointer to a list member specified by the given index
	Risk_Break_Scenario *get_ptr_scenario(const int index);

	///Sort the list after the probability
	void sort_list_after_probability(const bool total_probability);
	///Sort the list after the number of involved fpl-section
	void sort_list_after_number_involved_section(const bool ascending);

	///Set all entries to the list as selected
	void set_all_in_list_selected(const bool  flag);

	///Count number of selected scenarios in list
	int count_number_selected(void);

	///Count number of hydraulically calculated scenarios in list
	int count_number_hyd_calculated(void);

	///Add the break height from the database table to the break scenarios stored in list
	void add_break_height_by_database(QSqlDatabase *ptr_database);

	///Output the list to display/console
	void output_list(void);

	///Cut list by a given index; the list entrees after the index are deleted
	void cut_list(const int index);

	///Delete the scenario(s) of a list, which are not caluclated 
	void delete_not_calculated_in_list(void);
	///Delete the scenario(s) of a list, which are caluclated 
	void delete_calculated_in_list(void);

	///Set a scenario list with all available break scenarios
	void set_total_risk_break_list(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Set a scenario list with all available break scenarios for one given hydraulic boundary scenario
	void set_risk_break_list_hyd_bound(QSqlDatabase *ptr_database, const _sys_system_id id, Hyd_Boundary_Szenario *bound);


	///Set a new list with the nearest hydraulical calculated break-scenarios for a given scenario
	string set_new_list_nearest_hyd_calc_scenario(Risk_Catchment_Break_Scenario_List *new_list, Risk_Break_Scenario *scenario);
	//Find results of already calculated break scenarios, where the interval is unequal
	void find_break_scenario_unequal_interval(const string tofind, Risk_Break_Scenario *break_sc2predict, Risk_Catchment_Break_Scenario_List *new_list);

	///Sum up probabilities for all scenario(s) in the list, where the given one is included
	double sum_prob_included_scenario(Risk_Break_Scenario *given);

	///Merge two lists of break scenarios
	void merging_lists(Risk_Break_Scenario *scenario, const int number);




	///Get the flag, if the total break list is set
	bool get_total_break_list_is_set(void);

	///Calculate the statistic for the total break scenario list, then output it to the display/console
	void calculate_output_statistic_total_break_list(QSqlDatabase *ptr_database, const _sys_system_id id, const double boundary, Risk_Catchment_Break_Scenario_List *one_break_list, Hyd_Boundary_Szenario_Management *buff_hyd);

	///Prepare the list for the automatic scenario calculation
	void prepare_list_auto_generation(QSqlDatabase *ptr_database, const _sys_system_id id, const _risk_ca_scenario_control params);

	///Get the state of probability, which is reach 
	double get_prob_scenario_calculated(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Make list out of a given break scenario, where just one break scenario are in (cut break scenario)
	void make_one_break_list_by_cutting_break_scenario(Risk_Break_Scenario *sc2cut);

	///Set the predicted risk results to the break scenarios in the list
	void set_predicted_risk_result2break_sc(QSqlDatabase *ptr_database);

	///Sum up the predicted risk results of the break scenarios in the list
	void sum_predicted_risk_result_break_sc(_risk_predicted_risk *sum);



private:
	//members
	///Number of boundary scenarios in list
	int number;
	///List of boundary scenarios
	Risk_Break_Scenario *break_sc;

	///Flag if the list of all break scenarios is set (total list)
	bool total_is_set;

    ///Number of list-elements in a block for allocation
    const int block_elems;

	//method
	///Allocate the hydraulic boundary scenarios
	void allocate_list(void);
	///Delete the list
	void delete_list(void);

	///Get the break scenario with the highest economic predicted risk
	Risk_Break_Scenario *get_highest_predicted_ecn_risk(void);
	///Get the break scenario with the highest ecological predicted risk
	Risk_Break_Scenario *get_highest_predicted_eco_risk(void);
	///Get the break scenario with the highest people2risk affected predicted risk
	Risk_Break_Scenario *get_highest_predicted_pop_affected_risk(void);
	///Get the break scenario with the highest people2risk endangered predicted risk
	Risk_Break_Scenario *get_highest_predicted_pop_endangered_risk(void);
	///Get the break scenario with the highest psycho-social predicted risk
	Risk_Break_Scenario *get_highest_predicted_pys_risk(void);

	///Set error(s)
	Error set_error(const int err_type);
};
#endif
