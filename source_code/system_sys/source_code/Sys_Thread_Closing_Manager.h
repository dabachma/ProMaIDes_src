#pragma once
/**\class Sys_Thread_Closing_Manager
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_THREAD_CLOSING_MANAGER_H
#define SYS_THREAD_CLOSING_MANAGER_H

//qt libs
#include <QObject>
#include <QThread>

///Enumerator for specifiying the type of closing
enum _sys_close_types{
	///The complete application is closed
	app_close,
	///The project is closed
	pro_close,
	///The project is closed for open a new project
	pro_new_open,
	///The project is closed for open an existing project
	pro_exist_open,
	
};
Q_DECLARE_METATYPE(_sys_close_types);


///Thread-class for managing the closing of the different launched threads of the moduls \ingroup sys
/**

*/
class Sys_Thread_Closing_Manager : public QThread
{

//Macro for using signals and slots (Qt)in this class
Q_OBJECT 

public:
	///Default constructor
	Sys_Thread_Closing_Manager(void);
	///Default destructor
	~Sys_Thread_Closing_Manager(void);


	//methods

	///Set the pointer to the dam-module thread
	void set_ptr_dam_thread(QThread *ptr_dam_thread);
	///Set the pointer to the hyd-module thread
	void set_ptr_hyd_thread(QThread *ptr_hyd_thread);
	///Set the pointer to the fpl-module thread
	void set_ptr_fpl_thread(QThread *ptr_fpl_thread);
	///Set the pointer to the madm-module thread
	void set_ptr_madm_thread(QThread *ptr_madm_thread);
	///Set the pointer to the risk-module thread
	void set_ptr_risk_thread(QThread *ptr_risk_thread);
	///Set the pointer to the alt-module thread
	void set_ptr_alt_thread(QThread *ptr_alt_thread);
	///Set the pointer to the cost-module thread
	void set_ptr_cost_thread(QThread *ptr_cost_thread);
	///Set the pointer to the table loader thread 
	void set_ptr_table_loader_thread(QThread *ptr_table_loader);
	///Set the pointer to the project copy thread 
	void set_ptr_project_copy_thread(QThread *ptr_project_copy);
	//..introduce further modules

	///Set the closing type
	void set_closing_type(_sys_close_types close_type);
	///Set the flag for just using a waitng loop 
	void set_jusing_waiting_loop(const bool flag);


	///Start the thread for managing the closing of the threads
	void run(void);

signals:
	///Send that all threads are closed
	void send_threads_closed(_sys_close_types type);

private:
	//members
	///Pointer to the damage module thread
	QThread *ptr_dam_module;
	///Pointer to the hydraulic module thread
	QThread *ptr_hyd_module;
	///Pointer to the fpl module thread
	QThread *ptr_fpl_module;
	///Pointer to the madm module thread
	QThread *ptr_madm_module;
	///Pointer to the risk module thread
	QThread *ptr_risk_module;
	///Pointer to the cost module thread
	QThread *ptr_cost_module;
	///Pointer to the alt module thread
	QThread *ptr_alt_module;
	///Pointer to the sql table loader thread
	QThread *ptr_table_loader;
	///Pointer to the project copy thread
	QThread *ptr_project_copy;
	//..introduce further modules

	///The closing type
	_sys_close_types close_type;
	///Flag for using the just a waiting loop
	bool waiting_loop_flag;

	//method
	///Check if the threads are still running
	bool check_thread_running(void);


};
#endif
