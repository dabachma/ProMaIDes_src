#pragma once
/**\class Risk_Hydraulic_Break_Calculation
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef RISK_HYDRAULIC_BREAK_CALCULATION_H
#define RISK_HYDRAULIC_BREAK_CALCULATION_H

//qt
#include <QObject>
//system sys
#include "_Sys_Common_System.h"
#include "Common_Const.h"

//system hyd
#include "Hyd_Hydraulic_System.h"
#include "Hyd_Boundary_Szenario_Management.h"
//system fpl
#include "Fpl_Section.h"
//system risk
#include "Risk_Break_Scenario_Management.h"



//forward
class Risk_System;

///Class for the calculation of the hydraulic break scenarios \ingroup risk
/**

*/
class Risk_Hydraulic_Break_Calculation : public QObject, public _Sys_Common_System
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	Risk_Hydraulic_Break_Calculation(void);
	///Default destructor
	~Risk_Hydraulic_Break_Calculation(void);

	//members
	///Pointer to the hydraulic system for multi-thread use
	Hyd_Hydraulic_System *threads;


	//methods
	///Set the hydraulic state data
	void set_hyd_state_data(const _hyd_state_data hydraulic_state);
	///Set a pointer to the database (no copy)
	void set_ptr2database(QSqlDatabase *ptr_database);
	///Set the risk type
	void set_risk_type(const _risk_type type);
	///Set a pointer to the scenarios, which should be calculated
	void set_ptr2scenarios(Risk_Break_Scenario_Management *ptr_sc);

	///Allocate and load the hydraulic threads from database
	void alloc_load_hyd_threads(void);
	///Calculate the hydraulic threads
	void calculate_hyd_threads(void);


	///Get hydraulic break threads are running
	bool get_hydraulic_break_thread_running(int *number);
	///Get hydraulic break threads are running
	bool get_hydraulic_break_thread_running(void);
	
	///Set if the hydraulic system classes are active
	void set_hydsystem_is_active(const bool flag);

	///Set the flag for stopping the hydraulic calculation by the next occasion
	static void set_stop_next_hyd_calc(void);

	///Get the flag if a breach has started (just used for a scenario based approach)
	bool get_breach_has_started(void);

	///Set if there should be an output for the reached probability (just use it for the automatic state geneartion of the catchment approach!) 
	void set_probability_output(const bool flag);

	///Set the pointer to the risk thread
	void set_ptr_risk_thread(Risk_System *thread);

	///Manage the output of the hydraulic classes
	void recieve_output_required(int thread);




signals:
	///Emit the number of threads as string
	void emit_number_threads(QString number);
	///Send if the hydraulic thread is used
	void send_hyd_thread_runs(bool flag);
	///Send if the fpl thread is used
	void send_fpl_thread_runs(bool flag);
	///Send if the risk thread is sleeping
	void send_risk_thread_sleep(bool flag);
	///Send if the hydraulic thread is sleeping
	void send_hydraulic_thread_sleep(bool flag);


private:
	//members
	///Pointer to the database
	QSqlDatabase *ptr_database;

	///Pointer to the scenarios, which sholud be calculated
	Risk_Break_Scenario_Management *ptr_scenarios;

	///Pointer to the risk thread
	Risk_System *ptr_risk_thread;

	///Hydraulic state data
	_hyd_state_data hydraulic_state;
	///Required threads
	int required_threads;

	///Risk type for the calculation; just a scenarios based or a catchment based risk approach is possible here
	_risk_type risk_type;

	///Number of errors occurred
	_sys_error_module number_error;
	///Number of warning occurred
	_sys_warning_module number_warning;

	///Time classes for the real time calculation (start time point)
	time_t start_time;
	///Time classes for the real time calculation (actual time point)
	time_t actual_time;

	///Flag if the hydraulic system classes are active
	bool hyd_system_is_active;

	///Flag for stoping the thread by the next hydraulic calculation
	static bool stop_hyd_by_next;

	///Flag if a breach has started (just used for a scenario based approach)
	bool breach_has_started;

	///Flag if there should be an output for the reached probability (just use it for the automatic state geneartion of the catchment approach!)
	bool probability_output;




	//methods
	///Allocate the required hydraulic threads
	void allocate_hyd_threads(void);
	///Delete the hydraulic threads
	void delete_hyd_threads(void);

	///Perform the postprocessing of the hydraulic system(s) calculation 
	void make_postprocessing_hyd_system(void);
	///Waiting loop for the threads
	void wait_loop(void);


	///Initialise the break scenario hydraulic threads
	void init_break_scenario_hyd_threads(int *counter);
	///Reset the hydraulic threads
	void reset_hyd_threads(void);

	///Output the final statistic of loading the threads
	void output_final_statistic_loading(void);
	///Output the statistic of the hydraulic break-calculation
	void output_final_statistic_calculation(void);

	///Set exeptions at start of an action
	void set_exception_number_start(void);
	///Set exception_number at the end of an action
	void set_exception_number_end(void);

	///Check in case of a scenario based risk calculation if a break was launched in the hydraulic system
	void check_for_break_scenario_risk(void);

	///Get the identifer of a break scenario with the best possible break height for an section out of a list with possible calculated break scenarios
	string get_break_sc_id_for_break_height(const int sec_id, Risk_Break_Scenario *sc2calc, Risk_Catchment_Break_Scenario_List *sc_info);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);

};
#endif
