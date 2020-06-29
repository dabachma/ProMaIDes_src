#pragma once
/**\class Risk_Break_Scenario_Management
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef RISK_BREAK_SCENARIO_MANAGEMENT_H
#define RISK_BREAK_SCENARIO_MANAGEMENT_H

//QT libs


//system risk
#include "Risk_Break_Scenario.h"
#include "RiskGui_Risk_Scenario_Dia.h"
#include "RiskGui_Break_Height_Dia.h"
#include "RiskGui_Catchment_Scenario_Dia.h"
#include "Risk_Catchment_Scenario_Tree.h"
#include "Risk_Catchment_Break_Scenario_List.h"

//hyd-system
#include "Hyd_Boundary_Szenario_Management.h"
#include "Hyd_Boundary_Scenario_List.h"


//forward declaration
enum _risk_thread_type :short;

///Management-class for the evaluation and the management of the break-scenarios \ingroup risk
/**

*/
class Risk_Break_Scenario_Management : public _Sys_Common_System
{
public:
	///Default constructor
	Risk_Break_Scenario_Management(void);
	///Default destructor
	~Risk_Break_Scenario_Management(void);

	//members
	///Pointer to the table for break scenarios 
	static Tables *table;
	///Pointer to the table for failure probability of the fpl-sections 
	static Tables *table_fpl_sec_prob;

	///Manager class for the hydraulic boundary scenarios
	Hyd_Boundary_Szenario_Management hyd_bound_manager;
	///Tree of the break-scenario of the catchment area risk approach
	Risk_Catchment_Scenario_Tree catchment_sc_manager;

	///List of all break scenarios of the catchment area risk approach
	Risk_Catchment_Break_Scenario_List total_break_list;

	//method
	///Set the database table for the probabilities of the break scenarios: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Create the database table for the probabilities of the break scenarios
	static void create_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the probabilities of the break scenarios
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the probabilities of the break scenarios for the specified parameters
	static void delete_data_in_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete all data in the database table for the probabilities of the break scenarios for the specified parameters
	static void delete_data_in_table(QSqlDatabase *ptr_database, const _risk_type type, const _sys_system_id id);
	///Delete all data in the database table for the probabilities of the break scenarios for the specified parameters
	static void delete_data_in_table(QSqlDatabase *ptr_database, const _risk_type type, const int hyd_sc_id, const _sys_system_id id);
	///Select the data in the database table probabilities of the break scenarios for the specified parameters 
	static int select_data_in_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _risk_type type, const int hyd_sc_id, const _sys_system_id id);

	///Select the data in the database table probabilities of the break scenarios for the specified parameters 
	static int select_data_in_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _risk_type type, const _sys_system_id id);

	///Copy the results of a given system id to another one in database tables
	static void copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);

	///Check if the break scenario is existing in database table
	static bool check_break_scenario_exists(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_bound_id, const string break_sc_id);

	///Close and delete the database table for the probabilities of the break scenarios
	static void close_table(void);

	///Set results to the table of the probabilities of the break scenarios
	void set_result2table(Risk_Break_Scenario *scenario, QSqlDatabase *ptr_database, const int glob_id);

	///Set the database table for the probabilities of the fpl-sections: it sets the table name and the name of the columns and allocate them
	static void set_table_fpl_sec_prob(QSqlDatabase *ptr_database);
	///Create the database table for the probabilities of the fpl-sections
	static void create_table_fpl_sec_prob(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the probabilities of the fpl-sections
	static void delete_data_in_table_fpl_sec_prob(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the probabilities of the fpl-sections for the specified parameters
	static void delete_data_in_table_fpl_sec_prob(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Close and delete the database table for the probabilities of the fpl-sections
	static void close_table_fpl_sec_prob(void);
	///Select the data in the database table for the probabilities of the fpl-sections for the specified parameters 
	static int select_data_in_table_fpl_sec_prob(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int boundary_id, const int section_id);
	///Select the there are data in the database table for the probabilities of the fpl-sections 
	static int select_data_in_table_fpl_sec_prob(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id);




	///Set the list of scenarios to calculate
	void set_list_sc2calc(const int number, Risk_Break_Scenario *sc2calc, QSqlDatabase *ptr_database);
	///Get the number of scenarios to calculate
	int get_number_sc2calc(void);
	///Get the number of hydraulic calculation, which are required for the current list of scenarios
	int get_number_hyd_calculation(QSqlDatabase *ptr_database);
	///Get a pointer to the scenarios to calculate
	Risk_Break_Scenario *get_ptr_sc2calc(void);
	///Get a pointer to one scenario to calculate by a given index
	Risk_Break_Scenario *get_ptr_sc2calc(const int index);

	///Ask break scenario, which should be calculated per dialog
	bool ask_sc2calc_per_dialog(QWidget *parent, QSqlDatabase *ptr_database, const _risk_type type);

	


	///Set a list for the nobreak risk states for the catchment area risk approach
	void set_nobreak_risk_state_list(QSqlDatabase *ptr_database);
	///Set a list for the one-break risk states for the catchment area risk approach
	void set_one_break_risk_state_list(QSqlDatabase *ptr_database);
	///Set a list for the break risk states for the catchment area risk approach for a automatic calculation
	void set_auto_break_risk_state_list(QSqlDatabase *ptr_database, const _risk_ca_scenario_control params);
	///Set total scenario list to calculate for the catchment risk calculation (all are hydraulically calculated)
	void set_total_sc2calc_catchment_risk(QSqlDatabase *ptr_database);


	///Get a pointer to the total list for the risk states for the catchment area risk approach
	Risk_Catchment_Break_Scenario_List* get_total_risk_state_list(void);

	///Get number of relevant fpl sections of the system
	int get_number_fpl_section(void);
	///Get the array of the relevant fpl-section of the system
	Fpl_Section* get_ptr_fpl_sections(void);
	///Get one relevant fpl-section by a given section id
	Fpl_Section* get_ptr_fpl_section(const int section_id);
	///Get one relevant fpl-section by a given index
	Fpl_Section* get_ptr_fpl_section_index(const int index);
	///Load relevant fpl sections of the system from database
	void load_fpl_section(QSqlDatabase *ptr_database, const bool output=true);

	///Calculate and set the probabilities of the scenarios, which wehre calculated, to the database table
	void set_scenario_prob2table(QSqlDatabase *ptr_database);

	///Get a list of the break scenario to calculate compacted in _hyd_break2nobreak_sc structure (mainly for the damage calculation)
	int get_list_break_sc2calc(_dam_break2nobreak_sc **list);


	///Make list of scenarios with break waterlevels via Monte Carlo analysis
	void make_list_sc_via_monte_carlo(QSqlDatabase *ptr_database, const _risk_ca_scenario_control params);

	///Update probabilities of the scenario list with new evaluated waterlevels
	void update_scenario_probability(QSqlDatabase *ptr_database, const _risk_ca_scenario_control params);

	///Set the risk type of the manager
	void set_risk_type(const _risk_type type);

	///Set the system-id (_sys_system_id) of the object
	void set_systemid(const _sys_system_id actuel_id);

	///Ask per dialog (HydGui_Boundary_Scenario_Dia), which of the availabe hydraulic boundary scenarios should be handled and return the number
	bool ask_boundary_scenarios_per_dialog(QSqlDatabase *ptr_database, QWidget *parent, _risk_thread_type *type, const _risk_type risktype, int *number_selected);

	///Cut the sccenario list to calculate by a given index
	void cut_list_sc2calc(const int index);

	///Make a list of hydraulic boundary scenarios, which are required for calculation/generation
	bool make_list_hyd_bound_sc_calculation(QSqlDatabase *ptr_database, _risk_thread_type *type, Hyd_Boundary_Scenario_List *list);


	///Calculate and output the statistics of the catchment area risk approach
	void calc_output_catchment_area_risk_statistic(QSqlDatabase *ptr_database, const _risk_ca_scenario_control params);


	///Predict detailed risk results for not calculated break scenario(s)
	void predict_detailed_risk_results(QSqlDatabase *ptr_database, const _risk_ca_scenario_control params);


private:

	//members
	///List of break scenarios to calculate
	Risk_Break_Scenario *scenarios2calc;
	///Number of scenarios to calculate
	int number_sc2calc;

	///Number of relevant fpl-sections of the system
	int number_fpl_section;
	///Array of the relevant fpl-sections of the system
	Fpl_Section *fpl_sections;

	///The risk type for which the scenarios should be managed
	_risk_type risk_type;


	///method
	///Allocate the scenario list, which should be calculated
	void allocate_sc2calc(void);
	///Delete the scenario list, which should be calculated
	void delete_sc2calc(void);

	///Allocate the relevant fpl-sections of the system
	void allocate_fpl_section(void);
	///Delete the relevant fpl-sections of the system
	void delete_fpl_section(void);

	///Shorten the list to scenarios, which are not yet calculated
	void shorten_list2not_calculated(QSqlDatabase *ptr_database);
	///Transfer the list of the risk break scenario tree (\see Risk_Catchment_Scenario_Tree) to this list
	void transfer_list_break_scenario_tree2list(void);

	///Load the list of involved river profiles to the fpl-section
	void load_involved_profile2fpl_section(Hyd_Hydraulic_System *my_hyd);

	///Make a list of one break scenarios for the catchment risk approach
	void make_list_one_break_sc(QSqlDatabase *ptr_database, Risk_Catchment_Break_Scenario_List *list, Hyd_Boundary_Szenario_Management *hyd_sc_manager);


	///Update the probability boundaries of the mid probability in database table
	void update_boundary_mid_probability_db_table(QSqlDatabase *ptr_database, const int number_mc_calc);

	///Calculate the mid probaility of the min/max probability as a results of the integration of new hydraulic results in database table
	void middle_probability_new_hyd_info_db_table(QSqlDatabase *ptr_database);

	///Update the break scenarios with the new evaluated probabilities in database table after new hydraulic information are generated
	void update_probabilities_in_table(QSqlDatabase *ptr_database, const int hyd_bound_id, const string break_sc_id, const double new_prob, const bool predict_flag);

	///Reset the probabilities of the break scenarios in table
	void reset_probabilities_in_table(QSqlDatabase *ptr_database, const int hyd_bound_id, const bool predict_flag);

	///Calculate and transfer to database table the probabilities of each FPL-section for a given HYD-boundary scenario 
	void calc_transfer_prob_fpl_sec(Risk_Catchment_Break_Scenario_List *break_list, QSqlDatabase *ptr_database, Hyd_Boundary_Szenario *hyd_bound); 

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif
