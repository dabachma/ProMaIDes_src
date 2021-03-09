#pragma once
/**\class Fpl_Calculation
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_CALCULATION_H
#define FPL_CALCULATION_H

//QT libs
#include <QThread>
//system sys
#include "_Sys_Common_System.h"
#include "Common_Const.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Multipathsettings_Dia.h"

//system fpl
#include "Fpl_Mc_Sim.h"
#include "Fpl_Frc_Sim.h"
#include "Fpl_Section.h"
#include "FplGui_Section_Dia.h"

//system hydraulic



/////Enumerator to distinguish the type of the thread, e.g. peforming a determistic calculation or a fragility curve calculation \ingroup fpl
//enum _fpl_thread_type{

//	///Thread for importing fpl-section(s) from file into database
//	fpl_import_file,

//	///Thread for performing a deterministc calculation of selected sections
//	fpl_determ_selected,

//	///Thread for performing a monte-carlo calculation of selected sections
//	fpl_mc_selected,

//	///Thread for performing a fragility curve calculation of selected sections
//	fpl_frc_selected,

//	///Thread for testing the random-generator and the distribution,
//	fpl_test_random,

//	///Thread for setting the frc by the user
//	fpl_frc_by_hand,

//	///Thread is creating the fpl tables for a database project
//	fpl_create_tab,
//	///Thread is checking the fpl tables of a database project
//	fpl_check_tab,

//	///Thread is checking fpl-section(s) per file
//	fpl_check_sec_file,
//	///Thread is checking fpl-section(s) per database
//	fpl_check_sec_db,

//	///Thread is combining the hydraulic system with the fpl-system
//	fpl_combine_fpl2hyd,
//	///Thread is checking the idealization of the hydraulic system by the fpl-system
//	fpl_check_ideal_fpl2hyd,

//	///Thread is deleting selected fpl section and their appending tables
//	fpl_del_section,
//	///Thread is restoring default values in the tables
//	fpl_restore_default,

//	///Thread for a unknown action
//	fpl_thread_unknown


//};

///Class for the calculation of the reliabilty of a section of the FloodProtectionLine \ingroup fpl
/**
This reliabilty analysis can be:
	- a determinstic analysis,
	- a Monte-Carlo analysis or
	- a fragilty curve analysis (frc).

The last one includes a Monte-Carlo analysis for given waterlevels, it results
in a discrete fragilty curve. The Monte-Carlo analysis perform for each simualtion run 
a deterministic calculation for a given probabilistic generated parameter set. In the determinstic analysis 
the mechanisms are performed with a deterministic generated parameter set, in general the mean value of the
parameter.
\see Fpl_Mc_Sim, Fpl_Frc_Sim
*/
class Fpl_Calculation :  public QThread, public _Sys_Common_System
{

//Macro for using signals and slots (Qt)in this class
Q_OBJECT 

public:
	///Default constructor
	Fpl_Calculation(void);
	///Default destructor
	~Fpl_Calculation(void);

	//members
	
	///Pointer to the class for checking the random generation and the transformation of the random variables
	Fpl_Check_Distribution *random_checker;

	///List of section id's, which should be calculated/handled
	int *list_section_id;


	//methods
	///Set the number of files for calculation and the global filenames per dialog (gui)
	bool set_system_number_file_gui(void);
	///Set the number of files for calculation and the global filenames direct
	void set_system_number_file_direct(const QStringList files2import);

	///Set the thread type for specifiying the task of the thread
	void set_thread_type(_fpl_thread_type type);
	///Set a pointer to the database; the copy of the database is made in this method
	void set_ptr2database(QSqlDatabase *ptr_database);

	///Set the list of the section -ids, which should be handled
	void set_list_section_ids(QList<int> list);
	///Set the list of the section -ids, which should be handled
	void set_list_section_ids(const int number, _fpl_break_info* section_ids);
	///Set the list of the section-ids, which should be handled, via database. Here all relevant section for a risk calculation are set to the list
	void set_list_section_ids(void);
	///Set the list of the section-ids, just for the current measure state
	void set_list_section_ids_current(void);
	///Set the list of the section-ids, just for the current measure state and for section with the type set by hand
	void set_list_section_ids_current_set_by_hand(void);

	///Get the number of section in the list
	int get_number_section_in_list(void);

	///Get the thread type
	_fpl_thread_type get_thread_type(void);

	///Create a whole set of fpl database tables
	void create_fpl_database_tables(void);
	///Check all fpl database tables, for their existence in the database and their declaration in the database table-file
	void check_fpl_database_tables(void);
	///Delete the data of all fpl database tables
	void delete_data_fpl_database_tables(void);
	///Close all fpl database tables
	static void close_fpl_database_tables(void);

	///Ask for the section which are handled in the fpl system with a dialog
	bool ask_section2handle(QWidget *parent, const _fpl_thread_type type);
	///Ask flags which part of the database tables should be restored with the default values
	bool ask_restore_default_database(QWidget *parent);

	///Set the stop thread flag
	static void set_stop_thread_flag(const bool flag);
	///Check the stop thread flag
	static void check_stop_thread_flag(void);
	///Get the stop thread flag
	static bool get_stop_thread_flag(void);

	///Run the thread; the type of thread, which will be performed, has to be set before with set_thread_type(_fpl_thread_type type)
	void run(void);

	///Get the number of errors
	int get_number_error(void);

	///Set the flag if results should be overwritten
	void set_overwrite_flag(const bool flag);

	///Idealise the hydraulic system by a given fpl-section
	void idealise_hyd_system_by_section(Fpl_Section *section, Hyd_Hydraulic_System *system);

	///Output the final statistic of idealisation
	void output_statistic_idealisation(void);

	///Get the flag if the thread has started
	bool get_thread_has_started(void);

	///Import data of an fpl-section but just the probabilistic parameters from file to database (measure change fpl-section parameters)
	void import_file2database_prob_params(const QStringList new_file, const int sec_id_based_on);

	///Check the idealization of the hydraulic system by the fpl-system by a given river model; it is used in the Alt-System
	void check_ideal_hyd_system_fpl(Hyd_Model_River *river);

	///Set the section id to handle
	void set_section_id(const int id);

signals:
	///Send if the hydraulic thread is used
	void send_hyd_thread_runs(bool flag);
	///Send if the hydraulic thread will run or permit hydraulic module application
	void send_hyd_thread_enable(bool flag);


private:

	//members
	///Type of thread specifiying the task of the thread \see _Dam_thread_type
	_fpl_thread_type thread_type;

	///Flag if the thread are aborted by the user
	static bool abort_thread_flag;

	///Copy of the database
	QSqlDatabase qsqldatabase;

	///Number of errors occurred
	int number_error;
	///Number warning at the beginning of an action
	int number_warning_begin;
	///Number warning at the end of an action
	int number_warning;

	///Time classes for the real time calculation (start time point)
	time_t start_time;
	///Time classes for the real time calculation (actual time point)
	time_t actual_time;

	///Pointer to a monte-carlo simulation
	Fpl_Mc_Sim *mc_simulation;
	///Pointer to a fragility curve calculation
	Fpl_Frc_Sim *frc_simulation;
	///Pointer to the section, which is handled in calculation
	Fpl_Section *section2calc;

	///Number of section to calculate/handled
	int number_section;


	///Pointer to the random variables, for which the random generation and the transformation is checked
	Fpl_Random_Variables *random2check;

	///Flag if the deafult variables in the database tables should be restored
	bool flag_restore_default;
	///Flag if the parameters of the probabilistic simulation should be restored
	bool flag_restore_prob_param;
	///Flag if the fault-tree settings should be restored
	bool flag_restore_fault_tree;

	///Pointer to a string for multiple file names
	string *file_names;

	///Flag if results should be overwritten 
	bool overwrite_flag;

	///Flag if the threads has started
	bool thread_is_started;

	///Name of the output folder for results
	string output_folder;

	//methods
	///Allocate the file names
	void allocate_file_names(void);
	///Delete the file names
	void delete_file_names(void);

	///Allocate the Monte-Carlo simulation
	void allocate_mc_sim(void);
	///Delete the Monte-Carlo simulation
	void delete_mc_sim(void);
	///Allocate the fragility curve calculation
	void allocate_frc_sim(void);
	///Delete the fragility curve calculation
	void delete_frc_sim(void);
	///Allocate the section, which is handled in calculation
	void allocate_fpl_section(void);
	///Delete the section, which is handled in calculation
	void delete_fpl_section(void);

	///Allocate the class for checking the random generation and the transformation of the random variables
	void allocate_rand_checker(void);
	///Delete the class for checking the random generation and the transformation of the random variables
	void delete_rand_checker(void);

	///Allocate the list of the section ids
	void allocate_list_sec_id(void);
	///Delete the list of the section ids
	void delete_list_sec_id(void);

	///Import data of an fpl-section from file to database
	void import_file2database(void);
	///Perform a deterministic simulation for selected sections
	void deterministic_sim(void);
	///Perform a monte-carlo simulation for selected sections
	void monte_carlo_sim(void);
	///Perform a fragility curve simulation for selected sections
	void fragility_curve_sim(void);
	///Perform a test of the random generator
	void test_random_generator(void);
	///Set a fragility curve from dialog data
	void set_frc_dialog_data(void);

	///Check fpl-section(s) per file
	void check_section_file(void);
	///Check fpl-section(s) per database
	void check_section_database(void);

	///Export deterministic results of a fpl-section(s)
	void export_results_determ_section(void);
	///Export MC-results of a fpl-section(s)
	void export_results_mc_section(void);
	///Export FRC-results of a fpl-section(s)
	void export_results_frc_section(void);

	///Combine the fpl-system with the hydraulic system
	void combine_fpl2hyd_system(void);
	///Check the idealization of the hydraulic system by the fpl-system
	void check_ideal_hyd_system_fpl(void);

	///Delete selected setcion in database table
	void delete_selected_section_database(void);
	///Restore default values in database table
	void restore_default_database_table(void);

	///Set the number of warnings before an action
	void set_start_warnings_number(void);
	///Set the warning number, which occured during an action
	void set_warning_number(void);
	///Get the warning number, which occurred during an action
	int get_occured_warning(void);

	///Output the error statistic of the fpl system
	void output_error_statistic(void);
	///Output the error statistic of the fpl system import from file to database
	void output_import_statistic(void);
	///Output the error statistic of the fpl system check from database
	void output_check_statistic(void);
	///Output the export statistic of the fpl system check from database
	void output_export_statistic(void);
	///Output the statistic of the fpl-calculation
	void output_final_statistic_calculation(void);

	///Check for identical section in case of a new input per file
	void check_for_identical_section(Fpl_Section *new_sec);

	///Create the name of the output folder
	void create_output_folder_name(void);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
