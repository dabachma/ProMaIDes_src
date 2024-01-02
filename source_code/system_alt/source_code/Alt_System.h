#pragma once
/**\class Alt_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef ALT_SYSTEM_H
#define ALT_SYSTEM_H
//QT libs
#include <QThread>

//system sys
#include "Common_Const.h"
#include "_Sys_Common_System.h"
#include "Sys_Diverse_Text_Dia.h"

//system hyd
#include "Hyd_Boundary_Szenario_Management.h"
#include "Hyd_Multiple_Hydraulic_Systems.h"
#include "Hyd_Floodplain_Polysegment.h"

//system dam
#include "Dam_Damage_System.h"

//system fpl
#include "Fpl_Calculation.h"

//system risk
#include "Risk_System.h"


//system cost
#include "Cost_System.h"

///Enumerator for the measure category \ingroup alt
enum _alt_measure_category {
	///Hydraulic measures
	category_hyd,
	///Measure concerning the FloodProtectionLine (FPL)
	category_fpl,
	///Damage measures
	category_dam,

};

///Data structure to gather the general alternative information \ingroup alt
struct _alt_general_info {
	///Identifier of the alternative
	_sys_system_id id;
	///Name of the alternative
	string name;
	///Description
	string description;
	///Measure type
	_alt_measure_category category;
	///Repacing flag
	bool replacing;
	///String for the detailed measure type
	string detailed_type;
	///Identifier of the measurestate, on which the new alternative is based on; in general it is the base-area state
	int measurestate_based_on;
};

//system alt
//#include "AltGui_Alternative_Info_Dia.h"
//#include "AltGui_Measure_Dia.h"



///Enumerator to distinguish the type of the thread, e.g. calculation or data handling etc. \ingroup alt
enum _alt_thread_type{

	///Thread for creating the alt system tables in database
	alt_create_tab,
	///Thread for checking the alt system tables in database
	alt_check_tab,

	///Thread for inserting new measures to database
	alt_insert,

	///Thread for switching between measure states
	alt_switch,

	///Thread for deleting the alt data in database
	alt_del,
	///Thread for a unknown action
	alt_thread_unknown
};


//forward declaration
class Madm_Decision_Matrix;



///Class for the thread-based management of the alternative's implementation  \ingroup alt
/**


*/
class Alt_System : public QThread, public _Sys_Common_System
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT 


public:
	///Default constructor
	Alt_System(void);
	///Default destructor
	~Alt_System(void);

	//members
	///Pointer to the table for storing measure data
	static Tables *table_measure;
	///Pointer to the table for storing  the detailed data of replacing measures
	static Tables *table_replacing;



	//method
	///Set the database table for the measures: it sets the table name and the name of the columns and allocate them
	static void set_table_measure(QSqlDatabase *ptr_database);
	///Create the database table for measures
	static void create_table_measure(QSqlDatabase *ptr_database);
	///Delete all data in the database table of the measures
	static void delete_data_in_table_measure(QSqlDatabase *ptr_database);
	///Delete data in the database table of the measures specified by the paramters
	static void delete_data_in_table_measure(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Close and delete the database table for the measures
	static void close_table_measure(void);

	///Get the name and description of the measure from database table
	static bool get_name_descript_measure_table(QSqlDatabase *ptr_database, _sys_system_id id, string *name, string *descript);
	///Load the string-list (name/id) of the current avaliable alternatives in the table
	static void load_stringlist_current_alt(QSqlDatabase *ptr_database, const int area_state, QStringList *list);

	///Check if the measure is already set in database and get the information
	static bool check_measure_is_set_database(QSqlDatabase *ptr_database, const _sys_system_id id, _alt_general_info *data);
	///Check if a measure is already set in database and get the information
	static bool check_measure_is_set_database(QSqlDatabase *ptr_database);


	///Load the list of the current avaliable alternatives in the table
	void load_list_current_alt(QSqlDatabase *ptr_database, const int area_state);
 

	///Set the database table for the replacing measures: it sets the table name and the name of the columns and allocate them
	static void set_table_replacing_measure(QSqlDatabase *ptr_database);
	///Create the database table for replacing measures
	static void create_table_replacing_measure(QSqlDatabase *ptr_database);
	///Delete all data in the database table of the replacing measures
	static void delete_data_in_table_replacing_measure(QSqlDatabase *ptr_database);
	///Delete data in the database table of the replacing measures with the given system_state
	static void delete_data_in_table_replacing_measure(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Close and delete the database table for the replacing measures
	static void close_table_replacing_measure(void);

	///Get a new measure state id
	static int get_new_measure_state(QSqlDatabase *ptr_database);


	///Ask and set new measure data via dialog
	bool ask_new_measure_data(QWidget *parent, const bool fpl_required);
	///Edit the current measure data via dialog
	bool ask_edit_measure_data(QWidget *parent, const bool fpl_required);

	///Ask to which measure state should be switched
	bool ask_measure_state2switch(QWidget *parent);
	///Ask which measure state should be deleted
	bool ask_measure_state2delete(QWidget *parent);

	///Set the thread type for specifiying the task of the thread
	void set_thread_type(_alt_thread_type type);
	///Set a pointer to the database; the copy of the database is made in this method
	void set_ptr2database(QSqlDatabase *ptr_database);

	///Get the thread type
	_alt_thread_type get_thread_type(void);

	///Create a whole set of cost database tables
	void create_alt_database_tables(void);
	///Check all alt database tables, for their existence in the database and their declaration in the database table-file
	void check_alt_database_tables(void);
	///Delete the data of all alt database tables
	void delete_data_alt_database_tables(void);
	///Close all alt database tables
	static void close_alt_database_tables(void);

	///Set the stop thread flag
	void set_stop_thread_flag(const bool flag);
	///Check the stop thread flag
	static void check_stop_thread_flag(void);
	///Get the stop thread flag
	static bool get_stop_thread_flag(void);

	///Run the thread; the type of thread, which will be performed, has to be set before with set_thread_type(_alt_thread_type type)
	void run(void);

	///Get the number of errors
	int get_number_error(void);

	///Get number of current available alternatives
	int get_current_alternatives(void);
	///Get a pointer to a alternative info specified by the given index
	_alt_general_info *get_ptr_alt_info(const int index);
	///Get a pointer to all alternative info 
	_alt_general_info *get_ptr_alt_info(void);
	///Get a pointer to a alternative info specified by the identifier
	_alt_general_info *get_ptr_alt_info_by_id(const int id);


	///Convert a string into a measure category (_alt_measure_category)
	static _alt_measure_category convert_txt2measure_category(const string txt);
	///Convert a measure category (_alt_measure_category) into a string 
	static string convert_measure_category2txt(const _alt_measure_category cat);


	///Get the flag if the thread has started
	bool get_thread_has_started(void);

	///Get if the measure's implemenation was ok
	bool get_measure_implementation_ok(void);

	///Switch between measure states
	void switch_between_measure_state(void);

	///Get the system state, which should be deleted or switched
	_sys_system_id get_system_state_del_swi(void);

signals:
	///Send if the hydraulic thread is used
	void send_hyd_thread_runs(bool flag);
	///Send if the damage thread is used
	void send_dam_thread_runs(bool flag);
	///Send if the fpl thread is used
	void send_fpl_thread_runs(bool flag);



private:
	//members
	///Type of thread specifiying the task of the thread \see _alt_thread_type
	_alt_thread_type thread_type;

	///Flag if the thread are aborted by the user
	static bool abort_thread_flag;

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


	///List of the current available alternatives 
	_alt_general_info *list_alt;
	///Number of current available alternatives
	int number_alt;

	///Flag if the threads has started
	bool thread_is_started;

	///General information of the measure
	_alt_general_info general_alt_infos;
	///File list for replacing/adding measures
	QStringList file_list;
	///Identifier list for replacing the existing measures
	QList<int> id_list;
	///Identifier list, which existing measure should be replaced
	QList<int> id_list_current;

	///Flag if the measure implemenation was ok
	bool measure_implemt_ok;

	///Hydraulic system for file import of measures
	Hyd_Multiple_Hydraulic_Systems *hyd_system;
	///Fpl system for file import of measures
	Fpl_Calculation *fpl_system;

	///Dam system for intercepting the damage raster as measure
	Dam_Damage_System *dam_system;

	///System identifier for the new measure state
	_sys_system_id new_state_id;

	//System identifier for the measure state to switch to
	_sys_system_id switch_state_id;

	//methods

	///Set new measure data to database table
	void set_measure_data2database(QSqlDatabase *ptr_database, _alt_general_info data);
	///Set new measure detailed data to database table
	void set_measure_detailed_data2database(QSqlDatabase *ptr_database, const _sys_system_id id, const string table_name, QList<int> list);
	///Set the detailed information of a measure via database
	bool get_detailed_info_measure_database(QSqlDatabase *ptr_database, const _sys_system_id id, string *table_name, QList<int> *list);


	///Insert new measures into database
	void insert_new_measure2database(void);

	
	///Delete ameasure state in database
	void delete_measure_database(void);

	///Allocate list of current avalailabe alternatives
	void allocate_alt_list(void);
	///Delete the list of current available alternatives
	void delete_alt_list(void);


	///Set exeptions at start of an action
	void set_exception_number_start(void);
	///Set exception_number at the end of an action
	void set_exception_number_end(void);

	///Output the error statistic of the alt system
	void output_error_statistic(void);
	///Output the error statistic of the alt system for implementing a measure
	void output_implement_error_statistic(void);


	///Implement measures of the hydraulic system
	void implement_hyd_measures(void);
	///Allocate the hydraulic system class
	void allocate_hyd_system(void);
	///Delete the hydraulic system class
	void delete_hyd_system(void);

	///Implement measures of the fpl system
	void implement_fpl_measures(void);
	///Allocate the fpl system class
	void allocate_fpl_system(void);
	///Delete the fpl system class
	void delete_fpl_system(void);

	///Implement measures of the damage system
	void implement_dam_measures(void);
	///Allocate the dam system class
	void allocate_dam_system(void);
	///Delete the dam system class
	void delete_dam_system(void);

	///Implement (replacing) new hydraulic boundary scenarios
	void implement_new_hyd_boundary_scenarios(void);
	///Implement hyd-dikelines
	void implement_hyd_dikelines(const bool replace);
	///Change hyd-river profile(s)
	void change_hyd_river_prof(void);

	///Implement (replacing/adding) fpl-section
	void implement_fpl_section(void);
	///Implement replacing parameters of one/multiple FPL-section(s)
	void implement_replace_fpl_section_parameter(void);

	///Implement dam resettlement
	void implement_dam_resettlement(void);
	///Implement dam switching of ecn-function
	void switch_ecn_dam_functions(void);
	///Implement dam switching of eco-biotope type
	void switch_eco_dam_biotope_type(void);
	///Implement dam switching of eco-soil-type type
	void switch_eco_dam_soil_type(void);
	///Implement dam switching of people2risk category
	void switch_dam_people2risk_category(void);
	///Implement dam switching of psycho-social category
	void switch_dam_psychosocial_category(void);


	///Switch system to base system state (0)
	void switch_system2base_state(void);
	///Switch system to a given system state from the base system state
	void switch_system2given_state(const _sys_system_id id);


	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);


};
#endif
