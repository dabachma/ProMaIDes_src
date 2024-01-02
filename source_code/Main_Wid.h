#pragma once
/**\class Main_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/
#ifndef MAIN_WID_H
#define MAIN_WID_H

//libs

//qt class
#include <QMessageBox>
#include <QScrollBar>
#include <QTableView>
#include <QMetaType>
#include <QTimer>
#include <QProcess>
#include <QStyleFactory>
#include <QSystemTrayIcon>

//system sys
#include "Data_Base.h"
#include "Common_Const.h"
#include "Sys_Common_Output.h"
#include "Sys_Exception_Sender.h"
#include "Sys_Diverse_Text_Dia.h"
#include "Sys_Welcome_Wid.h"
#include "Sys_Status_Bar_Wid.h"
#include "Sys_Database_Projectbrowser_Wid.h"
#include "Sys_Project.h"
#include "Sys_Thread_Closing_Manager.h"
#include "Sys_Sql_Table_Wid.h"
#include "Sys_Output_Text_Wid.h"
#include "Sys_Delete_Logfile_Dia.h"
#include "Sys_Reconnect_Db_Dia.h"
#include "Sys_Version_Update.h"

//system fpl
#include "Fpl_Calculation.h"

//system hyd
#include "Hyd_Multiple_Hydraulic_Systems.h"
#include "HydGui_System_Filechooser_Dia.h"
#include "HydGui_System_State_Dia.h"
#include "HydGui_Profil2Dgm_Converter_Dia.h"
//system dam
#include "Dam_Damage_System.h"
//system risk
#include "Risk_System.h"
#include "RiskGui_Result_Reliability_Dia.h"
//system madm
#include "Madm_System.h"
//system cost
#include "Cost_System.h"
//system alt
#include "Alt_System.h"

//ui forms
#include "ui_Main_Wid.h"

//define the mainpage for doxygen
/** \mainpage


\author Produced by the <a href ='https://www.hs-magdeburg.de/en/research/research-centres/institut-fuer-wasserwirtschaft-und-oekotechnologie/working-group-flood-risk-management-ag-frm.html' target='_blank'>AG FRM</a> of the University of applied sciences Magdeburg-Stendal and the <a href ='https://www.iww.rwth-aachen.de/go/id/lygz/?lidx=1' target='_blank'>IWW</a> of RWTH Aachen University.
\version 0.0.8
\date 2021

Please follow also our online manuals:
	- <a href ='https://promaides.myjetbrains.com/youtrack/articles/PMID-A-7/General' target='_blank'>General information</a> <br>
	- <a href ='https://promaides.myjetbrains.com/youtrack/articles/PMID-A-1/Theory-guide' target='_blank'>Theory guide</a> <br> 
	- <a href ='https://promaides.myjetbrains.com/youtrack/articles/PMID-A-5/Application-guide' target='_blank'>Application guide</a> <br> 
	- <a href ='https://promaides.myjetbrains.com/youtrack/articles/PMID-A-12/Development-guide' target='_blank'>Development guide</a> <br> 
	- <a href ='https://promaides.myjetbrains.com/youtrack/articles/PMDP-A-31/General' target='_blank'>Guide to ProMaIDes Helpers</a> <br> 

ProMaIDes (<b>Pro</b>tection<b>M</b>easures <b>a</b>gainst <b>I</b>nnundation <b>De</b>sicionsupport<b>S</b>ystem) is a modular designed opne software package for a rsik-based
evaluation of flood risk mitigation measures.


From the computational/programming point of view, the tool is devided into modules:
	- <b>SYS</b>tem module (\ref sys): This module comprises classes, which are used from all other modules.
	Examples are classes for the GUI, output, exceptions, database management, project management etc.
	As a sub-module gemetrical classes are implemented (GEO_). This classes are used for geometrical
	calculation, like points, lines, rays, polygons etc.

	- <b>F</b>lood<b>P</b>rotection<b>L</b>ine module (\ref fpl): The objective of the reliability analysis is to quantify the failure probability of flood protection measures along the river or the coast line, e.g. dikes, walls etc.. 
	The probability of the complementary event (non-failure event) describes the reliability of a structure. 
	The base calculations are a probabilistic Monte-Carlo analysis combined with a fault-tree analysis. 
	The first analysis (Monte-Carlo analysis) models the probabilistic distribution of the relevant input parameter, like material-parameter or applied stresses.
	The second one (fault-tree analysis) combines the specific failure mode of a specific section type (e.g. dike, wall etc.) to one failure probability. 
	Further information are given in the main class Fpl_Calculation.

	- <b>HYD</b>raulic module (\ref hyd): The objective of the hydrodynamic analysis is to transform the characteristics of a discharge /storm event into hydraulic variables of 
	the subsystems river and hinterland while taking into consideration the morphological characteristics of the area under investigation. In principle, this transformation is 
	based on hydrodynamic laws which gets solvable by numerical methods. 
	The implemented submodels represents in the real world floodplains (2d-floodpain model), rivers (1d-river models) and the coastal area. 
	The natural interaction between rivers, coast and floodplains are modelled by a coupling of these submodels.
	For the solution of the numerical problem, 1d as well as 2d, the c-vode solver package is implemented as a
	sub-module of the hydaulic modul. Further information of the solver modul are given in
	https://computation.llnl.gov/casc/sundials/documentation/cv_guide.pdf. The main class of the hydraulic modul
	is Hyd_Hydraulic_System and for multi-threading application Hyd_Multiple_Hydraulic_System, where further detailed
	information are given.

	- <b>DAM</b>age module (\ref dam): The aim of the analysis of the consequences is to transform the given hydraulic values of a flood event into consequences to people, assets, infrastructure etc.. This transformation enables a direct statement about the consequences of a flood event.
	Different categories of consequences are implemented within ProMaIdes:
		- direct economic damages (ECN), further distinguished in mobile and immobile damages
		- ecological damages (ECO), further distinguished in biotope-type damages and due to soil erosion,
		- consequences to people (POP), further distinguished in endangered and affected people and
		- psycho-social consequences (PYS), further distinguished in different criteria.
	Required input data are the result data of the hydraulic analysis (HYD-module \ref hyd). Further information are given in
	the main class Dam_Damage_System.

	- <b>RISK</b> (\ref risk): The risk modules combines the result values of the probability calculation and the damage
	analysis to the risk. Three risk approaches are distinguished:
		- the nobreak risk approach, where is presumed that no failure occurs in the flood protection line. The
		risk is defined as the reoccurence probability of an flood event times the damages.
		- the scenario risk approach, where the failure probability of one flood protection section is
		evaluated with a pure Monte-Carlo analysis with an probabilistic waterlevl included. The damages
		are also reduced to a failure of one flood protection section. The risk is defined as the failure probabilty
		times the damages.
		- the catchment area approach, where the failure probability are integrated via a fragility curve. By a
		combination of hydraulic, failure- and non-failure probability and the event dependend damages the risk
		is evaluated on the catchment area scale. Further information are given in the main class Risk_System.

	- <b>ALT</b>ernative module (\ref alt): This module handles the implementation of measures into the system. This
	measure can be group into the three sub-systems of FPL, HYD and DAM. Further information are given in the main class Alt_System.

	- <b>COST</b> module (\ref cost): This module handles and calculate the cost due to a measure's
	implementation into the system. This can be economical cost as well as area requirement for a measrue.
	Further information are given in the main class Cost_System.

	- <b>M</b>ulti<b>A</b>ttribute<b>D</b>ecision<b>M</b>ethod module (\ref madm): The objective
	of this module is the ranking of the user-defined alternatives. This analysis is based on the decision matrix,
	where the alternatives are evaluated with criteria. Mathematical algorithms are used for
	the ranking of the alternatives. Examples are the SAW-, TOPSIS or the ELECTRE-approach. Thus, the decision
	process is supported and gets an objective base. Further information are given in the main class Madm_System.

These moduls can be used independent from each other: a probabilistic analysis can be performed without
any hydraulic calculations. Some moduls, like the damage- or the risk-moduls, requires data input, which is generated by other moduls.
Thus, an independent use of this moduls is not possible.
The data management of ProMaIDes is based in databases. Implemented and maintained is the connection to a PostgreSQL-database. 

*/

///This is the Main-Window class. The GUI-features of the main-window, e.g. menus, toolbars output etc. are implemented here
/**
This class produced the main window of the application. The main window consists of:
	- menus bar for the modules,
	- data view consisting of:
		- data tree,
		- database browser ,
		-f ile brower and viewer
	- output display for the modules and
	- status bar.

Here the menus are connected to the slots, the calculation
moduls are started here as threads etc.
The layout of the widget is produced in the QtDesigner.
*/
class Main_Wid :  public QMainWindow, private Ui::Main_Wid
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor with arguments
	Main_Wid(int argc, char *argv[]);
	///Default Destructor
	~Main_Wid(void);

private:
	//members
	///Project manager
	Sys_Project project_manager;

	///Pointer to the database
	Data_Base *system_database;

	///Pointer to the multiple hydraulc system class
	Hyd_Multiple_Hydraulic_Systems *hyd_calc;
	///Pointer to the damage system
	Dam_Damage_System *dam_calc;
	///Pointer to the fpl system
	Fpl_Calculation *fpl_calc;
	///Pointer to the risk system
	Risk_System *risk_calc;
	///Pointer to the madm system
	Madm_System *madm_calc;
	///Pointer to the alt system
	Alt_System *alt_calc;
	///Pointer to the cost system
	Cost_System *cost_calc;

	///Common System
	_Sys_Common_System system_state;

	///Pointer to the statusbar widget
	Sys_Status_Bar_Wid *status_wid;

	///The welcome widget (is still a test widget)
	Sys_Welcome_Wid welcome_wid;
	///Timer for the welcome widget
	QTimer welcome_timer;

	///QProcess for opening the developer's guide
	QProcess proc_developer_doku;

	///Flag if the project file is set the program start
	bool project_file_set;
	///Flag if the hydraulic tables are created
	bool hyd_tables_created;
	///Flag if the damage tables are created
	bool dam_tables_created;
	///Flag if the fpl-tables are created
	bool fpl_tables_created;
	///Flag if the risk-tables are created
	bool risk_tables_created;
	///Flag if the madm-tables are created
	bool madm_tables_created;
	///Flag if the alt-tables are created
	bool alt_tables_created;
	///Flag if the cost-tables are created
	bool cost_tables_created;
	//introduce further modules...

	///Manager for closing all threads
	Sys_Thread_Closing_Manager *closing_thread;
	///Flag if still a thread is in process
	bool thread_in_process;

	///Flags for representing the state of the risk calculation
	_risk_state_flags risk_flags;

	///Number of errors in database table checking
	int number_error_table_check;
	///Flag if the project should be saved
	bool with_project_save;

	///Version updater
	Sys_Version_Update version_update;

	//method
	///Connect the slots of all menus
	void slot_connect(void);
	///Connect the of the modul system
	void slot_connect_sys(void);
	///Connect the of the modul fpl
	void slot_connect_fpl(void);
	///Connect the of the modul hyd
	void slot_connect_hyd(void);
	///Connect the of the modul dam
	void slot_connect_dam(void);
	///Connect the of the modul dam
	void slot_connect_madm(void);
	///Connect the of the modul risk
	void slot_connect_risk(void);
	///Connect the of the modul alt
	void slot_connect_alt(void);
	///Connect the of the modul cost
	void slot_connect_cost(void);
	//..introduce further modules

	///Connect the output to GUI for all modules
	void output_connect(void);
	///Connect the output to GUI for module sys
	void output_connect_sys(void);
	///Connect the output to GUI for module exc (exception)
	void output_connect_exc(void);
	///Connect the output to GUI for module fpl
	void output_connect_fpl(void);
	///Connect the output to GUI for module hyd
	void output_connect_hyd(void);
	///Connect the output to GUI for module madm
	void output_connect_madm(void);
	///Connect the output to GUI for module dam
	void output_connect_dam(void);
	///Connect the output to GUI for module risk
	void output_connect_risk(void);
	///Connect the output to GUI for module alt
	void output_connect_alt(void);
	///Connect the output to GUI for module cost
	void output_connect_cost(void);
	///Connect the output to GUI for module hydrol
	void output_connect_hydrol(void);
	//..introduce further modules

	///Close the windows of the text search
	void close_text_searcher(void);

	///Delete and disconnect the output
	void delete_output_classes(void);

	///Allocate and connect the status bar widget
	void statusbar_connect(void);

	///Allocate and connect the system tray icon
	void systemtray_connect(void);

	///Start the postgresql database from the system tray context
	void systemtray_startdb(void);

	///Stop the postgresql database from the system tray context
	void systemtray_stopdb(void);

	///Opens the database config of the system tray
	void systemtray_configdb(void);

	///Delete system tray context
	void delete_system_tray(void);

	///private enum just for perform_action_on_database function
	enum database_command_action {
		database_start = 1,
		database_stop = 2
	};

	//Run the Postgres control command on the database path. (Command can be to start or stop the database)
	int perform_action_on_database(database_command_action action);

	///Enable/disable menu and show/hide the data tabs in the dataview corresponding the project type, when a project is open
	void enable_menu_project_open(const bool new_project);
	///Enable/disable menu and show/hide the data tabs in the dataview corresponding the project type, when a project is closed
	void enable_menu_project_closed(void);

	///Enable/disable menu
	void enable_menu(const bool flag);

	///Allocate new database
	void alloc_newdatabase(void);

	///Check if the close database action is enable
	void check_enable_db_close(void);

	///Check if the database is opent to enable the menus
	void menu_enable_checkdb(void);

	///Check if the hydraulic thread is running and set the menus and actions
	void check_hyd_thread_is_running(void);
	///Check if the hydraulic thread with file input is running and set the menus and actions
	void check_hyd_file_thread_is_running(void);
	///Allocate a multiple hydraulic system for a hydraulic calculation
	void allocate_multi_hydraulic_system(void);
	///Delete the multiple hydraulic system for a hydraulic calculation
	void delete_multi_hydraulic_system(void);

	///Check if the damage thread is running and set the menus and actions
	void check_dam_thread_is_running(void);
	///Allocate a damage system for the damage calculation
	void allocate_damage_system(void);
	///Delete the damage system for the damage calculation
	void delete_damage_system(void);

	///Check if the fpl thread is running and set the menus and actions
	void check_fpl_thread_is_running(void);
	///Allocate a fpl system for the fpl calculation
	void allocate_fpl_system(void);
	///Delete the fpl system for the fpl calculation
	void delete_fpl_system(void);

	///Check if the risk thread is running and set the menus and actions
	void check_risk_thread_is_running(void);
	///Allocate a risk system for the risk calculation
	void allocate_risk_system(void);
	///Delete the risk system for the risk calculation
	void delete_risk_system(void);

	///Check if the madm thread is running and set the menus and actions
	void check_madm_thread_is_running(void);
	///Allocate a madm system for the madm calculation
	void allocate_madm_system(void);
	///Delete the madm system for the madm calculation
	void delete_madm_system(void);

	///Check if the alt thread is running and set the menus and actions
	void check_alt_thread_is_running(void);
	///Allocate a alt system for the alt calculation
	void allocate_alt_system(void);
	///Delete the alt system for the alt calculation
	void delete_alt_system(void);

	///Check if the cost thread is running and set the menus and actions
	void check_cost_thread_is_running(void);
	///Allocate a cost system for the cost calculation
	void allocate_cost_system(void);
	///Delete the cost system for the cost calculation
	void delete_cost_system(void);

	///Initialize the thread-closing manager
	bool init_thread_closing_manager(void);
	///Delete the thread closing manager
	void delete_thread_closing_manager(void);

	///Set the risk state flags and change the statusbar
	void set_risk_state_flag(const _risk_state_flags flags);
	///Set the system state and change the status bar
	void set_system_state(const _sys_system_id id);

	///Check if the dam-thread is still ruuning
	bool dam_thread_running(void);
	///Check if the hyd-thread is still ruuning
	bool hyd_thread_running(void);
	///Check if the fpl-thread is still ruuning
	bool fpl_thread_running(void);
	///Check if the some thread is still ruuning
	bool some_thread_running(void);

	///Check for enabling the risk check-box
	void check_enabling_risk_check_box(const bool flag);
	///Reset exceptions of now action is running and a new action is started
	void reset_exception_new_action(void);

signals:

	///Send to if one thread to create the database tables is finished
	void send_table_creation_is_finished(void);
	///Send to if one thread check the database tables is finished
	void send_table_check_is_finished(void);
	///Send a text to the status bar
	void send_txt2statusbar(QString txt, int time);
	///Send a that a deleting of data is in action an refresh the data-tree view
	void send_delete2refresh_data_view(void);
	///Send a that file tree view can be set up
	void send_setup_file_tree_view(void);
	///Send that the task-by-file can start
	void send_task_by_file_start(void);

private slots:
	///The welcome window is closed; the mainwindow is started
	void start_main_window(void);

	///Set the main window to enable/disable
	void set_enabled(bool flag);

	///Ask for closing a current project, if the user wants to open an existing project
	void ask_close_project_open_existing(void);
	///Open an existing project
	void open_existing_project(void);

	///Ask for closing a current project, if the user wants to create a new project
	void ask_close_project_create_new(void);
	///Open and create a new project
	void open_new_project(void);

	///Terminate the threads in case of closing the whole application
	void terminate_threads_close_app(void);
	///Terminate the threads in case of closing the project
	void terminate_threads_close_proj(void);
	///Terminate the threads in case of closing the project by open an existing one
	bool terminate_threads_close_proj_open_existing(void);
	///Terminate the threads in case of closing the project by creating a new one
	bool terminate_threads_close_proj_create_new(void);

	///Close a project
	void close_project(void);
	///Delete completly a project
	void delete_project(void);
	///Copy a project
	void copy_project(void);
	///The project copy thread is finished
	void threads_copy_proj_finished(void);

	///Create the project tables
	void create_project_database_tables(void);
	///Create the project tables for an upgraded project
	void upgrade_create_project_database_tables(void);
	///Managing the table creation threads of the modules
	void tables_creation_manager(void);
	///Managing the table check threads of the modules
	void tables_check_manager(void);
	///Read an existing project in
	void read_existing_project(void);
	///Save the project parameter
	void save_project_parameter(void);
	///Edit the project parameter
	void edit_project_parameter(void);

	///Output the project parameters to display/console
	void show_project_param(void);

	///Get the warning number and set it to the statusbar widget
	void get_warning_number(const int number, const QString tooltip);
	///Get the error number and set it to the statusbar widget
	void get_error_number(const int number, const QString tooltip);
	///Get the workspace memory text and set it to the statusbar widget
	void get_memorytxt(const QString txt, const QString tooltip);
	///Set "ready" to status bar widget
	void set_ready2statusbar(QString txt);
	///Set text to status bar widget
	void set_text2statusbar(QString txt, int time);

	///Get a text for a fatal error and open a dialog
	void get_fatal_error(const QString text);

	///Recieve the text from the output class SYS and give it to the corresponding display tab widget
	void txt_to_system_out(QString txt);
	///Recieve the text from the output class FPL and give it to the corresponding display tab widget
	void txt_to_fpl_out(QString txt);
	///Recieve the text from the output class EXCEP and give it to the corresponding display tab widget
	void txt_to_excep_out(QString txt);
	///Recieve the text from the output class HYD and give it to the corresponding display tab widget
	void txt_to_hyd_out(QString txt);
	///Recieve the text from the output class MADM and give it to the corresponding display tab widget
	void txt_to_madm_out(QString txt);
	///Recieve the text from the output class DAM and give it to the corresponding display tab widget
	void txt_to_dam_out(QString txt);
	///Recieve the text from the output class RISK and give it to the corresponding display tab widget
	void txt_to_risk_out(QString txt);
	///Recieve the text from the output class ALT and give it to the corresponding display tab widget
	void txt_to_alt_out(QString txt);
	///Recieve the text from the output class COST and give it to the corresponding display tab widget
	void txt_to_cost_out(QString txt);
	///Recieve the text from the output class HYDROL and give it to the corresponding display tab widget
	void txt_to_hydrol_out(QString txt);
	//..introduce further modules

	///Close all database tables (menu/sys/common)
	void close_all_tables(void);
	///Delete all file in the logfile archiv (menu/sys/common)
	void delete_logfile_archiv(void);
	///Delete files in the logfile archiv (menu/sys/common)
	void delete_file_logfile_archiv(void);

	///Set a new path (menu/sys/project)
	void set_new_path(void);


	///Close the application (menu SYS)
	void my_close(const bool dialog = true);


	///Close the widget for closing the threads
	void close_thread_finished(_sys_close_types close_type);

	///Set the database connection by hand via a dialog (menu SYS/database)
	void db_con_dialog(void);
	///Set the database connection per file; the file is asked via a dialog (menu SYS/database)
	void db_con_file_dialog(void);
	///Set the database connection directly; it is for development (menu SYS/database)
	void db_con_development(void);

	///Open the database again from the project file
	void db_con_open(void);

	///Output the database connection parameter to display SYS (menu SYS/database)
	void db_parameter(void);
	///Output the database connection status to display SYS (menu SYS/database)
	void db_con_status(void);

	///Close the database connection (menu SYS/database)
	void db_con_close(void);

	///Show dialog that the database connection is broken during calculation; try to reconnect to database
	void database_connection_breaks(QString time);

	///Clean the database table view of the project browser
	void clean_db_view(void);

	///Check and get the risk state from the statusbar
	void check_change_risk_state(bool flag);

	///Reset the warning- and error-number (menu SYS/common)
	void reset_error_warning(void);
	///Reset the display SYS (menu SYS/common/System output text)
	void reset_system_outputtxt(void);
	///Set a new output flag for the system modul(menu SYS/common/System output text))
	void set_system_outputflag(void);
	///Set a new output logfile for the system modul(menu SYS/common)
	void set_system_outputlogfile(void);
	///Clear all output displays(menu SYS/common)
	void clear_all_output_displays(void);

	///Task by file action
	void start_task_by_file(void);
	///Interpret the task to do
	void interpret_task(QList<QVariant> list);
	///Interpret and start the task to do for FPL-module
	void start_task_fpl(QList<QVariant> list);
	///Interpret and start the task to do for HYD-module
	void start_task_hyd(QList<QVariant> list);
	///Check for the keywords in tasks NEW, ALL section ids
	QList<int> check_key_word_fpl(const QString key, const int last_number);
	///Check for the keywords in tasks NEW, ALL section ids
	QList<int> check_key_word_hyd(const QString key);

	///Import fpl-section(s) from file to database
	void import_section2database(void);
	///Import fpl-section(s) from file to database from task
	void import_section2database_task(QStringList list_id);

	///Create the database tables for the fpl system
	void create_fpl_system_database_tables(void);
	///Check for the normal end of the fpl create-tables thread
	void thread_fpl_create_tables_finished(void);

	///Check the database tables for the hydraulic system (menu fpl/common)
	void check_fpl_system_database_tables(void);
	///Check for the normal end of the fpl check-datbase tables thread
	void thread_fpl_check_tables_finished(void);

	///Check fpl-section(s) per file (menu fpl/Section check)
	void check_section_file(void);
	///Check fpl-section(s) per database (menu fpl/Section check)
	void check_section_database(void);

	///Export deterministic results of a FPL-section (menu fpl/Export results/)
	void export_results_determ(void);
	///Export deterministic results of a FPL-section from task
	void export_results_determ_task(QList<int> list_id);
	///Export Monte-Carlo results of a FPL-section (menu fpl/Export results/)
	void export_results_mc(void);
	///Export Monte-Carlo results of a FPL-section from task
	void export_results_mc_task(QList<int> list_id);
	///Export FRC-results of a FPL-section (menu fpl/Export results/)
	void export_results_frc(void);
	///Export FRC-results of a FPL-section from task
	void export_results_frc_task(QList<int> list_id);

	///Combine the fpl-system with the hydraulic system (menu fpl/HYD2FPL)
	void combine_fpl2hyd_system(void);
	///Check the idealization of the hydraulic system by the fpl-system (menu fpl/HYD2FPL)
	void check_ideal_hyd_system_fpl(void);

	///Perform a deterministic calculation (menu fpl/calculation)
	void perform_determ_calculation(void);
	///Perform a deterministic calculation from task
	void perform_determ_calculation_task(QList<int> list_id);

	///Perform a monte-carlo calculation (menu fpl/calculation)
	void perform_mc_calculation(void);
	///Perform a monte-carlo calculation from task
	void perform_mc_calculation_task(QList<int> list_id);

	///Perform a fragility curve calculation (menu fpl/calculation)
	void perform_frc_calculation(void);
	///Perform a fragility curve calculation task
	void perform_frc_calculation_task(QList<int> list_id);

	///Perform a test for the random generator (menu fpl/calculation)
	void perform_test_random(void);

	///Delete section of the fpl-module in database (menu fpl/Delete section...)
	void delete_fpl_section_database(void);
	///Delete section of the fpl-module in database task
	void delete_fpl_section_database_task(QList<int> list_id);

	///Restore default values of the fpl-module in database tables (menu fpl/Restore standard...)
	void restore_default_database_table(void);

	///The fragility curve is defined by the user (menu fpl/)
	void set_frc_by_hand(void);

	///User aborted the fpl calculation thread (menu fpl)
	void stop_fplcalc_thread(void);
	///Check for the normal end of the fpl calculation thread
	void thread_fpl_calc_finished(void);

	///Reset the display FPL (menu fpl/common)
	void reset_fpl_outputtxt(void);
	///Set a new output flag for the fpl modul(menu fpl/common)
	void set_fpl_outputflag(void);
	///Set a new output logfile for the fpl modul(menu fpl/common)
	void set_fpl_outputlogfile(void);

	///Recieve if a module extern uses the fpl thread
	void fpl_module_extern_runs(bool flag);

	///Set a hydraulic calculation per file (menu hyd/calculation/per file)
	void set_hydcalc_per_file(void);
	///Set a hydraulic calculation per file via task
	void set_hydcalc_per_file_task(QStringList list_id);
	///User aborted the hydraulic calculation thread (menu hyd)
	void stop_hydcalc_thread(void);
	///Check for the normal end of the hydraulic calculation thread
	void thread_hyd_calc_finished(void);
	///Catch the number of threads, which are launched from the multiple hydraulic system for calculation
	void catch_thread_number_hy_calc(QString number);
	///Catches the signal from the CPU/GPU working floodplain emitter from the Hyd_Hydraulic_System which is passed through the Hyd_Multiple_Hydraulic_Systems
	void catch_main_statusbar_hyd_solver_update(unsigned int cpu, unsigned int gpu);

	///Set a new output flag for the hyd modul(menu hyd/common)
	void set_hyd_outputflag(void);
	///Set a new output logfile for the hyd modul(menu hyd/common)
	void set_hyd_outputlogfile(void);
	///Reset the display HYD (menu hyd/common)
	void reset_hyd_outputtxt(void);

	///Set hydraulic system properties (menu hyd/common)
	void set_hydraulic_system_properties(void);

	///Create the database tables for the hydraulic system (menu hyd/common)
	void create_hyd_system_database_tables(void);
	///Check for the normal end of the hydraulic create-database tables thread
	void thread_hyd_create_tables_finished(void);

	///Check the database tables for the hydraulic system (menu hyd/common)
	void check_hyd_system_database_tables(void);
	///Check for the normal end of the hydraulic check-datbase tables thread
	void thread_hyd_check_tables_finished(void);

	///Import the hydraulic base system per file to a database (menu hyd/import files to database)
	void import_hyd_basesystem_file2database(void);
	///Import and add the hydraulic boundary szenarios per file to a database (menu hyd/import files to database)
	void add_hyd_boundary_sz_file2database(void);

	///Import and add the hydraulic boundary szenarios per file to a database via task
	void add_hyd_boundary_sz_file2database_task(QStringList list_data);

	///User aborted the hydraulic file-import thread (menu hyd)
	void stop_hydimport_thread(void);
	///Check for the normal end of the hydraulic file import thread
	void thread_hyd_import_finished(void);

	///Check the the hydraulic system (data, geometrical interception, coupling) per file (menu hyd/Model check)
	void check_hyd_system_perfile(void);
	///Check the the hydraulic system (data, geometrical interception, coupling) per database (menu hyd/Model check)
	void check_hyd_system_database(void);
	///User aborted the check of the hydraulic system thread (menu hyd)
	void stop_hyd_systemcheck_thread(void);
	///Check for the normal end of the check of the hydraulic system thread
	void thread_hyd_systemcheck_finished(void);

	///Set a hydraulic calculation per database (menu hyd/calculation/per database)
	void set_hydcalc_per_db(void);

	///Set a hydraulic temperature calculation per database (menu hyd/calculation/temp per database)
	void set_hydtempcalc_per_db(void);

	///Set a hydraulic calculation per task
	void set_hydcalc_per_task(QList<int> list_id);

	///Delete selected boundary scenarios (menu hyd/)
	void delete_selected_bound_sc(void);
	///Delete selected boundary scenarios via task)
	void delete_selected_bound_sc_task(QList<int> list_id);

	///Convert a given Gis-raster into a Prom-raster (menu hyd/tools)
	void convert_Gis2Prom_raster(void);
	///Convert profile data to a river DGM-W (menu hyd/tools)
	void convert_profile2dgmw(void);

	///Recieve if a module extern permits the use of the hydraulic module and disable the menus
	void enable_hyd_module_extern(bool flag);
	///Recieve if a module extern uses the hydraulic thread
	void hyd_module_extern_runs(bool flag);
	///Recieve if the hydraulic thread is sleeping
	void hydraulic_thread_sleeps(bool flag);

	///User aborted the damage calculation thread (menu dam)
	void stop_damcalc_thread(void);
	///Check for the normal end of the damage calculation thread
	void thread_dam_calc_finished(void);

	///Recieve if a module extern uses the damage thread
	void dam_module_extern_runs(bool flag);

	///Recieve if the damage thread is sleeping
	void dam_thread_sleeps(bool flag);

	///Create the database tables for the damage system
	void create_dam_system_database_tables(void);
	///Check for the normal end of the damage create-tables thread
	void thread_dam_create_tables_finished(void);
	///Check the database tables for the damage system (menu dam/common)
	void check_dam_system_database_tables(void);
	///Check for the normal end of the damage check-datbase tables thread
	void thread_dam_check_tables_finished(void);
	///Set a new output flag for the damage modul(menu dam/common)
	void set_dam_outputflag(void);
	///Set a new output logfile for the damage modul(menu dam/common)
	void set_dam_outputlogfile(void);
	///Reset the display DAM (menu dam/common)
	void reset_dam_outputtxt(void);

	///Import a hydraulic result raster from a file to database for a DAM-project as a base scenario (menu dam/Import HYD-results/Base scenario)
	void import_hyd_result_raster_base_sc(void);
	///Add a hydraulic result raster from a file to database for a DAM-project as a base scenario (menu dam/Import HYD-results/Add scenario)
	void add_hyd_result_raster_sc(void);
	///Delete hydraulic resuts in the database for a DAM_project (menu dam/Delete Hyd-Results)
	void delete_hyd_results_dam(void);

	///Import the economic damage function from a file to database (menu dam/ECoNomic/Import)
	void import_ecn_dam_function(void);
	///Add an economic raster from a files to database (menu dam/ECoNomic/Import)
	void add_ecn_dam_rasters(void);
	///Delete all economical data in the database to database (menu dam/ECoNomic/)
	void delete_all_ecn_dam(void);
	///Connect the econimical raster (menu dam/ECoNomic/)
	void connect_ecn_raster(void);

	///Import the ecological biotope-types from a file to database (menu dam/ECOlogic/Import)
	void import_eco_b_type(void);
	///Add an ecologic raster of the biotope-types from files to database (menu dam/ECoNomic/Import)
	void add_eco_b_type_rasters(void);
	///Delete all ecological biotope-type data in the database to database (menu dam/ECOlogic/)
	void delete_all_eco_b_type_dam(void);
	///Import the ecological soil-type cost function from a file to database (menu dam/ECOlogic/Import)
	void import_eco_soil_cost(void);
	///Import the ecological soil-type erosion function from a file to database (menu dam/ECOlogic/Import)
	void import_eco_soil_ero(void);
	///Add an ecologic raster of the soil erosion-types from files to database (menu dam/ECoNomic/Import)
	void add_eco_soil_ero_rasters(void);
	///Delete all ecological soil-erosion data in the database to database (menu dam/ECOlogic/)
	void delete_all_eco_soil_dam(void);
	///Connect the ecological raster (menu dam/ECOlogic/)
	void connect_eco_raster(void);

	///Import the people2risk damage function from a file to database (menu dam/PeOPle/Import)
	void import_pop_dam_function(void);
	///Add an people2risk raster from files to database (menu dam/PeOPle/Import)
	void add_pop_rasters(void);
	///Delete all people2risk data in the database to database (menu dam/PeOPle/)
	void delete_all_pop_dam(void);
	///Connect the people2risk raster (menu dam/PeOPle/)
	void connect_pop_raster(void);

	///Import the people2risk category with the criteria score function from a file to database (menu dam/PsYchoSocial/Import)
	void import_pys_category(void);
	///Import the people2risk density function from a file to database (menu dam/PsYchoSocial/Import)
	void import_pys_density_function(void);
	///Add an psycho-social raster from files to database (menu dam/PsYchoSocial/Import)
	void add_pys_rasters(void);
	///Delete all psycho-social data in the database to database (menu dam/PsYchoSocial/)
	void delete_all_pys_dam(void);
	///Connect the psycho-social raster (menu dam/PsYchoSocial/)
	void connect_pys_raster(void);

	///Import the simple counting points from a file to database (menu dam/SimpleCounting/Import)
	void import_sc_points(void);
	///Import the simple counting subcategories from a file to database (menu dam/SimpleCounting/Import)
	void import_sc_subcategories(void);
	///Delete all simple counting data in the database (menu dam/SimpleCounting/)
	void delete_all_sc_dam(void);
	///Connect the simple counting points (menu dam/SimpleCounting/)
	void connect_sc_points(void);

	///Import the CI data to database (menu dam/Critical Infrastructure/Import)
	void import_CI_data(void);
	///Delete all CI data in the database (menu dam/Critical Infrastructure/)
	void delete_all_CI_dam(void);
	///Connect the CI data(menu dam/Critical Infrastructure/)
	void connect_CI_data(void);

	///Check the damage system (menu dam/System/)
	void check_damage_system(void);
	///Output the the statistic of the damage system (menu dam/System/)
	void output_damage_system_statistic(void);
	///Connect the damage raster among each other and connect it to the hydraulic system (menu dam/System/)
	void connect_damage_rasters(void);
	///Calculate the damages for selected nobreak-scenarios (menu dam/System/)
	void calculate_damage_nobreak_sz(void);
	///Calculate the instationary damages for selected nobreak-scenarios (menu dam/System/)
	void calculate_instat_damage_nobreak_sz(void);

	///Recieve enabling/disabling the stop-at-next-step action (menu risk/calculation/catchment risk)
	void enable_stop_at_next_step_action(const bool flag);
	///User aborted the risk calculation thread (menu risk)
	void stop_riskcalc_thread(void);
	///Check for the normal end of the risk calculation thread
	void thread_risk_calc_finished(void);

	///Recieve if the risk thread is sleeping
	void risk_thread_sleeps(bool flag);

	///Create the database tables for the risk system
	void create_risk_system_database_tables(void);
	///Check for the normal end of the risk create-tables thread
	void thread_risk_create_tables_finished(void);
	///Check the database tables for the risk system (menu risk/common)
	void check_risk_system_database_tables(void);
	///Check for the normal end of the risk check-datbase tables thread
	void thread_risk_check_tables_finished(void);
	///Set a new output flag for the risk modul(menu risk/common)
	void set_risk_outputflag(void);
	///Set a new output logfile for the risk modul(menu risk/common)
	void set_risk_outputlogfile(void);
	///Reset the display RISK (menu risk/common)
	void reset_risk_outputtxt(void);

	///Calculate nobreak risk  (menu risk/calculation)
	void calculate_nobreak_risk(void);
	///Calculate scenario based risk (menu risk/calculation)
	void calculate_scenario_based_risk(void);

	///Generate the risk states for the catchment area risk approach (menu risk/calculation/catchment risk)
	void generate_catchment_risk_states(void);
	///Calculate catchment area risk for nobreak state (detailed risk result) (menu risk/calculation/catchment risk)
	void calculate_catchment_risk_nobreak(void);
	///Calculate catchment area risk for one break states (detailed risk result) (menu risk/calculation/catchment risk)
	void calculate_catchment_risk_one_break(void);
	///Calculate catchment area risk user defind risk states (detailed risk result) (menu risk/calculation/catchment risk)
	void calculate_catchment_risk_user_defined(void);
	///Calculate catchment area risk woth automatically generated risk states (detailed risk result) (menu risk/calculation/catchment risk)
	void calculate_catchment_risk_auto(void);
	///Calculate and cumulate the generate catchment area risk states to one risk result
	void cumulate_catchment_risk(void);
	///Calculate and output the statistic of the catchment risk approach (menu risk/)
	void calc_output_catchment_risk_statistic(void);

	///Calculate steps of the risk analysis in serail
	void calc_risk_analysis_steps_serial(void);

	///Stop by next hydraulic calculation (menu risk/calculation/catchment risk)
	void stop_by_next_hyd_calc(void);

	///Output the statistics of already generated detailed risk results to display/console (menu risk/)
	void output_statistics_detailed_risk_results(void);

	///Delete the risk data in database table (menu risk/delete results/ restore)
	void delete_risk_data_database(void);
	///Delete the risk result data in database table (activate by change the risk state to false)
	void delete_risk_result_database(const bool del_hyd_dam_fpl = true);
	///Delete the risk result data and the relvant result data of other modules (risk, fpl, dam, hyd) in database table (activate by change the risk state to true)
	void delete_risk_relevant_results_database(void);

	///Output the areastate and measure number (system id) to display ALT (menu ALT/Area-/measure-state)
	void show_systemid(void);
	///Set a new measure state (menu ALT/new area-/measure state)
	void set_new_measure_state(void);
	///Set a new area state (menu ALT/new area-/measure state)
	void set_new_area_state(void);
	///User aborted the alternative implmentation thread (menu ALT/new area-/measure state)
	void stop_alt_implement_thread(void);
	///Set a new measure state (menu ALT/edit area-/measure state)
	void edit_measure_state(void);
	///Set a new area state (menu ALT/edit area-/measure state)
	void edit_area_state(void);
	///Switch the measure state to a another one, which is already calculated (menu ALT/switch measure state)
	void switch_measure_state(void);
	///Delete a choosen measure state
	void delete_measure_state(void);
	///Check for switching or implemeneting a new measure state
	bool check_switch_new_implement_measure(const bool switching);
	///Create the database tables for the alt system
	void create_alt_system_database_tables(void);
	///Check for the normal end of the alt create-tables thread
	void thread_alt_create_tables_finished(void);
	///Check the database tables for the alt system (menu alt/common)
	void check_alt_system_database_tables(void);
	///Check for the normal end of the alt check-datbase tables thread
	void thread_alt_check_tables_finished(void);
	///Thread of the measure's implememtation is finished
	void thread_measure_implement_is_finished(void);
	///Thread to switch the measure state id finished
	void thread_measure_switch_is_finished(void);
	///Set a new output flag for the alt modul(menu alt/common)
	void set_alt_outputflag(void);
	///Set a new output logfile for the alt modul(menu alt/common)
	void set_alt_outputlogfile(void);
	///Reset the display ALT (menu alt/common)
	void reset_alt_outputtxt(void);

	///Set user-defined cost data (menu cost/ECoNomic/)
	void set_user_defined_ecn_cost(void);
	///Create the database tables for the cost system
	void create_cost_system_database_tables(void);
	///Check for the normal end of the cost create-tables thread
	void thread_cost_create_tables_finished(void);
	///Check the database tables for the cost system (menu cost/common)
	void check_cost_system_database_tables(void);
	///Check for the normal end of the cost check-datbase tables thread
	void thread_cost_check_tables_finished(void);
	///Set a new output flag for the cost modul(menu cost/common)
	void set_cost_outputflag(void);
	///Set a new output logfile for the cost modul(menu cost/common)
	void set_cost_outputlogfile(void);
	///Reset the display COST (menu cost/common)
	void reset_cost_outputtxt(void);

	///User aborted the madm calculation thread (menu madm)
	void stop_madmcalc_thread(void);
	///Import weigthing set via a file to database for the madm-analysis (menu madm/import)
	void import_weighting_set_file2database(void);
	///Delete selected weighting sets from database table (menu madm/)
	void delete_selcted_weighting_set_database(void);
	///Perform a madm-analysis via for a file input (menu madm/calculation)
	void madm_analysis_file(void);
	///Perform a madm-analysis via for database input (menu madm/calculation)
	void madm_analysis_database(void);
	///Check for the normal end of the madm calculation thread
	void thread_madm_calc_finished(void);
	///Create the database tables for the madm system
	void create_madm_system_database_tables(void);
	///Check for the normal end of the madm create-tables thread
	void thread_madm_create_tables_finished(void);
	///Check the database tables for the madm system (menu madm/common)
	void check_madm_system_database_tables(void);
	///Check for the normal end of the madm check-datbase tables thread
	void thread_madm_check_tables_finished(void);
	///Set a new output flag for the madm modul(menu madm/common)
	void set_madm_outputflag(void);
	///Set a new output logfile for the madm modul(menu madm/common)
	void set_madm_outputlogfile(void);
	///Reset the display MADM (menu madm/common)
	void reset_madm_outputtxt(void);

	///Open/close explorer display
	void open_close_explorer_display(bool flag);
	///Open/close status window
	void open_close_status_display(bool flag);
	///Open/close output display
	void open_close_output_display(bool flag);
	///Uncheck the explorer display action
	void uncheck_explorer_display(void);
	///Uncheck the output display action
	void uncheck_output_display(void);
	///Uncheck the status display action
	void uncheck_status_display(bool flag);

	///Message with the "about"-text (menu help)
	void about(void);
	///Open the available online guides (menu help)
	void open_doku(void);
	///Open the Community of users (menu help)
	void open_com_users(void);

private:
	//member
	///Flag if the window is closed
	bool close_flag;

	///Flag that tables are created
	bool table_create_flag;

	///Task file
	string task_file_name;
	///Task flag
	bool task_flag;
	///Number task
	int number_task;
	///Counter task
	int count_task;
	///List of tasks
	QList<QList<QVariant>> task_list;
	///New section (FPL)/system (HYD) with task imported or calculated
	int number_new_sec;
	///Total error during task
	int total_err_task;

	///System tray icon
	QSystemTrayIcon* trayIcon;

	///System tray context menu
	QMenu* trayContextMenu;

	///System tray start database action
	QAction* startDbAction;

	///System tray stop database action
	QAction* stopDbAction;

	///System tray stop database action
	QAction* configDbAction;

	///System tray exit program action
	QAction* exitAction;

	///New HYD-scenrio-ids in task
	QList<int> new_hyd_sc_list;

	//method
	///Allocate the table view for the database tables
	void allocate_db_table_view(void);
	///Delete the table view for the database tables
	void delete_db_table_view(void);

	///restore th widget settings
	void readSettings(void);

	///Handles the close event by the close-button of the main-widget
	void closeEvent(QCloseEvent *close);

	///Read task file
	void read_task_file(void);
	///Output task file list
	void output_task_list(void);

	///Set error(s)
	Error set_error(const int err_type);
};
#endif