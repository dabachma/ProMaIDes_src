#pragma once
/**\class Risk_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef RISK_SYSTEM_H
#define RISK_SYSTEM_H

//QT libs
#include <QThread>
//system sys
#include "_Sys_Common_System.h"
#include "Common_Const.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Project.h"

//system hyd
#include "Hyd_Multiple_Hydraulic_Systems.h"
//system dam
#include "Dam_Damage_System.h"
//system fpl
#include "Fpl_Calculation.h"

//system risk
#include "Risk_Hydraulic_Break_Calculation.h"
#include "Risk_Break_Scenario_Management.h"
#include "RiskGui_Calculate_All_Dia.h"
#include "Risk_Damage_Predictor.h"
#include "RiskGui_Optimisation_Prediction_Dia.h"



///Enumerator to distinguish the type of the thread, e.g. calculation or data handling etc.\ingroup risk
enum _risk_thread_type :short{
	///Thread for performing a nobreak risk analysis
	risk_calc_nobreak,
	///Thread for performing a scenario-based risk analysis
	risk_calc_scenario,
	///Thread for generating the risk states for the catchment area approach (detailed risk result) for the nobreak states
	risk_generate_catchment_state,
	///Thread for performing a catchment area risk analysis (detailed risk result) for the nobreak states
	risk_calc_catchment_nobreak,
	///Thread for performing a catchment area risk analysis (detailed risk result) for the one-break states
	risk_calc_catchment_one_break,
	///Thread for performing a catchment area risk analysis (detailed risk result) for user defined states
	risk_calc_catchment_hand,
	///Thread for performing a catchment area risk analysis (detailed risk result) for automatically generated states
	risk_calc_catchment_auto,
	///Thread for performing a catchment area risk analysis for the cumulation of the risk states
	risk_calc_catchment_cumulated,
	///Thread for calculation and output of the catchment are risk approach
	risk_catchment_statistic,
	///Thread is creating the risk tables for a database project
	risk_create_tab,
	///Thread is checking the risk tables of a database project
	risk_check_tab,
	///Thread for deleting the risk data
	risk_del,
	///Thread for deleting the risk results data of a system state in database
	risk_del_risk_result,
	///Thread for deleting all risk relevant results (fpl, dam, hyd, risk) data of a system state in database 
	risk_del_all_result,
	///Thread for outputing the statistcis of already generated risk-results to display/console
	risk_output_stat_result,
	///Thread for computing different steps of the risk analysis serial
	risk_calc_serial_steps,
	///Thread for a unknown action
	risk_thread_unknown

};
///Namespace for the keystring in a database table of the control parameters for a risk calculation \ingroup risk
namespace risk_control_param{
	///Keystring for identifier of the main outflow river 
	const string main_outflow_rv("MAIN_OUTFLOW_RIVER");
	///Keystring for the maximum of hydraulic calculation for the catchment risk calculation per hydraulic boundary scenario
	const string max_hydraulic("MAX_HYDRAULIC_SIM");
	///Keystring for the boundary of cummulated probability when a search for further break scenarios is stopped (maximum is 1.0)
	const string boundary_prob("BOUNDARY_PROBABILITY");
	///Keystring for number of monte-carlo simulation 
	const string number_mc_sim("NUMBER_MC");
	///Keystring for type of random generator
	const string rand_generator("RAND_GENERATOR");
	///Keystring for number of intervals for dividing the frc-cruves (minimum is 2) 
	const string number_intervals("NUMBER_FRC_INTERVALS");
	///Keystring if predicted values should be used 
	const string use_prediction("USE_PREDICTION");


	///Default control parameter: maximum of hydraulic calculation for the catchment risk calculation
	const int max_hydraulic_def=500;
	///Default control parameter: boundary of cummulated probability when a search for further break scenarios is stopped [-] (maximum is 1.0)
	const double boundary_prob_def=0.9;

	///Default control parameter: boundary of the predicted risk when a search for further break scenarios is stopped [%] (maximum is 100.0 %)
	const double boundary_risk_predicted_def=90.0;

	///Default control parameter: Maximum number of monte-carlo simulation for the generation of the catchment area scenarios
    const int max_mc_def=50000000;
	///Default control parameter: Minimum number of monte-carlo simulation for the generation of the catchment area scenarios
    const int min_mc_def=1000;
	///Default control parameter: Number of monte-carlo simulation for the generation of the catchment area scenarios
	const int no_mc_def=1000000;	
};
///Data structure for gathering the risk results \ingroup risk
struct _risk_risk_results{
	///Results of the damage calculation
	_dam_damage_results dam_results;
	///Maximum outflow of the main river
	double max_outflow;
	///Probability of ths break scenario
	double prob_break_sc;
	///Probability of the hydraulic boundary scenario
	double prob_hyd_bound_sc;
	///Reoccurence probability of the hydraulic boundary scenario
	double reocc_prob_hyd_bound_sc;
	///Annuality of the hydraulic boundary scenario
	int annuality_hyd_bound_sc;
	///Number of applied risk results
	int number_applied_result;
};

///Class for the calculation of the flood risk \ingroup risk
/**
There three types of risk calculation available:
	- a risk analysis without any break of a fpl-section,
	- a pure sceneario based risk analysis,
	- a risk analysis for the catchment area.

*/
class Risk_System: public QThread, public _Sys_Common_System
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT 

public:
	///Default constructor
	Risk_System(void);
	///Default destructor
	~Risk_System(void);


	//members
	///Pointer to the table for storing the configuration for the risk calculation
	static Tables *table_config;
	///Pointer to the table for detailed results of the risk calculation (risk per scenario)
	static Tables *table_detailed_results;
	///Pointer to the table for cumulated results of the risk calculation (risk per scenario)
	static Tables *table_cumulated_results;
	///Pointer to the table for results of the risk calculation (total risk)
	static Tables *table_results;

	///Pointer to the table for detailed results per damage raster-element of the economical damage raster
	static Tables *table_results_elem_ecn;
	///Pointer to the table for detailed results per damage raster-element of the ecological biotope-type damage raster
	static Tables *table_results_elem_eco_btype;
	///Pointer to the table for detailed results per damage raster-element of the ecological soil-erosion damage raster
	static Tables *table_results_elem_eco_soil;
	///Pointer to the table for detailed results per damage raster-element of the psycho-social damage raster
	static Tables *table_results_elem_pys;
	///Pointer to the table for detailed results per damage raster-element of the people2risk damage raster
	static Tables *table_results_elem_pop;
	///Pointer to the table for detailed results per damage point of the simple counting damage raster
	static Tables *table_results_point_sc;


	///Break scenario manager
	Risk_Break_Scenario_Management break_sc_manager;

	//method
	///Set the database table for the configuration: it sets the table name and the name of the columns and allocate them
	static void set_table_config(QSqlDatabase *ptr_database);
	///Create the database table for configuration
	static bool create_table_config(QSqlDatabase *ptr_database);
	///Reset all data in the database table for the configuration to the default values
	static void reset_data_in_table_config(QSqlDatabase *ptr_database);
	///Close and delete the database table for the configuration
	static void close_table_config(void);
	///Set the predefined data to the configuration database table; it is called after the creation of this table
	static void set_predefined_data2config_table(QSqlDatabase *ptr_database, const int river_id);

	///Set the database table for detailed results of the risk calculation: it sets the table name and the name of the columns and allocate them
	static void set_resulttable_detailed(QSqlDatabase *ptr_database, const bool not_close = false);
	///Create the database table for detailed results of the risk calculation
	static void create_resulttable_detailed(QSqlDatabase *ptr_database);
	///Delete all data in the database table for detailed results of the risk calculation
	static void delete_data_in_resulttable_detailed(QSqlDatabase *ptr_database);
	///Delete all data in the database table for detailed results of the risk calculation for specified parameters
	static void delete_data_in_resulttable_detailed(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type);
	///Delete all data in the database table for detailed results of the risk calculation for specified parameters 
	static void delete_data_in_resulttable_detailed(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id);
	///Delete all data in the database table for detailed results of the risk calculation for specified parameters
	static void delete_data_in_resulttable_detailed(QSqlDatabase *ptr_database, _sys_system_id id);
	///Close and delete the database table for detailed results of the risk calculation
	static void close_resulttable_detailed(void);
	///Select the data in the database table for detailed results of the risk calculation for specified parameters
	static int select_data_in_resulttable_detailed(QSqlQueryModel *model, QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type);
	///Select the data in the database table for detailed results of the risk calculation for specified parameters
	static int select_data_in_resulttable_detailed(QSqlQueryModel *model, QSqlDatabase *ptr_database, const int area_state, _risk_type type);
	///Select the data in the database table for detailed results of the risk calculation for specified parameters
	static int select_data_in_resulttable_detailed(QSqlQueryModel *model, QSqlDatabase *ptr_database, const int glob_id);

	///Check if there are detailed risk results available in database table for the given parameters
	static bool check_detailed_risk_results_available(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Check if there are detailed risk results available in database table for the given parameters
	static bool check_detailed_risk_results_available(QSqlDatabase *ptr_database, const _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id);


	///Set the database table for cumulated results of the risk calculation: it sets the table name and the name of the columns and allocate them
	static void set_resulttable_cumulated(QSqlDatabase *ptr_database, const bool not_close = false);
	///Create the database table for cumulated results of the risk calculation
	static void create_resulttable_cumulated(QSqlDatabase *ptr_database);
	///Delete all data in the database table for cumulated results of the risk calculation
	static void delete_data_in_resulttable_cumulated(QSqlDatabase *ptr_database);
	///Delete all data in the database table for cumulated results of the risk calculation for specified parameters
	static void delete_data_in_resulttable_cumulated(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type);
	///Delete all data in the database table for cumulated results of the risk calculation for specified parameters
	static void delete_data_in_resulttable_cumulated(QSqlDatabase *ptr_database, _sys_system_id id);
	///Close and delete the database table for cumulated results of the risk calculation
	static void close_resulttable_cumulated(void);
	///Select the data in the database table for cumulated results of the risk calculation for specified parameters
	static int select_data_in_resulttable_cumulated(QSqlQueryModel *model, QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type);
	///Select the data in the database table for cumulated results of the risk calculation for specified parameters
	static int select_data_in_resulttable_cumulated(QSqlQueryModel *model, QSqlDatabase *ptr_database, const int glob_id);

	///Set the database table for results of the risk calculation: it sets the table name and the name of the columns and allocate them
	static void set_resulttable(QSqlDatabase *ptr_database, const bool not_close = false);
	///Create the database table for results of the risk calculation
	static void create_resulttable(QSqlDatabase *ptr_database);
	///Delete all data in the database table for results of the risk calculation
	static void delete_data_in_resulttable(QSqlDatabase *ptr_database);
	///Delete all data in the database table for results of the risk calculation for specified parameters
	static void delete_data_in_resulttable(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type);
	///Delete all data in the database table for results of the risk calculation for specified parameters
	static void delete_data_in_resulttable(QSqlDatabase *ptr_database, _sys_system_id id);
	///Close and delete the database table for results of the risk calculation
	static void close_resulttable(void);
	///Select the data in the database table for results of the risk calculation for specified parameters
	static int select_data_in_resulttable(QSqlQueryModel *model, QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type);
	///Check if data in the database table for results of the risk calculation for specified parameters is set
	static bool check_data_in_resulttable(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type);
	///Select the data in the database table for results of the risk calculation for specified parameters
	static int select_data_in_resulttable(QSqlQueryModel *model, QSqlDatabase *ptr_database, const int glob_id);

	///Set the table for detailed results per damage raster-element of the economical damage raster: it sets the table name and the name of the columns and allocate them
	static void set_table_result_elem_ecn(QSqlDatabase *ptr_database);
	///Create the table for detailed results per damage raster-element of the economical damage raster
	static void create_table_result_elem_ecn(QSqlDatabase *ptr_database);
	///Delete all data in the table for detailed results per damage raster-element of the economical damage raster
	static void delete_data_in_table_result_elem_ecn(QSqlDatabase *ptr_database);
	///Close and delete the table for detailed results per damage raster-element of the economical damage raster
	static void close_table_result_elem_ecn(void);
	///Delete all data in the database table for detailed results per damage raster-element of the economical damage raster for specified parameters
	static void delete_data_in_resulttable_elem_ecn(QSqlDatabase *ptr_database, _sys_system_id id);
	///Delete all data in the database table for detailed results per damage raster-element of the economical damage raster for specified parameters
	static void delete_data_in_resulttable_elem_ecn(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type);
	///Delete all data in the database table for detailed results per damage raster-element of the economical damage raster for specified parameters 
	static void delete_data_in_resulttable_elem_ecn(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id);
	///Check if there are data available in the database table for detailed results per damage raster-element of the economical damage raster for specified parameters 
	static bool check_data_in_resulttable_elem_ecn(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id);

	///Set the table for detailed results per damage raster-element of the ecological biotope-type damage raster: it sets the table name and the name of the columns and allocate them
	static void set_table_result_elem_eco_btype(QSqlDatabase *ptr_database);
	///Create the table for detailed results per damage raster-element of the ecological biotope-type damage raster
	static void create_table_result_elem_eco_btype(QSqlDatabase *ptr_database);
	///Delete all data in the table for detailed results per damage raster-element of the ecological biotope-type damage raster
	static void delete_data_in_table_result_elem_eco_btype(QSqlDatabase *ptr_database);
	///Close and delete the table for detailed results per damage raster-element of the ecological biotope-type damage raster
	static void close_table_result_elem_eco_btype(void);
	///Delete all data in the database table for detailed results per damage raster-element of the ecological biotope-type damage raster for specified parameters
	static void delete_data_in_resulttable_elem_eco_btype(QSqlDatabase *ptr_database, _sys_system_id id);
	///Delete all data in the database table for detailed results per damage raster-element of the ecological biotope-type damage raster for specified parameters
	static void delete_data_in_resulttable_elem_eco_btype(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type);
	///Delete all data in the database table for detailed results per damage raster-element of the ecological biotope-type damage raster for specified parameters 
	static void delete_data_in_resulttable_elem_eco_btype(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id);
	///Check if there are data available in the database table for detailed results per damage raster-element of the ecological biotope-type damage raster for specified parameters 
	static bool check_data_in_resulttable_elem_eco_btype(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id);


	///Set the table for detailed results per damage raster-element of the ecological soil-erosion damage raster: it sets the table name and the name of the columns and allocate them
	static void set_table_result_elem_eco_soil(QSqlDatabase *ptr_database);
	///Create the table for detailed results per damage raster-element of the ecological soil-erosion damage raster
	static void create_table_result_elem_eco_soil(QSqlDatabase *ptr_database);
	///Delete all data in the table for detailed results per damage raster-element of the ecological soil-erosion damage raster
	static void delete_data_in_table_result_elem_eco_soil(QSqlDatabase *ptr_database);
	///Close and delete the table for detailed results per damage raster-element of the ecological soil-erosion damage raster
	static void close_table_result_elem_eco_soil(void);
	///Delete all data in the database table for detailed results per damage raster-element of the ecological soil-erosion damage raster for specified parameters
	static void delete_data_in_resulttable_elem_eco_soil(QSqlDatabase *ptr_database, _sys_system_id id);
	///Delete all data in the database table for detailed results per damage raster-element of the ecological soil-erosion damage raster for specified parameters
	static void delete_data_in_resulttable_elem_eco_soil(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type);
	///Delete all data in the database table for detailed results per damage raster-element of the ecological soil-erosion damage raster for specified parameters 
	static void delete_data_in_resulttable_elem_eco_soil(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id);
	///Check if there are data available in the database table for detailed results per damage raster-element of the ecological soil-erosion damage raster for specified parameters 
	static bool check_data_in_resulttable_elem_eco_soil(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id);

	///Set the table for detailed results per damage raster-element of the people2risk damage raster: it sets the table name and the name of the columns and allocate them
	static void set_table_result_elem_pop(QSqlDatabase *ptr_database);
	///Create the table for detailed results per damage raster-element of the people2risk damage raster
	static void create_table_result_elem_pop(QSqlDatabase *ptr_database);
	///Delete all data in the table for detailed results per damage raster-element of the people2risk damage raster
	static void delete_data_in_table_result_elem_pop(QSqlDatabase *ptr_database);
	///Close and delete the table for detailed results per damage raster-element of the people2risk damage raster
	static void close_table_result_elem_pop(void);
	///Delete all data in the database table for detailed results per damage raster-element of the people2risk damage raster for specified parameters
	static void delete_data_in_resulttable_elem_pop(QSqlDatabase *ptr_database, _sys_system_id id);
	///Delete all data in the database table for detailed results per damage raster-element of the people2risk damage raster for specified parameters
	static void delete_data_in_resulttable_elem_pop(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type);
	///Delete all data in the database table for detailed results per damage raster-element of the people2risk damage raster for specified parameters 
	static void delete_data_in_resulttable_elem_pop(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id);
	///Check if there are data available in the database table for detailed results per damage raster-element of the people2risk damage raster for specified parameters 
	static bool check_data_in_resulttable_elem_pop(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id);

	///Set the table for detailed results per damage raster-element of the psycho-social damage raster: it sets the table name and the name of the columns and allocate them
	static void set_table_result_elem_pys(QSqlDatabase *ptr_database);
	///Create the table for detailed results per damage raster-element of the psycho-social damage raster
	static void create_table_result_elem_pys(QSqlDatabase *ptr_database);
	///Delete all data in the table for detailed results per damage raster-element of the psycho-social damage raster
	static void delete_data_in_table_result_elem_pys(QSqlDatabase *ptr_database);
	///Close and delete the table for detailed results per damage raster-element of the psycho-social damage raster
	static void close_table_result_elem_pys(void);
	///Delete all data in the database table for detailed results per damage raster-element of the psycho-social damage raster for specified parameters
	static void delete_data_in_resulttable_elem_pys(QSqlDatabase *ptr_database, _sys_system_id id);
	///Delete all data in the database table for detailed results per damage raster-element of the psycho-social damage raster for specified parameters
	static void delete_data_in_resulttable_elem_pys(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type);
	///Delete all data in the database table for detailed results per damage raster-element of the psycho-social damage raster for specified parameters 
	static void delete_data_in_resulttable_elem_pys(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id);
	///Check if there are data available in the database table for detailed results per damage raster-element of the psycho-social damage raster for specified parameters 
	static bool check_data_in_resulttable_elem_pys(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id);

	///Set the table for detailed results per damage point of the simple counting damage: it sets the table name and the name of the columns and allocate them
	static void set_table_result_point_sc(QSqlDatabase *ptr_database);
	///Create the table for detailed results per damage point of the simple counting damage
	static void create_table_result_point_sc(QSqlDatabase *ptr_database);
	///Delete all data in the table for detailed results per damage point of the simple counting damage
	static void delete_data_in_table_result_point_sc(QSqlDatabase *ptr_database);
	///Close and delete the table for detailed results per damage point of the simple counting damage
	static void close_table_result_point_sc(void);
	///Delete all data in the database table for detailed results per damage point of the simple counting damage for specified parameters
	static void delete_data_in_resulttable_point_sc(QSqlDatabase *ptr_database, _sys_system_id id);
	///Delete all data in the database table for detailed results per damage point of the simple counting damage for specified parameters
	static void delete_data_in_resulttable_point_sc(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type);
	///Delete all data in the database table for detailed results per damage point of the simple counting damage for specified parameters 
	static void delete_data_in_resulttable_point_sc(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id);
	///Check if there are data available in the database table for detailed results per damage point of the simple counting damage for specified parameters 
	static bool check_data_in_resulttable_point_sc(QSqlDatabase *ptr_database, _sys_system_id id, _risk_type type, const string break_id, const int hyd_bound_id);

	///Input the control parameters per database table
	void set_input(QSqlDatabase *ptr_database, const bool with_output=true);

	///Ask flags via dialog which part of the database tables should be restored with the default values and which risk results should be deleted
	bool ask_delete_result_restore_database(QWidget *parent);

	///Ask the flags via dialog, which steps of the risk analysis should be calculated in serial
	bool ask_steps_serial_calculation(QWidget *parent);

	///Ask for the boundaries of the predicted risk for an automatic risk calculation in caase of the catchment area risk approach
	bool ask_boundries_predict_risk(QWidget *parent);

	///Set and connect the damage results and the probabilities to the detailed results for the nobreak risk approach
	void set_connect_dam_prob2detailed_result_nobreak(void);
	///Set and connect the damage results and the probabilities to the detailed results for the scenario- and catchment area base risk approach
	void set_connect_dam_prob2detailed_result_break(void);
	///Calculate the detailed risk 
	void calculate_detailed_result(void);
	///Calculate the cumulated risk result
	void calculate_cumulated_result(void);
	///Calculate the total risk result
	void calculate_total_result(void);


	///Set the hydraulic state data
	void set_hyd_state_data(const _hyd_state_data hydraulic_state);

	///Get the number of detailed risk results
	int get_number_detailed_results(void);
	///Get a pointer to the detailed risk results
	_risk_risk_results* get_ptr_detailed_results(void);
	///Get the detailed risk results by a given index
	_risk_risk_results get_detailed_results(const int index);
	///Get the number of cumulated risk results
	int get_number_cumulated_results(void);
	///Get a pointer to the cumulated risk results
	_risk_risk_results* get_ptr_cumulated_results(void);
	///Get the cumulated risk results by a given index
	_risk_risk_results get_cumulated_results(const int index);
	///Get the total risk results
	_risk_risk_results get_total_result(void);

	///Set the thread type for specifiying the task of the thread
	void set_thread_type(_risk_thread_type type);
	///Set a pointer to the database; the copy of the database is made in this method
	void set_ptr2database(QSqlDatabase *ptr_database);

	///Get the thread type
	_risk_thread_type get_thread_type(void);

	///Create a whole set of risk database tables
	void create_risk_database_tables(void);
	///Check all risk database tables, for their existence in the database and their declaration in the database table-file
	void check_risk_database_tables(void);
	///Delete the data of all risk database tables
	void delete_data_risk_database_tables(void);
	///Close all risk database tables
	static void close_risk_database_tables(void);

	///Set the stop thread flag
	void set_stop_thread_flag(const bool flag);
	///Set just the risk stop thread flag
	static void set_risk_stop_thread_flag(const bool flag);
	///Check the stop thread flag
	static void check_stop_thread_flag(void);
	///Get the stop thread flag
	static bool get_stop_thread_flag(void);

	///Get the control parameters for the catchment area based risk calculation
	_risk_ca_scenario_control get_ca_control_parameter(void);
	///Get the number of the main outflow river
	int get_number_main_outflow_river(void);


	///Set the risk state flag
	 void set_risk_state_flag(const _risk_state_flags flags);
	///Get the risk state flag
	_risk_state_flags get_risk_state_flag(void);

	///Run the thread; the type of thread, which will be performed, has to be set before with set_thread_type(_risk_thread_type type)
	void run(void);

	///Get the number of errors
	int get_number_error(void);

	///Output the configuration members of the risk calculation to display/console
	void output_members(void);

	///Set the system-id (_sys_system_id) of the object
	void set_systemid(const _sys_system_id actuel_id);

	///Set this thread to sleeping-modus
	void set_thread2sleep(const int sec);

	///Get the flag if the thread has started
	bool get_thread_has_started(void);

	///Cumulate the generate catchment area risk states to one risk result
	void cumulate_catchment_risk(const bool internaly);

	///Check if the probability boundary is reached (catchement area approach)
	bool check_probability_boundary_is_reached(void);

	///Set the flag if also the HYD-, DAM- and FPL-resuts should be deleted from database
	void set_del_hyd_dam_fpl_flag(const bool del_flag);

public slots:
	///Send the status of the fpl-thread
	void send_status_fpl_thread(void);
	///Send the status of the fpl-thread, if it is used in the hydraulic break calculation
	void sent_status_fpl_thread_break_calc(bool flag);
	///Send the status of the hyd-thread, if it is used in the hydraulic break calculation
	void sent_status_hyd_thread_break_calc(bool flag);
	///Send the number of the hyd-thread, if it is used in the hydraulic break calculation or just in the hydraulic caluclation
	void sent_number_hyd_thread_break_calc(QString number);

	///Recieve and output the number of hydraulic calculation, which are already done from the hydraulic thread (just nobreak risk approach)
	void recieve_output_hyd_calc_done(QString done);

	///Set the risk thread to sleeping-status
	void set_status_risk_thread(bool flag);
	///Set the hydraulic thread to sleeping-status
	void set_status_hydraulic_thread(bool flag);

signals:
	///Send if the hydraulic thread is used
	void send_hyd_thread_runs(bool flag);
	///Send if the damage thread is used
	void send_dam_thread_runs(bool flag);
	///Send if the fpl thread is used
	void send_fpl_thread_runs(bool flag);
	///Emit the number of threads as string
	void emit_number_threads(QString number);
	///Emit that the risk thread sleeps
	void send_risk_thread_sleeps(bool flag);
	///Emit that the hydraulic thread sleeps
	void send_hydraulic_thread_sleeps(bool flag);

	///Emit a flag for enabling the action for a stop at the next hydraulic calculation step
	void emit_stop_next_hyd_calc(bool flag);

private slots:

	///Manage the output of the hydraulic classes
	void recieve_output_required(int thread);

	///Reconnect the hydraulic thread of the HYD-system for the output requirement
	void reconnect_hyd_threads(void);

private:
	//members
	///Type of thread specifiying the task of the thread \see _risk_thread_type
	_risk_thread_type thread_type;

	///Flag if the thread are aborted by the user
	static bool abort_thread_flag;

	///Flag of the risk states, specifying the state of the risk calculation
	 _risk_state_flags risk_states;
	 
	///Hydraulic state data
	_hyd_state_data hydraulic_state;

	///Copy of the database
	QSqlDatabase qsqldatabase;

	///Number of errors occurred
	_sys_error_module number_error;
	///Number of warning occurred
	_sys_warning_module number_warning;


	///Time classes for the real time calculation (start time point)
	time_t start_time;
	///Time classes for the real time calculation (actual time point)
	time_t actual_time;

	///Identifier of the main outflow river 
	int id_main_river;
	///Control parameters for the generation of the break scenario for the catchment area risk approach
	_risk_ca_scenario_control control_params_ca;

	///Flag if the configuration parameters should be restored in the database table
	bool restore_config;
	///Flag if the results of the risk type nobreak (_risk_type) should be deleted
	bool del_nobreak_result;
	///Flag if the results of the risk type scenario based (_risk_type) should be deleted
	bool del_scenario_result;
	///Flag if the results of the risk type catchment based (_risk_type) should be deleted
	bool del_catchment_result;

	///Flag if the damage results should be deleted (from which risk type depends on the flags above, like del_scenario_result or del_catchment_result)
	bool del_dam_results;
	///Flag if the hydraulic results should be deleted (from which risk type depends on the flags above, like del_scenario_result or del_catchment_result)
	bool del_hyd_results;
	///Flag if the fpl results should be deleted (from which risk type depends on the flags above, like del_scenario_result or del_catchment_result)
	bool del_fpl_results;

	///Flag if also the HYD - , DAM - and FPL-results should be deleted from database
	bool hyd_dam_fpl_del_flag;


	///Pointer to the hydraulic system 
	Hyd_Multiple_Hydraulic_Systems *hyd_calc;
	///Pointer to the damage system 
	Dam_Damage_System *dam_calc;
	///Pointer to the fpl system
	Fpl_Calculation *fpl_calc;
	///Flag if the fpl-system is working
	bool fpl_system_working;

	///Type of risk calculation
	_risk_type risk_type;

	///Detailed results of the risk calculation 
	_risk_risk_results *detailed_results;
	///Number of detailed risk results
	int number_detailed_results;

	///Cumulated results of the risk calculation 
	_risk_risk_results *cumulated_results;
	///Number of detailed risk results
	int number_cumulated_results;

	///Total result of the risk calculation 
	_risk_risk_results total_results;

	///Hydraulic break calculation
	Risk_Hydraulic_Break_Calculation *hyd_break_calc;

	///Flag if the fpl-thread is used in the hydraulic break calculation class
	bool fpl_thread_is_used_extern;

	///Flag if the threads has started
	bool thread_is_started;

	///Flag for the calculation of the catchment risk nobreak scenarios in serial with other steps (it is set via dialog)
	bool calc_ca_nobreak_step;
	///Flag for the calculation of the catchment risk scenario list generation in serial with other steps (it is set via dialog)
	bool calc_ca_sc_lists_step;
	///Flag for the calculation of the catchment risk one break scenarios in serial with other steps (it is set via dialog)
	bool calc_ca_one_break_step;
	///Flag for the calculation of the catchment risk automatic scenario calculation in serial with other steps (it is set via dialog)
	bool calc_ca_auto_step;


	///Total risk (without integration) inclusive the predicted risk results
	_risk_predicted_risk total_predicted;
	///Total calculated risk (without integration) exclusive the predicted risk results
	_risk_predicted_risk calculated_predicted;

	//method
	///Calculate nobreak risk 
	void calculate_nobreak_risk(void);

	///Calculate scenario based risk
	void calculate_scenario_based_risk(void);

	///Generate the risk states for the catchment area risk approach via a Monte Carlo analysis
	void generate_risk_states_catchment(void);
	///Calculate catchment area risk for nobreak state (detailed risk result)
	void calculate_catchment_risk_nobreak(void);
	///Calculate catchment area risk for one break states (detailed risk result)
	void calculate_catchment_risk_one_break(void);
	///Calculate catchment area risk by user defined state (detailed risk result)
	void calculate_catchment_risk_user_defined(void);
	///Calculate catchment area risk by automatically generated states (detailed risk result)
	void calculate_catchment_risk_auto(void);


	///Calculate and output the statistics of the catchment risk approach
	void calc_output_catchment_risk_statistic(const bool internaly);

	///Calculate different steps of the risk analysis serial. The steps are ask via a dialog.
	void calc_steps_risk_analysis_serial(void);

	///Output detailed risk results to database table
	void output_detailed_risk_results2database(void);
	///Output detailed risk results to display/console
	void output_detailed_risk_results(void);
	///Output cumulated risk results to database table
	void output_cumulated_risk_results2database(void);
	///Output cumulated risk results to display/console
	void output_cumulated_risk_results(void);
	///Output total risk result to database table
	void output_total_result2database(void);
	///Output total risk results to display/console
	void output_total_results(void);

	///Delete the risk result data and restore the configuartion in database table
	void delete_risk_data(void);


	///Allocate the hydraulic system (just needed for the nobreak risk approach)
	void allocate_hyd_system(void);
	///Delete the hydraulic system (just needed for the nobreak risk approach)
	void delete_hyd_system(void);
	///Allocate the hydraulic scenario break calcuation (needed for the scenario and catchment area risk approach)
	void allocate_hyd_break_calculation(void);
	///Delete the hydraulic break calculation (needed for the scenario and catchment area risk approach)
	void delete_hyd_break_calculation(void);

	///Allocate the damage system
	void allocate_dam_system(void);
	///Delete the damage system
	void delete_dam_system(void);
	///Allocate the fpl system
	void allocate_fpl_system(void);
	///Delete the fpl system
	void delete_fpl_system(void);

	///Allocate the detailed risk result array
	void allocate_detailed_results(void);
	///Delete the detailed risk result array
	void delete_detailed_results(void);
	///Sort the detailed results after the probability of reoccurence of the hydraulic boundary scenario
	void sort_detailed_result(void);
	///Allocate the cumulated risk result array
	void allocate_cumulated_results(void);
	///Delete the cumulated risk result array
	void delete_cumulated_results(void);
	///Add antry to the cumulated risk result array
	void add_entry2cumulated_results(void);

	///Initialize the risk result structure
	void init_risk_result_struct(_risk_risk_results *init);
	///Set the risk result structure to the detailed risk
	void risk_result_struct2detailed(_risk_risk_results *result);
	///Add to a base risk result a given risk result for cumulation
	void add2risk_result_cumulation(_risk_risk_results *base, _risk_risk_results *given);
	///Stepwise integration of the total risk result
	void integration2total_result(const double lower_bound, const double upper_bound, _risk_risk_results *given);

	///Set exeptions at start of an action
	void set_exception_number_start(void);
	///Set exception_number at the end of an action
	void set_exception_number_end(void);

	///Output the error statistic of the risk system
	void output_error_statistic(void);
	///Output the statistic of the risk-calculation
	void output_final_statistic_calculation(void);

	///Check controll parameters
	void check_control_params(void);

	///Check if already calculated and calculate hydraulic boundary scenario (nobreak scenarios)
	void check_calc_hyd_nobreak_sc(void);
	///Check if already calculated and calculate damage results for hydraulic boundary scenario (nobreak scenarios)
	void check_calc_dam_nobreak_sc(void);
	///Check if already calculated and calculate damage results for hydraulic boundary scenario (scenario based and catchment area based risk approach)
	void check_calc_dam_sc(void);

	///Delete all risk results of a system state in database as thread
	void delete_risk_result(void);
	///Delete all risk relevant results (fpl, hyd, dam, risk) of a system state in database as thread
	void delete_all_relevant_result(void);

	///Waiting loop for the threads (hyd/fpl)
	void wait_loop(void);

	///Output the statistics of already generated detailed risk results
	void output_statistics_generated_detailed_results(void);

	///Calculate the statistic of the predicted risk
	void calculate_predicted_risk_statistic(const bool with_output);

	///Output the statistic of the predicted risk
	void output_statistic_predicted_risk(Risk_Catchment_Break_Scenario_List *list);


	///Calculate, generate and insert to database table the detailed risk result of the damage elements
	void calc_detailed_result_element(void);
	///Calculate, generate and insert to database table the detailed risk result of the economical damage elements
	void calc_detailed_result_element_ecn(_risk_risk_results *total);
	///Calculate, generate and insert to database table the detailed risk result of the ecological biotope-type damage elements
	void calc_detailed_result_element_eco_btype(_risk_risk_results *total);
	///Calculate, generate and insert to database table the detailed risk result of the ecological soil-erosion damage elements
	void calc_detailed_result_element_eco_soil(_risk_risk_results *total);
	///Calculate, generate and insert to database table the detailed risk result of the people2risk  damage elements
	void calc_detailed_result_element_pop(_risk_risk_results *total);
	///Calculate, generate and insert to database table the detailed risk result of the psycho-social damage elements
	void calc_detailed_result_element_pys(_risk_risk_results *total);
	///Calculate, generate and insert to database table the detailed risk result of the simple counting points
	void calc_detailed_result_point_sc(_risk_risk_results *total);

	///Check if there are data in one of the detailed risk results element table
	bool check_detailed_risk_element_results(const string break_id, const int hyd_bound_id);

	///Generate sql-header string for inserting data to the database table for detailed results per damage raster-element of the economical damage raster 
	string generate_sql_header_string_elem_ecn(void);
	///Generate sql-header string for inserting data to the database table for detailed results per damage raster-element of the ecological biotope-type damage raster 
	string generate_sql_header_string_elem_eco_btype(void);
	///Generate sql-header string for inserting data to the database table for detailed results per damage raster-element of the ecological soil-erosion damage raster 
	string generate_sql_header_string_elem_eco_soil(void);
	///Generate sql-header string for inserting data to the database table for detailed results per damage raster-element of the people2risk damage raster 
	string generate_sql_header_string_elem_pop(void);
	///Generate sql-header string for inserting data to the database table for detailed results per damage raster-element of the psycho-social damage raster
	string generate_sql_header_string_elem_pys(void);
	///Generate sql-header string for inserting data to the database table for detailed results per damage point of the simple counting damage 
	string generate_sql_header_string_point_sc(void);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
