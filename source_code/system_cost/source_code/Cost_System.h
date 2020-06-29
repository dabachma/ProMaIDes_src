#pragma once
/**\class Cost_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef COST_SYSTEM_H
#define COST_SYSTEM_H
//QT libs
#include <QThread>

//system sys
#include "Common_Const.h"
#include "_Sys_Common_System.h"

//system cost
#include "Cost_Ecn_Cost_System.h"


///Enumerator to distinguish the type of the thread, e.g. calculation or data handling etc. \ingroup cost
enum _cost_thread_type{

	///Thread for creating the cost system tables in database
	cost_create_tab,
	///Thread for checking the cost system tables in database
	cost_check_tab,

	///Thread for deleting the cost data in database
	cost_del,
	///Thread for a unknown action
	cost_thread_unknown
};

///Class for the thread-based management of the evaluation of a alternative's costs  \ingroup cost
/**


*/
class Cost_System : public QThread, public _Sys_Common_System
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT 

public:
	///Default constructor
	Cost_System(void);
	///Default destructor
	~Cost_System(void);


	//method

	///Set the thread type for specifiying the task of the thread
	void set_thread_type(_cost_thread_type type);
	///Set a pointer to the database; the copy of the database is made in this method
	void set_ptr2database(QSqlDatabase *ptr_database);

	///Get the thread type
	_cost_thread_type get_thread_type(void);

	///Create a whole set of cost database tables
	void create_cost_database_tables(void);
	///Check all madm database tables, for their existence in the database and their declaration in the database table-file
	void check_cost_database_tables(void);
	///Delete the data of all madm database tables
	void delete_data_cost_database_tables(void);
	///Close all madm database tables
	static void close_cost_database_tables(void);

	///Set the stop thread flag
	static void set_stop_thread_flag(const bool flag);
	///Check the stop thread flag
	static void check_stop_thread_flag(void);
	///Get the stop thread flag
	static bool get_stop_thread_flag(void);

	///Run the thread; the type of thread, which will be performed, has to be set before with set_thread_type( _cost_thread_type type)
	void run(void);

	///Ask economical cost data per dialog (user-defined)
	bool ask_ecn_cost_per_dia(QWidget *parent);

	///Get the number of errors
	int get_number_error(void);

	///Get the flag if the thread has started
	bool get_thread_has_started(void);

private:
	//members
	///Type of thread specifiying the task of the thread \see _cost_thread_type
	_cost_thread_type thread_type;

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

	///Economical cost system
	Cost_Ecn_Cost_System ecn_system;

	///Flag if the threads has started
	bool thread_is_started;

	//methods

	///Set the number of warnings before an action
	void set_start_warnings_number(void);
	///Set the warning number, which occured during an action
	void set_warning_number(void);
	///Get the warning number, which occurred during an action
	int get_occured_warning(void);

	///Output the error statistic of the cost system
	void output_error_statistic(void);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
