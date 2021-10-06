#pragma once
/**\class Hyd_Boundary_Szenario
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_BOUNDARY_SZENARIO_MANAGEMENT_H
#define HYD_BOUNDARY_SZENARIO_MANAGEMENT_H


//hyd-class
#include "Hyd_Boundary_Szenario.h"
#include "Hyd_Boundary_Scenario_List.h"
#include "HydGui_Boundary_Scenario_Dia.h"
#include "HydGui_Bound_Scenario_Data_Dia.h"
//sys-classe
#include "Tables.h"
#include "Common_Const.h"
#include "Sys_Diverse_Text_Dia.h"

///Managment class for the boundary szenarios for one hydraulic system \ingroup hyd
/**
This class manages, which are already defined, calculated etc. The defined szenarios are stored
in a database table, which is also managed here as an static pointer (table).
This class sets to the child-classes of Hyd_Boundary_Szenario the index of the szenario, which should be handeld
(data-import, calculation etc.). In general, this setting is done via the hydraulic system (Hyd_Hydraulic_System)
as interface. The further spreading of the information is done by the hydraulic system.
 
\see Hyd_Boundary_Szenario, Hyd_Hydraulic_System 

*/

class Hyd_Boundary_Szenario_Management : public _Sys_Common_System
{
public:
	///Default constructor
	Hyd_Boundary_Szenario_Management(void);
	///Default destructor
	~Hyd_Boundary_Szenario_Management(void);

	//members
	///Pointer to the database table the boundary szenarios of an hydraulic system
	/**This pointer is allocated with set_table(QSqlDatabase *ptr_database) and deleted with void close_table(void) */
	static Tables *table;

	//method

	///Get number of szenarios
	int get_number_sz(void);

	///Get a pointer to the scenario with the given index 
	Hyd_Boundary_Szenario* get_ptr_sz(const int number);
	///Get apointer to the scenario with a given scenario index
	Hyd_Boundary_Szenario* get_ptr_sz_id_given(const int id);

	///Create the database table for the boundary szenarios of an hydraulic system
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the boundary szenarios of an hydraulic system: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table the boundary szenarios of an hydraulic system
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the boundary szenarios of an hydraulic system
	static void close_table(void);
	///Check if a base boundary scenario is set in the database table
	static bool check_base_scenario_is_set(QSqlDatabase *ptr_database);
	///Switch the applied-flag for the hydraulic boundary scenario in the database table for a defined system state
	static void switch_applied_flag_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the hydraulic boundary scenario in the database table for a defined system state and id
	static void switch_applied_flag_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int hyd_sc);
	///Select and count the number of boundary scenarios for the given measure state in a database table
	static int select_boundary_scenario_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id);


	///Set the relevant hydraulic boundary scenarios per database table
	void set_szenario_per_db(QSqlDatabase *ptr_database);
	///Set the existing hydraulic boundary scenarios per database table
	void set_existing_szenario_per_db(QSqlDatabase *ptr_database);
	///Set the hydraulic boundary scenarios of the area state per database table
	void set_area_state_szenario_per_db(QSqlDatabase *ptr_database);
	///Select hydraulic boundary scenarios, which hydraulic results have not yet calculated
	int select_scenarios_without_hyd_result(QSqlDatabase *ptr_database);

	///Ask per dialog (HydGui_Boundary_Scenario_Dia), which of the availabe hydraulic boundary scenarios should be handled and return the number
	int ask_boundary_scenarios_per_dialog(QSqlDatabase *ptr_database, const string txt, QWidget *parent);
	///Ask per dialog (HydGui_Boundary_Scenario_Dia), which of the availabe hydraulic boundary scenarios should be handled and return the number
	bool ask_boundary_scenarios_per_dialog(const string txt, QWidget *parent, Hyd_Boundary_Scenario_List *list, int *number_selected);
	///Set per list, which of the availabe hydraulic boundary scenarios should be handled and return the number
	int set_boundary_scenarios_per_list(QSqlDatabase *ptr_database, QList<int> list_id);

	///Delete the data in table and create a new base szenario with id =0
	void delete_table_create_base_sz(QSqlDatabase *ptr_database);
	///Delete the data of a hydraulic boundary scenario in the database table by a given id
	static void delete_scenario_by_id(QSqlDatabase *ptr_database, const int sc_id, const _sys_system_id id);
	///Delete the data of a hydraulic boundary scenario in the database table by a given system state
	static void delete_scenario(QSqlDatabase *ptr_database, const _sys_system_id id);


	///Append a new szenario to the database table; the szenario id is chosen after the already existing szenarios
	int insert_new_hyd_sz(Hyd_Boundary_Szenario *ptr_sz, QSqlDatabase *ptr_database, const int index);

	///Get the id of a next possible hydraulic boundary scenario
	int get_new_hyd_sz_id(QSqlDatabase *ptr_database);

	///Get a pointer to all read in boundary scenarios
	Hyd_Boundary_Szenario* get_ptr_boundary_scenario(void);

	///Count the number of selected scenarios and return it; the selected scenarios are handled in some way, e.g. calculation, checking etc.
	int counter_number_selected_scenarios(void);

	///Ask the new scenario data per dialog
	bool ask_scenario_data_per_dialog(const int number, string *file_name, QSqlDatabase *ptr_database, const bool new_base_sc, QWidget *parent=NULL);

	///Set all existing scenarios to selected status
	void set_all_sc_bound2selected(void);


	///Set new boundary scenario directly without a GUI dialog for an import via the hydraulic system
	void set_new_boundary_scenario_directly(Hyd_Boundary_Scenario_List *list, QWidget *parent = NULL);


	///Allocate the boundary scenario data dialog
	void allocate_scenario_dia(QWidget *parent=0);

private:

	//members

	///Pointer to the boundary szenarios
	/**This pointer is allocated with allocate_szenerios(void) and deleted delete_szenarios(void) */
	Hyd_Boundary_Szenario *sz_bound;
	///Number of boundary szenarios
	int number_sz_bound;
	///Pointer to a hydraulic boundary scenario dialog
	HydGui_Bound_Scenario_Data_Dia *data_dia;



	//method

	///Recalculate the probability of the Max-event
	void recalc_prob_maxevent(QSqlDatabase *ptr_database);

	///Sort the boundary scenarios after ascending annualtiy
	void sort_bound_sz_annu(void);

	///Allocate the boundary szenarios
	void allocate_szenarios(void);
	///Delete the boundary szenarios
	void delete_szenarios(void);


	///Delete the boundary scenario data dialog
	void delete_scenario_dia(void);
	

	///Set error(s)
	Error set_error(const int err_type);

};
#endif
