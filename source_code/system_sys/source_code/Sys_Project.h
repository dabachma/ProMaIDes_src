#pragma once
#ifndef SYS_PROJECT_H
#define SYS_PROJECT_H
/**\class Sys_Project
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
//c++-libs
#include <ctime>//time-funtcions
//qt libs
#include <QFileDialog>
#include <QFile>
#include <QObject>

//system sys
#include "Sys_New_Project_Dia.h"
#include "_Sys_Common_System.h"
#include "Sys_Project_Copier.h"



//define the licenses for ProMaIDes
#define proj_all_license
//#define proj_risk_license
//#define proj_dam_hyd_license
//#define proj_hyd_license
//#define proj_fpl_license
//#define proj_dam_license
//#define proj_hydrol_license

//define for development
#define development





///Data structure storing the flags representing the state of the risk calculation \ingroup sys
struct _risk_state_flags{
	///Risk state falg, other user-defined calculation are finished, e.g. hydraulic, risk calculation can start
	bool risk_state_flag;
	///Reliability flags of the hydraulic results
	bool reliability_hyd_result;
	///Reliability of the damage results
	bool reliability_dam_result;
	///Reliability of the fpl results
	bool reliability_fpl_result;

	///Flag if the catchment area risk approach is further applied
	bool catchment_risk_applied;
	///Flag if the nobreak risk approach is further applied
	bool nobreak_risk_applied;

};
///Data structure storing the system data for the hydraulic calculation \ingroup sys
struct _hyd_state_data{
	///Flag if an file output is required
	bool file_output_required;
	///Number of required threads
	int number_threads;
};


///Managing-class for projects of ProMaIDes \ingroup sys
/**

*/
class Sys_Project : public QObject
{
//Macro for using signals and slots (Qt)in this class
	Q_OBJECT
public:

	///Default constructor
	Sys_Project(void);
	///Default destructor
	~Sys_Project(void);


	//members
	///Thread to copy a project
	Sys_Project_Copier *thread_pro_copy;

	//methods
	///Ask and load an existing project
	bool ask_existing_project(QWidget *parent=NULL);
	///Set the project file directly
	bool set_project_file(const string filename);

	///Build a new project
	bool build_new_project(QWidget *parent);

	///Get the project file name
	string get_project_file_name(void);
	///Get complete project file name with path and suffix
	string get_complete_project_file_name(void);
	///Get the project type
	 static _sys_project_type get_project_type(void);
	///Get the database connection paramater
	 _sys_database_params get_database_connection_param(void);
	 ///Get complete project name used as schemata name on psql
	static string get_complete_project_database_schemata_name(void);
	///Get complete project name used a prefix for the database table names
	static string get_complete_project_database_table_prefix_name(void);

    ///Get version number
    static string get_version_number(void);
    ///Get version date
    static string get_version_date(void);

	///Get the current path
	static string get_current_path(void);
	///Set the current path
	static void set_current_path(const string path);
	///Get the main path of the project
	static string get_main_path(void);
	///Set the main path of the project
	static void set_main_path(const string main_path);
	///Get the project name
	static string get_project_name(void);
	///Convert string to the project type (_sys_project_type)
	static _sys_project_type convert_txt2project_type(const string txt);
	///Convert project type (_sys_project_type) to string 
	static string convert_project_type2txt(const _sys_project_type type);

	///Get the flag if the logfile should be saved 
	static bool get_logfile_save_flag(void);
	///Set the flag if the logfile should be saved 
	static void set_logfile_save_flag(const bool flag);

	///Read an existing project in
	void read_existing_project(void);

	///Close and reset the project
	void close_project(void);

	///Output the project data to display/console
	void output_project_param(void);

	///Get the title for the main window
	string get_main_window_title(void);

	///Save the project parameter to file
	void save_project_paramater(void);
	///Edit project parameter
	bool edit_project_parameter(QWidget *parent, bool *is_upgrade);

	///Set the database connection parameter
	void set_database_connection_param(_sys_database_params conn_param);

	///Close and delete the project completely and unrecoverable
	void close_delete_project(QSqlDatabase *ptr_database);

	///Set current system state
	void set_current_system_state(const _sys_system_id state);
	///Set current risk state flags
	void set_current_risk_state(const _risk_state_flags flag);
	///Set hydraulic system data
	void set_current_hydraulic_state(const _hyd_state_data state);
	///Get stored system state
	_sys_system_id get_stored_system_state(void);
	///Get stored risk state flags
	_risk_state_flags get_stored_risk_state(void);
	///Get hydraulic system data
	_hyd_state_data get_stored_hydraulic_state(void);

	///Copy an open project
	void copy_open_project(Data_Base *ptr_database, QWidget *parent);

	///Delete the thread for project copying
	void delete_pro_copy_thread(void);


signals:

	///Send to the main-window to generate the required tables in database and to write the table-txt to the project file
	void send_table_creation(void);
	///Send a text to the status bar
	void send_txt2statusbar(QString txt, int time);


private:

	//members
	///Name and path of the project file
	string project_file_name;
	///Project name
	static string project_name;
    ///Version number
    static string version;
    ///Version date
    static string version_date;
	///Author name of the project
	string author_name;
	///Project description
	string description;
	///Project type
	static _sys_project_type project_type;
	///Project file path
	string file_path;
	///Flag for new project directory creating
	bool create_new_dir;
	///Database connection parameters
	_sys_database_params database_conn_params;
	///String with the project generation time
	string generation_time;
	///Current path
	static string current_path;
	///Main path
	static string main_path;

	///Current or stored risk state flags
	_risk_state_flags risk_state_flag;
	///Current or stored system id 
	_sys_system_id system_id;
	///System data for the hydraulic calculation
	_hyd_state_data hyd_state;

	///Counter for changing the project name
	int counter_name_change;

	///Flag if the project name is already change once (create a new project)
	bool name_changed_once;

	///Flag if the logfile should be saved in the logfile archive after closing the project
	static bool save_logfile;


	//method
	///Create a new project
	void create_new_project(const bool copy);

	///Check project name
	void check_project_name(void);


	///Get complete project folder name with path
	string get_complete_project_folder_name(void);
	///Change project name if a folder, file or schemate/table already exits
	void change_project_name(void);

	///Check if the mysql database table exists
	bool check_mysql_table_exist(void);
	///Check if the psql database schemata exists
	bool check_psql_schmeta_exist(void);

	///Write the project file
	void write_project_file(const bool copy);

	///Get the current time as string 
	string get_current_time(void);

	///Find the key values for the file input
	void find_key_values_file(string myline , int *must_found_counter);

	///Remove end-of-line from the descrition string
	void remove_endl_description(void);

	///Insert end-of-line into the descritpion string
	string insert_endl2descritpion(void);

	///Create project folder for the data
	void create_project_folder_data(void);
	///Create project folder for the output
	void create_project_folder_output(void);

	///Generate project paramter text for the project file
	string generate_project_txt2file(void);

	

	///Copy the table data in the project file to another project file
	void copy_tables_data_project_file(const string src, const string dest);


	///Allocate the thread for project copying
	void allocate_pro_copy_thread(void);


	///Set error(s)
	Error set_error(const int err_type);
};
#endif
