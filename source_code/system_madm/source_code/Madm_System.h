#pragma once
/**\class Madm_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef MADM_SYSTEM_H
#define MADM_SYSTEM_H
//QT libs
#include <QThread>
//system sys
#include "Common_Const.h"
#include "Sys_Multipathsettings_Dia.h"
//system madm
#include "Madm_Analysis.h"
#include "MadmGui_Weighting_Set_Dia.h"


//system cost
#include "Cost_System.h"


///Enumerator to distinguish the type of the thread, e.g. calculation or data handling etc. \ingroup madm
enum _madm_thread_type{

	///Thread for creating the madm system tables in database
	madm_create_tab,
	///Thread for checking the madm system tables in database
	madm_check_tab,


	//Thread for importing aweigthing set to database
	madm_imp_weight,

	///Thread for performing a madm-analysis per database
	madm_calc_database,
	///Thread for performing a madm-analysis per file
	madm_calc_file,
	///Thread for deleting the madm-weighting sets in database
	madm_del_weight,
	///Thread for a unknown action
	madm_thread_unknown
};

///Data structure for storing the information of a weight set
struct _madm_weight_set_info{
	///Id of the set
	int id;
	///Name of the set
	string name;
	///Description of the set
	string description;

};

///Namespace to fix the min/max setting of the criteria. If this flag is true maximum values are preferred. \ingroup madm
namespace madm_crit_min_max{

	///Criteria variation of risk economic
	const bool max_crit_risk_ecn=true;
	//Criteria variation of risk ecologic
	const bool max_crit_risk_eco=true;
	///Criteria variation of risk psycho-social
	const bool max_crit_risk_pys=true;
	///Criteria variation of risk people2risk (affected people)
	const bool max_crit_risk_pop_aff=true;
	///Criteria variation of risk people2risk (endangered people)
	const bool max_crit_risk_pop_dan=true;
	///Criteria variation of risk outflow 
	const bool max_crit_risk_outflow=true;

	///Criteria variation of risk simple counting public buildings 
	const bool max_crit_risk_sc_pub=true;
	///Criteria variation of risk simple counting ecologic perilous sites or buildings 
	const bool max_crit_risk_sc_eco=true;
	///Criteria variation of risk simple counting cultural heritage
	const bool max_crit_risk_sc_cult=true;
	///Criteria variation of risk simple counting buildings with highly vulnerable person
	const bool max_crit_risk_sc_person=true;


	///Criteria variation of ci criteria 
	const bool max_crit_risk_ci = true;


	///Criteria cost economic
	const bool max_crit_cost_ecn=false;

};

///Class for the thread-based management of the evaluation of a decision matrix \ingroup madm
/**

The mathematical method of madm for the evaluation of the decision matrix are implemented in Madm_Analysis. 

*/
class Madm_System : public QThread, public _Sys_Common_System
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT 

public:
	///Default constructor
	Madm_System(void);
	///Default destructor
	~Madm_System(void);


	//members
	///Pointer to the table for the weight sets 
	static Tables *table_sets;



	//methods
	///Set the database table for the weight sets: it sets the table name and the name of the columns and allocate them
	static void set_table_sets(QSqlDatabase *ptr_database, const bool not_close = false);
	///Create the database table for the weight sets
	static void create_table_sets(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the weight sets
	static void delete_data_in_table_sets(QSqlDatabase *ptr_database);
	///Close and delete the database table for the weight sets
	static void close_table_sets(void);
	///Set the default weighting sets to database
	static void set_default_weighting_set2datbase(QSqlDatabase *ptr_database);

	///Select the data in the database table for weighting sets 
	static int select_data_in_settable(QSqlQueryModel *model, QSqlDatabase *ptr_database);
	///Select the data in the database table for weighting sets for specified parameters
	static int select_data_in_settable(QSqlQueryModel *model, QSqlDatabase *ptr_database, const int set_id);
	///Check if weighting sets are set in database table
	static bool check_data_in_settable(QSqlDatabase *ptr_database);
	



	///Ask per file dialog to delete weighting sets
	bool ask_weighting_set2delete(QWidget *parent);


	///Set the number of files for an analysis and the global filenames per dialog (gui)
	bool set_analysis_number_file_gui(QWidget *parent);

	///Set the number of files for an import of weighting sets and the global filenames per dialog (gui)
	bool set_weight_set_number_file_gui(QWidget *parent);

	///Set the thread type for specifiying the task of the thread
	void set_thread_type(_madm_thread_type type);
	///Set a pointer to the database; the copy of the database is made in this method
	void set_ptr2database(QSqlDatabase *ptr_database);

	///Get the thread type
	_madm_thread_type get_thread_type(void);

	///Create a whole set of madm database tables
	void create_madm_database_tables(void);
	///Check all madm database tables, for their existence in the database and their declaration in the database table-file
	void check_madm_database_tables(void);
	///Delete the data of all madm database tables
	void delete_data_madm_database_tables(void);
	///Close all madm database tables
	static void close_madm_database_tables(void);


	///Initialize the defined criteria
	void init_defined_criteria(void);


	///Set the stop thread flag
	static void set_stop_thread_flag(const bool flag);
	///Check the stop thread flag
	static void check_stop_thread_flag(void);
	///Get the stop thread flag
	static bool get_stop_thread_flag(void);

	///Run the thread; the type of thread, which will be performed, has to be set before with set_thread_type(_madm_thread_type type)
	void run(void);

	///Get the number of errors
	int get_number_error(void);

	///Check if all required data are available in database for an madm-analysis
	bool check_data_available_in_database(const bool nobreak_applied, const bool catchment_applied);

	///Get the flag if the thread has started
	bool get_thread_has_started(void);

private:
	//members
	///Type of thread specifiying the task of the thread \see _madm_thread_type
	_madm_thread_type thread_type;

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

	///Number of madm analysis
	int number_analysis;

	///Pointer to a string for multiple file names for calculation
	string *file_names;

	///Pointer to the madm-analysis class
	Madm_Analysis *analysis;


	///Number of weighting sets
	int number_weight_sets;
	///List of weighting set ids
	int *list_weight_set_id;
	///Information of the current weight set
	_madm_weight_set_info weight_set_info;
	///Number of defined criteria for an MADM-analysis for floodprotection measures
	const int number_defined_criteria;
	///Weighting sets
	_madm_criteria *defined_criteria;

	///Flag if the threads has started
	bool thread_is_started;

	//methods
	///Delete the data in the database table for the weighting set for a specified set-id
	void delete_data_in_set_table(const int set_id);
	///Delete the data in the database table for the measures ranking for a specified set-id
	void delete_data_in_ranking_table(const int set_id);
	///Delete the data in the database table for the measures ranking
	void delete_data_in_ranking_table(void);
	///Delete the data in the database table for the decision
	void delete_data_in_matrix_table(void);


	///Set the current weight set from database table
	void set_current_weight_set(const int set_id);


	///Allocate the file names for calculation
	void allocate_file_names(void);
	///Delete the file names for calculation
	void delete_file_names(void);

	///Allocate the madm-analysis class
	void allocate_analysis(void);
	///Delete the madm-analysis class
	void delete_analysis(void);

	///Allocate the defined criteria for the flood-protection DSS
	void allocate_defined_criteria(void);
	///Delete the defined criteria for the flood-protection DSS
	void delete_defined_criteria(void);

	///Allocate the list of weighting set ids
	void allocate_list_weighting_set_id(void);
	///Delete the list of weighting set ids
	void delete_list_weighting_set_id(void);

	///Import a weighting set to database table
	void import_weighting_set(void);
	///Delete weighting sets in database table and all appending results in the database; the list of weighting sets ids have to be set before
	void delete_weighting_sets_in_database(void);
	///Perform a madm-analysis per file
	void analysis_per_file(void);
	///Perform a madm-analysis per database
	void analysis_per_database(void);



	///Set the number of warnings before an action
	void set_start_warnings_number(void);
	///Set the warning number, which occured during an action
	void set_warning_number(void);
	///Get the warning number, which occurred during an action
	int get_occured_warning(void);

	///Output the error statistic of the madm system
	void output_error_statistic(void);
	///Output the statistic of the madm-analysis
	void output_final_statistic_calculation(void);
	///Output the statistic of the madm-analysis per database
	void output_final_statistic_calculation_database(const int counter);


	///Transfer a weighting set to database
	void transfer_weighting2database(void);


	///Check the file for the madm-weighting sets if the !$BEGIN... always are close with !$END...
	void check_input_file_weighting_sets(const string file_name);

	///Read in weighting set file
	void read_in_weighting_set_file(const string filename);
	///Read in the weighting set block per file
	void read_in_weighting_set_block(QFile *ifile, int *line_counter);
	///Find the number of weighting sets in file
	bool find_number_weighting_set_file(const string myline);
	///Find keywords in file for the weighting set
	void find_keywords_file_weighting_set(const string myline, int *must_found);

	///Load weighting set id-list per database
	void load_weighting_set_list(void);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
