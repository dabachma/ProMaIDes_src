#pragma once
/**\class Hyd_Multiple_Hydraulic_Systems
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_MULTIPLE_HYDRAULIC_SYSTEM_H
#define HYD_MULTIPLE_HYDRAULIC_SYSTEM_H
//system sys libs
#include "Sys_Multipathsettings_Dia.h"
//system hyd libs
#include "Hyd_Hydraulic_System.h"
//managing class for the hydraulic boundary szenarios
#include "Hyd_Boundary_Szenario_Management.h"
//conversion class for raster
#include "Hyd_Gis2Promraster_Converter.h"
//dialog class for profile conversion
#include "HydGui_Profil2Dgm_Converter_Dia.h"

//conversion class for profile to DGM-W
#include "Hyd_Profil2Dgm_Converter.h"

///Enumerator to distinguish the type of the thread, e.g. peforming a hydraulic calculation or the importing of file data to a database \ingroup hyd
enum _hyd_thread_type{
	///Thread is performing a hydraulic calculation
	hyd_calculation,
	///Thread is performing a temperature hydraulic calculation
	hyd_temp_calculation,
	///Thread is importing the hydraulic input data of a file to a database ass the base hydraulic system
	hyd_data_import,
	///Thread is creating the hydraulic tables for a database project
	hyd_create_tab,
	///Thread is checking the hydraulic tables of a database project
	hyd_check_tab,
	///Thread is checking the hydraulic system (data, geometrical interception, couplings)
	hyd_check_system,
	///Thread for adding hydraulic boundary scenarios
	hyd_add_sz,
	///Thread for deleting selected hydraulic boundary scenarios
	hyd_del_sz,
	///Convert Gis to a Prom-raster
	hyd_convert_Gis2Prom,
	///Convert Profile to a river DGM-Wr
	hyd_convert_profil2dgmw,


	///Unknown thread, nothings happen
	hyd_thread_unknown
};


///Managing class for managing multiple hydralic system (Hyd_System_Hydraulic) \ingroup hyd
/**
This class manages the calculation of multiple boundary- or break scenarios of the hydraulic system.

*/
class Hyd_Multiple_Hydraulic_Systems: public QThread , public _Sys_Common_System
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT


public:
	///Default constructor
	Hyd_Multiple_Hydraulic_Systems(void);
	///Default destructor
	~Hyd_Multiple_Hydraulic_Systems(void);

	//members
	///Pointer to the hydraulic system for multi-thread use
	Hyd_Hydraulic_System *threads;

	///Manager of the hydraulic boundary szenarios
	Hyd_Boundary_Szenario_Management sz_bound_manager;

	//methods
	///Ask the number of hydraulic systems and the names of the global files per console
	void ask_system_number_file_console(void);
	///Set the number of files for calculation and the global filenames per dialog (gui)
	bool set_system_number_file_gui(void);
	///Set one scenario for calculation directly
	bool set_system_number_file_direct(QStringList list_id, QList<int> *new_id);

	///Ask per dialog (HydGui_Boundary_Scenario_Dia), which of the availabe hydraulic boundary scenarios should be handled and return the number
	int ask_boundary_scenarios_per_dialog(QSqlDatabase *ptr_database, QWidget *parent);
	///Set per list, which of the availabe hydraulic boundary scenarios should be handled and return the number
	int set_boundary_scenarios_per_list(QSqlDatabase *ptr_database, QList<int> list_id);

	///Ask the file for raster conversion per dialog
	bool ask_file_raster_conversion_dialog(void);
	///Ask the file for profile conversion per dialog
	bool ask_file_profile_conversion_dialog(void);

	///Set the number of hydraulic systems and the names of the global files direct
	void set_system_number_file(const int number, char *file[]);
	///Set the number of hydraulic systems and the names of the global files direct
	void set_system_number_file(QStringList *list);
	///Set the file name of one hydraulic systems 
	void set_one_system_filename(string file);

	///Set a pointer to the database and copy it
	void set_ptr2database(QSqlDatabase *ptr_database);

	///Set the number of hydraulic systems
	void set_number_systems(const int number);

	///Set thread type; use it before starting a thread
	void set_thread_type(_hyd_thread_type type);
	///Get the thread type for specifiying the task of the thread
	_hyd_thread_type get_thread_type(void);

	///Make multiple hydraulic calculations
	void run(void);

	///Set the stop thread flag
	static void set_stop_thread_flag(const bool flag);
	///Check the stop thread flag
	static void check_stop_thread_flag(void);
	///Get the stop thread flag
	static bool get_stop_thread_flag(void);

	///Set the system-id (_sys_system_id) of the object
	void set_systemid(const _sys_system_id actuel_id);

	///Get the number of errors
	int get_error_number(void);

	///Set number of required threads
	void set_number_required_threads(const int number);
	///Get the number of threads
	int get_number_threads(void);
	///Get a pointer to the thread specified by the index
	Hyd_Hydraulic_System * get_ptr_hyd_threads(const int index);	

	///Set the flag if a file output is required in case of a database based hydraulic calculation
	void set_required_output2file(const bool flag);

	///Get the flag if the thread has started
	bool get_thread_has_started(void);

	///Get if the hydraulic threads are running
	bool get_hydraulic_thread_running(int *number);

	///Delete hydraulic boundary scenarios specified by their id
	void delete_hydraulic_boundary_scenarios(const int id);

signals:
	///Emit the number of threads as string
	void emit_number_threads(QString number);
	///Emit that the threads are allocated
	void emit_threads_allocated(void);

	///Emit the number of calculation, which are already performed, as text
	void emit_number_performed_calculation(QString done);
	
	///Emit the number of floodplains that will run on CPU and/or GPU (emit to the Main_Wid)
	void statusbar_main_hyd_solver_update(unsigned int cpu_count, unsigned int gpu_count);

public slots:
	///Manage the output of the hydraulic classes
	void recieve_output_required(int thread);

	///Catches the signal from the CPU/GPU working floodplain emitter from the Hyd_Hydraulic_System then send again to the Main_Wid
	void catch_multi_statusbar_hyd_solver_update(unsigned int cpu_count, unsigned int gpu_count);

private:

	//members
	///Numbers of hydraulic systems
	int number_systems;
	///Pointer to a string for multiple file names
	string *file_names;

	///Copy of the database
	QSqlDatabase qsqldatabase;


	///Counter for the total number of warnings
	int counter_warnings;
	///Counter for the total number of errors
	int counter_error;

	///Time classes for the real time calculation (start time point)
	time_t start_time;
	///Time classes for the real time calculation (actual time point)
	time_t actual_time;

	///Type of the thread
	_hyd_thread_type type;

	
	///Flag if the thread are aborted by the user
	static bool abort_thread_flag;

	///Required threads
	int required_threads;
	
	//Flag if a file output is required in case of a database based hydraulic calculation
	bool output2file_required;

	///Flag if the threads has started
	bool thread_is_started;

	///Gis-raster, which is the input-raster
	Hyd_Gis2Promraster_Converter *gis_raster;
	///Prom-raster. which is the output-raster
	Hyd_Gis2Promraster_Converter *prom_raster;

	///Dialog for the file input of the raster
	Sys_Multi_Filechooser_Dia *raster_import_dia;

	///Dialog for the file input of the profile and streamlines
	HydGui_Profil2Dgm_Converter_Dia *profil_import_dia;
	///Conversion class of profile and streamlines to DGM-W
	Hyd_Profil2Dgm_Converter *profil_dgm_conversion;

	//methods
	///Set the required threads and initialize them
	void set_required_threads(void);
	///Allocate the required hydraulic threads
	void allocate_hyd_threads(void);
	///Delete the hydraulic threads
	void delete_hyd_threads(void);
	///Set hydraulic system(s) of the threads the first time per file for calculation
	void set_hyd_system_new_file(int *counter_sys);
	///Decide for new loading a hydraulic system or just reseting and new loading the boundary conditions from database
	void decide_new_reset_db(int *counter_sys);
	///Set hydraulic system(s) of the threads the first time per database for calculation
	void set_hyd_system_new_db(int *counter_sys, const int i);
	///Reset hydraulic system(s) boundary condition and set them new per databse for calculation
	void reset_hyd_system_boundaries_db(int *counter_sys, const int i);

	///Set hydraulic temperature system(s) of the threads the first time per database for calculation
	void set_hyd_temp_system_new_db(int *counter_sys, const int i);
	///Reset hydraulic temperature system(s) boundary condition and set them new per databse for calculation
	void reset_hyd_temp_system_boundaries_db(int *counter_sys, const int i);

	///Calculate the hydraulic system
	void calculate_hyd_system(void);
	///Calculate the hydraulic temperature system
	void calculate_hyd_temp_system(void);
	///Perform the postprocessing of the hydraulic system(s) calculation 
	void make_postprocessing_hyd_system(void);
	///Perform the postprocessing of the hydraulic temperature system(s) calculation 
	void make_postprocessing_hyd_temp_system(void);
	///Waiting loop for the threads
	void wait_loop(void);

	///Allocate the file names
	void allocate_file_names(void);
	///Delete the file names
	void delete_file_names(void);

	///Calculate one/multiple hydraulic system(s) from files
	void calculate_file_systems(void);

	///Import the base hydraulic system from a file to the database (hydraulic boundary szenario 0)
	void import_basesystem_file2database(void);

	///Add additional boundary szenarios from a file to the database 
	void add_boundary_szenarios_file2database(void);

	///Create the required database tables for an hydraulic project
	void create_database_tables(void);
	///Check the database for the required tables for an hydraulic project
	void check_database_tables(void);

	///Check a hydraulic system, read in from a file, for the data, the geometrical interceptions, the couplings etc.; here the complete pre-processing is performed
	void check_hydraulic_system_perfile(void);
	///Check a hydraulic system, read in from a database, for the data, the geometrical interceptions, the couplings etc.; here the complete pre-processing is performed
	void check_hydraulic_system_database(void);
	///Calculate one/multiple hydraulic system, read in from a database
	void calculate_hydraulic_system_database(void);

	///Calculate one/multiple hydraulic temperature system, read in from a database
	void calculate_hydraulic_temp_system_database(void);

	///Delete selected boundary scenarios from the database table
	void delete_selected_scenarios(void);

	///Convert a given Gis-raster into a Prom-raster (HYD-tools)
	void convert_Gis2Promraster(void);
	///Convert a profile data to a river DGM-W (HYD-tools)
	void convert_profil2dgmw(void);


	///Output the statistic of the hydraulic tables create-action
	void output_final_statistic_create(void);
	///Output the statistic of the hydraulic tables check-action
	void output_final_statistic_check(void);
	///Output the statistic of the raster conversion
	void output_final_statistic_raster_convert(void);
	///Output the statistic of the hydraulic system check-action
	void output_final_statistic_systemcheck(void);
	///Output the statistic of the data import of the hydraulic base system
	void output_final_statistic_basesystem_import(void);
	//Output the statistic of adding additional hydraulic boundary szenarios
	void output_final_statistic_add_boundsz(void);
	///Output the statistic of the multiple calculation action
	void output_final_statistic(void);
	///Output the statistic of the multiple hydraulic boundary scenario calculation
	void output_final_statistic_multi_hydsz(void);

	///Allocate the raster classes for conversion
	void allocate_raster_class(void);
	///Delete the raster classes for conversion
	void delete_raster_class(void);

	///Allocate the dialog of the raster path for a  raster import
	void allocate_raster_path_dia(void);
	///Delete the dialog of the raster path for a raster import
	void delete_raster_path_dia(void);

	///Allocate the dialog of the profile and streamline data
	void allocate_profile_dia(void);
	///Delete the dialog of the profile and streamline data
	void delete_profile_path_dia(void);


	///Allocate the conversion class of profile and streamlines to DGM-W
	void allocate_profile_conversion(void);
	///Delete the conversion class of profile and streamlines to DGM-W
	void delete_profile_conversion(void);

 

	///Set error(s)
	Error set_error(const int err_type);

	///Set warning(s)
	Warning set_warning(const int warn_type);


};
#endif
