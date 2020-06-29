#pragma once
/**\class Risk_Catchment_Scenario_Tree
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef RISK_CATCHMENT_SCENARIO_TREE_H
#define RISK_CATCHMENT_SCENARIO_TREE_H

//system sys
#include "_Sys_Common_System.h"
#include "Common_Const.h"

//fpl-libs
#include "Fpl_Random_Generator_Ranmar.h"
#include "Fpl_Random_Generator_Zikurat.h"
#include "Fpl_Random_Generator_MofAll.h"
#include "Fpl_Random_Generator_Sfmt.h"

//system risk
#include "Risk_Break_Scenario.h"
#include "Risk_Fpl_Stress.h"

///Structure for gathering the information of an generated break scenario via the monte-carlo analysis \ingroup risk
struct _risk_mc_scenario{
	///Identifer string
	string id_str;
	///Number of occurence
	int number;
	///Occurrence probabilty
	double occ_prob;
	///Flag if the scenario is generated (true) or set (false)
	bool generated_flag;
};
///Structure to handle the _risk_mc_scenario in a block
struct _risk_mc_scenario_block{
	///Block of the scenarios
	_risk_mc_scenario scenarios[constant::risk_sc_block_number];
	///Number until which the block is set
	int number_set;
	///Completly full
	bool full;

};
///Structure for gathering the configuration parameters of the break scenario generation via the monte-carlo analysis and the risk calculation via the catchment risk approach \ingroup risk
struct _risk_ca_scenario_control{
	///Number of monte-carlo simulation
	int number_mc;
	///Number of intervals for dividing the frc-curve
	int number_intervals;
	///Random generator type
	_fpl_random_generator_type gen_type;
	///Probability boundary
	double prob_bound;
	///Flag if the predicted values are used
	bool use_predict_values;
	///Maximum of hydraulic calculation
	double max_hyd_calc;

	///Boundary for an optimisation of the economical predicted risks
	double boundary_ecn_risk;
	///Boundary for an optimisation of the ecological predicted risks
	double boundary_eco_risk;
	///Boundary for an optimisation of the people2risk affected predicted risks
	double boundary_pop_affected_risk;
	///Boundary for an optimisation of the people2risk endangered predicted risks
	double boundary_pop_endangered_risk;
	///Boundary for an optimisation of the psycho-social predicted risks
	double boundary_pys_risk;

};

///Class for calculation and management of the catchment area risk scenarios \ingroup risk
/**


*/
class Risk_Catchment_Scenario_Tree : public _Sys_Common_System
{
public:
	///Default constructor
	Risk_Catchment_Scenario_Tree(void);
	///Default destructor
	~Risk_Catchment_Scenario_Tree(void);


	///Set the configuartion parameters for the break scenario generation
	void set_break_scenario_generation_param(const _risk_ca_scenario_control params);
	///Generate list of the break scenarios for the catchment area risk approach via the monte carlo analysis
	void generate_break_scenario_list(QSqlDatabase *ptr_database, const int number_fpl_section, Fpl_Section *fpl_section, Hyd_Boundary_Szenario *hyd_bound_sc);
	

	///Generate a new list for updating the probabilities, including new hydraulic information
	void generate_new_break_scenario_list_update(QSqlDatabase *ptr_database, const int number_fpl_section, Fpl_Section *fpl_section, Hyd_Boundary_Szenario *hyd_bound_sc, Risk_Catchment_Break_Scenario_List *existing_sc, const bool predict_prob);

	///Output the generated list of scenarios to display/console
	void output_scenario_list(void);

	///Reset the scenario tree
	void reset_scenario_tree(void);

	///Get the number of break scenarios
	int get_number_break_sc(void);
	///Get a pointer to the break scenario list
	Risk_Break_Scenario* get_ptr_break_sc_list(void);

	///Get a pointer to the control parameter
	_risk_ca_scenario_control *get_ptr_control_params(void);

	

private:

	//members
	///Control parameters for the generation of the break scenario tree
	_risk_ca_scenario_control control_params;
	///Random generator
	_Fpl_Random_Generator *random_generator;
	///Random generator for same time of maximum waterlevel occurence
	_Fpl_Random_Generator *random_generator_max_time;


	///Number of monte-carlo scenarios
	int number_mc_sc;
	///Monte-Carlo scenarios list
	_risk_mc_scenario *mc_sc_list;

	///Number of blocks
	int number_blocks;
	///Blocks for faster storing
	_risk_mc_scenario_block *sc_blocks;


	///Pointer to the hydraulic boundary scenario
	Hyd_Boundary_Szenario *hyd_bound_sc;


	///Number of break scenarios
	int number_break_sc;
	///List of break scenarios
	Risk_Break_Scenario *break_sc_list;

	///Number of scenarios to the boundary
	int number_sc2bound;

	///Pointer to the new hydraulic result information
	Risk_Fpl_Stress *additional_hyd_stress;
	///Number of additional hydraulic result information
	int number_additional_hyd_result;

	//method
	///Allocate the random generator
	void allocate_random_generator(void);
	///Delete the random generator
	void delete_random_generator(void);

	///Allocate the random generator for same time of maximum waterlevel occurence
	void allocate_random_generator_max_time(void);
	///Delete the random generator for same time of maximum waterlevel occurence
	void delete_random_generator_max_time(void);

	///Allocate the break scenario list
	void allocate_break_scenario_list(void);
	///Delete the break scenario list
	void delete_break_scenario_list(void);

	///Update the list of the monte-carlo scenarios
	int update_list_mc_sc(const string sc_id_str);
	///Allocate the list of the monte-carlo scenarios
	void allocate_list_mc_sc(void);
	///Delete the list of the monte-carlo scenarios
	void delete_list_mc_sc(void);
	///Sort the list of monte-carlo scenarios after their occurence
	void sort_list_mc_sc(void);
	///Check if one break scenario is in list
	bool check_one_break_sc_in_list(const string sc_id_str); 
	///Get the index when a given boundary of sum of occurence probability is reached
	int get_index_sum_prob_reach(void);

	///Allocate the blocks of the monte-carlo scenarios
	void allocate_blocks_mc_sc(void);
	///Delete the blocks of the monte-carlo scenarios
	void delete_blocks_mc_sc(void);

	///Transfer the blocks to list
	void transfer_blocks2list(void);
	///Copy the blocks 
	void copy_blocks(_risk_mc_scenario_block *src, _risk_mc_scenario_block *dest);
	///Update the blocks
	void update_blocks(const string sc_id_str);


	///Allocate and set from database the additional hydraulic result information
	void allocate_additional_hyd_info_database(QSqlDatabase *ptr_database, const int number_fpl_section, Fpl_Section *fpl_section, Hyd_Boundary_Szenario *hyd_bound_sc, Risk_Catchment_Break_Scenario_List *existing_sc);
	///Delete the additional hydraulic result information 
	void delete_additional_hyd_info(void);

	///Find the relevant break scenario
	Risk_Fpl_Stress *find_relevant_break_scenario_hyd_info(const string break_string, Risk_Fpl_Stress *old, bool *new_found);

	///Sort information of the fpl-section after the time; if the time is equal sort it randomly
	void sort_result_fpl_section_time(Risk_Fpl_Stress *tosort, Fpl_Section *fpl_section  );

	///Transfer monte-carlo scenario list to break scenario list
	void transfer_mc_sc2break_sc_list(void);


	///Set error(s)
	Error set_error(const int err_type);
};
#endif