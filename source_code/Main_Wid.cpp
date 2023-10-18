//#include "Main_Wid.h"
#include "Main_Headers_Precompiled.h"
//init static variables

//constructor
Main_Wid::Main_Wid(int argc, char *argv[]){
	//important to use this custom defined enumerator with the signal and slot-mechanism
	qRegisterMetaType<_sys_close_types>("_sys_close_types");

	//task variables
	this->task_file_name = label::not_set;
	this->task_flag = false;
	this->number_task=0;
	this->count_task=0;
	this->number_new_sec = 0;
	this->total_err_task = 0;

	//project settings
	if(argc==1){
		this->project_file_set=false;
	}
	else{
		if(this->project_manager.set_project_file(argv[1])==true){
			this->project_file_set=true;
		}
		else{
			this->project_file_set=false;
		}
		if(argc == 3) {
			this->task_file_name = argv[2];
		}
		
	}

	

	this->hyd_tables_created=false;
	this->dam_tables_created=false;
	this->fpl_tables_created=false;
	this->risk_tables_created=false;
	this->madm_tables_created=false;
	this->alt_tables_created=false;
	this->cost_tables_created=false;

	this->number_error_table_check=0;
	this->with_project_save=true;

	this->close_flag=false;
	this->table_create_flag=false;

	this->setupUi(this);
	this->setGeometry(40,60, 1200,900);

	//init pointers
	this->system_database=NULL;
	this->hyd_calc=NULL;
	this->dam_calc=NULL;
	this->fpl_calc=NULL;
	this->risk_calc=NULL;
	this->madm_calc=NULL;
	this->alt_calc=NULL;
	this->cost_calc=NULL;

	//output display widget
	//connect the text-display widget
	this->output_connect();

	this->actionTemp_per_database->setVisible(false);

	//connect and set-up the system tray icon
	this->systemtray_connect();

	//connect and set-up the status bar widget
	this->statusbar_connect();

	//connect the slots
	this->slot_connect();

	//syncronisation of the tab-widgets of the data-view and data-tree-view
	this->tabWidget_treeview->setCurrentIndex(3);
	QIcon my_icon1;
	my_icon1.addFile(":data_icon");
	this->tabWidget_treeview->setTabIcon(0,my_icon1);
	QIcon my_icon2;
	my_icon2.addFile(":database_icon");
	this->tabWidget_treeview->setTabIcon(1,my_icon2);
	QIcon my_icon3;
	my_icon3.addFile(":file_icon");
	this->tabWidget_treeview->setTabIcon(3,my_icon3);
	QIcon my_icon4;
	my_icon4.addFile(":file_icon");
	this->tabWidget_treeview->setTabIcon(2,my_icon4);
	this->tabWidget_data_view->setCurrentIndex(3);

	QIcon my_icon11;
	my_icon11.addFile(":data_icon");
	this->tabWidget_data_view->setTabIcon(0,my_icon11);
	QIcon my_icon21;
	my_icon21.addFile(":database_icon");
	this->tabWidget_data_view->setTabIcon(1,my_icon21);

	QIcon my_icon31;
	my_icon31.addFile(":file_icon");
	this->tabWidget_data_view->setTabIcon(3,my_icon31);
	QIcon my_icon41;
	my_icon41.addFile(":file_icon");
	this->tabWidget_data_view->setTabIcon(2,my_icon41);
	this->menu_enable_checkdb();

	//connect and set up the database browser
	this->database_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	this->status_txt->setReadOnly(true);
	this->status_txt->setText(tr("Disconnected from database"));
	this->browser_database->set_ptr_table_model(this->database_table_view);
	this->browser_database->set_ptr_status_msg(this->status_txt);
	this->browser_database->set_ptr_sys_system_id(this->system_state.get_ptr_sys_system_id());

	//data tree view
	this->treeWidget_data->set_ptr_data_tab_wid(this->tabWidget_data_output);
	this->treeWidget_data->set_ptr_sys_system_id(this->system_state.get_ptr_sys_system_id());
	this->treeWidget_data->set_ptr_overall_data_tab_wid(this->tabWidget_data_view);
	this->treeWidget_data->set_ptr_main_window(this);

	this->treeWidget_data->set_ptr_current_project(&project_manager);

	//file tree view
	this->treeview_file->set_ptr_text_wid(this->text_browser_file);

	//toolbar set to invisible at the beginning
	this->proj_toolbar->setVisible(false);
	this->db_toolbar->setVisible(false);

	this->menu_dam_common->setEnabled(true);

	//close thread thread-class
	this->closing_thread=NULL;
	this->thread_in_process=false;

	//risk state
	this->risk_flags.reliability_dam_result=false;
	this->risk_flags.reliability_fpl_result=false;
	this->risk_flags.reliability_hyd_result=false;
	this->risk_flags.risk_state_flag=false;
	this->risk_flags.catchment_risk_applied=false;
	this->risk_flags.nobreak_risk_applied=false;

	//show welcome widget
	this->welcome_wid.show();
	this->welcome_timer.setSingleShot(true);
	QObject::connect(&welcome_timer, SIGNAL(timeout()), this, SLOT(start_main_window()));
	this->welcome_timer.start(5500);

     this->setWindowTitle(Sys_Project::get_version_number().c_str());

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Main_Wid),_sys_system_modules::SYS_SYS);//count the memory
	//approximated memory for the main-widget
    Sys_Memory_Count::self()->add_mem((int)1024*1024*69.0,_sys_system_modules::SYS_SYS);//count the memory


	this->action_developer_guide->setEnabled(true);
	//restore windows settings
	this->readSettings();

	//ostringstream cout;

	//QStringList buff = QStyleFactory::keys();

	//for (int i = 0; i < buff.count(); i++) {
	//	cout << buff.at(i).toStdString() << endl;
	//}

	//


}
//destructor
Main_Wid::~Main_Wid(void){
	//delete multi hydraulic calculations
	this->delete_multi_hydraulic_system();
	//delete the damage calculation
	this->delete_damage_system();
	//delete the fpl system
	this->delete_fpl_system();
	//delete the risk system
	this->delete_risk_system();
	//delete the madm system
	this->delete_madm_system();
	//delete the cost system
	this->delete_cost_system();
	//delete the alt system
	this->delete_alt_system();

	//close and delete the database at last
	if(this->system_database!=NULL){
		this->system_database->close_database();
		delete this->system_database;
		this->system_database=NULL;
	}

	//close database tables
    Hyd_Hydraulic_System::close_hyd_database_tables();
	Dam_Damage_System::close_dam_database_tables();
	Fpl_Calculation::close_fpl_database_tables();
	Risk_System::close_risk_database_tables();

	//close and delete the system tray context
	this->delete_system_tray();

	//close and delete the output classes (text display window)
	this->delete_output_classes();

	this->delete_thread_closing_manager();

	//close possible processes like the help viewer
	proc_developer_doku.close();

	//count the memory
	Sys_Memory_Count::self()->minus_mem((sizeof(Main_Wid)), _sys_system_modules::SYS_SYS);
	//approximated memory for the main-widget
    Sys_Memory_Count::self()->minus_mem((int)1024*1024*69.0,_sys_system_modules::SYS_SYS);//count the memory
	//reset the memory counter
	Sys_Memory_Count::self()->set_output_widget(NULL);
	//remove cloned database
	if(QSqlDatabase::contains(sys_label::str_sys.c_str())==true){
		if(QSqlDatabase::database(sys_label::str_sys.c_str(),false).isOpen()==true){
			QSqlDatabase::database(sys_label::str_sys.c_str(),false).close();
		}
		QSqlDatabase::removeDatabase(sys_label::str_sys.c_str());
	}
}
//________________
//private
//Connect the slots of all menus
void Main_Wid::slot_connect(void){
	//fatal error to extra dialog
	QObject::connect(Sys_Exception_Sender::self(), SIGNAL(send_fatal_error(const QString )), this, SLOT(get_fatal_error(const QString )));

	QObject::connect(Sys_Exception_Sender::self(), SIGNAL(send_reconnection_try_dia_open(QString )), this, SLOT(database_connection_breaks(QString )));

	//connect all main menus of the modules
	this->slot_connect_sys();
	this->slot_connect_fpl();
	this->slot_connect_hyd();
	this->slot_connect_dam();
	this->slot_connect_madm();
	this->slot_connect_risk();
	this->slot_connect_alt();
	this->slot_connect_cost();

	//menu window
	//Open/close explorer display
	QObject::connect(this->action_explorer_display,SIGNAL(triggered(bool )), this, SLOT(open_close_explorer_display(bool )));
	//Open/close output display
	QObject::connect(this->action_output_display,SIGNAL(triggered(bool )), this, SLOT(open_close_output_display(bool )));
	//Open/close status display
	QObject::connect(this->action_Status_Display,SIGNAL(triggered(bool )), this, SLOT(open_close_status_display(bool )));
	//Uncheck the explorer display action
	QObject::connect(this->dockWidget_tree_view,SIGNAL(close_procedure( )), this, SLOT(uncheck_explorer_display()));
	//Uncheck the output display action
	QObject::connect(this->dockWidget_outtxt,SIGNAL(close_procedure( )), this, SLOT(uncheck_output_display( )));
	//Uncheck the output display action
	QObject::connect(this->status_wid,SIGNAL(emit_close(bool )), this, SLOT(uncheck_status_display(bool )));
	

	//menu help
	QObject::connect(this->action_About,SIGNAL(triggered()), this, SLOT(about()));
	QObject::connect(this->action_developer_guide,SIGNAL(triggered()), this, SLOT(open_doku()));
	QObject::connect(this->action_Community_of_Users, SIGNAL(triggered()), this, SLOT(open_com_users()));

	//Send a that a deleting of data is in action an refresh the data-tree view
	QObject::connect(this,SIGNAL(send_delete2refresh_data_view()), this->treeWidget_data, SLOT(refresh_tree()));


}
//Connect the of the modul system
void Main_Wid::slot_connect_sys(void){
	//connect tab widgets
	QObject::connect(this->tabWidget_data_view, SIGNAL(currentChanged(int )), this->tabWidget_treeview, SLOT(setCurrentIndex(int )));
	QObject::connect(this->tabWidget_treeview, SIGNAL(currentChanged(int )), this->tabWidget_data_view, SLOT(setCurrentIndex(int )));

	//toolbar
	QObject::connect(this->action_show_project_tool, SIGNAL(toggled(bool)), this->proj_toolbar, SLOT(setVisible(bool )));
	QObject::connect(this->action_show_db_tool, SIGNAL(toggled(bool )), this->db_toolbar, SLOT(setVisible(bool )));

	//close the application
	QObject::connect(this->action_Quit, SIGNAL(triggered()), this, SLOT(terminate_threads_close_app()));

	//Clear all output displays(menu SYS/common)
	QObject::connect(this->action_clear_all_output_displays, SIGNAL(triggered()), this, SLOT(clear_all_output_displays()));

	//Close all database tables (menu/sys/common)
	QObject::connect(this->action_close_all_tables, SIGNAL(triggered()), this, SLOT(close_all_tables()));

	//Delete files in the logfile archiv (menu/sys/common)
	QObject::connect(this->action_delete_files_logfile_archiv, SIGNAL(triggered()), this, SLOT(delete_file_logfile_archiv()));

	///Set a new path (menu/sys/project)
	QObject::connect(this->action_set_working_dir, SIGNAL(triggered()), this, SLOT(set_new_path()));

	//project
	QObject::connect(this->action_open_project, SIGNAL(triggered()), this, SLOT(ask_close_project_open_existing()));
	QObject::connect(this->action_new_project, SIGNAL(triggered()), this, SLOT(ask_close_project_create_new()));
	QObject::connect(this->action_close_project, SIGNAL(triggered()), this, SLOT(terminate_threads_close_proj()));
	QObject::connect(this->action_show_project, SIGNAL(triggered()), this, SLOT(show_project_param()));
	QObject::connect(this->action_edit_project, SIGNAL(triggered()), this, SLOT(edit_project_parameter()));
	QObject::connect(this->action_save_project, SIGNAL(triggered()), this, SLOT(save_project_parameter()));
	QObject::connect(this->action_delete_project, SIGNAL(triggered()), this, SLOT(delete_project()));
	QObject::connect(this->action_copy_project, SIGNAL(triggered()), this, SLOT(copy_project()));

	//database
	//database connection
	QObject::connect(this->action_Open_Connection, SIGNAL(triggered()), this, SLOT(db_con_open()));

	//db status to display SYS
	QObject::connect(this->action_Show_Connection_Param, SIGNAL(triggered()), this, SLOT(db_parameter()));
	//db connection data to display SY
	QObject::connect(this->actionShow_Connection_Status, SIGNAL(triggered()), this, SLOT(db_con_status()));
	//close the database
	QObject::connect(this->action_Close_Connection, SIGNAL(triggered()), this, SLOT(db_con_close()));

	//common
	//reset error- and warning-number
	QObject::connect(this->action_Reset_Error_Warning, SIGNAL(triggered()), this, SLOT(reset_error_warning()));
	//delete text of the display sys
	QObject::connect(this->action_reset_display_sys, SIGNAL(triggered()), this, SLOT(reset_system_outputtxt()));
	//set a new output flag
	QObject::connect(this->action_set_outputflag_sys, SIGNAL(triggered()), this, SLOT(set_system_outputflag()));
	//set a new logfile
	QObject::connect(this->action_set_logfile_sys, SIGNAL(triggered()), this, SLOT(set_system_outputlogfile()));

	//activate the browser dockwidget
	QObject::connect(this->browser_database, SIGNAL(send_db_clear()), this, SLOT(clean_db_view()));

	//connect risk state flag (checkbox) with the menu of the risk calculation
	QObject::connect(this->status_wid->get_ptr_risk_state_check_box(), SIGNAL( clicked(bool )), this, SLOT(check_change_risk_state(bool )));

	//set up the file tree view
	QObject::connect(this,SIGNAL(send_setup_file_tree_view()), this->treeview_file, SLOT(set_view()), Qt::QueuedConnection);
}
//Connect the of the modul fpl
void Main_Wid::slot_connect_fpl(void){
	//Import setion from file to database
	QObject::connect(this->action_fpl_import_file, SIGNAL(triggered()), this, SLOT(import_section2database()));
	//Perform a deterministic calculation (menu fpl/calculation)
	QObject::connect(this->action_calc_determ, SIGNAL(triggered()), this, SLOT(perform_determ_calculation()));
	//Perform a monte-carlo calculation (menu fpl/calculation
	QObject::connect(this->action_calc_mc, SIGNAL(triggered()), this, SLOT(perform_mc_calculation()));
	//Perform a fragility curve calculation (menu fpl/calculation)
	QObject::connect(this->action_calc_frc, SIGNAL(triggered()), this, SLOT(perform_frc_calculation()));
	//Perform a test for the random generator (menu fpl/calculation)
	QObject::connect(this->action_test_random, SIGNAL(triggered()), this, SLOT(perform_test_random()));

	//The fragility curve is defined by the user (menu fpl/)
	QObject::connect(this->action_user_defined_frc, SIGNAL(triggered()), this, SLOT(set_frc_by_hand()));

	//User aborted the fpl calculation thread (menu fpl)
	QObject::connect(this->action_stop_fpl_calc, SIGNAL(triggered()), this, SLOT(stop_fplcalc_thread()));

	//Check the database tables for the hydraulic system (menu fpl/common)
	QObject::connect(this->action_check_fpl_tables, SIGNAL(triggered()), this, SLOT(check_fpl_system_database_tables()));
	//Reset the display FPL (menu fpl/common)
	QObject::connect(this->action_Reset_Fpl_display, SIGNAL(triggered()), this, SLOT(reset_fpl_outputtxt()));
	//set a new output flag (menu fpl/common)
	QObject::connect(this->action_set_outputflag_fpl, SIGNAL(triggered()), this, SLOT(set_fpl_outputflag()));
	//set a new output logfile (menu fpl/common)
	QObject::connect(this->action_set_logfile_fpl, SIGNAL(triggered()), this, SLOT(set_fpl_outputlogfile()));

	//Delete section of the fpl-module in database (menu fpl/Delete section...)
	QObject::connect(this->action_delete_fpl_section, SIGNAL(triggered()), this, SLOT(delete_fpl_section_database()));

	//Restore default values of the fpl-module in database tables (menu fpl/Restore standard...)
	QObject::connect(this->action_restore_standard_fpl, SIGNAL(triggered()), this, SLOT(restore_default_database_table()));

	//Check fpl-section(s) per file (menu fpl/Section check)
	QObject::connect(this->action_check_fpl_file, SIGNAL(triggered()), this, SLOT(check_section_file()));
	//Check fpl-section(s) per database (menu fpl/Section check)
	QObject::connect(this->action_check_fpl_db, SIGNAL(triggered()), this, SLOT(check_section_database()));

	//Export deterministic results of a FPL-section (menu fpl/Export results/)
	QObject::connect(this->action_export_DETERM, SIGNAL(triggered()), this, SLOT(export_results_determ()));
	//Export MC-results of a FPL-section (menu fpl/Export results/)
	QObject::connect(this->action_export_MC, SIGNAL(triggered()), this, SLOT(export_results_mc()));
	//Export FRC-results of a FPL-section (menu fpl/Export results/)
	QObject::connect(this->action_export_FRC, SIGNAL(triggered()), this, SLOT(export_results_frc()));

	//Combine the fpl-system with the hydraulic system (menu fpl/HYD2FPL)
	QObject::connect(this->action_fpl2hyd_combine, SIGNAL(triggered()), this, SLOT(combine_fpl2hyd_system()));
	//Check the idealization of the hydraulic system by the fpl-system (menu fpl/HYD2FPL)
	QObject::connect(this->action_fpl2hyd_check_ideal, SIGNAL(triggered()), this, SLOT(check_ideal_hyd_system_fpl()));
}
//Connect the of the modul hyd
void Main_Wid::slot_connect_hyd(void){
	//set a hyd calculation per file (menu/calculation/per file)
	QObject::connect(this->action_hydcalc_per_file, SIGNAL(triggered()), this, SLOT(set_hydcalc_per_file()));
	//Set a hydraulic calculation per database (menu hyd/calculation/per database)
	QObject::connect(this->action_hydcalc_per_db, SIGNAL(triggered()), this, SLOT(set_hydcalc_per_db()));

	//set a new output flag (menu hyd/common)
	QObject::connect(this->action_set_output_flag_hyd, SIGNAL(triggered()), this, SLOT(set_hyd_outputflag()));
	//set a new output logfile (menu hyd/common)
	QObject::connect(this->action_set_logfile_hyd, SIGNAL(triggered()), this, SLOT(set_hyd_outputlogfile()));
	//Reset the display HYD (menu hyd/common)
	QObject::connect(this->action_reset_display_hyd, SIGNAL(triggered()), this, SLOT(reset_hyd_outputtxt()));
	//Set hydraulic system properties (menu hyd/common)
	QObject::connect(this->action_properties_hyd, SIGNAL(triggered()), this, SLOT(set_hydraulic_system_properties()));

	//Import the hydraulic base system per file to a database (menu hyd/import files to database)
	QObject::connect(this->action_hyd_import_base_sz, SIGNAL(triggered()), this, SLOT(import_hyd_basesystem_file2database()));
	//Import and add hydraulic boundary scenarios per file to a database (menu hyd/import files to database)
	QObject::connect(this->action_hyd_import_add_sz, SIGNAL(triggered()), this, SLOT(add_hyd_boundary_sz_file2database()));

	//Check the database tables for the hydraulic system (menu hyd/common)
	QObject::connect(this->action_check_hyd_tables, SIGNAL(triggered()), this, SLOT(check_hyd_system_database_tables()));

	//Check the the hydraulic system (data, geometrical interception, coupling) per file (menu hyd/Model check)
	QObject::connect(this->action_hyd_check_file, SIGNAL(triggered()), this, SLOT(check_hyd_system_perfile()));
	//Check the the hydraulic system (data, geometrical interception, coupling) per file (menu hyd/Model check)
	QObject::connect(this->action_hyd_check_database, SIGNAL(triggered()), this, SLOT(check_hyd_system_database()));

	//User aborted the hyd calculation; the hyd-thread is stopped (menu hyd)
	QObject::connect(this->action_stop_hyd_calc, SIGNAL(triggered()), this, SLOT(stop_hydcalc_thread()));
	//User aborted the hyd import process; the hyd-import thread is stopped (menu hyd)
	QObject::connect(this->action_stop_hyd_import, SIGNAL(triggered()), this, SLOT(stop_hydimport_thread()));
	//User aborted the hyd system check process; the hyd-import thread is stopped (menu hyd)
	QObject::connect(this->action_stop_hyd_check, SIGNAL(triggered()), this, SLOT(stop_hyd_systemcheck_thread()));

	//Delete selected boundary scenarios (menu hyd/)
	QObject::connect(this->action_delete_boundary_scenario, SIGNAL(triggered()), this, SLOT(delete_selected_bound_sc()));

	//Convert a given Gis-raster into a Prom-raster (menu hyd/tools)
	QObject::connect(this->action_convert_gis2prom_raster, SIGNAL(triggered()), this, SLOT(convert_Gis2Prom_raster()));
	//Convert profile data to a river DGM-W
	QObject::connect(this->action_generate_dgm_from_profile, SIGNAL(triggered()), this, SLOT(convert_profile2dgmw()));
}
//Connect the of the modul dam
void Main_Wid::slot_connect_dam(void){
	//Check the database tables for the damage system (menu dam/common)
	QObject::connect(this->action_check_dam_tables, SIGNAL(triggered()), this, SLOT(check_dam_system_database_tables()));

	//set a new output flag (menu dam/common)
	QObject::connect(this->action_set_output_flag_dam, SIGNAL(triggered()), this, SLOT(set_dam_outputflag()));
	//set a new output logfile (menu dam/common)
	QObject::connect(this->action_set_logfile_dam, SIGNAL(triggered()), this, SLOT(set_dam_outputlogfile()));
	//Reset the display DAM (menu dam/common)
	QObject::connect(this->action_reset_display_dam, SIGNAL(triggered()), this, SLOT(reset_dam_outputtxt()));

	//User aborted the dam calculation; the dam-thread is stopped (menu hyd)
	QObject::connect(this->action_stop_dam_calc, SIGNAL(triggered()), this, SLOT(stop_damcalc_thread()));

	//Import a hydraulic result raster from a file to database for a DAM_HYD-project as a base scenario (menu dam/Import HYD-results/Base scenario)
	QObject::connect(this->action_import_hyd_res_base, SIGNAL(triggered()), this, SLOT(import_hyd_result_raster_base_sc()));
	//Add a hydraulic result raster from a file to database for a DAM_HYD-project as a base scenario (menu dam/Import HYD-results/Add scenario)
	QObject::connect(this->action_add_hyd_res, SIGNAL(triggered()), this, SLOT(add_hyd_result_raster_sc()));
	//Delete hydraulic resuts in the database for a DAM_project (menu dam/Delete Hyd-Results)
	QObject::connect(this->action_delete_hyd_results, SIGNAL(triggered()), this, SLOT(delete_hyd_results_dam()));

	//ecn
	//Import the economic damage function from a file to database (menu dam/ECoNomic/Import)
	QObject::connect(this->action_imp_ecn_dam_func, SIGNAL(triggered()), this, SLOT(import_ecn_dam_function()));
	//Add an economic raster from a files to database (menu dam/ECoNomic/Import)
	QObject::connect(this->action_add_ecn_dam_raster, SIGNAL(triggered()), this, SLOT(add_ecn_dam_rasters()));
	//Delete all economical data in the database to database (menu dam/ECoNomic/)
	QObject::connect(this->action_del_all_ecn, SIGNAL(triggered()), this, SLOT(delete_all_ecn_dam()));
	//Connect the econimical raster (menu dam/ECoNomic/)
	QObject::connect(this->action_connect_raster_ecn, SIGNAL(triggered()), this, SLOT(connect_ecn_raster()));

	//eco
	//Import the ecological biotope-types from a file to database (menu dam/ECOlogic/Import)
	QObject::connect(this->action_imp_eco_biotope_types, SIGNAL(triggered()), this, SLOT(import_eco_b_type()));
	//Add an ecologic raster of the biotope-types from files to database (menu dam/ECoNomic/Import)
	QObject::connect(this->action_add_eco_biotope_raster, SIGNAL(triggered()), this, SLOT(add_eco_b_type_rasters()));
	///Delete all ecological biotope-type data in the database to database (menu dam/ECOlogic/)
	QObject::connect(this->action_del_eco_btype, SIGNAL(triggered()), this, SLOT(delete_all_eco_b_type_dam()));

	//Import the ecological soil-type cost function from a file to database (menu dam/ECOlogic/Import)
	QObject::connect(this->action_imp_eco_soil_cost_functions, SIGNAL(triggered()), this, SLOT(import_eco_soil_cost()));
	//Import the ecological soil-type erosion function from a file to database (menu dam/ECOlogic/Import)
	QObject::connect(this->action_imp_eco_soil_erosion_functions, SIGNAL(triggered()), this, SLOT(import_eco_soil_ero()));
	//Import the ecological soil-type erosion function from a file to database (menu dam/ECOlogic/Import)
	QObject::connect(this->action_add_eco_soil_raster, SIGNAL(triggered()), this, SLOT(add_eco_soil_ero_rasters()));
	//Delete all ecological soil-erosion data in the database to database (menu dam/ECOlogic/)
	QObject::connect(this->action_del_eco_soil, SIGNAL(triggered()), this, SLOT(delete_all_eco_soil_dam()));

	//Connect the ecological raster (menu dam/ECOlogic/)
	QObject::connect(this->action_connect_raster_eco, SIGNAL(triggered()), this, SLOT(connect_eco_raster()));

	//pop
	//Import the people2risk damage function from a file to database (menu dam/PeOPle/Import)
	QObject::connect(this->action_imp_pop_dam_func, SIGNAL(triggered()), this, SLOT(import_pop_dam_function()));
	//Add an people2risk raster from files to database (menu dam/PeOPle/Import)
	QObject::connect(this->action_add_pop_raster, SIGNAL(triggered()), this, SLOT(add_pop_rasters()));
	//Delete all people2risk data in the database to database (menu dam/PeOPle/)
	QObject::connect(this->action_del_all_pop, SIGNAL(triggered()), this, SLOT(delete_all_pop_dam()));

	//Connect the people2risk raster (menu dam/PeOPle/)
	QObject::connect(this->action_connect_raster_pop, SIGNAL(triggered()), this, SLOT(connect_pop_raster()));

	//pys
	//Import the people2risk category with the criteria score function from a file to database (menu dam/PsYchoSocial/Import)
	QObject::connect(this->action_imp_pys_cat, SIGNAL(triggered()), this, SLOT(import_pys_category()));
	//Import the people2risk density function from a file to database (menu dam/PsYchoSocial/Import)
	QObject::connect(this->action_imp_pys_density, SIGNAL(triggered()), this, SLOT(import_pys_density_function()));
	//Add an psycho-social raster from files to database (menu dam/PsYchoSocial/Import)
	QObject::connect(this->action_add_pys_raster, SIGNAL(triggered()), this, SLOT(add_pys_rasters()));
	//Delete all psycho-social data in the database to database (menu dam/PsYchoSocial/)
	QObject::connect(this->action_del_all_pys, SIGNAL(triggered()), this, SLOT(delete_all_pys_dam()));
	//Connect the psycho-social raster (menu dam/PsYchoSocial/)
	QObject::connect(this->action_connect_raster_pys, SIGNAL(triggered()), this, SLOT(connect_pys_raster()));

	//sc
	//Import the simple counting points from a file to database (menu dam/SimpleCounting/Import)
	QObject::connect(this->action_imp_points, SIGNAL(triggered()), this, SLOT(import_sc_points()));
	//Import the simple counting subcategories from a file to database (menu dam/SimpleCounting/Import)
	QObject::connect(this->action_imp_subcategory, SIGNAL(triggered()), this, SLOT(import_sc_subcategories()));
	//Delete all simple counting data in the database (menu dam/SimpleCounting/)
	QObject::connect(this->action_sc_delete, SIGNAL(triggered()), this, SLOT(delete_all_sc_dam()));
	//Connect the simple counting points (menu dam/SimpleCounting/)
	QObject::connect(this->action_sc_connect, SIGNAL(triggered()), this, SLOT(connect_sc_points()));

	//CI
	//Import the CI data to database (menu dam/Critical Infrastructure/Import)
	QObject::connect(this->action_CI_Import, SIGNAL(triggered()), this, SLOT(import_CI_data()));
	//Delete all CI data in the database (menu dam/Critical Infrastructure/)
	QObject::connect(this->action_CI_delete, SIGNAL(triggered()), this, SLOT(delete_all_CI_dam()));
	//Connect the CI data (menu dam/Critical Infrastructure/)
	QObject::connect(this->action_CI_connect, SIGNAL(triggered()), this, SLOT(connect_CI_data()));

	//system
	//check the damage system (menu dam/System/)
	QObject::connect(this->action_check_dam_system, SIGNAL(triggered()), this, SLOT(check_damage_system()));
	//Output the the statistic of the damage system (menu dam/System/)
	QObject::connect(this->action_show_dam_statistic, SIGNAL(triggered()), this, SLOT(output_damage_system_statistic()));
	//Connect the damage raster among each other and connect it to the hydraulic system (menu dam/System/)
	QObject::connect(this->action_dam_connect, SIGNAL(triggered()), this, SLOT(connect_damage_rasters()));
	//Calculate the damages for selected nobreak-scenarios (menu dam/System/)
	QObject::connect(this->action_dam_calculate, SIGNAL(triggered()), this, SLOT(calculate_damage_nobreak_sz()));
	//Calculate the instat damages for selected nobreak-scenarios (menu dam/System/)
	QObject::connect(this->action_dam_instat_calculate, SIGNAL(triggered()), this, SLOT(calculate_instat_damage_nobreak_sz()));
	
}
//Connect the of the modul dam
void Main_Wid::slot_connect_madm(void){
	//Check the database tables for the madm system (menu madm/common)
	QObject::connect(this->action_check_madm_tables, SIGNAL(triggered()), this, SLOT(check_madm_system_database_tables()));

	//Perform a madm-analysis via for a file input (menu madm/calculation)
	QObject::connect(this->action_madm_calc_file, SIGNAL(triggered()), this, SLOT(madm_analysis_file()));
	//Perform a madm-analysis via for a file input (menu madm/calculation)
	QObject::connect(this->action_madm_calc_database, SIGNAL(triggered()), this, SLOT(madm_analysis_database()));

	//Import weigthing set via a file to database for the madm-analysis (menu madm/import)
	QObject::connect(this->action_import_weighting_sets, SIGNAL(triggered()), this, SLOT(import_weighting_set_file2database()));

	//Delete selected weighting sets from database table (menu madm/)
	QObject::connect(this->action_delete_results_madm, SIGNAL(triggered()), this, SLOT(delete_selcted_weighting_set_database()));

	//set a new output flag (menu madm/common)
	QObject::connect(this->action_set_output_flag_madm, SIGNAL(triggered()), this, SLOT(set_madm_outputflag()));
	//set a new output logfile (menu madm/common)
	QObject::connect(this->action_set_logfile_madm, SIGNAL(triggered()), this, SLOT(set_madm_outputlogfile()));
	//Reset the display MADM (menu madm/common)
	QObject::connect(this->action_reset_display_madm, SIGNAL(triggered()), this, SLOT(reset_madm_outputtxt()));

	//User aborted the dam calculation; the madm-thread is stopped (menu madm)
	QObject::connect(this->action_stop_madm_calc, SIGNAL(triggered()), this, SLOT(stop_madmcalc_thread()));
}
//Connect the of the modul risk
void Main_Wid::slot_connect_risk(void){
	//Check the database tables for the risk system (menu risk/common)
	QObject::connect(this->action_check_risk_tables, SIGNAL(triggered()), this, SLOT(check_risk_system_database_tables()));

	//set a new output flag (menu risk/common)
	QObject::connect(this->action_set_output_flag_risk, SIGNAL(triggered()), this, SLOT(set_risk_outputflag()));
	//set a new output logfile (menu risk/common)
	QObject::connect(this->action_set_logfile_risk, SIGNAL(triggered()), this, SLOT(set_risk_outputlogfile()));
	//Reset the display DAM (menu risk/common)
	QObject::connect(this->action_reset_display_risk, SIGNAL(triggered()), this, SLOT(reset_risk_outputtxt()));

	//Set hydraulic system properties (menu risk/common)
	QObject::connect(this->action_properties_hyd_2, SIGNAL(triggered()), this, SLOT(set_hydraulic_system_properties()));

	//User aborted the dam calculation; the risk-thread is stopped (menu risk)
	QObject::connect(this->action_stop_risk_calc, SIGNAL(triggered()), this, SLOT(stop_riskcalc_thread()));

	//Calculate nobreak risk  (menu risk/calculation)
	QObject::connect(this->action_calc_nobreak_risk, SIGNAL(triggered()), this, SLOT(calculate_nobreak_risk()));
	//Calculate scenario based risk (menu risk/calculation)
	QObject::connect(this->action_calc_scenario_risk, SIGNAL(triggered()), this, SLOT(calculate_scenario_based_risk()));

	//Generate the risk states for the catchment area risk approach (menu risk/calculation/catchment risk)
	QObject::connect(this->action_risk_state_generation, SIGNAL(triggered()), this, SLOT(generate_catchment_risk_states()));

	//Calculate catchment area risk for nobreak state (detailed risk result) (menu risk/calculation/catchment risk)
	QObject::connect(this->action_calc_nobreak_state, SIGNAL(triggered()), this, SLOT(calculate_catchment_risk_nobreak()));
	//Calculate catchment area risk for maximum one break states (detailed risk result) (menu risk/calculation/catchment risk)
	QObject::connect(this->action_calc_one_break_state, SIGNAL(triggered()), this, SLOT(calculate_catchment_risk_one_break()));
	//Calculate catchment area risk user defind scenarios (menu risk/calculation/catchment risk)
	QObject::connect(this->action_calc_user_defined, SIGNAL(triggered()), this, SLOT(calculate_catchment_risk_user_defined()));
	//Calculate catchment area risk automatically (menu risk/calculation/catchment risk)
	QObject::connect(this->action_calc_automatic, SIGNAL(triggered()), this, SLOT(calculate_catchment_risk_auto()));
	//Cumulate the generate catchment area risk states to one risk result (menu risk/calculation/catchment risk)
	QObject::connect(this->action_risk_cumulation, SIGNAL(triggered()), this, SLOT(cumulate_catchment_risk()));

	//Calculate steps of the risk analysis in serail (menu risk/catchment risk)
	QObject::connect(this->action_calc_all, SIGNAL(triggered()), this, SLOT(calc_risk_analysis_steps_serial()));

	//Calculate and output the statistic of the catchment risk approach (menu risk/)
	QObject::connect(this->action_output_statistic_catchment_risk, SIGNAL(triggered()), this, SLOT(calc_output_catchment_risk_statistic()));

	//Generate the risk states for the catchment area risk approach (menu risk/calculation/)
	//TODO_CI select all available scenarios
	QObject::connect(this->action_CI_DAM_instat_calculation, SIGNAL(triggered()), this, SLOT(calculate_instat_damage_nobreak_sz()));

	//Perform a test for the random generator (menu fpl/calculation)
	QObject::connect(this->action_test_random_generator_1, SIGNAL(triggered()), this, SLOT(perform_test_random()));

	//Stop by next hydraulic calculation (menu risk/calculation/catchment risk)
	QObject::connect(this->action_stop_next_HYD_calc, SIGNAL(triggered()), this, SLOT(stop_by_next_hyd_calc()));

	//Output the statistics of already generated detailed risk results to display/console (menu risk/)
	QObject::connect(this->action_output_statistics_risk, SIGNAL(triggered()), this, SLOT(output_statistics_detailed_risk_results()));

	//Delete the risk data in database table (menu risk/delete results/ restore)
	QObject::connect(this->action_delete_results_risk, SIGNAL(triggered()), this, SLOT(delete_risk_data_database()));
}
//Connect the of the modul alt
void Main_Wid::slot_connect_alt(void){
	//Set a new measure state (menu ALT/new area-/measure state)
	QObject::connect(this->action_new_measure_state, SIGNAL(triggered()), this, SLOT(set_new_measure_state()));

	//User aborted the alternative implmentation thread (menu ALT/new area-/measure state)
	QObject::connect(this->action_stop_measure_implementation, SIGNAL(triggered()), this, SLOT(stop_alt_implement_thread()));

	//Set a new area state (menu ALT/new area-/measure state)
	QObject::connect(this->action_new_area_state, SIGNAL(triggered()), this, SLOT(set_new_area_state()));

	//Output the areastate and measure number (system id) to display ALT (menu ALT/)
	QObject::connect(this->action_show_area_measure_state, SIGNAL(triggered()), this, SLOT(show_systemid()));

	//Set a new measure state (menu ALT/edit area-/measure state)
	QObject::connect(this->action_edit_measure_state, SIGNAL(triggered()), this, SLOT(edit_measure_state()));
	//Set a new area state (menu ALT/edit area-/measure state)
	QObject::connect(this->action_edit_area_state, SIGNAL(triggered()), this, SLOT(edit_area_state()));
	//Switch the measure state to a another one, which is already calculated
	QObject::connect(this->action_switch_measure_state, SIGNAL(triggered()), this, SLOT(switch_measure_state()));
	//Delete a choosen measure state
	QObject::connect(this->action_delete_measure_state, SIGNAL(triggered()), this, SLOT(delete_measure_state()));

	//Check the database tables for the alt system (menu alt/common)
	QObject::connect(this->action_check_alt_tables, SIGNAL(triggered()), this, SLOT(check_alt_system_database_tables()));
	//set a new output flag (menu alt/common)
	QObject::connect(this->action_set_output_flag_alt, SIGNAL(triggered()), this, SLOT(set_alt_outputflag()));
	//set a new output logfile (menu alt/common)
	QObject::connect(this->action_set_logfile_alt, SIGNAL(triggered()), this, SLOT(set_alt_outputlogfile()));
	//Reset the display ALT (menu alt/common)
	QObject::connect(this->action_reset_display_alt, SIGNAL(triggered()), this, SLOT(reset_alt_outputtxt()));
}
//Connect the of the modul cost
void Main_Wid::slot_connect_cost(void){
	//Set user-defined cost data (menu cost/ECoNomic/)
	QObject::connect(this->action_ecn_cost_user_defined, SIGNAL(triggered()), this, SLOT(set_user_defined_ecn_cost()));

	//Check the database tables for the cost system (menu cost/common)
	QObject::connect(this->action_check_cost_tables, SIGNAL(triggered()), this, SLOT(check_cost_system_database_tables()));
	//set a new output flag (menu cost/common)
	QObject::connect(this->action_set_output_flag_cost, SIGNAL(triggered()), this, SLOT(set_cost_outputflag()));
	//set a new output logfile (menu cost/common)
	QObject::connect(this->action_set_logfile_cost, SIGNAL(triggered()), this, SLOT(set_cost_outputlogfile()));
	//Reset the display COST (menu cost/common)
	QObject::connect(this->action_reset_display_cost, SIGNAL(triggered()), this, SLOT(reset_cost_outputtxt()));
}
//Connect the output to GUI for all modules
void Main_Wid::output_connect(void){
	//is important to get the output from one thread to the texteditr
	qRegisterMetaType<QTextCursor>("QTextCursor");

	this->output_connect_sys();
	this->output_connect_exc();
	this->output_connect_fpl();
	this->output_connect_hyd();
	this->output_connect_madm();
	this->output_connect_dam();
	this->output_connect_risk();
	this->output_connect_alt();
	this->output_connect_cost();

	//set the current tab widget to system output
	this->tabWidget_outtext->setCurrentWidget(this->tab_systemtxt);
}
//Connect the output to GUI for module sys
void Main_Wid::output_connect_sys(void){
	//for system output
	Sys_Common_Output::new_output_system(this->textEdit_systemtxt);
	Sys_Common_Output::output_system->set_ptr_parent_display_output(this->tabWidget_outtext);
	this->textEdit_systemtxt->connect_clear_act(this,_sys_system_modules::SYS_SYS);
	this->textEdit_systemtxt->connect_setDetailed_act(this,_sys_system_modules::SYS_SYS);
	this->textEdit_systemtxt->my_searcher.set_window_title(_sys_system_modules::SYS_SYS);

	//connect the slots
	QObject::connect(Sys_Common_Output::output_system, SIGNAL(gui_text(const QString )), this, SLOT(txt_to_system_out(const QString )));
}
//Connect the output to GUI for module exc (exception)
void Main_Wid::output_connect_exc(void){
	//for exception output
	Sys_Common_Output::new_output_excep(this->textEdit_exceptiontxt);
	Sys_Common_Output::output_excep->set_ptr_parent_display_output(this->tabWidget_outtext);
	this->textEdit_exceptiontxt->connect_clear_act(this,_sys_system_modules::EXC_SYS);
	this->textEdit_exceptiontxt->my_searcher.set_window_title(_sys_system_modules::EXC_SYS);

	//connect the slots
	QObject::connect(Sys_Common_Output::output_excep, SIGNAL(gui_text(const QString )), this, SLOT(txt_to_excep_out(const QString )));
}
//Connect the output to GUI for module fpl
void Main_Wid::output_connect_fpl(void){
	//for fpl output
	Sys_Common_Output::new_output_fpl(this->textEdit_fpltxt);
	Sys_Common_Output::output_fpl->set_ptr_parent_display_output(this->tabWidget_outtext);
	this->textEdit_fpltxt->connect_clear_act(this,_sys_system_modules::FPL_SYS);
	this->textEdit_fpltxt->connect_setDetailed_act(this,_sys_system_modules::FPL_SYS);
	this->textEdit_fpltxt->my_searcher.set_window_title(_sys_system_modules::FPL_SYS);
	//connect the slots
	QObject::connect(Sys_Common_Output::output_fpl, SIGNAL(gui_text(const QString )), this, SLOT(txt_to_fpl_out(const QString )));
}
//Connect the output to GUI for module hyd
void Main_Wid::output_connect_hyd(void){
		//for fpl output
	Sys_Common_Output::new_output_hyd(this->textEdit_hydtxt);
	Sys_Common_Output::output_hyd->set_ptr_parent_display_output(this->tabWidget_outtext);

	this->textEdit_hydtxt->connect_clear_act(this,_sys_system_modules::HYD_SYS);
	this->textEdit_hydtxt->connect_setDetailed_act(this,_sys_system_modules::HYD_SYS);
	this->textEdit_hydtxt->my_searcher.set_window_title(_sys_system_modules::HYD_SYS);

	//connect the slots
	QObject::connect(Sys_Common_Output::output_hyd, SIGNAL(gui_text(const QString )), this, SLOT(txt_to_hyd_out(const QString )));
}
//Connect the output to GUI for module madm
void Main_Wid::output_connect_madm(void){
	//for madm output
	Sys_Common_Output::new_output_madm(this->textEdit_madmtxt);
	Sys_Common_Output::output_madm->set_ptr_parent_display_output(this->tabWidget_outtext);
	this->textEdit_madmtxt->my_searcher.set_window_title(_sys_system_modules::MADM_SYS);

	this->textEdit_madmtxt->connect_clear_act(this,_sys_system_modules::MADM_SYS);
	this->textEdit_madmtxt->connect_setDetailed_act(this,_sys_system_modules::MADM_SYS);

	//connect the slots
	QObject::connect(Sys_Common_Output::output_madm, SIGNAL(gui_text(const QString )), this, SLOT(txt_to_madm_out(const QString )));
}
//Connect the output to GUI for module dam
void Main_Wid::output_connect_dam(void){
	//for dam output
	Sys_Common_Output::new_output_dam(this->textEdit_damtxt);
	Sys_Common_Output::output_dam->set_ptr_parent_display_output(this->tabWidget_outtext);
	this->textEdit_damtxt->connect_clear_act(this,_sys_system_modules::DAM_SYS);
	this->textEdit_damtxt->connect_setDetailed_act(this,_sys_system_modules::DAM_SYS);
	this->textEdit_damtxt->my_searcher.set_window_title(_sys_system_modules::DAM_SYS);

	//connect the slots
	QObject::connect(Sys_Common_Output::output_dam, SIGNAL(gui_text(const QString )), this, SLOT(txt_to_dam_out(const QString )));
}
//Connect the output to GUI for module risk
void Main_Wid::output_connect_risk(void){
	//for risk output
	Sys_Common_Output::new_output_risk(this->textEdit_risktxt);
	Sys_Common_Output::output_risk->set_ptr_parent_display_output(this->tabWidget_outtext);

	this->textEdit_risktxt->connect_clear_act(this,_sys_system_modules::RISK_SYS);
	this->textEdit_risktxt->connect_setDetailed_act(this,_sys_system_modules::RISK_SYS);
	this->textEdit_risktxt->my_searcher.set_window_title(_sys_system_modules::RISK_SYS);

	//connect the slots
	QObject::connect(Sys_Common_Output::output_risk, SIGNAL(gui_text(const QString )), this, SLOT(txt_to_risk_out(const QString )));
}
//Connect the output to GUI for module alt
void Main_Wid::output_connect_alt(void){
	//for alt output
	Sys_Common_Output::new_output_alt(this->textEdit_alttxt);
	Sys_Common_Output::output_alt->set_ptr_parent_display_output(this->tabWidget_outtext);

	this->textEdit_alttxt->connect_clear_act(this,_sys_system_modules::ALT_SYS);
	this->textEdit_alttxt->connect_setDetailed_act(this,_sys_system_modules::ALT_SYS);
	this->textEdit_alttxt->my_searcher.set_window_title(_sys_system_modules::ALT_SYS);

	//connect the slots
	QObject::connect(Sys_Common_Output::output_alt, SIGNAL(gui_text(const QString )), this, SLOT(txt_to_alt_out(const QString )));
}
//Connect the output to GUI for module cost
void Main_Wid::output_connect_cost(void){
	//for cost output
	Sys_Common_Output::new_output_cost(this->textEdit_costtxt);
	Sys_Common_Output::output_cost->set_ptr_parent_display_output(this->tabWidget_outtext);

	this->textEdit_costtxt->connect_clear_act(this,_sys_system_modules::COST_SYS);
	this->textEdit_costtxt->connect_setDetailed_act(this,_sys_system_modules::COST_SYS);
	this->textEdit_costtxt->my_searcher.set_window_title(_sys_system_modules::COST_SYS);

	//connect the slots
	QObject::connect(Sys_Common_Output::output_cost, SIGNAL(gui_text(const QString )), this, SLOT(txt_to_cost_out(const QString )));
}
//Connect the output to GUI for module hydrol
void Main_Wid::output_connect_hydrol(void) {
	//for hydrol output
	Sys_Common_Output::new_output_hydrol(this->textEdit_costtxt);
	Sys_Common_Output::output_hydrol->set_ptr_parent_display_output(this->tabWidget_outtext);

	this->textEdit_costtxt->connect_clear_act(this, _sys_system_modules::HYDROL_SYS);
	this->textEdit_costtxt->connect_setDetailed_act(this, _sys_system_modules::HYDROL_SYS);
	this->textEdit_costtxt->my_searcher.set_window_title(_sys_system_modules::HYDROL_SYS);

	//connect the slots
	QObject::connect(Sys_Common_Output::output_hydrol, SIGNAL(gui_text(const QString)), this, SLOT(txt_to_hydrol_out(const QString)));

}
//Close the windows of the text search
void Main_Wid::close_text_searcher(void){
	this->textEdit_systemtxt->my_searcher.close();
	this->textEdit_risktxt->my_searcher.close();
	this->textEdit_damtxt->my_searcher.close();
	this->textEdit_madmtxt->my_searcher.close();
	this->textEdit_fpltxt->my_searcher.close();
	this->textEdit_hydtxt->my_searcher.close();
	this->textEdit_exceptiontxt->my_searcher.close();
}
//Delete and disconnect the output
void Main_Wid::delete_output_classes(void){
	//static function to close the output classes
	Sys_Common_Output::delete_output_system();
	Sys_Common_Output::delete_output_fpl();
	Sys_Common_Output::delete_output_excep();
	Sys_Common_Output::delete_output_hyd();
	Sys_Common_Output::delete_output_dam();
	Sys_Common_Output::delete_output_madm();
	Sys_Common_Output::delete_output_risk();
	Sys_Common_Output::delete_output_cost();
	Sys_Common_Output::delete_output_alt();
	Sys_Common_Output::delete_output_hydrol();
	//..introduce further modules
}
//Delete the system tray context
void Main_Wid::delete_system_tray(void) {
	if (startDbAction != NULL)		{delete startDbAction;		startDbAction = NULL;}
	if (stopDbAction != NULL)		{delete stopDbAction;		stopDbAction = NULL;}
	if (exitAction != NULL)			{delete exitAction;			exitAction = NULL;}
	if (trayContextMenu != NULL)	{delete trayContextMenu;	trayContextMenu = NULL;}
	if (trayIcon != NULL)			{delete trayIcon;			trayIcon = NULL;}
}
//Allocate and connect the system tray icon
void Main_Wid::systemtray_connect(void) {

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(QIcon(":prom_icon"));  // Set the icon for the tray
	trayIcon->setToolTip("ProMaIDeS");  // Set a tooltip for the icon

	// Create a context menu for the tray icon
	trayContextMenu = new QMenu(this);
	startDbAction = new QAction("Start Database", this);
	stopDbAction = new QAction("Stop Database", this);
	startDbAction->setCheckable(true);
	stopDbAction->setCheckable(true);
	exitAction = new QAction("Exit", this);

	// Connect actions to slots or functions
	connect(startDbAction, &QAction::triggered, this, &Main_Wid::systemtray_startdb);
	connect(stopDbAction, &QAction::triggered, this, &Main_Wid::systemtray_stopdb);
	connect(exitAction, &QAction::triggered, this, &Main_Wid::terminate_threads_close_app);

	trayContextMenu->addAction(startDbAction);
	trayContextMenu->addAction(stopDbAction);
	trayContextMenu->addSeparator();
	trayContextMenu->addAction(exitAction);

	// Set the context menu for the tray icon
	trayIcon->setContextMenu(trayContextMenu);
	// Show the System Tray icon
	trayIcon->show();
}

//Start the postgresql database from the system tray context
void Main_Wid::systemtray_startdb(void) {
	const char* command = "C:/Progra~1/PostgreSQL/16/bin/pg_ctl.exe start -D C:/Progra~1/PostgreSQL/16/data";
	int status = system(command);

	if (status == 0) {
		startDbAction->setChecked(true);
		stopDbAction->setChecked(false);
		std::cout << "PostgreSQL started successfully." << std::endl;
	}
	else {
		std::cerr << "Failed to start PostgreSQL." << std::endl;
	}
}

//Stop the postgresql database from the system tray context
void Main_Wid::systemtray_stopdb(void) {
	const char* command = "C:/Progra~1/PostgreSQL/16/bin/pg_ctl.exe stop -D C:/Progra~1/PostgreSQL/16/data";
	int status = system(command);
	
	if (status == 0) {
		startDbAction->setChecked(false);
		stopDbAction->setChecked(true);
		std::cout << "PostgreSQL stopped successfully." << std::endl;
	}
	else {
		std::cerr << "Failed to stop PostgreSQL." << std::endl;
	}

}

//Allocate and connect the status bar widget
void Main_Wid::statusbar_connect(void){
	this->status_wid=new Sys_Status_Bar_Wid();
	//this->status_wid->setParent(this->statusBar());
	//set it to the status bar
	this->statusBar()->addPermanentWidget(this->status_wid,0);
	this->statusBar()->showMessage("Ready", 0);
	//connect the slots: error counter
	QObject::connect(Sys_Exception_Sender::self(), SIGNAL(send_warning_number(const int, const QString )), this, SLOT(get_warning_number(const int, const QString )));
	//warning counter
	QObject::connect(Sys_Exception_Sender::self(), SIGNAL(send_error_number(const int, const QString )), this, SLOT(get_error_number(const int, const QString )));
	//memory count
	QObject::connect(Sys_Memory_Count::self(), SIGNAL(send_memorytxt(const QString, const QString )), this, SLOT(get_memorytxt(const QString, const QString )));

	//text output
	QObject::connect(this, SIGNAL(send_txt2statusbar(QString , int )), this, SLOT(set_text2statusbar(QString , int )));
	QObject::connect(&this->project_manager, SIGNAL(send_txt2statusbar(QString , int)), this, SLOT(set_text2statusbar(QString , int )));
	QObject::connect(this->treeWidget_data, SIGNAL(send_txt2statusbar(QString , int )), this, SLOT(set_text2statusbar(QString , int )), Qt::DirectConnection);
	QObject::connect(this->treeWidget_data, SIGNAL(send_enabled_main(bool )), this, SLOT(set_enabled(bool )), Qt::DirectConnection);

	QObject::connect(this->treeview_file, SIGNAL(send_txt2statusbar(QString , int )), this, SLOT(set_text2statusbar(QString , int )));
	QObject::connect(this->browser_database->get_ptr_tree_wid(), SIGNAL(send_txt2statusbar(QString , int )), this, SLOT(set_text2statusbar(QString , int )));
	QObject::connect(this->statusBar(), SIGNAL(messageChanged(QString )), this, SLOT(set_ready2statusbar(QString)));

	//set it to the memory counter class
	Sys_Memory_Count::self()->set_output_widget(this->status_wid);
}
//Enable/disable menu and show/hide the data tabs in the dataview corresponding the project type, when a project is open
void Main_Wid::enable_menu_project_open(const bool new_project){
	this->actionTemp_per_database->setVisible(false);

	this->system_state.set_systemid(this->project_manager.get_stored_system_state());
	this->setEnabled(false);
	this->menu_Database->setEnabled(true);
	this->action_show_db_tool->setEnabled(true);
	this->menu_hyd_import->setEnabled(true);
	this->action_hyd_check_database->setEnabled(true);
	this->action_hydcalc_per_db->setEnabled(true);
	this->action_delete_boundary_scenario->setEnabled(true);
	this->action_check_hyd_tables->setEnabled(true);
	this->action_close_all_tables->setEnabled(true);
	this->menu_hyd_import->setEnabled(true);
	this->action_delete_boundary_scenario->setEnabled(true);
	this->menu_fpl2hyd->setEnabled(false);
	this->menu_import_HYD_results->setEnabled(false);
	this->action_delete_hyd_results->setEnabled(false);
	this->action_madm_calc_database->setEnabled(false);
	this->menu_edit_area_measure_state->setEnabled(false);

	//project actions
	this->action_show_project->setEnabled(true);
	this->action_set_working_dir->setEnabled(true);
	this->action_close_project->setEnabled(true);
	this->action_edit_project->setEnabled(true);
	this->action_save_project->setEnabled(true);
	this->action_delete_project->setEnabled(true);
	this->action_copy_project->setEnabled(true);
	this->menu_Database->setEnabled(true);
	this->action_show_db_tool->setEnabled(true);
	this->menu_sys_common->setEnabled(true);

	//data-tabs of the txt output icons exception
	QIcon my_icon22;
	my_icon22.addFile(":sys_icon");
	this->tabWidget_outtext->addTab(this->tab_systemtxt, my_icon22, sys_label::str_sys.c_str());

	QIcon my_icon4_excep;
	my_icon4_excep.addFile(":excep_icon" );
	this->tabWidget_outtext->addTab(this->tab_exceptiontxt, my_icon4_excep, sys_label::str_excep.c_str());

	if(this->project_manager.get_project_type()==_sys_project_type::proj_all){
		this->menu_FPL_system->setEnabled(true);
		this->menu_HYD_system->setEnabled(true);
		this->menu_DAM_system->setEnabled(true);
		this->menu_RISK_system->setEnabled(true);
		this->menu_MADM_system->setEnabled(true);
		this->menu_risk_calc->setEnabled(true);
		if(new_project==false){
			this->check_enabling_risk_check_box(true);
		}
		else{
			this->status_wid->get_ptr_risk_state_check_box()->setEnabled(true);
		}
		this->menu_ALT_system->setEnabled(true);
		if(Sys_Project::get_project_type()==_sys_project_type::proj_all && this->system_database!=NULL && this->system_database->check_con_status()==true && this->table_create_flag==false){
			QStringList list;

            Alt_System::load_stringlist_current_alt(this->system_database->get_database(),this->system_state.get_ptr_sys_system_id()->area_state,&list);
			Alt_System::close_table_measure();
			if(list.count()>0 && this->risk_flags.risk_state_flag==true){
				this->action_madm_calc_database->setEnabled(true);
			}
		}
		if(this->system_state.get_sys_system_id().measure_nr>0){
			this->menu_COST_system->setEnabled(true);
			this->menu_cost_ecn->setEnabled(true);
			this->menu_edit_area_measure_state->setEnabled(true);
			//hyd
			this->menu_hyd_import->setEnabled(false);
			this->action_delete_boundary_scenario->setEnabled(false);
			//dam
			this->menu_dam_ecn->setEnabled(false);
			this->menu_dam_eco->setEnabled(false);
			this->menu_dam_pys->setEnabled(false);
			this->menu_dam_pop->setEnabled(false);
			this->menu_dam_sc->setEnabled(false);
			this->menu_dam_CI->setEnabled(false);
			this->action_dam_connect->setEnabled(false);
			//fpl
			this->action_fpl_import_file->setEnabled(false);
			this->action_delete_fpl_section->setEnabled(false);
		}

		this->menu_fpl2hyd->setEnabled(true);

		//data-tabs of the data view
		QIcon my_icon;
		my_icon.addFile(":fpl_icon");
		this->tabWidget_data_output->addTab(this->tab_fpl_data_view, my_icon, sys_label::str_fpl.c_str());
		QIcon my_icon1;
		my_icon1.addFile(":hyd_icon");
		this->tabWidget_data_output->addTab(this->tab_hyd_data_view, my_icon1,sys_label::str_hyd.c_str());
		QIcon my_icon2;
		my_icon2.addFile(":dam_icon");
		this->tabWidget_data_output->addTab(this->tab_dam_data_view, my_icon2,sys_label::str_dam.c_str());
		QIcon my_icon3;
		my_icon3.addFile(":risk_icon");
		this->tabWidget_data_output->addTab(this->tab_risk_data_view, my_icon3,sys_label::str_risk.c_str());

		QIcon my_icon13;
		my_icon13.addFile(":cost_icon");
		this->tabWidget_data_output->addTab(this->tab_cost_data_view, my_icon13, sys_label::str_cost.c_str());
		QIcon my_icon12;
		my_icon12.addFile(":madm_icon");
		this->tabWidget_data_output->addTab(this->tab_madm_data_view, my_icon12, sys_label::str_madm.c_str());
		QIcon my_icon14;
		my_icon14.addFile(":alt_icon");
		this->tabWidget_data_output->addTab(this->tab_alt_data_view, my_icon14, sys_label::str_alt.c_str());
		

		//data-tabs of the txt output
		QIcon my_icon4;
		my_icon4.addFile(":fpl_icon" );
		this->tabWidget_outtext->addTab(this->tab_fpltxt, my_icon4,sys_label::str_fpl.c_str());
		QIcon my_icon5;
		my_icon5.addFile(":hyd_icon");
		this->tabWidget_outtext->addTab(this->tab_hydtxt, my_icon5, sys_label::str_hyd.c_str());
		QIcon my_icon6;
		my_icon6.addFile(":dam_icon");
		this->tabWidget_outtext->addTab(this->tab_damtxt, my_icon6,sys_label::str_dam.c_str());
		QIcon my_icon7;
		my_icon7.addFile(":risk_icon");
		this->tabWidget_outtext->addTab(this->tab_risktxt, my_icon7,sys_label::str_risk.c_str());
		QIcon my_icon9;
		my_icon9.addFile(":cost_icon");
		this->tabWidget_outtext->addTab(this->tab_costtxt, my_icon9, sys_label::str_cost.c_str());
		QIcon my_icon10;
		my_icon10.addFile(":madm_icon");
		this->tabWidget_outtext->addTab(this->tab_madmtxt, my_icon10, sys_label::str_madm.c_str());
		QIcon my_icon8;
		my_icon8.addFile(":alt_icon");
		this->tabWidget_outtext->addTab(this->tab_alttxt, my_icon8, sys_label::str_alt.c_str());


		//set pointer to data-tree of the tab data widgets
		this->treeWidget_data->set_ptr_data_tab_hyd(this->tab_hyd_data_view);
		this->treeWidget_data->set_ptr_data_tab_dam(this->tab_dam_data_view);
		this->treeWidget_data->set_ptr_data_tab_fpl(this->tab_fpl_data_view);
		this->treeWidget_data->set_ptr_data_tab_madm(this->tab_madm_data_view);
		this->treeWidget_data->set_ptr_data_tab_risk(this->tab_risk_data_view);
		this->treeWidget_data->set_ptr_data_tab_alt(this->tab_alt_data_view);
		this->treeWidget_data->set_ptr_data_tab_cost(this->tab_cost_data_view);
	}
	else if(this->project_manager.get_project_type()==_sys_project_type::proj_hyd || this->project_manager.get_project_type() == _sys_project_type::proj_hyd_temp){
		this->menu_FPL_system->setEnabled(false);
		this->menu_HYD_system->setEnabled(true);
		this->menu_DAM_system->setEnabled(false);
		this->menu_RISK_system->setEnabled(false);
		this->menu_ALT_system->setEnabled(false);
		this->menu_MADM_system->setEnabled(false);

		//data-tabs of the data view
		QIcon my_icon1;
		my_icon1.addFile(":hyd_icon");
		this->tabWidget_data_output->addTab(this->tab_hyd_data_view, my_icon1,sys_label::str_hyd.c_str());

		QIcon my_icon5;
		my_icon5.addFile(":hyd_icon");
		this->tabWidget_outtext->addTab(this->tab_hydtxt, my_icon5, sys_label::str_hyd.c_str());

		//set pointer to data-tree of the tab data widgets
		this->treeWidget_data->set_ptr_data_tab_hyd(this->tab_hyd_data_view);

		if (this->project_manager.get_project_type() == _sys_project_type::proj_hyd_temp) {
			this->actionTemp_per_database->setVisible(true);
			//Set a hydraulic calculation per database (menu hyd/calculation/per database)
			QObject::connect(this->actionTemp_per_database, SIGNAL(triggered()), this, SLOT(set_hydtempcalc_per_db()));
			
		}
	}
	else if(this->project_manager.get_project_type()==_sys_project_type::proj_hyd_file ){
		this->menu_FPL_system->setEnabled(false);
		this->menu_HYD_system->setEnabled(true);
		this->menu_DAM_system->setEnabled(false);
		this->menu_RISK_system->setEnabled(false);
		this->menu_ALT_system->setEnabled(false);
		this->menu_MADM_system->setEnabled(false);

		this->action_copy_project->setEnabled(false);

		this->menu_Database->setEnabled(false);
		this->action_show_db_tool->setEnabled(false);
		this->menu_hyd_import->setEnabled(false);
		this->action_hyd_check_database->setEnabled(false);
		this->action_hydcalc_per_db->setEnabled(false);
		this->action_delete_boundary_scenario->setEnabled(false);
		this->action_check_hyd_tables->setEnabled(false);
		this->action_close_all_tables->setEnabled(false);
		this->menu_hyd_import->setEnabled(false);
		this->action_delete_boundary_scenario->setEnabled(false);

		//set the file view
		this->treeview_file->set_view();
		QIcon my_icon5;
		my_icon5.addFile(":hyd_icon");
		this->tabWidget_outtext->addTab(this->tab_hydtxt, my_icon5, sys_label::str_hyd.c_str());
	}
	else if(this->project_manager.get_project_type()==_sys_project_type::proj_risk){
		this->menu_FPL_system->setEnabled(true);
		this->menu_HYD_system->setEnabled(true);
		this->menu_DAM_system->setEnabled(true);
		this->menu_RISK_system->setEnabled(true);
		this->menu_risk_calc->setEnabled(true);
		if(new_project==false){
			this->check_enabling_risk_check_box(true);
		}
		else{
			this->status_wid->get_ptr_risk_state_check_box()->setEnabled(true);
		}
		this->menu_ALT_system->setEnabled(false);
		this->menu_MADM_system->setEnabled(false);
		this->menu_fpl2hyd->setEnabled(true);

		//data-tabs of the data view
		QIcon my_icon;
		my_icon.addFile(":fpl_icon");
		this->tabWidget_data_output->addTab(this->tab_fpl_data_view, my_icon, sys_label::str_fpl.c_str());
		QIcon my_icon1;
		my_icon1.addFile(":hyd_icon");
		this->tabWidget_data_output->addTab(this->tab_hyd_data_view, my_icon1,sys_label::str_hyd.c_str());
		QIcon my_icon2;
		my_icon2.addFile(":dam_icon");
		this->tabWidget_data_output->addTab(this->tab_dam_data_view, my_icon2,sys_label::str_dam.c_str());
		QIcon my_icon3;
		my_icon3.addFile(":risk_icon");
		this->tabWidget_data_output->addTab(this->tab_risk_data_view, my_icon3,sys_label::str_risk.c_str());

		//data-tabs of the txt output
		QIcon my_icon4;
		my_icon4.addFile(":fpl_icon" );
		this->tabWidget_outtext->addTab(this->tab_fpltxt, my_icon4,sys_label::str_fpl.c_str());
		QIcon my_icon5;
		my_icon5.addFile(":hyd_icon");
		this->tabWidget_outtext->addTab(this->tab_hydtxt, my_icon5, sys_label::str_hyd.c_str());
		QIcon my_icon6;
		my_icon6.addFile(":dam_icon");
		this->tabWidget_outtext->addTab(this->tab_damtxt, my_icon6,sys_label::str_dam.c_str());
		QIcon my_icon7;
		my_icon7.addFile(":risk_icon");
		this->tabWidget_outtext->addTab(this->tab_risktxt, my_icon7,sys_label::str_risk.c_str());

		//set pointer to data-tree of the tab data widgets
		this->treeWidget_data->set_ptr_data_tab_hyd(this->tab_hyd_data_view);
		this->treeWidget_data->set_ptr_data_tab_dam(this->tab_dam_data_view);
		this->treeWidget_data->set_ptr_data_tab_fpl(this->tab_fpl_data_view);
		this->treeWidget_data->set_ptr_data_tab_risk(this->tab_risk_data_view);
	}
	else if(this->project_manager.get_project_type()==_sys_project_type::proj_dam){
		this->menu_FPL_system->setEnabled(false);
		this->menu_HYD_system->setEnabled(false);
		this->menu_import_HYD_results->setEnabled(true);
		this->action_delete_hyd_results->setEnabled(true);
		this->menu_DAM_system->setEnabled(true);
		this->menu_RISK_system->setEnabled(false);
		this->menu_ALT_system->setEnabled(false);
		this->menu_MADM_system->setEnabled(false);

		//data-tabs of the data view
		QIcon my_icon1;
		my_icon1.addFile(":hyd_icon");
		this->tabWidget_data_output->addTab(this->tab_hyd_data_view, my_icon1,sys_label::str_hyd.c_str());
		QIcon my_icon2;
		my_icon2.addFile(":dam_icon");
		this->tabWidget_data_output->addTab(this->tab_dam_data_view, my_icon2,sys_label::str_dam.c_str());

		//data-tabs of the txt output
		QIcon my_icon5;
		my_icon5.addFile(":hyd_icon");
		this->tabWidget_outtext->addTab(this->tab_hydtxt, my_icon5, sys_label::str_hyd.c_str());
		QIcon my_icon6;
		my_icon6.addFile(":dam_icon");
		this->tabWidget_outtext->addTab(this->tab_damtxt, my_icon6,sys_label::str_dam.c_str());

		//set pointer to data-tree of the tab data widgets
		this->treeWidget_data->set_ptr_data_tab_hyd(this->tab_hyd_data_view);
		this->treeWidget_data->set_ptr_data_tab_dam(this->tab_dam_data_view);
	}
	else if(this->project_manager.get_project_type()==_sys_project_type::proj_dam_hyd){
		this->menu_FPL_system->setEnabled(false);
		this->menu_HYD_system->setEnabled(true);
		this->menu_DAM_system->setEnabled(true);
		this->menu_RISK_system->setEnabled(false);
		this->menu_ALT_system->setEnabled(false);
		this->menu_MADM_system->setEnabled(false);

		//data-tabs of the data view
		QIcon my_icon1;
		my_icon1.addFile(":hyd_icon");
		this->tabWidget_data_output->addTab(this->tab_hyd_data_view, my_icon1,sys_label::str_hyd.c_str());
		QIcon my_icon2;
		my_icon2.addFile(":dam_icon");
		this->tabWidget_data_output->addTab(this->tab_dam_data_view, my_icon2,sys_label::str_dam.c_str());

		//data-tabs of the txt output
		QIcon my_icon5;
		my_icon5.addFile(":hyd_icon");
		this->tabWidget_outtext->addTab(this->tab_hydtxt, my_icon5, sys_label::str_hyd.c_str());
		QIcon my_icon6;
		my_icon6.addFile(":dam_icon");
		this->tabWidget_outtext->addTab(this->tab_damtxt, my_icon6,sys_label::str_dam.c_str());

		//set pointer to data-tree of the tab data widgets
		this->treeWidget_data->set_ptr_data_tab_hyd(this->tab_hyd_data_view);
		this->treeWidget_data->set_ptr_data_tab_dam(this->tab_dam_data_view);
	}
	else if(this->project_manager.get_project_type()==_sys_project_type::proj_fpl){
		this->menu_FPL_system->setEnabled(true);
		this->menu_HYD_system->setEnabled(false);
		this->menu_DAM_system->setEnabled(false);
		this->menu_RISK_system->setEnabled(false);
		this->menu_ALT_system->setEnabled(false);
		this->menu_MADM_system->setEnabled(false);
		this->menu_fpl2hyd->setEnabled(false);

		//data-tabs of the data view
		QIcon my_icon;
		my_icon.addFile(":fpl_icon");
		this->tabWidget_data_output->addTab(this->tab_fpl_data_view, my_icon, sys_label::str_fpl.c_str());

		//data-tabs of the txt output
		QIcon my_icon4;
		my_icon4.addFile(":fpl_icon" );
		this->tabWidget_outtext->addTab(this->tab_fpltxt, my_icon4,sys_label::str_fpl.c_str());

		//set pointer to data-tree of the tab data widgets
		this->treeWidget_data->set_ptr_data_tab_fpl(this->tab_fpl_data_view);
	}
	this->setEnabled(true);
}
//Enable/disable menu and show/hide the data tabs in the dataview corresponding the project type, when a project is closed
void Main_Wid::enable_menu_project_closed(void){
	this->setEnabled(false);
	//close database tables
	Hyd_Hydraulic_System::close_hyd_database_tables();
	Dam_Damage_System::close_dam_database_tables();
	Fpl_Calculation::close_fpl_database_tables();
	Risk_System::close_risk_database_tables();
	Madm_System::close_madm_database_tables();
	Alt_System::close_alt_database_tables();
	Cost_System::close_cost_database_tables();

	_risk_state_flags buffer;
	buffer.reliability_dam_result=false;
	buffer.reliability_fpl_result=false;
	buffer.reliability_hyd_result=false;
	buffer.risk_state_flag=false;
	this->set_risk_state_flag(buffer);
	this->status_wid->get_ptr_risk_state_check_box()->setEnabled(false);

	_sys_system_id buffer_id;
	buffer_id.area_state=0;
	buffer_id.measure_nr=0;
	this->set_system_state(buffer_id);

	this->menu_FPL_system->setEnabled(false);
	this->menu_HYD_system->setEnabled(false);
	this->menu_DAM_system->setEnabled(false);
	this->menu_RISK_system->setEnabled(false);
	this->menu_ALT_system->setEnabled(false);
	this->menu_MADM_system->setEnabled(false);
	this->menu_COST_system->setEnabled(false);

	//project actions
	this->action_show_project->setEnabled(false);
	this->action_set_working_dir->setEnabled(false);
	this->action_close_project->setEnabled(false);
	this->action_edit_project->setEnabled(false);
	this->action_save_project->setEnabled(false);
	this->action_delete_project->setEnabled(false);
	this->action_copy_project->setEnabled(false);
	this->menu_Database->setEnabled(false);
	this->action_show_db_tool->setEnabled(false);
	this->menu_sys_common->setEnabled(false);

	int number_tabs=this->tabWidget_data_output->count();
	//remove all tabs in data view
	for(int i=number_tabs-1; i>=0; i--){
		this->tabWidget_data_output->removeTab(i);
	}
	//remove all tabs in txt-output view
	number_tabs=this->tabWidget_outtext->count();
	for(int i=number_tabs-1; i>=0; i--){
		this->tabWidget_outtext->removeTab(i);
	}
	//set for the system and exception
	this->tabWidget_outtext->addTab(this->tab_systemtxt, sys_label::str_sys.c_str());
	QIcon my_icon4_excep;
	my_icon4_excep.addFile(":excep_icon" );
	this->tabWidget_outtext->addTab(this->tab_exceptiontxt, my_icon4_excep, sys_label::str_excep.c_str());

	//set pointer to data-tree of the tab data widgets
	this->treeWidget_data->set_ptr_data_tab_hyd(NULL);
	this->treeWidget_data->set_ptr_data_tab_dam(NULL);
	this->treeWidget_data->set_ptr_data_tab_fpl(NULL);
	this->treeWidget_data->set_ptr_data_tab_madm(NULL);
	this->treeWidget_data->set_ptr_data_tab_risk(NULL);
	//file tree
	this->treeview_file->clear_view();
	this->setEnabled(true);
}
//Enable/disable menu
void Main_Wid::enable_menu(const bool flag) {
	this->menu_FPL_system->setEnabled(flag);
	this->menu_HYD_system->setEnabled(flag);
	this->menu_DAM_system->setEnabled(flag);
	this->menu_RISK_system->setEnabled(flag);
	this->menu_ALT_system->setEnabled(flag);
	this->menu_MADM_system->setEnabled(flag);
	this->menu_COST_system->setEnabled(flag);
	//toolbars
	this->menu_window->setEnabled(flag);
	this->db_toolbar->setEnabled(flag);
	this->proj_toolbar->setEnabled(flag);

	//widget
	//this->dockWidget_tree_view->setEnabled(flag);
	//this->tabWidget_data_view->setEnabled(flag);
	//project actions
	this->menu_project->setEnabled(flag);
	this->action_show_project->setEnabled(flag);
	this->action_set_working_dir->setEnabled(flag);
	this->action_close_project->setEnabled(flag);
	this->action_edit_project->setEnabled(flag);
	this->action_save_project->setEnabled(flag);
	this->action_delete_project->setEnabled(flag);
	this->action_copy_project->setEnabled(flag);
	this->menu_Database->setEnabled(flag);
	this->action_show_db_tool->setEnabled(flag);
	this->menu_sys_common->setEnabled(flag);
}
//alloc new database
void Main_Wid::alloc_newdatabase(void){
	if(this->system_database==NULL){
		this->system_database=new Data_Base;
	}
}
//Check if the close database action is enable
void Main_Wid::check_enable_db_close(void){
	if(this->system_database!=NULL && this->system_database->check_con_status()==true){
		if(this->hyd_calc!=NULL && this->hyd_calc->isRunning()==true){
			this->action_Close_Connection->setEnabled(false);
			return;
		}
		if(this->dam_calc!=NULL && this->dam_calc->isRunning()==true){
			this->action_Close_Connection->setEnabled(false);
			return;
		}
		if(this->fpl_calc!=NULL && this->fpl_calc->isRunning()==true){
			this->action_Close_Connection->setEnabled(false);
			return;
		}
		if(this->risk_calc!=NULL && this->risk_calc->isRunning()==true){
			this->action_Close_Connection->setEnabled(false);
			return;
		}
		this->action_Close_Connection->setEnabled(true);
	}
	else{
		this->action_Close_Connection->setEnabled(false);
	}
}
// check if the database is set to enable the menu
void Main_Wid::menu_enable_checkdb(void){
	if(this->system_database!=NULL && this->system_database->check_con_status()==true){
		//menu fpl
		this->action_check_fpl_tables->setEnabled(true);
		this->action_export_DETERM->setEnabled(true);
		this->action_export_MC->setEnabled(true);
		this->action_export_FRC->setEnabled(true);

		this->action_user_defined_frc->setEnabled(true);
		this->action_calc_frc->setEnabled(true);
		this->action_calc_mc->setEnabled(true);
		this->action_calc_determ->setEnabled(true);
		if(Sys_Project::get_project_type()==_sys_project_type::proj_risk || Sys_Project::get_project_type()==_sys_project_type::proj_all){
			this->menu_fpl2hyd->setEnabled(true);
		}
		this->action_check_fpl_db->setEnabled(true);
		if(this->system_state.get_sys_system_id().measure_nr==0){
			this->action_fpl_import_file->setEnabled(true);
			this->action_delete_fpl_section->setEnabled(true);
		}

		//menu sys
		this->action_Open_Connection->setEnabled(false);
		this->action_copy_project->setEnabled(true);
		//menu hyd
		if(this->system_state.get_sys_system_id().measure_nr==0){
			this->menu_hyd_import->setEnabled(true);
			this->action_delete_boundary_scenario->setEnabled(true);
		}
		this->action_check_hyd_tables->setEnabled(true);
		this->action_hydcalc_per_db->setEnabled(true);
		this->action_hyd_check_database->setEnabled(true);
		//menu dam
		if(this->system_state.get_sys_system_id().measure_nr==0){
			this->menu_dam_ecn->setEnabled(true);
			this->menu_ecn_imp->setEnabled(true);
			this->menu_dam_eco->setEnabled(true);
			this->menu_dam_pop->setEnabled(true);
			this->menu_dam_pys->setEnabled(true);
			this->menu_pys_imp->setEnabled(true);
			this->menu_dam_sc->setEnabled(true);
			this->menu_dam_CI->setEnabled(true);
		}
		this->menu_dam_system->setEnabled(true);
		this->action_check_dam_tables->setEnabled(true);
		//menu risk
		this->menu_risk_calc->setEnabled(true);
		this->action_check_risk_tables->setEnabled(true);
		this->check_enabling_risk_check_box(true);
		this->action_output_statistics_risk->setEnabled(true);
		this->action_output_statistic_catchment_risk->setEnabled(true);
		this->action_delete_results_risk->setEnabled(true);
		this->set_risk_state_flag(this->project_manager.get_stored_risk_state());
		this->action_check_risk_tables->setEnabled(true);

		//menu cost
		this->action_check_cost_tables->setEnabled(true);

		//menu madm
		this->action_check_madm_tables->setEnabled(true);
		this->action_import_weighting_sets->setEnabled(true);
		this->action_delete_results_madm->setEnabled(true);
		//menu alt
		this->action_check_alt_tables->setEnabled(true);
		this->menu_new_area_measure_state->setEnabled(this->risk_flags.risk_state_flag);
		this->action_switch_measure_state->setEnabled(true);
		this->action_delete_measure_state->setEnabled(true);

		if(Sys_Project::get_project_type()==_sys_project_type::proj_all && this->system_database!=NULL && this->system_database->check_con_status()==true && this->table_create_flag==false){
			QStringList list;

			Alt_System::load_stringlist_current_alt(this->system_database->get_database(),this->system_state.get_ptr_sys_system_id()->area_state,&list);
			Alt_System::close_table_measure();
			if(list.count()>0 && this->risk_flags.risk_state_flag==true){
				this->action_madm_calc_database->setEnabled(true);
			}
		}

		if(this->system_state.get_sys_system_id().measure_nr>0){
			this->action_ecn_cost_user_defined->setEnabled(true);
			this->menu_edit_area_measure_state->setEnabled(true);
		}

		//dataviews
		this->tab_data_database->setEnabled(true);
		this->tab_data_data->setEnabled(true);
		this->tab_tree_database->setEnabled(true);
		this->tab_tree_data->setEnabled(true);

		//set the connection to the sql-browser
		this->browser_database->set_db_connection(this->system_database->get_database());
		//set connection to the data view
		this->treeWidget_data->set_db_connection(this->system_database->get_database());

		//set the status bar
		this->status_wid->set_dbcon(true);
	}
	else{
		//menu fpl
		this->action_check_fpl_tables->setEnabled(false);
		this->action_export_DETERM->setEnabled(false);
		this->action_export_MC->setEnabled(false);
		this->action_export_FRC->setEnabled(false);
		this->action_user_defined_frc->setEnabled(false);
		this->action_calc_frc->setEnabled(false);
		this->action_calc_mc->setEnabled(false);
		this->action_calc_determ->setEnabled(false);
		this->menu_fpl2hyd->setEnabled(false);
		this->action_check_fpl_db->setEnabled(false);
		this->action_fpl_import_file->setEnabled(false);
		this->action_delete_fpl_section->setEnabled(false);
		//menu hyd
		this->menu_hyd_import->setEnabled(false);
		this->action_check_hyd_tables->setEnabled(false);
		this->action_hydcalc_per_db->setEnabled(false);
		this->action_hyd_check_database->setEnabled(false);
		this->action_delete_boundary_scenario->setEnabled(false);
		//menu dam
		this->menu_dam_ecn->setEnabled(false);
		this->menu_ecn_imp->setEnabled(false);
		this->menu_dam_eco->setEnabled(false);
		this->menu_dam_pop->setEnabled(false);
		this->menu_dam_pys->setEnabled(false);
		this->menu_pys_imp->setEnabled(false);
		this->menu_dam_sc->setEnabled(false);
		this->menu_dam_CI->setEnabled(false);
		this->menu_dam_system->setEnabled(false);
		this->action_check_dam_tables->setEnabled(false);
		//menu sys
		this->action_Open_Connection->setEnabled(true);
		this->action_copy_project->setEnabled(false);
		//menu risk
		this->menu_risk_calc->setEnabled(false);
		this->action_check_risk_tables->setEnabled(false);
		this->status_wid->get_ptr_risk_state_check_box()->setEnabled(false);
		this->action_output_statistics_risk->setEnabled(false);
		this->action_output_statistic_catchment_risk->setEnabled(false);
		this->action_delete_results_risk->setEnabled(false);
		this->action_check_risk_tables->setEnabled(false);

		//menu cost
		this->action_check_cost_tables->setEnabled(false);
		this->action_ecn_cost_user_defined->setEnabled(false);
		//menu madm
		this->action_check_madm_tables->setEnabled(false);
		this->action_import_weighting_sets->setEnabled(false);
		this->action_delete_results_madm->setEnabled(false);
		this->action_madm_calc_database->setEnabled(false);
		//menu alt
		this->action_check_alt_tables->setEnabled(false);
		this->menu_edit_area_measure_state->setEnabled(false);
		this->menu_new_area_measure_state->setEnabled(false);
		this->action_switch_measure_state->setEnabled(false);
		this->action_delete_measure_state->setEnabled(false);

		//dataviews
		this->tab_data_database->setEnabled(false);
		this->tab_data_data->setEnabled(false);
		this->tab_tree_database->setEnabled(false);
		this->tab_tree_data->setEnabled(false);

		//reset connection to the data view
		this->treeWidget_data->set_db_connection(NULL);

		//set the status bar
		this->status_wid->set_dbcon(false);

		//close database tables
		Hyd_Hydraulic_System::close_hyd_database_tables();
		Dam_Damage_System::close_dam_database_tables();
		Fpl_Calculation::close_fpl_database_tables();
	}
	this->check_enable_db_close();
}
///Check if the hydraulic thread is running and set the menus and actions
void Main_Wid::check_hyd_thread_is_running(void){
	if(this->hyd_calc!=NULL && this->hyd_calc->isRunning()==true){
		//hyd
		this->menu_hyd_import->setEnabled(false);
		this->menu_hyd_calc->setEnabled(false);
		this->menu_hyd_check->setEnabled(false);
		this->action_delete_boundary_scenario->setEnabled(false);
		this->action_check_hyd_tables->setEnabled(false);
		this->action_set_logfile_hyd->setEnabled(false);
		this->action_properties_hyd->setEnabled(false);
		this->menu_hyd_tools->setEnabled(false);
		//dam
		this->action_dam_connect->setEnabled(false);
		this->action_dam_calculate->setEnabled(false);
		this->action_connect_raster_ecn->setEnabled(false);
		this->action_connect_raster_pop->setEnabled(false);
		this->action_connect_raster_eco->setEnabled(false);
		this->action_connect_raster_pys->setEnabled(false);
		this->action_sc_connect->setEnabled(false);
		this->action_CI_connect->setEnabled(false);
		this->action_dam_instat_calculate->setEnabled(false);

		//fpl
		this->menu_fpl2hyd->setEnabled(false);

		//risk
		this->status_wid->get_ptr_risk_state_check_box()->setEnabled(false);

		//sys
		this->action_delete_project->setEnabled(false);
		this->action_Close_Connection->setEnabled(false);
		this->action_close_all_tables->setEnabled(false);
		this->action_Reset_Error_Warning->setEnabled(false);
		this->action_copy_project->setEnabled(false);
		this->action_edit_project->setEnabled(false);

		this->status_wid->set_hydthread(true);
		this->tabWidget_outtext->setCurrentWidget(this->tab_hydtxt);
		this->check_enable_db_close();
	}
	else{
		//hyd
		this->menu_hyd_calc->setEnabled(true);
		this->menu_hyd_check->setEnabled(true);
		this->menu_hyd_tools->setEnabled(true);

		if(this->system_state.get_sys_system_id().measure_nr==0){
			this->menu_hyd_import->setEnabled(true);
			this->action_delete_boundary_scenario->setEnabled(true);
		}
		this->action_check_hyd_tables->setEnabled(true);
		this->action_set_logfile_hyd->setEnabled(true);
		this->action_properties_hyd->setEnabled(true);
		Sys_Common_Output::output_hyd->reset_userprefix();
		//dam
		if(this->dam_thread_running()==false){
			if(this->system_state.get_sys_system_id().measure_nr==0){
				this->action_dam_connect->setEnabled(true);
			}
			this->action_dam_calculate->setEnabled(true);
			this->action_connect_raster_ecn->setEnabled(true);
			this->action_connect_raster_pop->setEnabled(true);
			this->action_connect_raster_eco->setEnabled(true);
			this->action_connect_raster_pys->setEnabled(true);
			this->action_sc_connect->setEnabled(true);
			this->action_CI_connect->setEnabled(true);
			this->action_dam_instat_calculate->setEnabled(true);
		}
		//fpl
		if(this->fpl_thread_running()==false ){
			this->menu_fpl2hyd->setEnabled(true);
		}

		//risk
		if(this->some_thread_running()==false){
			this->check_enabling_risk_check_box(true);

			//sys
			this->action_Close_Connection->setEnabled(true);
			this->action_delete_project->setEnabled(true);
			this->action_close_all_tables->setEnabled(true);
			this->check_enable_db_close();
			this->action_Reset_Error_Warning->setEnabled(true);
			this->action_copy_project->setEnabled(true);
			this->action_edit_project->setEnabled(true);
		}
		this->tabWidget_outtext->setCurrentWidget(this->tab_hydtxt);
		this->status_wid->set_hydthread(false);
	}
}
//Check if the hydraulic thread with file input is running and set the menus and actions
void Main_Wid::check_hyd_file_thread_is_running(void){
	if(this->hyd_calc!=NULL && this->hyd_calc->isRunning()==true){
		//hyd
		this->menu_hyd_calc->setEnabled(false);
		this->menu_hyd_check->setEnabled(false);
		this->action_set_logfile_hyd->setEnabled(false);
		this->status_wid->set_hydthread(true);
		this->tabWidget_outtext->setCurrentWidget(this->tab_hydtxt);
		this->menu_hyd_tools->setEnabled(false);

		this->action_delete_project->setEnabled(false);
		this->action_edit_project->setEnabled(false);
	}
	else{
		//hyd
		this->menu_hyd_calc->setEnabled(true);
		this->menu_hyd_check->setEnabled(true);
		this->action_set_logfile_hyd->setEnabled(true);
		this->menu_hyd_tools->setEnabled(true);
		Sys_Common_Output::output_hyd->reset_userprefix();
		this->tabWidget_outtext->setCurrentWidget(this->tab_hydtxt);
		this->status_wid->set_hydthread(false);

		this->action_delete_project->setEnabled(true);
		this->action_edit_project->setEnabled(true);
	}
}
//Allocate a multiple hydraulic system for a hydraulic calculation
void Main_Wid::allocate_multi_hydraulic_system(void){
	try{
		if(this->hyd_calc==NULL){
			this->hyd_calc=new Hyd_Multiple_Hydraulic_Systems;
			this->hyd_calc->set_systemid(this->system_state.get_sys_system_id());
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);
	}
}
//Delete the multiple hydraulic system for a hydraulic calculation
void Main_Wid::delete_multi_hydraulic_system(void){
	if(this->hyd_calc!=NULL){
		if (this->task_flag == true) {
			this->total_err_task = this->total_err_task + this->hyd_calc->get_error_number();
		}
		delete this->hyd_calc;
		this->hyd_calc=NULL;
		QSqlDatabase::removeDatabase(sys_label::str_hyd.c_str());
	}
}
///Check if the damage thread is running and set the menus and actions
void Main_Wid::check_dam_thread_is_running(void){
	if(this->dam_calc!=NULL && this->dam_calc->isRunning()==true){
		//dam
		this->menu_dam_ecn->setEnabled(false);
		this->menu_dam_eco->setEnabled(false);
		this->menu_dam_pop->setEnabled(false);
		this->menu_dam_pys->setEnabled(false);
		this->menu_dam_sc->setEnabled(false);
		this->menu_dam_CI->setEnabled(false);
		this->menu_dam_system->setEnabled(false);
		this->action_check_dam_tables->setEnabled(false);
		this->menu_import_HYD_results->setEnabled(false);
		this->action_delete_hyd_results->setEnabled(false);
		//risk
		this->status_wid->get_ptr_risk_state_check_box()->setEnabled(false);

		//sys
		this->status_wid->set_damthread(true);
		this->tabWidget_outtext->setCurrentWidget(this->tab_damtxt);
		this->check_enable_db_close();
		this->action_delete_project->setEnabled(false);
		this->action_Close_Connection->setEnabled(false);
		this->action_close_all_tables->setEnabled(false);
		this->action_Reset_Error_Warning->setEnabled(false);
		this->action_copy_project->setEnabled(false);
		this->action_edit_project->setEnabled(false);
	}
	else{
		//dam
		if(this->system_state.get_sys_system_id().measure_nr==0){
			this->menu_dam_ecn->setEnabled(true);
			this->menu_dam_eco->setEnabled(true);
			this->menu_dam_pop->setEnabled(true);
			this->menu_dam_pys->setEnabled(true);
			this->menu_pys_imp->setEnabled(true);
			this->menu_dam_sc->setEnabled(true);
			this->menu_dam_CI->setEnabled(true);
			if(Sys_Project::get_project_type()==_sys_project_type::proj_dam){
				this->menu_import_HYD_results->setEnabled(true);
				this->action_delete_hyd_results->setEnabled(true);
			}
		}
		this->menu_dam_system->setEnabled(true);
		this->action_check_dam_tables->setEnabled(true);

		if(this->hyd_thread_running()==false){
			if(this->system_state.get_sys_system_id().measure_nr==0){
				this->action_dam_connect->setEnabled(true);
			}
			this->action_dam_calculate->setEnabled(true);
			this->action_connect_raster_ecn->setEnabled(true);
			this->action_connect_raster_pop->setEnabled(true);
			this->action_connect_raster_eco->setEnabled(true);
			this->action_connect_raster_pys->setEnabled(true);
			this->action_sc_connect->setEnabled(true);
			this->action_CI_connect->setEnabled(true);
		}

		Sys_Common_Output::output_dam->reset_userprefix();

		//risk
		if(this->some_thread_running()==false){
			this->check_enabling_risk_check_box(true);
			//sys
			this->action_delete_project->setEnabled(true);
			this->check_enable_db_close();
			this->action_close_all_tables->setEnabled(true);
			this->action_Close_Connection->setEnabled(true);
			this->action_Reset_Error_Warning->setEnabled(true);
			this->action_copy_project->setEnabled(true);
			this->action_edit_project->setEnabled(true);
		}

		this->tabWidget_outtext->setCurrentWidget(this->tab_damtxt);
		this->status_wid->set_damthread(false);
	}
}
//Allocate a damage system for the damage calculation
void Main_Wid::allocate_damage_system(void){
	try{
		if(this->dam_calc==NULL){
			this->dam_calc=new Dam_Damage_System;
			QObject::connect(this->dam_calc, SIGNAL(send_hyd_thread_runs(bool )), this, SLOT(hyd_module_extern_runs(bool )));
			QObject::connect(this->dam_calc, SIGNAL(send_hyd_thread_enable(bool )), this, SLOT(enable_hyd_module_extern(bool )));
			QObject::connect(this->dam_calc, SIGNAL(send_dam_thread_sleeps(bool )), this, SLOT(dam_thread_sleeps(bool )));

			this->dam_calc->set_systemid(this->system_state.get_sys_system_id());
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);
	}
}
//Delete the damage system for the damage calculation
void Main_Wid::delete_damage_system(void){
	if(this->dam_calc!=NULL){
		QObject::disconnect(this->dam_calc, SIGNAL(send_hyd_thread_runs(bool )), this, SLOT(hyd_module_extern_runs(bool )));
		QObject::disconnect(this->dam_calc, SIGNAL(send_hyd_thread_enable(bool )), this, SLOT(enable_hyd_module_extern(bool )));
		QObject::disconnect(this->dam_calc, SIGNAL(send_dam_thread_sleeps(bool )), this, SLOT(dam_thread_sleeps(bool )));

		delete this->dam_calc;
		this->dam_calc=NULL;
		QSqlDatabase::removeDatabase(sys_label::str_dam.c_str());

		if(Sys_Project::get_project_type()==_sys_project_type::proj_dam){
				Hyd_Multiple_Hydraulic_Systems::set_stop_thread_flag(false);
			}
	}
}
//Check if the fpl thread is running and set the menus and actions
void Main_Wid::check_fpl_thread_is_running(void){
	if(this->fpl_calc!=NULL && this->fpl_calc->isRunning()==true){
		//fpl
		this->menu_fpl_calc->setEnabled(false);
		this->action_stop_fpl_calc->setEnabled(true);
		this->action_check_fpl_tables->setEnabled(false);
		this->action_delete_fpl_section->setEnabled(false);
		this->action_export_DETERM->setEnabled(false);
		this->action_export_MC->setEnabled(false);
		this->action_export_FRC->setEnabled(false);
		this->action_restore_standard_fpl->setEnabled(false);
		this->menu_fpl2hyd->setEnabled(false);
		this->menu_check_fpl->setEnabled(false);
		this->action_fpl_import_file->setEnabled(false);
		this->action_user_defined_frc->setEnabled(false);

		this->action_stop_fpl_calc->setEnabled(true);
		//risk
		this->status_wid->get_ptr_risk_state_check_box()->setEnabled(false);
		//sys
		this->status_wid->set_fplthread(true);
		this->tabWidget_outtext->setCurrentWidget(this->tab_fpltxt);
		this->check_enable_db_close();
		this->action_delete_project->setEnabled(false);
		this->action_Close_Connection->setEnabled(false);
		this->action_close_all_tables->setEnabled(false);
		this->action_Reset_Error_Warning->setEnabled(false);
		this->action_copy_project->setEnabled(false);
		this->action_edit_project->setEnabled(false);
	}
	else{
		//fpl
		this->menu_fpl_calc->setEnabled(true);
		this->action_stop_fpl_calc->setEnabled(false);
		this->action_check_fpl_tables->setEnabled(true);

		this->action_export_DETERM->setEnabled(true);
		this->action_export_MC->setEnabled(true);
		this->action_export_FRC->setEnabled(true);

		this->action_restore_standard_fpl->setEnabled(true);
		this->action_stop_fpl_calc->setEnabled(false);
		if(Sys_Project::get_project_type()==_sys_project_type::proj_risk || Sys_Project::get_project_type()==_sys_project_type::proj_all){
			this->menu_fpl2hyd->setEnabled(true);
		}
		this->menu_check_fpl->setEnabled(true);

		if(this->system_state.get_sys_system_id().measure_nr==0){
			this->action_fpl_import_file->setEnabled(true);
			this->action_delete_fpl_section->setEnabled(true);
		}
		this->action_user_defined_frc->setEnabled(true);

		Sys_Common_Output::output_fpl->reset_userprefix();

		//risk
		if(this->some_thread_running()==false){
			this->check_enabling_risk_check_box(true);
			//sys
			this->check_enable_db_close();
			this->action_Close_Connection->setEnabled(true);
			this->action_close_all_tables->setEnabled(true);
			this->action_delete_project->setEnabled(true);
			this->action_Reset_Error_Warning->setEnabled(true);
			this->action_copy_project->setEnabled(true);
			this->action_edit_project->setEnabled(true);
		}
		this->status_wid->set_fplthread(false);
		this->tabWidget_outtext->setCurrentWidget(this->tab_fpltxt);
	}
}
//Allocate a fpl system for the fpl calculation
void Main_Wid::allocate_fpl_system(void){
	try{
		if(this->fpl_calc==NULL){
			this->fpl_calc=new Fpl_Calculation;
			QObject::connect(this->fpl_calc, SIGNAL(send_hyd_thread_runs(bool )), this, SLOT(hyd_module_extern_runs(bool )));
			QObject::connect(this->fpl_calc, SIGNAL(send_hyd_thread_enable(bool )), this, SLOT(enable_hyd_module_extern(bool )));
			this->fpl_calc->set_systemid(this->system_state.get_sys_system_id());
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);
	}
}
//Delete the fpl system for the fpl calculation
void Main_Wid::delete_fpl_system(void){
	if (this->fpl_calc != NULL) {
		QObject::disconnect(this->fpl_calc, SIGNAL(send_hyd_thread_runs(bool)), this, SLOT(hyd_module_extern_runs(bool)));
		QObject::disconnect(this->fpl_calc, SIGNAL(send_hyd_thread_enable(bool)), this, SLOT(enable_hyd_module_extern(bool)));
		if (this->task_flag == true) {
			this->total_err_task = this->total_err_task+ this->fpl_calc->get_number_error();
		}
		delete this->fpl_calc;
		this->fpl_calc=NULL;
		QSqlDatabase::removeDatabase(sys_label::str_fpl.c_str());
	}
}
//Check if the risk thread is running and set the menus and actions
void Main_Wid::check_risk_thread_is_running(void){
	if(this->risk_calc!=NULL && this->risk_calc->isRunning()==true){
		//risk
		this->action_stop_risk_calc->setEnabled(true);
		this->action_check_risk_tables->setEnabled(false);
		this->menu_risk_calc->setEnabled(false);
		this->menu_catchment_risk->setEnabled(false);
		this->action_output_statistics_risk->setEnabled(false);
		this->action_output_statistic_catchment_risk->setEnabled(false);
		this->action_delete_results_risk->setEnabled(false);
		this->action_properties_hyd_2->setEnabled(false);

		this->action_calc_all->setEnabled(false);

		//sys
		this->action_delete_project->setEnabled(false);
		this->status_wid->set_riskthread(true);
		this->tabWidget_outtext->setCurrentWidget(this->tab_risktxt);
		this->check_enable_db_close();
		this->status_wid->get_ptr_risk_state_check_box()->setEnabled(false);
		this->action_close_all_tables->setEnabled(false);
		this->action_Close_Connection->setEnabled(false);
		this->action_Reset_Error_Warning->setEnabled(false);
		this->action_copy_project->setEnabled(false);
		this->action_edit_project->setEnabled(false);

		//madm
		this->action_madm_calc_database->setEnabled(false);
		//alt
		this->menu_new_area_measure_state->setEnabled(false);
		this->action_switch_measure_state->setEnabled(false);
		this->action_delete_measure_state->setEnabled(false);
	}
	else{
		//risk
		this->action_stop_risk_calc->setEnabled(false);
		this->action_stop_next_HYD_calc->setEnabled(false);

		this->action_calc_nobreak_risk->setEnabled(this->risk_flags.nobreak_risk_applied);

		this->action_calc_scenario_risk->setEnabled(true);
		this->menu_risk_common->setEnabled(true);
		this->action_calc_nobreak_state->setEnabled(true);
		this->action_risk_state_generation->setEnabled(true);
		this->action_calc_one_break_state->setEnabled(true);
		this->action_calc_user_defined->setEnabled(true);
		this->action_calc_automatic->setEnabled(true);
		this->action_risk_cumulation->setEnabled(true);
		this->action_properties_hyd_2->setEnabled(true);
		this->action_calc_all->setEnabled(true);

		this->action_check_risk_tables->setEnabled(true);
		this->menu_risk_calc->setEnabled(true);

		this->menu_catchment_risk->setEnabled(this->risk_flags.catchment_risk_applied);
		this->action_output_statistic_catchment_risk->setEnabled(this->risk_flags.catchment_risk_applied);
		this->action_output_statistics_risk->setEnabled(true);
		this->action_delete_results_risk->setEnabled(true);

		Sys_Common_Output::output_risk->reset_userprefix();

		//sys
		if(this->some_thread_running()==false){
			this->check_enabling_risk_check_box(true);
			this->action_delete_project->setEnabled(true);
			this->check_enable_db_close();
			this->action_Close_Connection->setEnabled(true);
			this->action_close_all_tables->setEnabled(true);
			this->action_Reset_Error_Warning->setEnabled(true);
			this->action_copy_project->setEnabled(true);
			this->action_edit_project->setEnabled(true);
		}
		this->tabWidget_outtext->setCurrentWidget(this->tab_risktxt);
		this->status_wid->set_riskthread(false);

		//madm
		if(Sys_Project::get_project_type()==_sys_project_type::proj_all && this->system_database!=NULL && this->system_database->check_con_status()==true && this->table_create_flag==false){
			QStringList list;

			Alt_System::load_stringlist_current_alt(this->system_database->get_database(),this->system_state.get_ptr_sys_system_id()->area_state,&list);
			Alt_System::close_table_measure();
			if(list.count()>0 && this->risk_flags.risk_state_flag==true){
				this->action_madm_calc_database->setEnabled(true);
			}
		}

		//alt
		if(this->risk_flags.risk_state_flag==true){
			this->menu_new_area_measure_state->setEnabled(true);
			this->action_switch_measure_state->setEnabled(true);
			this->action_delete_measure_state->setEnabled(true);
		}
	}
}
//Allocate a risk system for the risk calculation
void Main_Wid::allocate_risk_system(void){
	try{
		if(this->risk_calc==NULL){
			this->risk_calc=new Risk_System;
			QObject::connect(this->risk_calc, SIGNAL(send_hyd_thread_runs(bool )), this, SLOT(hyd_module_extern_runs(bool )));
			QObject::connect(this->risk_calc, SIGNAL(send_dam_thread_runs(bool )), this, SLOT(dam_module_extern_runs(bool )));
			QObject::connect(this->risk_calc, SIGNAL(send_fpl_thread_runs(bool )), this, SLOT(fpl_module_extern_runs(bool )));
			QObject::connect(this->risk_calc, SIGNAL(emit_number_threads(QString )),this,SLOT(catch_thread_number_hy_calc(QString )));

			QObject::connect(this->risk_calc, SIGNAL(emit_stop_next_hyd_calc(bool )),this,SLOT(enable_stop_at_next_step_action(bool )));
			QObject::connect(this->risk_calc, SIGNAL(send_risk_thread_sleeps(bool )),this,SLOT(risk_thread_sleeps(bool )));
			QObject::connect(this->risk_calc, SIGNAL(send_hydraulic_thread_sleeps(bool )),this,SLOT(hydraulic_thread_sleeps(bool )));

			this->risk_calc->set_systemid(this->system_state.get_sys_system_id());
			this->risk_calc->set_risk_state_flag(this->risk_flags);
			this->risk_calc->set_hyd_state_data(this->project_manager.get_stored_hydraulic_state());
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);
	}
}
//Delete the risk system for the risk calculation
void Main_Wid::delete_risk_system(void){
	if(this->risk_calc!=NULL){
		QObject::disconnect(this->risk_calc, SIGNAL(send_hyd_thread_runs(bool )), this, SLOT(hyd_module_extern_runs(bool )));
		QObject::disconnect(this->risk_calc, SIGNAL(send_dam_thread_runs(bool )), this, SLOT(dam_module_extern_runs(bool )));
		QObject::disconnect(this->risk_calc, SIGNAL(send_fpl_thread_runs(bool )), this, SLOT(fpl_module_extern_runs(bool )));
		QObject::disconnect(this->risk_calc, SIGNAL(emit_number_threads(QString )),this,SLOT(catch_thread_number_hy_calc(QString )));
		QObject::disconnect(this->risk_calc, SIGNAL(emit_stop_next_hyd_calc(bool )),this,SLOT(enable_stop_at_next_step_action(bool )));
		QObject::disconnect(this->risk_calc, SIGNAL(send_risk_thread_sleeps(bool )),this,SLOT(risk_thread_sleeps(bool )));
		QObject::disconnect(this->risk_calc, SIGNAL(send_risk_thread_sleeps(bool )),this,SLOT(risk_thread_sleeps(bool )));
		QObject::disconnect(this->risk_calc, SIGNAL(send_hydraulic_thread_sleeps(bool )),this,SLOT(hydraulic_thread_sleeps(bool )));

		delete this->risk_calc;
		this->risk_calc=NULL;
		QSqlDatabase::removeDatabase(sys_label::str_risk.c_str());
	}
}
//Check if the madm thread is running and set the menus and actions
void Main_Wid::check_madm_thread_is_running(void){
	if(this->madm_calc!=NULL && this->madm_calc->isRunning()==true){
		//madm
		this->action_stop_madm_calc->setEnabled(true);
		this->action_check_madm_tables->setEnabled(false);
		this->action_import_weighting_sets->setEnabled(false);
		this->action_delete_results_madm->setEnabled(false);
		this->menu_madm_calc->setEnabled(false);

		//sys
		this->action_delete_project->setEnabled(false);
		this->status_wid->set_madmthread(true);
		this->tabWidget_outtext->setCurrentWidget(this->tab_madmtxt);
		this->check_enable_db_close();
		this->status_wid->get_ptr_risk_state_check_box()->setEnabled(false);
		this->action_close_all_tables->setEnabled(false);
		this->action_Close_Connection->setEnabled(false);
		this->action_Reset_Error_Warning->setEnabled(false);
		this->action_copy_project->setEnabled(false);
		this->action_edit_project->setEnabled(false);

		//alt
		this->menu_new_area_measure_state->setEnabled(false);
		this->action_switch_measure_state->setEnabled(false);
		this->action_delete_measure_state->setEnabled(false);
	}
	else{
		//madm
		this->action_stop_madm_calc->setEnabled(false);
		this->action_check_madm_tables->setEnabled(true);
		this->action_import_weighting_sets->setEnabled(true);
		this->action_delete_results_madm->setEnabled(true);
		this->menu_madm_calc->setEnabled(true);

		Sys_Common_Output::output_madm->reset_userprefix();

		//sys
		if(this->some_thread_running()==false){
			this->action_Reset_Error_Warning->setEnabled(true);
			//sys
			this->check_enabling_risk_check_box(true);
			this->action_delete_project->setEnabled(true);
			this->check_enable_db_close();
			this->action_Close_Connection->setEnabled(true);
			this->action_close_all_tables->setEnabled(true);
			this->action_copy_project->setEnabled(true);
			this->action_edit_project->setEnabled(true);
		}
		this->tabWidget_outtext->setCurrentWidget(this->tab_madmtxt);
		this->status_wid->set_madmthread(false);

		//alt
		if(this->risk_flags.risk_state_flag==true){
			this->menu_new_area_measure_state->setEnabled(true);
			this->action_switch_measure_state->setEnabled(true);
			this->action_delete_measure_state->setEnabled(true);
		}
	}
}
//Allocate a madm system for the madm calculation
void Main_Wid::allocate_madm_system(void){
	try{
		if(this->madm_calc==NULL){
			this->madm_calc=new Madm_System;
			this->madm_calc->set_systemid(this->system_state.get_sys_system_id());
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);
	}
}
//Delete the madm system for the madm calculation
void Main_Wid::delete_madm_system(void){
	if(this->madm_calc!=NULL){
		delete this->madm_calc;
		this->madm_calc=NULL;
		QSqlDatabase::removeDatabase(sys_label::str_madm.c_str());
	}
}
//Check if the alt thread is running and set the menus and actions
void Main_Wid::check_alt_thread_is_running(void){
	if(this->alt_calc!=NULL && this->alt_calc->isRunning()==true){
		//sys
		this->action_delete_project->setEnabled(false);
		this->tabWidget_outtext->setCurrentWidget(this->tab_alttxt);
		this->check_enable_db_close();
		this->status_wid->get_ptr_risk_state_check_box()->setEnabled(false);
		this->action_close_all_tables->setEnabled(false);
		this->action_Close_Connection->setEnabled(false);
		this->action_Reset_Error_Warning->setEnabled(false);
		this->action_copy_project->setEnabled(false);
		this->action_edit_project->setEnabled(false);

		this->menu_COST_system->setEnabled(false);
		this->action_madm_calc_database->setEnabled(false);
	}
	else{
		Sys_Common_Output::output_alt->reset_userprefix();
		//sys
		if(this->some_thread_running()==false){
			this->action_Reset_Error_Warning->setEnabled(true);
			//sys
			this->check_enabling_risk_check_box(true);
			this->action_delete_project->setEnabled(true);
			this->check_enable_db_close();
			this->action_Close_Connection->setEnabled(true);
			this->action_close_all_tables->setEnabled(true);
			this->action_copy_project->setEnabled(true);
			this->action_edit_project->setEnabled(true);
		}
		this->tabWidget_outtext->setCurrentWidget(this->tab_alttxt);
		if(this->system_state.get_sys_system_id().measure_nr>0){
			this->menu_COST_system->setEnabled(true);
		}
		//madm
		if(Sys_Project::get_project_type()==_sys_project_type::proj_all && this->system_database!=NULL && this->system_database->check_con_status()==true && this->table_create_flag==false){
			QStringList list;
			Alt_System::load_stringlist_current_alt(this->system_database->get_database(),this->system_state.get_ptr_sys_system_id()->area_state,&list);
			Alt_System::close_table_measure();
			if(list.count()>0 && this->risk_flags.risk_state_flag==true){
				this->action_madm_calc_database->setEnabled(true);
			}
		}
	}
}
//Allocate a alt system for the alt calculation
void Main_Wid::allocate_alt_system(void){
	try{
		if(this->alt_calc==NULL){
			this->alt_calc=new Alt_System;
			QObject::connect(this->alt_calc, SIGNAL(send_hyd_thread_runs(bool )), this, SLOT(hyd_module_extern_runs(bool )));
			QObject::connect(this->alt_calc, SIGNAL(send_dam_thread_runs(bool )), this, SLOT(dam_module_extern_runs(bool )));
			QObject::connect(this->alt_calc, SIGNAL(send_fpl_thread_runs(bool )), this, SLOT(fpl_module_extern_runs(bool )));

			this->alt_calc->set_systemid(this->system_state.get_sys_system_id());
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);
	}
}
//Delete the alt system for the alt calculation
void Main_Wid::delete_alt_system(void){
	if(this->alt_calc!=NULL){
		delete this->alt_calc;
		this->alt_calc=NULL;
		QSqlDatabase::removeDatabase(sys_label::str_alt.c_str());
	}
}
//Check if the cost thread is running and set the menus and actions
void Main_Wid::check_cost_thread_is_running(void){
	if(this->cost_calc!=NULL && this->cost_calc->isRunning()==true){
		//sys
		this->action_delete_project->setEnabled(false);
		this->tabWidget_outtext->setCurrentWidget(this->tab_costtxt);
		this->check_enable_db_close();
		this->status_wid->get_ptr_risk_state_check_box()->setEnabled(false);
		this->action_close_all_tables->setEnabled(false);
		this->action_Close_Connection->setEnabled(false);
		this->action_Reset_Error_Warning->setEnabled(false);
		this->action_copy_project->setEnabled(false);
		this->action_edit_project->setEnabled(false);
	}
	else{
		Sys_Common_Output::output_cost->reset_userprefix();

		//sys
		if(this->some_thread_running()==false){
			this->action_Reset_Error_Warning->setEnabled(true);
			//sys
			this->check_enabling_risk_check_box(true);
			this->action_delete_project->setEnabled(true);
			this->check_enable_db_close();
			this->action_Close_Connection->setEnabled(true);
			this->action_close_all_tables->setEnabled(true);
			this->action_copy_project->setEnabled(true);
			this->action_edit_project->setEnabled(true);
		}
		this->tabWidget_outtext->setCurrentWidget(this->tab_costtxt);
	}
}
//Allocate a cost system for the cost calculation
void Main_Wid::allocate_cost_system(void){
	try{
		if(this->cost_calc==NULL){
			this->cost_calc=new Cost_System;
			this->cost_calc->set_systemid(this->system_state.get_sys_system_id());
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);
	}
}
//Delete the cost system for the cost calculation
void Main_Wid::delete_cost_system(void){
	if(this->cost_calc!=NULL){
		delete this->cost_calc;
		this->cost_calc=NULL;
		QSqlDatabase::removeDatabase(sys_label::str_cost.c_str());
	}
}
//Initialize the thread-closing manager
bool Main_Wid::init_thread_closing_manager(void){
	//check threads
	this->thread_in_process=false;
	if(this->hyd_calc!=NULL && this->hyd_calc->isRunning()==true){
		this->thread_in_process=true;
	}
	if(this->dam_calc!=NULL && this->dam_calc->isRunning()==true){
		this->thread_in_process=true;
	}
	if(this->fpl_calc!=NULL && this->fpl_calc->isRunning()==true){
		this->thread_in_process=true;
	}
	if(this->risk_calc!=NULL && this->risk_calc->isRunning()==true){
		this->thread_in_process=true;
	}
	if(this->alt_calc!=NULL && this->alt_calc->isRunning()==true){
		this->thread_in_process=true;
	}
	if(this->cost_calc!=NULL && this->cost_calc->isRunning()==true){
		this->thread_in_process=true;
	}
	if(this->madm_calc!=NULL && this->madm_calc->isRunning()==true){
		this->thread_in_process=true;
	}
	//..introduce further modules
	if(this->browser_database->get_ptr_table_loader_thread()!=NULL && this->browser_database->get_ptr_table_loader_thread()->isRunning()==true){
		this->thread_in_process=true;
	}
	if(this->project_manager.thread_pro_copy!=NULL && this->project_manager.thread_pro_copy->isRunning()==true){
		this->thread_in_process=true;
	}

	if(this->thread_in_process==true){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"There are still threads in process." << endl;
		txt<<"All data will be lost." << endl;
		txt<<"Close all running threads?" << endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.setIcon(QMessageBox::Warning);
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			return false;
		}
		emit send_txt2statusbar("The threads are closing...\n Please wait!",0);
	}

	this->setEnabled(false);
	//stop the threads
	if(this->hyd_calc!=NULL && this->hyd_calc->isRunning()==true){
		this->hyd_calc->set_stop_thread_flag(true);
	}
	if(this->dam_calc!=NULL && this->dam_calc->isRunning()==true){
		this->dam_calc->set_stop_thread_flag(true);
	}
	if(this->fpl_calc!=NULL && this->fpl_calc->isRunning()==true){
		this->fpl_calc->set_stop_thread_flag(true);
	}
	if(this->risk_calc!=NULL && this->risk_calc->isRunning()==true){
		this->risk_calc->set_stop_thread_flag(true);
	}
	if(this->madm_calc!=NULL && this->madm_calc->isRunning()==true){
		this->madm_calc->set_stop_thread_flag(true);
	}
	if(this->alt_calc!=NULL && this->alt_calc->isRunning()==true){
		this->alt_calc->set_stop_thread_flag(true);
	}
	if(this->cost_calc!=NULL && this->cost_calc->isRunning()==true){
		this->cost_calc->set_stop_thread_flag(true);
	}
	//..introduce further modules

	//init the management class
	this->closing_thread= new Sys_Thread_Closing_Manager;
	this->closing_thread->set_ptr_dam_thread(this->dam_calc);
	this->closing_thread->set_ptr_hyd_thread(this->hyd_calc);
	this->closing_thread->set_ptr_fpl_thread(this->fpl_calc);
	this->closing_thread->set_ptr_madm_thread(this->madm_calc);
	this->closing_thread->set_ptr_alt_thread(this->alt_calc);
	this->closing_thread->set_ptr_cost_thread(this->cost_calc);
	this->closing_thread->set_ptr_risk_thread(this->risk_calc);
	this->closing_thread->set_ptr_table_loader_thread(this->browser_database->get_ptr_table_loader_thread());
	this->closing_thread->set_ptr_project_copy_thread(this->project_manager.thread_pro_copy);

	QObject::connect(this->closing_thread, SIGNAL(send_threads_closed(_sys_close_types )), this, SLOT(close_thread_finished(_sys_close_types )));

	return true;
}
//Delete the thread closing manager
void Main_Wid::delete_thread_closing_manager(void){
	if(this->closing_thread!=NULL){
		this->setEnabled(true);
		QObject::disconnect(this->closing_thread, SIGNAL(send_threads_closed(_sys_close_types )), this, SLOT(close_thread_finished(_sys_close_types )));
		delete this->closing_thread;
		this->closing_thread=NULL;
	}
}
//Set the risk state flag and change the statusbar
void Main_Wid::set_risk_state_flag(const _risk_state_flags flags){
	if(Sys_Project::get_project_type()==_sys_project_type::proj_all || Sys_Project::get_project_type()==_sys_project_type::proj_risk){
		this->risk_flags=flags;
		this->status_wid->get_ptr_risk_state_check_box()->setChecked(this->risk_flags.risk_state_flag);

		if(this->risk_flags.risk_state_flag==true){
			this->status_wid->set_tooltip_risk_state_check_box(this->risk_flags.reliability_fpl_result, this->risk_flags.reliability_dam_result, this->risk_flags.reliability_hyd_result);
		}
		else{
			this->status_wid->reset_tooltip_risk_state_check_box();
		}

		this->project_manager.set_current_risk_state(this->risk_flags);
		this->menu_RISK_system->setEnabled(this->risk_flags.risk_state_flag);
		this->menu_new_area_measure_state->setEnabled(this->risk_flags.risk_state_flag);

		this->action_switch_measure_state->setEnabled(this->risk_flags.risk_state_flag);

		this->menu_catchment_risk->setEnabled(this->risk_flags.catchment_risk_applied);
		this->action_calc_nobreak_risk->setEnabled(this->risk_flags.nobreak_risk_applied);
		if(Sys_Project::get_project_type()==_sys_project_type::proj_all && this->system_database!=NULL && this->system_database->check_con_status()==true && this->table_create_flag==false){
			QStringList list;

			Alt_System::load_stringlist_current_alt(this->system_database->get_database(),this->system_state.get_ptr_sys_system_id()->area_state,&list);
			Alt_System::close_table_measure();
			if(list.count()>0 && this->risk_flags.risk_state_flag==true){
				this->action_madm_calc_database->setEnabled(true);
			}
			else{
				this->action_madm_calc_database->setEnabled(false);
			}
		}
		else{
			this->action_madm_calc_database->setEnabled(false);
		}
		if(this->system_state.get_sys_system_id().measure_nr>0 && this->risk_flags.risk_state_flag==true){
			this->action_ecn_cost_user_defined->setEnabled(this->risk_flags.risk_state_flag);
		}
		else{
			this->action_ecn_cost_user_defined->setEnabled(false);
		}
		this->menu_HYD_system->setEnabled(!this->risk_flags.risk_state_flag);
		this->menu_DAM_system->setEnabled(!this->risk_flags.risk_state_flag);
		this->menu_FPL_system->setEnabled(!this->risk_flags.risk_state_flag);
	}
}
//Set the system state and change the status bar
void Main_Wid::set_system_state(const _sys_system_id id){
	this->system_state.set_systemid(id);
	this->status_wid->set_system_state(this->system_state.get_sys_system_id());
	this->project_manager.set_current_system_state(this->system_state.get_sys_system_id());
}
//Check if the dam-thread is still ruuning
bool Main_Wid::dam_thread_running(void){
	if(this->dam_calc!=NULL && this->dam_calc->get_thread_has_started()==true){
		return true;
	}
	return false;
}
//Check if the hyd-thread is still ruuning
bool Main_Wid::hyd_thread_running(void){
	if(this->hyd_calc!=NULL && this->hyd_calc->get_thread_has_started()==true){
		return true;
	}
	return false;
}
//Check if the fpl-thread is still ruuning
bool Main_Wid::fpl_thread_running(void){
	if(this->fpl_calc!=NULL && this->fpl_calc->get_thread_has_started()==true){
		return true;
	}
	return false;
}
//Check if the some thread is still ruuning
bool Main_Wid::some_thread_running(void){
    if(this->hyd_calc!=NULL  && this->hyd_calc->get_thread_has_started()==true){
		return true;
	}
    if(this->dam_calc!=NULL && this->dam_calc->get_thread_has_started()==true){
		return true;
	}
    if(this->fpl_calc!=NULL && this->fpl_calc->get_thread_has_started()==true){
		return true;
	}
    if(this->risk_calc!=NULL && this->risk_calc->get_thread_has_started()==true){
		return true;
	}
    if(this->madm_calc!=NULL && this->madm_calc->get_thread_has_started()==true){
		return true;
	}
    if(this->cost_calc!=NULL && this->cost_calc->get_thread_has_started()==true){
		return true;
	}
    if(this->alt_calc!=NULL  && this->alt_calc->get_thread_has_started()==true){
		return true;
	}
	//introduce further modules

	return false;
}
//Check for enabling the risk check-box
void Main_Wid::check_enabling_risk_check_box(const bool flag){
	if(flag==false){
		this->status_wid->get_ptr_risk_state_check_box()->setEnabled(false);
	}
	else{
		if((Sys_Project::get_project_type()==_sys_project_type::proj_all ||
			Sys_Project::get_project_type()==_sys_project_type::proj_risk) && this->system_state.get_ptr_sys_system_id()->measure_nr==0 && this->system_state.get_ptr_sys_system_id()->area_state==0){
				if(this->system_database!=NULL && this->system_database->check_con_status()==true){
					bool alt_set=false;
					try{
						alt_set=Alt_System::check_measure_is_set_database(this->system_database->get_database());
					}
					catch(Error msg){
						alt_set=false;
					}
					if(alt_set==false){
						this->status_wid->get_ptr_risk_state_check_box()->setEnabled(flag);
					}
					else{
						this->status_wid->get_ptr_risk_state_check_box()->setEnabled(false);
					}
				}
				else{
					this->status_wid->get_ptr_risk_state_check_box()->setEnabled(false);
				}
		}
		else{
			this->status_wid->get_ptr_risk_state_check_box()->setEnabled(false);
		}
	}
}
//Reset exceptions of now action is running and a new action is started
void Main_Wid::reset_exception_new_action(void){
	if(this->some_thread_running()==true){
		return;
	}
	else{
		this->reset_error_warning();
	}
}
//____________________
//private slots
//The welcome window is closed; the mainwindow is started
void Main_Wid::start_main_window(void){
	this->welcome_wid.close();
	this->enable_menu_project_closed();
	this->setEnabled(false);
	this->show();
	this->statusBar()->close();
	this->statusBar()->show();
	this->statusBar()->showMessage("Ready", 0);
	//show project dialog
	if(this->project_file_set==false){
		this->project_file_set=this->project_manager.ask_existing_project(this);
	}

	//show the main window
	QObject::disconnect(&welcome_timer, SIGNAL(timeout()), this, SLOT(start_main_window()));

	if(this->project_file_set==true){
		//this->enable_menu_project_closed();
		this->read_existing_project();
	}
	else{
		this->setEnabled(true);
	}





	//else{
	//	this->enable_menu_project_closed();
	//}
}
//Set the main window to enable/disable
void Main_Wid::set_enabled(bool flag){
	this->setEnabled(flag);
}
//Ask for closing a current project, if the user wants to open an existing project
void Main_Wid::ask_close_project_open_existing(void){
	if(Sys_Project::get_project_name()!=label::not_set){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"The project "<< this->project_manager.get_project_name()<< " is still active!"<<endl;
		txt<<"Do you want open a new project ?" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			return;
		}
		else{
			if(this->terminate_threads_close_proj_open_existing()==false){
				return;
			}
			else{
				if(this->closing_thread==NULL){
					this->open_existing_project();
				}
			}
		}
	}
	else{
		this->open_existing_project();
	}
}
//Open an existing project
void Main_Wid::open_existing_project(void){
	try{
		//show project dialog
		if(this->project_manager.ask_existing_project(this)==false){
			this->enable_menu_project_closed();
			return;
		}
		else{
			this->read_existing_project();
		}
	}
	catch(Error msg){
		msg.output_msg(0);
	}
}
//Ask for closing a current project, if the user wants to create a new project
void Main_Wid::ask_close_project_create_new(void){
	if(Sys_Project::get_project_name()!=label::not_set){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"The project "<< this->project_manager.get_project_name()<< " is still active!"<<endl;
		txt<<"Do you want open a new project ?" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			return;
		}
		else{
			if(this->terminate_threads_close_proj_create_new()==false){
				return;
			}
			else{
				if(this->closing_thread==NULL){
					this->open_new_project();
				}
			}
		}
	}
	else{
		this->open_new_project();
	}
}
//Open and create a new project
void Main_Wid::open_new_project(void){
	bool project_new=false;
	try{
		project_new=this->project_manager.build_new_project(this);
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->reset_exception_new_action();
	this->reset_system_outputtxt();
	this->setWindowTitle(this->project_manager.get_main_window_title().c_str());
	if(project_new==true){
		this->enable_menu_project_open(true);
		this->create_project_database_tables();
	}
	else{
		this->enable_menu_project_closed();
	}
}
//Terminate the threads in case of closing the whole application
void Main_Wid::terminate_threads_close_app(void){
	if(this->init_thread_closing_manager()==false){
		this->delete_thread_closing_manager();
		return;
	};
	if(this->thread_in_process==true){
		this->setEnabled(false);
		this->closing_thread->set_closing_type(_sys_close_types::app_close);
		this->closing_thread->start();
	}
	else{
		this->delete_thread_closing_manager();
		this->my_close();
	}
}
//Terminate the threads in case of closing the project
void Main_Wid::terminate_threads_close_proj(void){
	if(this->init_thread_closing_manager()==false){
		this->delete_thread_closing_manager();
		return;
	};
	if(this->thread_in_process==true){
		this->setEnabled(false);
		this->closing_thread->set_closing_type(_sys_close_types::pro_close);
		this->closing_thread->start();
	}
	else{
		this->delete_thread_closing_manager();
		this->close_project();
	}
}
//Terminate the threads in case of closing the project by open an existing one
bool Main_Wid::terminate_threads_close_proj_open_existing(void){
	if(this->init_thread_closing_manager()==false){
		this->delete_thread_closing_manager();
		return false;
	};
	if(this->thread_in_process==true){
		this->setEnabled(false);
		this->closing_thread->set_closing_type(_sys_close_types::pro_exist_open);
		this->closing_thread->start();
	}
	else{
		this->delete_thread_closing_manager();
		this->close_project();
	}
	return true;
}
//Terminate the threads in case of closing the project by creating a new one
bool Main_Wid::terminate_threads_close_proj_create_new(void){
	if(this->init_thread_closing_manager()==false){
		this->delete_thread_closing_manager();
		return false;
	};
	if(this->thread_in_process==true){
		this->setEnabled(false);
		this->closing_thread->set_closing_type(_sys_close_types::pro_new_open);
		this->closing_thread->start();
	}
	else{
		this->delete_thread_closing_manager();
		this->close_project();
	}
	return true;
}
//Close a project
void Main_Wid::close_project(void){
	if(Sys_Project::get_project_name()==label::not_set){
		return;
	}
	try{
		this->setEnabled(false);
		//save the project
		if(this->with_project_save==true){
			this->project_manager.save_project_paramater();
		}

		//close the database
		this->db_con_close();
		//close database tables
		this->close_all_tables();

		//close the project
		this->project_manager.close_project();

		//reset the output widgets and the exceptions
		this->reset_dam_outputtxt();
		this->reset_risk_outputtxt();
		this->reset_error_warning();
		this->reset_fpl_outputtxt();
		this->reset_hyd_outputtxt();
		this->reset_madm_outputtxt();
		this->reset_alt_outputtxt();
		this->reset_cost_outputtxt();
		this->reset_system_outputtxt();
		//..introduce further models

		Sys_Common_Output::output_fpl->set_detailflag(false);
		Sys_Common_Output::output_hyd->set_detailflag(false);
		Sys_Common_Output::output_dam->set_detailflag(false);
		Sys_Common_Output::output_risk->set_detailflag(false);
		Sys_Common_Output::output_madm->set_detailflag(false);
		Sys_Common_Output::output_alt->set_detailflag(false);
		Sys_Common_Output::output_cost->set_detailflag(false);
		Sys_Common_Output::output_system->set_detailflag(false);
		//..introduce further models

		//project settings
		this->project_file_set=false;
		this->hyd_tables_created=false;
		this->dam_tables_created=false;
		this->fpl_tables_created=false;
		this->risk_tables_created=false;
		this->madm_tables_created=false;
		this->alt_tables_created=false;
		this->cost_tables_created=false;
		//..introduce further models

		//risk state
		this->risk_flags.reliability_dam_result=false;
		this->risk_flags.reliability_fpl_result=false;
		this->risk_flags.reliability_hyd_result=false;
		this->risk_flags.risk_state_flag=false;

		_sys_system_id buffer;
		buffer.area_state=0;
		buffer.measure_nr=0;
		this->set_system_state(buffer);

		this->with_project_save=true;

		//reset the window title
		this->setWindowTitle(this->project_manager.get_main_window_title().c_str());

		this->browser_database->sql_help_row_wid.close();
		this->browser_database->sql_help_cond_wid.close();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	//enable the menu
	this->enable_menu_project_closed();

	//close the logfiles
	Sys_Common_Output::output_system->close_file();
	Sys_Common_Output::output_dam->close_file();
	Sys_Common_Output::output_excep->close_file();
	Sys_Common_Output::output_fpl->close_file();
	Sys_Common_Output::output_hyd->close_file();
	Sys_Common_Output::output_madm->close_file();
	Sys_Common_Output::output_risk->close_file();
	Sys_Common_Output::output_alt->close_file();
	Sys_Common_Output::output_cost->close_file();
	//..introduce further models

	this->setEnabled(true);
}
//Delete completly a project
void Main_Wid::delete_project(void){
	if(Sys_Project::get_project_name()==label::not_set){
		return;
	}

	Sys_Diverse_Text_Dia dialog2;
	ostringstream txt;
	txt<<"Do you really want to delete the "<< this->project_manager.get_project_name()<< " completely!"<<endl;
	txt<<"You can not recover the project!" << endl;
	txt<<"The project file and the project database tables are removed. Just the data folders are kept." << endl;
	dialog2.set_dialog_question(txt.str());
	dialog2.setIcon(QMessageBox::Warning);
	bool flag2=dialog2.start_dialog();
	if(flag2==false){
		return;
	}

	//enable the menu
	this->enable_menu_project_closed();
	this->delete_logfile_archiv();

	try{
		//stop the threads
		this->stop_fplcalc_thread();
		this->stop_hydcalc_thread();
		this->stop_hyd_systemcheck_thread();
		this->stop_hydimport_thread();
		this->stop_damcalc_thread();
		this->stop_riskcalc_thread();

		try{
			if(Sys_Project::get_project_type()!=_sys_project_type::proj_hyd_file && this->system_database!=NULL && this->system_database->check_con_status()==false){
				this->alloc_newdatabase();
				this->system_database->set_input_byfile(this->project_manager.get_project_file_name());
			}
		}
		catch(Error msg){
			msg.output_msg(0);
			return;
		}

		//close the project
		this->project_manager.close_delete_project(this->system_database->get_database());

		//reset the output widgets and the exceptions
		this->reset_dam_outputtxt();
		this->reset_risk_outputtxt();
		this->reset_error_warning();
		this->reset_fpl_outputtxt();
		this->reset_hyd_outputtxt();
		this->reset_system_outputtxt();

		Sys_Common_Output::output_fpl->set_detailflag(false);
		Sys_Common_Output::output_hyd->set_detailflag(false);
		Sys_Common_Output::output_dam->set_detailflag(false);
		Sys_Common_Output::output_risk->set_detailflag(false);
		Sys_Common_Output::output_madm->set_detailflag(false);
		Sys_Common_Output::output_system->set_detailflag(false);

		//project settings
		this->project_file_set=false;
		this->hyd_tables_created=false;
		this->dam_tables_created=false;
		this->fpl_tables_created=false;
		this->risk_tables_created=false;
		this->madm_tables_created=false;

		//risk state
		this->risk_flags.reliability_dam_result=false;
		this->risk_flags.reliability_fpl_result=false;
		this->risk_flags.reliability_hyd_result=false;
		this->risk_flags.risk_state_flag=false;

		this->with_project_save=true;

		//close the database
		this->db_con_close();

		//reset the window title
		this->setWindowTitle(this->project_manager.get_main_window_title().c_str());
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
}
//Copy a project
void Main_Wid::copy_project(void){
	this->set_text2statusbar("Copy project...", 0);
	this->setEnabled(false);
	this->reset_error_warning();
	this->project_manager.copy_open_project(this->system_database, this);

	if(this->project_manager.thread_pro_copy!=NULL && Error::get_number_sys_errors()==0){
		QObject::connect(this->project_manager.thread_pro_copy,SIGNAL(finished()),this,SLOT(threads_copy_proj_finished()));
		this->project_manager.thread_pro_copy->start();
	}
	else{
		this->setEnabled(true);
	}
}
//The project copy thread is finished
void Main_Wid::threads_copy_proj_finished(void){
	this->setEnabled(true);
	this->set_ready2statusbar("");
	this->project_manager.delete_pro_copy_thread();
	ostringstream cout;
	cout<<  "Project copy is finished!"<<endl;
	Sys_Common_Output::output_system->output_txt(&cout,false);
}
//Create the project tables
void Main_Wid::create_project_database_tables(void){
	if(Sys_Project::get_project_type()==_sys_project_type::proj_hyd_file){
		return;
	}

	//open database
	try{
		this->alloc_newdatabase();
		this->system_database->set_input_given(this->project_manager.get_database_connection_param(), this->project_manager.get_complete_project_file_name());
		//this->system_database->set_input_byfile(this->project_manager.get_complete_project_file_name());
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	try{
		//create a schema
		if(this->project_manager.get_database_connection_param().driver_type==_sys_driver_type::POSTGRESQL){
			ostringstream filter;
			filter <<"CREATE SCHEMA " << this->project_manager.get_complete_project_database_schemata_name();
			QSqlQuery my_query(filter.str().c_str(), *this->system_database->get_database());
			my_query.exec();
		}

		//create the table
		QObject::connect(this, SIGNAL(send_table_creation_is_finished()), this, SLOT(tables_creation_manager()));
		this->setEnabled(false);
		emit send_txt2statusbar("Create project database tables...",0);
		this->tables_creation_manager();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
}
//Create the project tables for an upgraded project
void Main_Wid::upgrade_create_project_database_tables(void){
	try{
		if(this->system_database==NULL){
			this->alloc_newdatabase();
		}
		if(this->system_database->check_con_status()==false){
			this->system_database->open_database();
		}
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	try{
		//create the table
		QObject::connect(this, SIGNAL(send_table_creation_is_finished()), this, SLOT(tables_creation_manager()));
		this->setEnabled(false);
		emit send_txt2statusbar("Upgrade project database tables...",0);
		this->tables_creation_manager();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
}
//Managing the table creation threads od the modules
void Main_Wid::tables_creation_manager(void){
	this->table_create_flag=true;
		if(this->project_manager.get_project_type()==_sys_project_type::proj_all){
			if(this->fpl_tables_created==false){
				this->create_fpl_system_database_tables();
				return;
			}
			if(this->hyd_tables_created==false){
				this->create_hyd_system_database_tables();
				return;
			}
			if(this->dam_tables_created==false){
				this->create_dam_system_database_tables();
				return;
			}

			if(this->risk_tables_created==false){
				this->create_risk_system_database_tables();
				return;
			}
			if(this->cost_tables_created==false){
				this->create_cost_system_database_tables();
				return;
			}
			if(this->madm_tables_created==false){
				this->create_madm_system_database_tables();
				return;
			}
			if(this->alt_tables_created==false){
				this->create_alt_system_database_tables();
				return;
			}
		}
		else if(this->project_manager.get_project_type()==_sys_project_type::proj_hyd || this->project_manager.get_project_type() == _sys_project_type::proj_hyd_temp){
			if(this->hyd_tables_created==false){
				this->create_hyd_system_database_tables();
				return;
			};
		}
		else if(this->project_manager.get_project_type()==_sys_project_type::proj_risk){
			if(this->fpl_tables_created==false){
				this->create_fpl_system_database_tables();
				return;
			}
			if(this->hyd_tables_created==false){
				this->create_hyd_system_database_tables();
				return;
			}
			if(this->dam_tables_created==false){
				this->create_dam_system_database_tables();
				return;
			}

			if(this->risk_tables_created==false){
				this->create_risk_system_database_tables();
				return;
			}
		}
		else if(this->project_manager.get_project_type()==_sys_project_type::proj_dam_hyd){
			if(this->hyd_tables_created==false){
				this->create_hyd_system_database_tables();
				return;
			}
			if(this->dam_tables_created==false){
				this->create_dam_system_database_tables();
				return;
			}
		}
		else if(this->project_manager.get_project_type()==_sys_project_type::proj_dam){
			if(this->hyd_tables_created==false){
				this->create_hyd_system_database_tables();
				return;
			}
			if(this->dam_tables_created==false){
				this->create_dam_system_database_tables();
				return;
			}
		}
		else if(this->project_manager.get_project_type()==_sys_project_type::proj_fpl){
			if(this->fpl_tables_created==false){
				this->create_fpl_system_database_tables();
				return;
			}
		}

		QObject::disconnect(this, SIGNAL(send_table_creation_is_finished()), this, SLOT(tables_creation_manager()));
		this->hyd_tables_created=false;
		this->dam_tables_created=false;
		this->fpl_tables_created=false;
		this->risk_tables_created=false;
		this->madm_tables_created=false;
		this->alt_tables_created=false;
		this->cost_tables_created=false;
		//..introduce further modules
		try{
			this->table_create_flag=false;
			this->menu_enable_checkdb();
			//all tables are created; set up the data tree
			this->treeWidget_data->set_up_tree_data();
			this->tabWidget_outtext->setCurrentWidget(this->tab_systemtxt);
			//set the file view

			this->treeview_file->set_view();
			this->setEnabled(true);
		}
		catch(Error msg){
			this->setEnabled(true);
			msg.output_msg(0);
			emit send_txt2statusbar("Error", 0);
		}
		this->close_all_tables();
		emit send_txt2statusbar("Ready", 0);
}
//Managing the table check threads of the modules
void Main_Wid::tables_check_manager(void){
	this->table_create_flag=true;
	if(this->project_manager.get_project_type()==_sys_project_type::proj_all){
		//set system parameters
		this->set_risk_state_flag(this->project_manager.get_stored_risk_state());

		if(this->fpl_tables_created==false){
			this->check_fpl_system_database_tables();
			return;
		}
		if(this->hyd_tables_created==false){
			this->check_hyd_system_database_tables();
			return;
		}
		if(this->dam_tables_created==false){
			this->check_dam_system_database_tables();
			return;
		}

		if(this->risk_tables_created==false){
			this->check_risk_system_database_tables();
			return;
		}
		if(this->cost_tables_created==false){
			this->check_cost_system_database_tables();
			return;
		}
		if(this->madm_tables_created==false){
			this->check_madm_system_database_tables();
			return;
		}
		if(this->alt_tables_created==false){
			this->check_alt_system_database_tables();
			return;
		}
	}
	else if(this->project_manager.get_project_type()==_sys_project_type::proj_hyd || this->project_manager.get_project_type() == _sys_project_type::proj_hyd_temp){
		if(this->hyd_tables_created==false){
			this->check_hyd_system_database_tables();
			return;
		};
	}
	else if(this->project_manager.get_project_type()==_sys_project_type::proj_risk){
		//set system parameters
		this->set_risk_state_flag(this->project_manager.get_stored_risk_state());

		if(this->fpl_tables_created==false){
			this->check_fpl_system_database_tables();
			return;
		}
		if(this->hyd_tables_created==false){
			this->check_hyd_system_database_tables();
			return;
		}
		if(this->dam_tables_created==false){
			this->check_dam_system_database_tables();
			return;
		}

		if(this->risk_tables_created==false){
			this->check_risk_system_database_tables();
			return;
		}
	}
	else if(this->project_manager.get_project_type()==_sys_project_type::proj_dam_hyd){
		if(this->hyd_tables_created==false){
			this->check_hyd_system_database_tables();
			return;
		}
		if(this->dam_tables_created==false){
			this->check_dam_system_database_tables();
			return;
		}
	}
	else if(this->project_manager.get_project_type()==_sys_project_type::proj_dam){
		if(this->hyd_tables_created==false){
			this->check_hyd_system_database_tables();
			return;
		}
		if(this->dam_tables_created==false){
			this->check_dam_system_database_tables();
			return;
		}
	}
	else if(this->project_manager.get_project_type()==_sys_project_type::proj_fpl){
		if(this->fpl_tables_created==false){
			this->check_fpl_system_database_tables();
			return;
		}
	}

	if(this->number_error_table_check>0){
		Error msg=this->set_error(1);
		Sys_Common_Output::output_excep->set_focus_on_display();
		msg.output_msg(0);
		this->with_project_save=false;
		Sys_Project::set_logfile_save_flag(true);
		QObject::disconnect(this, SIGNAL(send_table_check_is_finished()), this, SLOT(tables_check_manager()));
		this->hyd_tables_created=false;
		this->dam_tables_created=false;
		this->fpl_tables_created=false;
		this->risk_tables_created=false;
		this->madm_tables_created=false;
		this->alt_tables_created=false;
		this->cost_tables_created=false;
		//..introduce further modules

		this->close_project();
		this->setEnabled(true);
	}
	else{
		QObject::disconnect(this, SIGNAL(send_table_check_is_finished()), this, SLOT(tables_check_manager()));
		this->hyd_tables_created=false;
		this->dam_tables_created=false;
		this->fpl_tables_created=false;
		this->risk_tables_created=false;
		this->madm_tables_created=false;
		this->alt_tables_created=false;
		this->cost_tables_created=false;
		//..introduce further modules

		try{
			this->table_create_flag=false;
			this->menu_enable_checkdb();
			//all tables are created; set up the data tree
			this->treeWidget_data->set_up_tree_data();
			this->tabWidget_outtext->setCurrentWidget(this->tab_systemtxt);
			//set the file view

			this->treeview_file->set_view();
			this->setEnabled(true);
		}
		catch(Error msg){
			this->setEnabled(true);
			msg.output_msg(0);
			emit send_txt2statusbar("Error", 0);
		}

		this->set_system_state(this->project_manager.get_stored_system_state());
		this->close_all_tables();

		emit send_txt2statusbar("Ready", 0);
	}
}
//Save the project parameter
void Main_Wid::save_project_parameter(void){
	if(Sys_Project::get_project_name()==label::not_set){
		QMessageBox::information(this, "Project Info", "No project is set for saving");
		return;
	}
	try{
		this->project_manager.save_project_paramater();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
}
//Edit the project parameter
void Main_Wid::edit_project_parameter(void){
	if(Sys_Project::get_project_name()==label::not_set){
		QMessageBox::information(this, "Project Info", "No project is set for editing");
		return;
	}

	try{
		bool project_is_upgrade=false;
		if(this->project_manager.edit_project_parameter(this, &project_is_upgrade)==true){
			if(Sys_Project::get_project_type()!=_sys_project_type::proj_hyd_file && project_is_upgrade==true){
				this->alloc_newdatabase();
				this->system_database->set_input_byfile(this->project_manager.get_project_file_name());
				this->upgrade_create_project_database_tables();
				this->enable_menu_project_open(true);
			}
		}
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
}
//Read an existing project in
void Main_Wid::read_existing_project(void){
	//read project parameters
	try{
		this->project_manager.read_existing_project();
	}
	catch(Error msg){
		msg.output_msg(0);
		this->with_project_save=false;
		Sys_Project::set_logfile_save_flag(true);
		this->close_project();
		this->setEnabled(true);
		emit send_txt2statusbar("Ready", 0);
		this->reset_error_warning();
		return;
	}

	if(this->project_manager.get_project_type()!=_sys_project_type::proj_hyd_file){
		//open database
		try{
			emit send_txt2statusbar("Read project database parameter...", 0);
			this->alloc_newdatabase();
			this->system_database->set_input_byfile(this->project_manager.get_project_file_name());
			if(this->system_database->check_con_status()==false){
				this->close_project();
				return;
			}

			emit send_txt2statusbar("Ready", 0);
		}
		catch(Error msg){
			this->project_manager.set_database_connection_param(this->system_database->get_database_conn_param());
			msg.output_msg(0);
			this->with_project_save=false;
			Sys_Project::set_logfile_save_flag(true);
			this->close_project();
			return;
		}
		this->project_manager.set_database_connection_param(this->system_database->get_database_conn_param());
		this->setWindowTitle(this->project_manager.get_main_window_title().c_str());

		//check the table
		QObject::connect(this, SIGNAL(send_table_check_is_finished()), this, SLOT(tables_check_manager()));
		this->enable_menu_project_open(false);
		this->setEnabled(false);
		this->number_error_table_check=0;
		emit send_txt2statusbar("Check for project updates...", 0);
		//update project version
		try{
			if (Sys_Project::get_project_type() != _sys_project_type::proj_hydrol && Sys_Project::get_project_type() != _sys_project_type::proj_hyd_temp) {
				this->version_update.check_update_hyd_table_obs_point(this->system_database->get_database());
				this->version_update.check_update_hyd_table_polysegment_name(this->project_manager.get_project_file_name());
				this->version_update.check_update_hyd_table_system_member_sync(this->system_database->get_database(), this->project_manager.get_project_file_name());
				this->version_update.check_update_fpl_table_result_sec_type_dike(this->system_database->get_database(), this->project_manager.get_project_file_name());
				this->version_update.check_update_fpl_table_dune_results(this->system_database->get_database());
				this->version_update.check_update_hyd_table_elem_result_smax(this->system_database->get_database(), this->project_manager.get_project_file_name());
				this->version_update.check_update_hyd_table_global_param(this->system_database->get_database(), this->project_manager.get_project_file_name());
				this->version_update.check_update_hyd_table_instat_results(this->system_database->get_database());
				this->version_update.check_update_hyd_table_instat_results_rv(this->system_database->get_database(), this->project_manager.get_project_file_name());
				this->version_update.check_update_hyd_view_bound2elements_profile(this->system_database->get_database());
				this->version_update.check_update_fpl_output_control(this->system_database->get_database());
				this->version_update.check_update_dam_ci(this->system_database->get_database(), this->project_manager.get_project_file_name());
				this->version_update.check_update_connect_results_dam_ci(this->system_database->get_database(), this->project_manager.get_project_file_name());
				this->version_update.check_update_dam_ci_elements(this->system_database->get_database(), this->project_manager.get_project_file_name());
				this->version_update.check_update_ci_reults(this->system_database->get_database(), this->project_manager.get_project_file_name());
			}
			
		}
		catch(Error msg){
			msg.output_msg(0);
			this->with_project_save=false;
			Sys_Project::set_logfile_save_flag(true);
			this->close_project();
			return;
		}
		emit send_txt2statusbar("Check project database tables...", 0);
		this->tables_check_manager();
	}
	else{
		this->setWindowTitle(this->project_manager.get_main_window_title().c_str());
		this->enable_menu_project_open(false);
		emit send_txt2statusbar("Ready", 0);
	}

	//Sys_Common_Output::output_system->output_txt(&cout, false);

	if (this->task_file_name != label::not_set) {
		this->task_flag = true;
		//Send a that task-
		QObject::connect(this, SIGNAL(send_task_by_file_start()), this, SLOT(start_task_by_file()));
	}



}
//Output the project parameters to display/console
void Main_Wid::show_project_param(void){
	if(Sys_Project::get_project_name()==label::not_set){
		ostringstream cout;
		cout << "No project is set!"<<endl;
		Sys_Common_Output::output_system->output_txt(&cout,false);
	}
	else{
		this->project_manager.output_project_param();
		if(this->project_manager.get_project_type()!=_sys_project_type::proj_hyd_file){
			this->db_parameter();
		}
	}
}
//Get the warning number and set it to the statusbar widget
void Main_Wid::get_warning_number(const int number, const QString tooltip){
	this->status_wid->set_warning_number(number, tooltip);
}
//Get the error number and set it to the statusbar widget
void Main_Wid::get_error_number(const int number, const QString tooltip){
	this->status_wid->set_error_number(number, tooltip);
}
//Get the workspace memory text and set it to the statusbar widget
void Main_Wid::get_memorytxt(const QString txt, const QString tooltip){
	this->status_wid->set_required_memory(txt, tooltip);
}
//Set "ready" to status bar widget
void Main_Wid::set_ready2statusbar(QString txt){
	if(txt.length()==0){
		this->statusBar()->showMessage("Ready", 0);
	}
}
//Set text to status bar widget
void Main_Wid::set_text2statusbar(QString txt, int time){
	this->statusBar()->showMessage(txt, time);
	ostringstream cout;
	cout << txt.toStdString() << endl;
	Sys_Common_Output::output_system->output_txt(&cout, true);
}
//Get a text for a fatal error and open a dialog
void Main_Wid::get_fatal_error(const QString text){
	QMessageBox fatal(QMessageBox::Critical,"Fatal error", text, QMessageBox::Ok,this);
	//set the font type with fixed pitch
	QFont myfont("Courier", 8);
	fatal.setFont(myfont);
	//show it
	fatal.exec();
}
//Recieve the text from the output class SYS and give it to the corresponding display tab widget
void Main_Wid::txt_to_system_out(const QString txt){
	//lock interaction
	this->textEdit_systemtxt->setTextInteractionFlags(Qt::NoTextInteraction);
	//move the  curser to the end
	this->textEdit_systemtxt->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	//set text
	this->textEdit_systemtxt->insertPlainText(txt);
	//free interaction
	this->textEdit_systemtxt->setTextInteractionFlags(Qt::TextSelectableByMouse);
}
//Recieve the text from the output class FPL and give it to the corresponding display tab widgett
void Main_Wid::txt_to_fpl_out(const QString txt){
	//lock interaction
	this->textEdit_fpltxt->setTextInteractionFlags(Qt::NoTextInteraction);
	//move the  curser to the end
	this->textEdit_fpltxt->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	//set text
	this->textEdit_fpltxt->insertPlainText(txt);
	//free interaction
	this->textEdit_fpltxt->setTextInteractionFlags(Qt::TextSelectableByMouse);
}
//Recieve the text from the output class EXCEP and give it to the corresponding display tab widget
void Main_Wid::txt_to_excep_out(const QString txt){
	//lock interaction
	this->textEdit_exceptiontxt->setTextInteractionFlags(Qt::NoTextInteraction);
	//move the curser to the end
	this->textEdit_exceptiontxt->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	//set text
	this->textEdit_exceptiontxt->insertPlainText(txt);
	//free interaction
	this->textEdit_exceptiontxt->setTextInteractionFlags(Qt::TextSelectableByMouse);
}
//Recieve the text from the output class HYD and give it to the corresponding display tab widget
void Main_Wid::txt_to_hyd_out(QString txt){
	//lock interaction
	this->textEdit_hydtxt->setTextInteractionFlags(Qt::NoTextInteraction);
	//move the curser to the end
	this->textEdit_hydtxt->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	//set text
	this->textEdit_hydtxt->insertPlainText(txt);
	//free interaction
	this->textEdit_hydtxt->setTextInteractionFlags(Qt::TextSelectableByMouse);
}
//Recieve the text from the output class MADM and give it to the corresponding display tab widget
void Main_Wid::txt_to_madm_out(QString txt){
	//lock interaction
	this->textEdit_madmtxt->setTextInteractionFlags(Qt::NoTextInteraction);
	//move the curser to the end
	this->textEdit_madmtxt->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	//set text
	this->textEdit_madmtxt->insertPlainText(txt);
	//free interaction
	this->textEdit_madmtxt->setTextInteractionFlags(Qt::TextSelectableByMouse);
}
//Recieve the text from the output class DAM and give it to the corresponding display tab widget
void Main_Wid::txt_to_dam_out(QString txt){
	//lock interaction
	this->textEdit_damtxt->setTextInteractionFlags(Qt::NoTextInteraction);
	//move the curser to the end
	this->textEdit_damtxt->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	//set text
	this->textEdit_damtxt->insertPlainText(txt);
	//free interaction
	this->textEdit_damtxt->setTextInteractionFlags(Qt::TextSelectableByMouse);
}
//Recieve the text from the output class RISK and give it to the corresponding display tab widget
void Main_Wid::txt_to_risk_out(QString txt){
	//lock interaction
	this->textEdit_risktxt->setTextInteractionFlags(Qt::NoTextInteraction);
	//move the curser to the end
	this->textEdit_risktxt->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	//set text
	this->textEdit_risktxt->insertPlainText(txt);
	//free interaction
	this->textEdit_risktxt->setTextInteractionFlags(Qt::TextSelectableByMouse);
}
//Recieve the text from the output class ALT and give it to the corresponding display tab widget
void Main_Wid::txt_to_alt_out(QString txt){
	//lock interaction
	this->textEdit_alttxt->setTextInteractionFlags(Qt::NoTextInteraction);
	//move the curser to the end
	this->textEdit_alttxt->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	//set text
	this->textEdit_alttxt->insertPlainText(txt);
	//free interaction
	this->textEdit_alttxt->setTextInteractionFlags(Qt::TextSelectableByMouse);
}
//Recieve the text from the output class COST and give it to the corresponding display tab widget
void Main_Wid::txt_to_cost_out(QString txt){
	//lock interaction
	this->textEdit_costtxt->setTextInteractionFlags(Qt::NoTextInteraction);
	//move the curser to the end
	this->textEdit_costtxt->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	//set text
	this->textEdit_costtxt->insertPlainText(txt);
	//free interaction
	this->textEdit_costtxt->setTextInteractionFlags(Qt::TextSelectableByMouse);
}
//Recieve the text from the output class HYDROL and give it to the corresponding display tab widget
void Main_Wid::txt_to_hydrol_out(QString txt) {
	//lock interaction
	this->textEdit_hydroltxt->setTextInteractionFlags(Qt::NoTextInteraction);
	//move the curser to the end
	this->textEdit_hydroltxt->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	//set text
	this->textEdit_hydroltxt->insertPlainText(txt);
	//free interaction
	this->textEdit_hydroltxt->setTextInteractionFlags(Qt::TextSelectableByMouse);

}
//________
//menu sys
//Close all database tables (menu/sys/common)
void Main_Wid::close_all_tables(void){
	ostringstream text;
	text << "Close all database tables " << endl;
	Sys_Common_Output::output_system->output_txt(&text,true);

	Hyd_Hydraulic_System::close_hyd_database_tables();
	Dam_Damage_System::close_dam_database_tables();
	Fpl_Calculation::close_fpl_database_tables();
	Risk_System::close_risk_database_tables();
	Madm_System::close_madm_database_tables();
	Alt_System::close_alt_database_tables();
	Cost_System::close_cost_database_tables();
}
//Delete all file in the logfile archiv (menu/sys/common)
void Main_Wid::delete_logfile_archiv(void){
	if(Sys_Project::get_project_name()==label::not_set){
		return;
	}
	QDir my_dir;
	ostringstream buff;
	string buffer;

	buff <<Sys_Project::get_main_path()<<sys_label::folder_logfile_archiv;
	buffer=buff.str();
	if(my_dir.exists(buffer.c_str())==true){
		my_dir.cd(buffer.c_str());
		QStringList list;
		list=my_dir.entryList(QDir::Files);
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"All logfiles ("<<list.count()<<") in the archiv will be deleted!"<< endl;
		txt<<"Do you want to continue?" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			return;
		}
		for(int i=0; i< list.count(); i++){
			my_dir.remove(list.at(i));
		}
		my_dir.rmdir(buffer.c_str());
	}
}
//Set a new path (menu/sys/project)
void Main_Wid::set_new_path(void){
	if(Sys_Project::get_project_name()==label::not_set){
		Sys_Diverse_Text_Dia my_dia2(true,this);
		my_dia2.set_dialog_question("No project is set!");
		my_dia2.start_dialog();
		return;
	}

	Sys_Multi_Filechooser_Dia my_dia(this);
	QIcon my_icon;
	my_dia.set_number_file_browser(1,my_icon);
	my_dia.set_window_title("Change working directory...");
	my_dia.set_main_text_label("Choose a new working directory for this project");
	my_dia.get_ptr_file_browser(0)->set_text_label("");
	my_dia.get_ptr_file_browser(0)->set_current_path(Sys_Project::get_current_path().c_str());
	my_dia.get_ptr_file_browser(0)->transfer_file2path_chooser(true);

	if(my_dia.start_dialog()==true){
		Sys_Project::set_current_path(my_dia.get_ptr_file_browser(0)->get_current_path());
	}
}
//Delete files in the logfile archiv (menu/sys/common)
void Main_Wid::delete_file_logfile_archiv(void){
	if(Sys_Project::get_project_name()==label::not_set){
		return;
	}
	QDirModel my_model;
	my_model.setReadOnly(true);
	QDir my_dir;
	ostringstream buff;
	string buffer;

	Sys_Delete_Logfile_Dia my_dia(this);

	buff <<Sys_Project::get_main_path()<<sys_label::folder_logfile_archiv;
	buffer=buff.str();
	if(my_dir.exists(buffer.c_str())==true){
		my_dir.cd(buffer.c_str());

		my_dia.set_dialog(&my_model, &my_dir);

		if(my_dia.start_dialog()==false){
			return;
		}
		else{
			QModelIndexList selected_list;
			selected_list=my_dia.get_model()->selectionModel()->selectedRows();
			int number_selected=selected_list.count();

			if(number_selected==0){
				return;
			}
			//second dialog
			Sys_Diverse_Text_Dia dialog2;
			ostringstream txt;
			txt<<number_selected<<" selected logfiles in the archiv will be deleted!"<< endl;
			txt<<"Do you want to continue?" << endl;
			dialog2.set_dialog_question(txt.str());
			bool flag2=dialog2.start_dialog();
			if(flag2==false){
				return;
			}
			else{
				QString file;
				for(int i=0; i< number_selected; i++){
					file=my_model.filePath(selected_list.at(i));
					my_dir.remove(file);
				}
			}
		}
	}
	else{
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"The log-file archiv do not exists!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
	}
}
//Close the application (menu SYS)
void Main_Wid::my_close(const bool dialog) {

	ostringstream text;
	this->close_project();
	this->db_con_close();
	this->close_text_searcher();

	if (dialog == true) {
		text << "   ProMaIDeS will be closed!   " << endl;
		text << "    Thanks for joining us      " << endl;
		QMessageBox::about(this, "Closing...", text.str().c_str());
	}

	this->close_flag=true;

	this->close();
}
//Close the widget for closing the threads
void Main_Wid::close_thread_finished(_sys_close_types close_type){
	this->setEnabled(true);
	this->delete_thread_closing_manager();
	emit send_txt2statusbar("Ready...",0);

	switch(close_type){
		case _sys_close_types::app_close:
			this->my_close();
			break;
		case _sys_close_types::pro_close:
			this->close_project();
			break;
		case _sys_close_types::pro_new_open:
			this->open_new_project();
			break;
		case _sys_close_types::pro_exist_open:
			this->open_existing_project();
			break;
		default:
			this->my_close();
	}
}
//Set the database connection by hand via dialog (menu SYS/database)
void Main_Wid::db_con_dialog(void){
	try{
		this->alloc_newdatabase();
		this->system_database->set_input_dia_all();
		this->tabWidget_outtext->setCurrentWidget(this->tab_systemtxt);
		this->menu_enable_checkdb();
	}
	catch(Error msg){
		msg.output_msg(0);
	}
}
//Set the database connection per file; the file is asked via a dialog (menu SYS/database)
void Main_Wid::db_con_file_dialog(void){
	try{
		this->alloc_newdatabase();
		this->system_database->set_input_file_dialog();
		this->menu_enable_checkdb();
	}
	catch(Error msg){
		msg.output_msg(0);
	}
}
//Set the database connection directly; it is for development (menu SYS/database))
void Main_Wid::db_con_development(void){
	try{
		this->alloc_newdatabase();
		this->system_database->set_input_development();
		this->system_database->output_members();
		this->tabWidget_outtext->setCurrentWidget(this->tab_systemtxt);
		this->menu_enable_checkdb();
	}
	catch(Error msg){
		msg.output_msg(0);
	}
}
//Open the database again from the project file
void Main_Wid::db_con_open(void){
	if(this->project_manager.get_project_file_name()==label::not_set || this->project_manager.get_project_file_name().empty()==true){
		QMessageBox::critical(this,"No project file","Can not open the database. No project file is set!");
		return;
	}
	//open database
	try{
		this->alloc_newdatabase();
		this->reset_exception_new_action();
		this->system_database->set_input_byfile(this->project_manager.get_project_file_name());
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->menu_enable_checkdb();
	this->tabWidget_outtext->setCurrentWidget(this->tab_systemtxt);
}
//Output the database connection parameter to display SYS (menu SYS/database)
void Main_Wid::db_parameter(void){
	this->alloc_newdatabase();
	this->system_database->output_members();
	this->tabWidget_outtext->setCurrentWidget(this->tab_systemtxt);
}
//Output the database connection status to display SYS (menu SYS/database)
void Main_Wid::db_con_status(void){
	this->alloc_newdatabase();
	this->system_database->output_con_status();
	this->tabWidget_outtext->setCurrentWidget(this->tab_systemtxt);
}
//Close the database connection (menu SYS/database)
void Main_Wid::db_con_close(void){
	if(this->system_database!=NULL){
		//disconnect the sql-browser
		this->browser_database->disconnect_db();
		//close the datbase
		this->system_database->close_database();

		QSqlDatabase::removeDatabase(sys_label::str_sys.c_str());
		//reset the menu enable
		this->menu_enable_checkdb();
	}
}
//Show dialog that the database connection is broken during calculation; try to reconnect to database
void Main_Wid::database_connection_breaks(QString time){
	if(this->system_database!=NULL){
		///Reconnection dialog, if the database connection breaks unattended
		Sys_Reconnect_Db_Dia my_dia(this);

		QObject::connect(Sys_Exception_Sender::self(), SIGNAL(send_reconnection_try_dia_close()), &my_dia, SLOT(close_dialog()));

		ostringstream txt;
		txt<<"The database connection is not established!"<< endl;
		txt<<"Trying to reconnect to the database "<< this->system_database->get_database_conn_param().database_name <<" since "<< time.toStdString()<<" !"<< endl;
		txt<<"Do you want to stop the reconnection tries?"<<endl;
		my_dia.set_dialog_question(txt.str());
		this->setEnabled(false);
		my_dia.start_dialog();
		this->setEnabled(true);
	}
}
//Clean the database table view of the project browser
void Main_Wid::clean_db_view(void){
	this->allocate_db_table_view();
}
//Check and get the risk state from the statusbar
void Main_Wid::check_change_risk_state(bool flag){
	if(Sys_Project::get_project_type()==_sys_project_type::proj_all || Sys_Project::get_project_type()==_sys_project_type::proj_risk){
		if(flag==true){
			try{
				//do some cheks before
				//fpl-checks
				this->setEnabled(false);
				this->statusBar()->showMessage("Check system for risk calculation...", 0);
				//if(Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
				//	Sys_Diverse_Text_Dia dialog2(true);
				//	ostringstream txt;
				//	txt<<"There have to be some FPL-section set in database before any risk calculations can be done!"<< endl;
				//	dialog2.set_dialog_question(txt.str());
				//	dialog2.start_dialog();
				//	this->status_wid->get_ptr_risk_state_check_box()->setChecked(false);
				//	this->setEnabled(true);
				//	this->statusBar()->showMessage("Ready", 0);
				//	return;
				//}
				string text_buffer;
				if(Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id())==true &&
					Fpl_Section::check_set_by_hand_section_result(this->system_database->get_database(), this->system_state.get_sys_system_id(), &text_buffer)==true){
					Sys_Diverse_Text_Dia dialog2(true);
					ostringstream txt;
					txt<<"The reliability results of FPL-section with section type set_by_hand have to be set before any risk calculations can be done!"<< endl;
					txt<<"Further informations: " << endl;
					txt<<text_buffer;
					txt<<endl;
					txt<<"Set the reliability results of the FPL-section(s)!"<<endl;
					dialog2.set_dialog_question(txt.str());
					dialog2.adjustSize();
					dialog2.start_dialog();
					this->status_wid->get_ptr_risk_state_check_box()->setChecked(false);
					this->setEnabled(true);
					this->statusBar()->showMessage("Ready", 0);
					return;
				}
				//hyd-checks
				if(Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database())==false){
					Sys_Diverse_Text_Dia dialog2(true);
					ostringstream txt;
					txt<<"A HYD-base scenario must be set before any risk calculations can be done!"<< endl;
					dialog2.set_dialog_question(txt.str());
					dialog2.start_dialog();
					this->status_wid->get_ptr_risk_state_check_box()->setChecked(false);
					this->setEnabled(true);
					this->statusBar()->showMessage("Ready", 0);
					return;
				}
				//combined hyd-fpl-checks
				if(Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id())==true &&
					Fpl_Section::check_all_combined2hyd_system(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
					Sys_Diverse_Text_Dia dialog2(true);
					ostringstream txt;
					txt<<"All FPL-section(s) and the HYD-system have to be combined before any risk calculations can be done!"<< endl;
					dialog2.set_dialog_question(txt.str());
					dialog2.start_dialog();
					this->status_wid->get_ptr_risk_state_check_box()->setChecked(false);
					this->setEnabled(true);
					this->statusBar()->showMessage("Ready", 0);
					return;
				}
				//dam-checks
				if(Dam_Damage_System::check_some_raster_set(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
					Sys_Diverse_Text_Dia dialog2(true);
					ostringstream txt;
					txt<<"DAM-raster have to be set before any risk calculations can be done!"<< endl;
					dialog2.set_dialog_question(txt.str());
					dialog2.start_dialog();
					this->status_wid->get_ptr_risk_state_check_box()->setChecked(false);
					this->setEnabled(true);
					this->statusBar()->showMessage("Ready", 0);
					return;
				}
				//combined dam-hyd-checks
				if(Dam_Damage_System::check_all_raster_connected2hyd(this->system_database->get_database(), &text_buffer, this->system_state.get_sys_system_id())==false){
					Sys_Diverse_Text_Dia dialog2(true);
					ostringstream txt;
					txt<<"The DAM-raster have to be connected to the HYD-system before any risk calculations can be done!"<< endl;
					txt<<"Further informations: " << endl;
					txt<<text_buffer;
					txt<<endl;
					txt<<"Connect the DAM-raster(s)/-point(s) to the HYD-system!"<<endl;
					dialog2.set_dialog_question(txt.str());
					dialog2.adjustSize();
					dialog2.start_dialog();
					this->status_wid->get_ptr_risk_state_check_box()->setChecked(false);
					this->setEnabled(true);
					this->statusBar()->showMessage("Ready", 0);
					return;
				}
			}
			catch(Error msg){
				this->setEnabled(true);
				this->statusBar()->showMessage("Ready", 0);
				msg.output_msg(0);
				return;
			}
			this->setEnabled(true);
			this->statusBar()->showMessage("Ready", 0);

			if(this->system_state.get_sys_system_id().area_state==0 && this->system_state.get_sys_system_id().measure_nr==0){
				Sys_Multi_CheckBox_Dia my_dia(this);
				my_dia.set_button_txt("&Check", "&To RISK-calc");
				//set up dialog
				QIcon icon;
				icon.addFile(":risk_icon");

				my_dia.set_number_check_boxes(3,icon);
				stringstream buffer;
				buffer << "Do yo want ot check the system before a RISK-calculation?"<< endl;
				my_dia.set_main_text_label(buffer.str());
				buffer.str("");
				my_dia.set_window_title("Choose systems for checking");
				my_dia.get_ptr_check_box(0)->setText("Check HYD-system");
				my_dia.get_ptr_check_box(1)->setText("Check DAM-system");
				my_dia.get_ptr_check_box(2)->setText("Check idealisation HYD2FPL");
				if(Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
					my_dia.get_ptr_check_box(2)->setChecked(false);
					my_dia.get_ptr_check_box(2)->setEnabled(false);
				}

				//start dialog
				bool check_hyd=false;
				bool check_dam=false;
				bool check_fpl2hyd=false;
				if(my_dia.start_dialog()==true){
					check_hyd=my_dia.get_bool_check_box(0);
					check_dam=my_dia.get_bool_check_box(1);
					check_fpl2hyd=my_dia.get_bool_check_box(2);
					if(check_hyd==true){
						this->check_hyd_system_database();
					}
					if(check_dam==true){
						this->check_damage_system();
					}
					if(check_fpl2hyd==true && check_hyd==false){
						this->check_ideal_hyd_system_fpl();
					}
					else if(check_fpl2hyd==true && check_hyd==true){
						QObject::connect(this->hyd_calc, SIGNAL(finished()), this, SLOT(check_ideal_hyd_system_fpl()));
					}
					this->status_wid->get_ptr_risk_state_check_box()->setChecked(false);
					return;
				}
			}
		}

		if(flag==true){
			RiskGui_Result_Reliability_Dia my_dia(this);
			bool fpl_important=Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id());
			if(my_dia.start_dialog(this->system_database->get_database(), this->system_state.get_sys_system_id(), fpl_important)==false){
				flag=false;
				this->status_wid->get_ptr_risk_state_check_box()->setChecked(flag);
				this->risk_flags.reliability_hyd_result=false;
				this->risk_flags.reliability_fpl_result=false;
				this->risk_flags.reliability_dam_result=false;
				this->status_wid->reset_tooltip_risk_state_check_box();
			}
			else{
				//get from dialog
				bool buffer_ca_applied=this->risk_flags.catchment_risk_applied;
				bool buffer_no_applied=this->risk_flags.nobreak_risk_applied;
				this->risk_flags=my_dia.get_risk_state_flags();
				this->risk_flags.catchment_risk_applied=buffer_ca_applied;
				this->risk_flags.nobreak_risk_applied=buffer_no_applied;
				if(this->system_state.get_sys_system_id().measure_nr==0 &&this->system_state.get_sys_system_id().area_state==0){
					this->risk_flags.catchment_risk_applied=true;
					this->risk_flags.nobreak_risk_applied=true;
				}
				this->status_wid->set_tooltip_risk_state_check_box(this->risk_flags.reliability_fpl_result, this->risk_flags.reliability_dam_result, this->risk_flags.reliability_hyd_result);
				//delete the generated risk results
				this->delete_risk_relevant_results_database();
				this->reset_exception_new_action();
			}
		}
		else{
				Sys_Diverse_Text_Dia dialog2;
				ostringstream txt;
				txt<<"By leaving the RISK-State the generated risk results of this system state are irretrievable deleted in database!" <<endl;
				txt <<"It is further recommended to delete also the generated HYD, DAM and FPL results in database!" << endl;
				txt<<"For deleting please check the box below!"<< endl;
				txt<<"Do you want to continue?" << endl;
				dialog2.set_dialog_question(txt.str());
				dialog2.setIcon(QMessageBox::Critical);
				QCheckBox my_check;
				my_check.setText("Delete HYD, DAM and FPL results?");
				dialog2.setCheckBox(&my_check);

				bool flag2=dialog2.start_dialog();
				if (flag2 == true) {

					this->reset_exception_new_action();
					this->risk_flags.reliability_hyd_result = false;
					this->risk_flags.reliability_fpl_result = false;
					this->risk_flags.reliability_dam_result = false;
					this->status_wid->reset_tooltip_risk_state_check_box();
					//delete the generated risk results
					
					this->delete_risk_result_database(my_check.isChecked());
	
					this->risk_flags.reliability_hyd_result=false;
					this->risk_flags.reliability_fpl_result=false;
					this->risk_flags.reliability_dam_result=false;
					if(this->system_state.get_sys_system_id().measure_nr==0 &&this->system_state.get_sys_system_id().area_state==0){
						this->risk_flags.catchment_risk_applied=true;
						this->risk_flags.nobreak_risk_applied=true;
					}
					this->status_wid->reset_tooltip_risk_state_check_box();
				}
				else{
					flag=true;
					this->status_wid->get_ptr_risk_state_check_box()->setChecked(flag);
				}
		}

		this->menu_RISK_system->setEnabled(flag);
		this->menu_new_area_measure_state->setEnabled(flag);
		if(Sys_Project::get_project_type()==_sys_project_type::proj_all && this->system_database!=NULL && this->system_database->check_con_status()==true && this->table_create_flag==false){
			QStringList list;

			Alt_System::load_stringlist_current_alt(this->system_database->get_database(),this->system_state.get_ptr_sys_system_id()->area_state,&list);
			Alt_System::close_table_measure();
			if(list.count()>0 && this->risk_flags.risk_state_flag==true){
				this->action_madm_calc_database->setEnabled(true);
			}
			else{
				this->action_madm_calc_database->setEnabled(false);
			}
		}
		else{
			this->action_madm_calc_database->setEnabled(false);
		}
		/*if(this->system_state.get_sys_system_id().measure_nr>1 && flag==true){
			this->action_madm_calc_database->setEnabled(flag);
		}
		else{
			this->action_madm_calc_database->setEnabled(false);
		}*/

		this->action_switch_measure_state->setEnabled(flag);
		this->menu_HYD_system->setEnabled(!flag);
		this->menu_DAM_system->setEnabled(!flag);
		this->menu_FPL_system->setEnabled(!flag);
		//hyd
		this->menu_hyd_import->setEnabled(!flag);
		this->action_delete_boundary_scenario->setEnabled(!flag);
		//dam
		this->menu_dam_ecn->setEnabled(!flag);
		this->menu_dam_eco->setEnabled(!flag);
		this->menu_dam_pys->setEnabled(!flag);
		this->menu_dam_pop->setEnabled(!flag);
		this->menu_dam_sc->setEnabled(!flag);
		this->menu_dam_CI->setEnabled(!flag);
		this->action_dam_connect->setEnabled(!flag);
		//fpl
		this->action_fpl_import_file->setEnabled(!flag);
		this->action_delete_fpl_section->setEnabled(!flag);

		//risk state
		this->risk_flags.risk_state_flag=flag;
		this->project_manager.set_current_risk_state(this->risk_flags);
		this->save_project_parameter();
	}
}
//Reset the warning- and error-number (menu SYS/common)
void Main_Wid::reset_error_warning(void){
	if(this->some_thread_running()==true){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"The exceptions can not be reset during a thread is running!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	Error::reset_counter();
	Warning::reset_counter();
	//clear the display
	this->textEdit_exceptiontxt->clear();
	Sys_Common_Output::output_excep->welcome_to_gui();
	//close the file and remove it
	if(Sys_Project::get_logfile_save_flag()==false){
		Sys_Common_Output::output_excep->close_file();
		Sys_Common_Output::output_excep->remove_file();
	}
	else{
		Sys_Common_Output::output_excep->save_logfile2archiv();
	}
	Sys_Common_Output::output_excep->remove_file();
	//ostringstream cout;
	//cout << "Error and Warning Numbers are reset"<< endl;
	//cout << "Exception file is closed"<<endl;
	//cout << "Exception display is cleared" << endl;
	//Sys_Common_Output::output_system->output_txt(cout.str(),false);
}
//Reset the display SYS (menu SYS/common)
void Main_Wid::reset_system_outputtxt(void){
	//clear the display
	this->textEdit_systemtxt->clear();
	Sys_Common_Output::output_system->welcome_to_gui();
}
//Set a new output flag for the system modul(menu SYS/common/System output text))
void Main_Wid::set_system_outputflag(void){
	Sys_Common_Output::output_system->set_detailflag_gui(this);
}
//Set a new output logfile for the system modul(menu SYS/common)
void Main_Wid::set_system_outputlogfile(void){
	Sys_Common_Output::output_system->set_new_logfile_gui(this);
}
//Clear all output displays(menu SYS/common)
void Main_Wid::clear_all_output_displays(void){
	this->reset_alt_outputtxt();
	this->reset_cost_outputtxt();
	this->reset_dam_outputtxt();
	this->reset_fpl_outputtxt();
	this->reset_hyd_outputtxt();
	this->reset_risk_outputtxt();
	this->reset_madm_outputtxt();
	this->reset_system_outputtxt();
}
//_________
//task
//Task by file action
void Main_Wid::start_task_by_file(void) {

	

	if (this->close_flag == true) {
		return;
	}

	if (this->task_flag==true) {
		try {
			ostringstream cout;
			this->enable_menu(false);

			//read task for the first time
			if (this->count_task == 0) {
				cout << "Task modus is activated!" << endl;
				Sys_Common_Output::output_system->output_txt(&cout, false);
				this->read_task_file();

				this->output_task_list();

			}
			bool task_found = false;
			for (int i = 0; i<this->number_task; i++) {
				if (this->closing_thread != NULL) {
					break;
				}
				if (i == this->count_task) {
					this->enable_menu(false);
					ostringstream prefix;
					prefix << "TASK" << "> ";
					Sys_Common_Output::output_system->set_userprefix(&prefix);
					cout << "Peforming task no. " << this->count_task << endl;
					Sys_Common_Output::output_system->output_txt(&cout, false);
					this->count_task++;
					this->interpret_task(this->task_list.at(i));
					Sys_Common_Output::output_system->rewind_userprefix();
					
					task_found = true;
					break;

				}
				

			}
			if(task_found==false){
				//close programm
				this->enable_menu(false);
				ostringstream prefix;
				prefix << "TASK" << "> ";
				Sys_Common_Output::output_system->set_userprefix(&prefix);
				cout << "Last task finished! " << this->count_task << endl;
				cout << "TOTAL ERROR - NUMBER IN TASK" << endl;
				cout << "     Error(s) : " << this->total_err_task << endl;
				cout << "     For Warnings and Errors please check the exception- and the module-logfile(s)!" <<  endl;
				
				cout << "Close... " << endl;
				Sys_Common_Output::output_system->output_txt(&cout, false);
				Sys_Common_Output::output_system->rewind_userprefix();
				this->my_close(false);

			}
		}
		catch (Error msg) {
			msg.output_msg(0);
			return;
		}


	}
}
//Interpret the task to do
void Main_Wid::interpret_task(QList<QVariant> list) {
	string buff_module;
	buff_module = list.at(0).toString().toStdString();

		ostringstream cout;
		if (buff_module == "FPL") {
			this->start_task_fpl(list);

		}
		else if (buff_module == "HYD") {
			this->start_task_hyd(list);
		}
		else {
			cout << "Unknown module in task-list " << buff_module << endl;
			Sys_Common_Output::output_system->output_txt(&cout, false);
		}


}
//Interpret and start the task to do for FPL-module
void Main_Wid::start_task_fpl(QList<QVariant> list) {
	string buff_command;

	buff_command = list.at(1).toString().toStdString();

	ostringstream cout;
	//import
	if (buff_command == "import") {
		QStringList buff_sec;
		for (int i = 2; i < list.count(); i++) {
			buff_sec.append(list.at(i).toString());
		}
		this->number_new_sec = buff_sec.count();
		cout << "Import following FPL-section(s) to database "  << endl;
		for (int i = 0; i < buff_sec.count(); i++) {
			cout <<i+1<<" "<< buff_sec.at(i).toStdString() << endl;
		}
		Sys_Common_Output::output_system->output_txt(&cout, false);
		this->import_section2database_task(buff_sec);

	}
	//calc determ
	else if (buff_command == "calc_determ") {
		QList<int> buff_sec;
		buff_sec=this->check_key_word_fpl(list.at(2).toString(), this->number_new_sec);
		if (buff_sec.count() == 0) {
			for (int i = 2; i < list.count(); i++) {
				buff_sec.append(list.at(i).toInt());
			}
		}
		cout << "Deterministic calculation of following FPL-section(s) " << endl;
		for (int i = 0; i < buff_sec.count(); i++) {
			cout << i + 1 << " " << buff_sec.at(i) << endl;
		}
		Sys_Common_Output::output_system->output_txt(&cout, false);

		this->perform_determ_calculation_task(buff_sec);

	}
	else if (buff_command == "calc_mc") {
		QList<int> buff_sec;
		buff_sec = this->check_key_word_fpl(list.at(2).toString(), this->number_new_sec);
		if (buff_sec.count() == 0) {
			for (int i = 2; i < list.count(); i++) {
				buff_sec.append(list.at(i).toInt());
			}
		}
		cout << "MC calculation of following FPL-section(s) " << endl;
		for (int i = 0; i < buff_sec.count(); i++) {
			cout << i + 1 << " " << buff_sec.at(i) << endl;
		}
		Sys_Common_Output::output_system->output_txt(&cout, false);

		this->perform_mc_calculation_task(buff_sec);


	}
	else if (buff_command == "calc_frc") {
		QList<int> buff_sec;
		buff_sec = this->check_key_word_fpl(list.at(2).toString(), this->number_new_sec);
		if (buff_sec.count() == 0) {
			for (int i = 2; i < list.count(); i++) {
				buff_sec.append(list.at(i).toInt());
			}
		}
		cout << "FRC calculation of following FPL-section(s) " << endl;
		for (int i = 0; i < buff_sec.count(); i++) {
			cout << i + 1 << " " << buff_sec.at(i) << endl;
		}
		Sys_Common_Output::output_system->output_txt(&cout, false);

		this->perform_frc_calculation_task(buff_sec);


	}
	else if (buff_command == "delete") {
		QList<int> buff_sec;
		buff_sec = this->check_key_word_fpl(list.at(2).toString(), this->number_new_sec);
		if (buff_sec.count() == 0) {
			for (int i = 2; i < list.count(); i++) {
				buff_sec.append(list.at(i).toInt());
			}
		}
		cout << "Delete following FPL-section(s) " << endl;
		for (int i = 0; i < buff_sec.count(); i++) {
			cout << i + 1 << " " << buff_sec.at(i) << endl;
		}
		Sys_Common_Output::output_system->output_txt(&cout, false);

		this->delete_fpl_section_database_task(buff_sec);


	}
	else if (buff_command == "export_determ") {
		QList<int> buff_sec;
		buff_sec = this->check_key_word_fpl(list.at(2).toString(), this->number_new_sec);
		if (buff_sec.count() == 0) {
			for (int i = 2; i < list.count(); i++) {
				buff_sec.append(list.at(i).toInt());
			}
		}
		cout << "Export deterministic results of following FPL-section(s) " << endl;
		for (int i = 0; i < buff_sec.count(); i++) {
			cout << i + 1 << " " << buff_sec.at(i) << endl;
		}
		Sys_Common_Output::output_system->output_txt(&cout, false);

		this->export_results_determ_task(buff_sec);


	}
	else if (buff_command == "export_mc") {
		QList<int> buff_sec;
		buff_sec = this->check_key_word_fpl(list.at(2).toString(), this->number_new_sec);
		if (buff_sec.count() == 0) {
			for (int i = 2; i < list.count(); i++) {
				buff_sec.append(list.at(i).toInt());
			}
		}
		cout << "Export Monte-Carlo results of following FPL-section(s) " << endl;
		for (int i = 0; i < buff_sec.count(); i++) {
			cout << i + 1 << " " << buff_sec.at(i) << endl;
		}
		Sys_Common_Output::output_system->output_txt(&cout, false);

		this->export_results_mc_task(buff_sec);


	}
	else if (buff_command == "export_frc") {
		QList<int> buff_sec;
		buff_sec = this->check_key_word_fpl(list.at(2).toString(), this->number_new_sec);
		if (buff_sec.count() == 0) {
			for (int i = 2; i < list.count(); i++) {
				buff_sec.append(list.at(i).toInt());
			}
		}
		cout << "Export FRC results of following FPL-section(s) " << endl;
		for (int i = 0; i < buff_sec.count(); i++) {
			cout << i + 1 << " " << buff_sec.at(i) << endl;
		}
		Sys_Common_Output::output_system->output_txt(&cout, false);

		this->export_results_frc_task(buff_sec);


	}
	else {
		cout << "Unknown command in task-list " << buff_command << endl;
		Sys_Common_Output::output_system->output_txt(&cout, false);
		Sys_Common_Output::output_system->rewind_userprefix();
		this->count_task++;
		emit send_task_by_file_start();
	}
}
//Interpret and start the task to do for HYD-module
void Main_Wid::start_task_hyd(QList<QVariant> list) {
	string buff_command;

	buff_command = list.at(1).toString().toStdString();

	ostringstream cout;
	//import
	if (buff_command == "calc_file") {
		QStringList buff_sec;
		for (int i = 2; i < list.count(); i++) {
			QString buff = Sys_Project::get_main_path().c_str() + list.at(i).toString();
			buff_sec.append(buff);
		}
		
		cout << "Calculate following HYD-system(s) per file " << endl;
		for (int i = 0; i < buff_sec.count(); i++) {
			cout << i + 1 << " " << buff_sec.at(i).toStdString() << endl;
		}
		Sys_Common_Output::output_system->output_txt(&cout, false);
		this->set_hydcalc_per_file_task(buff_sec);

	}
	else if (buff_command == "add_db") {
		QStringList buff_sec;
		for (int i = 2; i < list.count(); i++) {
			buff_sec.append(list.at(i).toString());
		}
		cout << "Add following HYD-scenario(s) to database " << endl;
		if (buff_sec.count() == 4) {
			cout << " " << buff_sec.at(0).toStdString() << " " << buff_sec.at(1).toStdString()<< " "<< buff_sec.at(2).toInt() << " " << buff_sec.at(3).toDouble() << endl;
			Sys_Common_Output::output_system->output_txt(&cout, false);
			this->add_hyd_boundary_sz_file2database_task(buff_sec);
		}
		else {
			cout << "Wrong command; command for adding HYD-scenario is: HYD, add_db, relative_path_to_.ilm, scenario_name, annuality, probability_of_event " << endl;
			Sys_Common_Output::output_system->output_txt(&cout, false);
			Sys_Common_Output::output_system->rewind_userprefix();
			this->count_task++;
			emit send_task_by_file_start();
		}

	}
	else if (buff_command == "delete_db") {

		QList<int> buff_sec;
		buff_sec = this->check_key_word_hyd(list.at(2).toString());
		if (buff_sec.count() == 0) {
			for (int i = 2; i < list.count(); i++) {
				buff_sec.append(list.at(i).toInt());
			}
		}
		cout << "Delete following HYD-scenario(s) from database " << endl;
		for (int i = 0; i < buff_sec.count(); i++) {
			cout << i + 1 << " " << buff_sec.at(i) << endl;
		}
		Sys_Common_Output::output_system->output_txt(&cout, false);

		this->delete_selected_bound_sc_task(buff_sec);

	}
	else if (buff_command == "calc_db") {
		QList<int> buff_sec;
		buff_sec = this->check_key_word_hyd(list.at(2).toString());
		if (buff_sec.count() == 0) {
			for (int i = 2; i < list.count(); i++) {
				buff_sec.append(list.at(i).toInt());
			}
		}
		cout << "HYD calculation of following HYD-scenario(s) " << endl;
		for (int i = 0; i < buff_sec.count(); i++) {
			cout << i + 1 << " " << buff_sec.at(i) << endl;
		}
		Sys_Common_Output::output_system->output_txt(&cout, false);

		this->set_hydcalc_per_task(buff_sec);


	}
	else {
		cout << "Unknown command in task-list " << buff_command << endl;
		Sys_Common_Output::output_system->output_txt(&cout, false);
		Sys_Common_Output::output_system->rewind_userprefix();
		this->count_task++;
		emit send_task_by_file_start();
	}

}
//Check for the keywords in tasks NEW, ALL section ids
QList<int> Main_Wid::check_key_word_fpl(const QString key, const int last_number) {
	QList<int> sec_id;
	if (key == "ALL") {
		//fill sec_id
		sec_id = Fpl_Section::get_list_relevant_section_database(this->system_database->get_database(), this->system_state.get_sys_system_id());
		return sec_id;
	}
	else if (key == "NEW") {
		if (last_number==0) {
			return sec_id;
		}
		//fill sec_id
		sec_id = Fpl_Section::get_list_last_section_database(this->system_database->get_database(), this->system_state.get_sys_system_id(), last_number);
		return sec_id;
	}

	return sec_id;

}
//Check for the keywords in tasks NEW, ALL section ids
QList<int> Main_Wid::check_key_word_hyd(const QString key) {
	QList<int> sec_id;
	if (key == "ALL") {
		//fill sec_id
		QSqlQueryModel results;
		int buff_number = Hyd_Boundary_Szenario_Management::select_boundary_scenario_database(&results, this->system_database->get_database(), this->system_state.get_sys_system_id());
		if (buff_number == 0) {

			return sec_id;
		}
		else {
			for(int i=0;i< buff_number; i++){
				sec_id.append(results.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt());
			}
			return sec_id;
		}
		
	}
	else if (key == "NEW") {
		return this->new_hyd_sc_list;
	}
	


	return sec_id;

}
//________
//menu FPL
//Import fpl-section(s) from file to database (menu fpl/)
void Main_Wid::import_section2database(void){
	//allocate the thread
	try{
		this->allocate_fpl_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	//set thread specific members
	this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_import_file);
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	if(this->fpl_calc->set_system_number_file_gui()==false){
		this->delete_fpl_system();
		return;
	}
	this->reset_exception_new_action();
	//connect the thread when is finished
	QObject::connect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_calc_finished()));
	//start the thread
	this->fpl_calc->start();
	this->check_fpl_thread_is_running();
}
//Import fpl-section(s) from file to database from task
void Main_Wid::import_section2database_task(QStringList list_id) {
	//allocate the thread
	try {
		this->allocate_fpl_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	//set thread specific members
	this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_import_file);
	this->fpl_calc->set_ptr2database(this->system_database->get_database());

	this->fpl_calc->set_system_number_file_direct(list_id);
	//this->reset_exception_new_action();
	//connect the thread when is finished
	QObject::connect(this->fpl_calc, SIGNAL(finished()), this, SLOT(thread_fpl_calc_finished()));
	//start the thread
	this->fpl_calc->start();
	this->check_fpl_thread_is_running();

}
//Create the database tables for the fpl system
void Main_Wid::create_fpl_system_database_tables(void){
		//allocate the thread
		try{
			this->allocate_fpl_system();
		}
		catch(Error msg){
			msg.output_msg(0);
			return;
		}
		//connect the thread when is finished
		QObject::connect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_create_tables_finished()));
		//set thread specific members
		this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_create_tab);
		this->fpl_calc->set_ptr2database(this->system_database->get_database());
		//start the thread
		this->fpl_calc->start();
		this->check_fpl_thread_is_running();
		this->fpl_tables_created=true;
}
//Check for the normal end of the fpl create-tables thread
void Main_Wid::thread_fpl_create_tables_finished(void){
	if(this->fpl_calc!=NULL && this->fpl_calc->isRunning()==false){
		this->action_stop_fpl_calc->setEnabled(false);

		this->check_fpl_thread_is_running();

		//disconnect the thread when is finished
		QObject::disconnect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_create_tables_finished()));
		this->delete_fpl_system();
		emit send_table_creation_is_finished();
	}
}
//Check the database tables for the hydraulic system (menu fpl/common)
void Main_Wid::check_fpl_system_database_tables(void){
	try{
		this->allocate_fpl_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_check_tab);

	//connect the thread when is finished
	QObject::connect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_check_tables_finished()));

	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->fpl_calc->start();
	this->check_fpl_thread_is_running();
	this->fpl_tables_created=true;
}
//Check for the normal end of the fpl check-datbase tables thread
void Main_Wid::thread_fpl_check_tables_finished(void){
	if(this->fpl_calc!=NULL && this->fpl_calc->isRunning()==false){
		this->check_fpl_thread_is_running();

		this->number_error_table_check=this->number_error_table_check+this->fpl_calc->get_number_error();

		//disconnect the thread when is finished
		QObject::disconnect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_check_tables_finished()));
		this->delete_fpl_system();
		emit send_table_check_is_finished();
		if (this->task_flag == true && this->project_manager.get_project_type() == _sys_project_type::proj_fpl) {
			emit send_task_by_file_start();
		}

	}
}
//Check fpl-section(s) per file (menu fpl/Section check)
void Main_Wid::check_section_file(void){
	//allocate the thread
	try{
		this->allocate_fpl_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	//set thread specific members
	this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_check_sec_file);
	if(this->fpl_calc->set_system_number_file_gui()==false){
		this->delete_fpl_system();
		return;
	}
	this->reset_exception_new_action();
	//connect the thread when is finished
	QObject::connect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_calc_finished()));
	//start the thread
	this->fpl_calc->start();
	this->check_fpl_thread_is_running();
}
//Check fpl-section(s) per database (menu fpl/Section check)
void Main_Wid::check_section_database(void){
	//allocate the thread
	try{
		this->allocate_fpl_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	if(this->fpl_calc->ask_section2handle(this, _fpl_thread_type::fpl_check_sec_db)==false){
		this->delete_fpl_system();
		return;
	}
	else{
		this->reset_exception_new_action();
		//connect the thread when is finished
		QObject::connect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_calc_finished()));
		//start the thread
		this->fpl_calc->start();
		this->check_fpl_thread_is_running();
	}
}
//Export deterministic results of a FPL-section (menu fpl/Export results/)
void Main_Wid::export_results_determ(void) {
	//allocate the thread
	try {
		this->allocate_fpl_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	if (this->fpl_calc->ask_section2handle(this, _fpl_thread_type::fpl_export_determ_sec) == false) {
		this->delete_fpl_system();
		return;
	}
	else {
		this->reset_exception_new_action();
		//connect the thread when is finished
		QObject::connect(this->fpl_calc, SIGNAL(finished()), this, SLOT(thread_fpl_calc_finished()));
		//start the thread
		this->fpl_calc->start();
		this->check_fpl_thread_is_running();
	}
}
//Export deterministic results of a FPL-section from task
void Main_Wid::export_results_determ_task(QList<int> list_id) {
	try {
		this->allocate_fpl_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_export_determ_sec);


	this->fpl_calc->set_list_section_ids(list_id);

	//connect the thread when is finished
	QObject::connect(this->fpl_calc, SIGNAL(finished()), this, SLOT(thread_fpl_calc_finished()));
	//this->reset_exception_new_action();
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->fpl_calc->start();
	this->check_fpl_thread_is_running();
}
//Export Monte-Carlo results of a FPL-section (menu fpl/Export results/)
void Main_Wid::export_results_mc(void) {
	//allocate the thread
	try {
		this->allocate_fpl_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	if (this->fpl_calc->ask_section2handle(this, _fpl_thread_type::fpl_export_mc_sec) == false) {
		this->delete_fpl_system();
		return;
	}
	else {
		this->reset_exception_new_action();
		//connect the thread when is finished
		QObject::connect(this->fpl_calc, SIGNAL(finished()), this, SLOT(thread_fpl_calc_finished()));
		//start the thread
		this->fpl_calc->start();
		this->check_fpl_thread_is_running();
	}
}
//Export Monte-Carlo results of a FPL-section from task
void Main_Wid::export_results_mc_task(QList<int> list_id) {
	try {
		this->allocate_fpl_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_export_mc_sec);


	this->fpl_calc->set_list_section_ids(list_id);

	//connect the thread when is finished
	QObject::connect(this->fpl_calc, SIGNAL(finished()), this, SLOT(thread_fpl_calc_finished()));
	//this->reset_exception_new_action();
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->fpl_calc->start();
	this->check_fpl_thread_is_running();
}
//Export FRC-results of a FPL-section (menu fpl/Export results/)
void Main_Wid::export_results_frc(void) {
	//allocate the thread
	try {
		this->allocate_fpl_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	if (this->fpl_calc->ask_section2handle(this, _fpl_thread_type::fpl_export_frc_sec) == false) {
		this->delete_fpl_system();
		return;
	}
	else {
		this->reset_exception_new_action();
		//connect the thread when is finished
		QObject::connect(this->fpl_calc, SIGNAL(finished()), this, SLOT(thread_fpl_calc_finished()));
		//start the thread
		this->fpl_calc->start();
		this->check_fpl_thread_is_running();
	}
}
//Export FRC-results of a FPL-section from task
void Main_Wid::export_results_frc_task(QList<int> list_id) {
	try {
		this->allocate_fpl_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_export_frc_sec);


	this->fpl_calc->set_list_section_ids(list_id);

	//connect the thread when is finished
	QObject::connect(this->fpl_calc, SIGNAL(finished()), this, SLOT(thread_fpl_calc_finished()));
	//this->reset_exception_new_action();
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->fpl_calc->start();
	this->check_fpl_thread_is_running();
}
//Combine the fpl-system with the hydraulic system (menu fpl/HYD2FPL)
void Main_Wid::combine_fpl2hyd_system(void){
	try{
		if(Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database())==false){
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			txt<<"A HYD-base scenario must be set before a combining to the FPL-section(s) can be done!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return;
		}
		if(Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			txt<<"There have to be some FPL-section(s) set in database before a combining to the HYD-system can be done!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return;
		}

		//allocate the thread
		this->allocate_fpl_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	//set thread specific members
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_combine_fpl2hyd);
	//connect the thread when is finished
	QObject::connect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_calc_finished()));
	this->reset_exception_new_action();
	//start the thread
	this->fpl_calc->start();
	this->check_fpl_thread_is_running();
}
//Check the idealization of the hydraulic system by the fpl-system (menu fpl/HYD2FPL)
void Main_Wid::check_ideal_hyd_system_fpl(void){
	try{
		if(Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database())==false){
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			txt<<"A HYD-base scenario must be set before an idealisation of the HYD-system due to the FPL-section(s) can be done!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return;
		}
		if(Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			txt<<"There have to be some FPL-section(s) set in database before an idealisation of the HYD-system due to the FPL-section(s) can be done!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return;
		}
		if(Fpl_Section::check_all_combined2hyd_system(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			txt<<"The FPL-section and the HYD-system have to be combined before an idealisation of the HYD-system due to the FPL-section(s) can be done!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return;
		}
		this->allocate_fpl_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	//set thread specific members
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_check_ideal_fpl2hyd);
	//connect the thread when is finished
	QObject::connect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_calc_finished()));
	this->reset_exception_new_action();
	//start the thread
	this->fpl_calc->start();
	this->check_fpl_thread_is_running();
}
//Perform a deterministic calculation (menu fpl/calculation)
void Main_Wid::perform_determ_calculation(void){
	//allocate the thread
	try{
		this->allocate_fpl_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	//set thread specific members
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	if(this->fpl_calc->ask_section2handle(this, _fpl_thread_type::fpl_determ_selected)==false){
		this->delete_fpl_system();
		return;
	}
	else{
		//connect the thread when is finished
		QObject::connect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_calc_finished()));
		this->reset_exception_new_action();
		//start the thread
		this->fpl_calc->start();
		this->check_fpl_thread_is_running();
	}
}
//Perform a deterministic calculation from task
void Main_Wid::perform_determ_calculation_task(QList<int> list_id){
	try {
		this->allocate_fpl_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_determ_selected);


	this->fpl_calc->set_list_section_ids(list_id);

	//connect the thread when is finished
	QObject::connect(this->fpl_calc, SIGNAL(finished()), this, SLOT(thread_fpl_calc_finished()));
	//this->reset_exception_new_action();
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->fpl_calc->start();
	this->check_fpl_thread_is_running();

}
//Perform a monte-carlo calculation (menu fpl/calculation)
void Main_Wid::perform_mc_calculation(void){
	//allocate the thread
	try{
		this->allocate_fpl_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	//set thread specific members
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	if(this->fpl_calc->ask_section2handle(this, _fpl_thread_type::fpl_mc_selected)==false){
		this->delete_fpl_system();
		return;
	}
	else{
		//connect the thread when is finished
		QObject::connect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_calc_finished()));
		this->reset_exception_new_action();
		//start the thread
		this->fpl_calc->start();
		this->check_fpl_thread_is_running();
	}
}
//Perform a monte-carlo calculation from task
void Main_Wid::perform_mc_calculation_task(QList<int> list_id) {
	try {
		this->allocate_fpl_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_mc_selected);


	this->fpl_calc->set_list_section_ids(list_id);

	//connect the thread when is finished
	QObject::connect(this->fpl_calc, SIGNAL(finished()), this, SLOT(thread_fpl_calc_finished()));
	//this->reset_exception_new_action();
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->fpl_calc->start();
	this->check_fpl_thread_is_running();

}
//Perform a fragility curve calculation (menu fpl/calculation)
void Main_Wid::perform_frc_calculation(void){
	//allocate the thread
	try{
		this->allocate_fpl_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	//set thread specific members
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	if(this->fpl_calc->ask_section2handle(this, _fpl_thread_type::fpl_frc_selected)==false){
		this->delete_fpl_system();
		return;
	}
	else{
		//connect the thread when is finished
		QObject::connect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_calc_finished()));
		this->reset_exception_new_action();
		//start the thread
		this->fpl_calc->start();
		this->check_fpl_thread_is_running();
	}
}
//Perform a fragility curve calculation task
void Main_Wid::perform_frc_calculation_task(QList<int> list_id) {
	try {
		this->allocate_fpl_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_frc_selected);


	this->fpl_calc->set_list_section_ids(list_id);

	//connect the thread when is finished
	QObject::connect(this->fpl_calc, SIGNAL(finished()), this, SLOT(thread_fpl_calc_finished()));
	//this->reset_exception_new_action();
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->fpl_calc->start();
	this->check_fpl_thread_is_running();



}
//Perform a test for the random generator (menu fpl/calculation)
void Main_Wid::perform_test_random(void){
	//allocate the thread
	try{
		this->allocate_fpl_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	//set thread specific members
	this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_test_random);
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	if(this->fpl_calc->random_checker->ask_check_setting_per_dia()==false){
		this->delete_fpl_system();
		return;
	}
	//connect the thread when is finished
	QObject::connect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_calc_finished()));
	this->reset_exception_new_action();
	//start the thread
	this->fpl_calc->start();
	this->check_fpl_thread_is_running();
}
//Delete section of the fpl-module in database (menu fpl/Delete section...)
void Main_Wid::delete_fpl_section_database(void){
	//allocate the thread
	try{
		this->allocate_fpl_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	//set thread specific members
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	if(this->fpl_calc->ask_section2handle(this, _fpl_thread_type::fpl_del_section)==false){
		this->delete_fpl_system();
		return;
	}
	else{
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"All data of the selected fpl-section(s) will be deleted!"<< endl;
		txt<<"Do you want to continue?" << endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.setIcon(QMessageBox::Warning);
		bool flag2=dialog2.start_dialog();
		if(flag2==true){
			//connect the thread when is finished
			QObject::connect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_calc_finished()));
			//start the thread
			this->fpl_calc->start();
			this->check_fpl_thread_is_running();
		}
		else{
			this->delete_fpl_system();
			return;
		}
	}
}
///Delete section of the fpl-module in database task
void Main_Wid::delete_fpl_section_database_task(QList<int> list_id) {
	try {
		this->allocate_fpl_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_del_section);


	this->fpl_calc->set_list_section_ids(list_id);

	//connect the thread when is finished
	QObject::connect(this->fpl_calc, SIGNAL(finished()), this, SLOT(thread_fpl_calc_finished()));
	//this->reset_exception_new_action();
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->fpl_calc->start();
	this->check_fpl_thread_is_running();
}
//Restore default values of the fpl-module in database tables (menu fpl/Restore standard...)
void Main_Wid::restore_default_database_table(void){
		//allocate the thread
	try{
		this->allocate_fpl_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_restore_default);
	if(this->fpl_calc->ask_restore_default_database(this)==false){
		this->delete_fpl_system();
		return;
	}
	else{
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"All data in the selected tables will be deleted and replaced with the default values!"<< endl;
		txt<<"Do you want to continue?" << endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.setIcon(QMessageBox::Warning);
		bool flag2=dialog2.start_dialog();
		if(flag2==true){
			//connect the thread when is finished
			QObject::connect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_calc_finished()));
			//start the thread
			this->fpl_calc->start();
			this->check_fpl_thread_is_running();
		}
		else{
			this->delete_fpl_system();
			return;
		}
	}
}
//The fragility curve is defined by the user (menu fpl/)
void Main_Wid::set_frc_by_hand(void){
	//allocate the thread
	try{
		this->allocate_fpl_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	//set thread specific members
	this->fpl_calc->set_ptr2database(this->system_database->get_database());
	if(this->fpl_calc->ask_section2handle(this, _fpl_thread_type::fpl_frc_by_hand)==false){
		this->delete_fpl_system();
		return;
	}
	else{
		//start the not as aa thread
		this->fpl_calc->run();
	}
}
//User aborted the fpl calculation thread (menu fpl)
void Main_Wid::stop_fplcalc_thread(void){
	//be sure that the thread is allocated and started
	if(this->fpl_calc!=NULL && this->fpl_calc->isRunning()==true){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"Do you want stop the FPL-thread?" << endl;
		txt<<"Result data will be lost" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			return;
		}
		else{
			this->fpl_calc->set_stop_thread_flag(true);
		}
	}
}
//Check for the normal end of the fpl calculation thread
void Main_Wid::thread_fpl_calc_finished(void){
	if(this->fpl_calc!=NULL && this->fpl_calc->isRunning()==false){
		this->check_fpl_thread_is_running();

		//disconnect the thread when is finished
		QObject::disconnect(this->fpl_calc,SIGNAL(finished()),this,SLOT(thread_fpl_calc_finished()));
		if(this->fpl_calc->random_checker!=NULL && this->fpl_calc->get_number_error()==0){
			try{
				this->fpl_calc->random_checker->get_results2plot(this->tab_fpl_data_view);
				this->tabWidget_data_view->setCurrentWidget(this->tab_data_data);
				this->tabWidget_data_output->setCurrentWidget(this->tab_fpl_data_view);
			}
			catch(Error msg){
				msg.output_msg(1);
			}
		}
		if(this->fpl_calc->get_thread_type()==_fpl_thread_type::fpl_del_section || this->fpl_calc->get_thread_type()==_fpl_thread_type::fpl_determ_selected
			|| this->fpl_calc->get_thread_type()==_fpl_thread_type::fpl_import_file ||
			this->fpl_calc->get_thread_type()==_fpl_thread_type::fpl_mc_selected ||
			this->fpl_calc->get_thread_type()==_fpl_thread_type::fpl_frc_selected
			|| this->fpl_calc->get_thread_type()==_fpl_thread_type::fpl_frc_by_hand ||
			this->fpl_calc->get_thread_type()==_fpl_thread_type::fpl_restore_default){
			emit send_delete2refresh_data_view();
		}
		this->delete_fpl_system();

		if (this->task_flag == true) {
			emit send_task_by_file_start();
		}
	}
}
//Reset the display FPL (menu fpl/common)
void Main_Wid::reset_fpl_outputtxt(void){
	//clear the display
	this->textEdit_fpltxt->clear();
	Sys_Common_Output::output_fpl->welcome_to_gui();
	Sys_Common_Output::output_fpl->reset_prefix_was_outputed();
	ostringstream cout;
	cout << "FPL-display is cleared" << endl;
	Sys_Common_Output::output_system->output_txt(cout.str(),false);
}
//Set a new output flag for the fpl modul(menu fpl/common)
void Main_Wid::set_fpl_outputflag(void){
	Sys_Common_Output::output_fpl->set_detailflag_gui(this);
}
//Set a new output logfile for the fpl modul(menu fpl/common)
void Main_Wid::set_fpl_outputlogfile(void){
	Sys_Common_Output::output_fpl->set_new_logfile_gui(this);
}
//Recieve if a module extern uses the fpl thread
void Main_Wid::fpl_module_extern_runs(bool flag){
	this->status_wid->set_fplthread(flag);
}
//________
//menu HYD
//Set a hydraulic calculation per file (menu hyd/calculation/per file)
void Main_Wid::set_hydcalc_per_file(void){
	try{
		this->allocate_multi_hydraulic_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->hyd_calc->set_number_required_threads(this->project_manager.get_stored_hydraulic_state().number_threads);
	this->hyd_calc->set_required_output2file(this->project_manager.get_stored_hydraulic_state().file_output_required);
	this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_calculation);

	if(this->hyd_calc->set_system_number_file_gui()==false){
		this->delete_multi_hydraulic_system();
		return;
	}

	//connect the thread when is finished
	QObject::connect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_calc_finished()));
	QObject::connect(this->hyd_calc,SIGNAL(emit_number_threads(QString )),this,SLOT(catch_thread_number_hy_calc(QString )));
	this->action_stop_hyd_calc->setEnabled(true);

	this->reset_exception_new_action();
	//start calculation
	this->hyd_calc->start();
	if(Sys_Project::get_project_type()==_sys_project_type::proj_hyd_file){
		this->check_hyd_file_thread_is_running();
	}
	else{
		this->check_hyd_thread_is_running();
	}
}
//Set a hydraulic calculation per file via task
void Main_Wid::set_hydcalc_per_file_task(QStringList list_id) {
	try {
		this->allocate_multi_hydraulic_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	this->hyd_calc->set_number_required_threads(list_id.count());
	this->hyd_calc->set_required_output2file(this->project_manager.get_stored_hydraulic_state().file_output_required);
	this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_calculation);

	this->hyd_calc->set_system_number_file(&list_id);

	

	//connect the thread when is finished
	QObject::connect(this->hyd_calc, SIGNAL(finished()), this, SLOT(thread_hyd_calc_finished()));
	QObject::connect(this->hyd_calc, SIGNAL(emit_number_threads(QString)), this, SLOT(catch_thread_number_hy_calc(QString)));
	this->action_stop_hyd_calc->setEnabled(true);

	this->reset_exception_new_action();
	//start calculation
	this->hyd_calc->start();
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_file) {
		this->check_hyd_file_thread_is_running();
	}
	else {
		this->check_hyd_thread_is_running();
	}




}
//User aborted the hydraulic calculation; the hyd-thread is stopped (menu hyd)
void Main_Wid::stop_hydcalc_thread(void){
	//be sure that the thread is allocated and started
	if(this->hyd_calc!=NULL && this->hyd_calc->isRunning()==true){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"Do you want stop the HYD-thread?" << endl;
		txt<<"Result data will be lost" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			return;
		}
		else{
			this->hyd_calc->set_stop_thread_flag(true);
		}
	}
}
//Check for the normal end of the hyd calculation thread
void Main_Wid::thread_hyd_calc_finished(void){
	if(this->hyd_calc->isRunning()==false){
		this->action_stop_hyd_calc->setEnabled(false);

		if(Sys_Project::get_project_type()==_sys_project_type::proj_hyd_file){
			this->check_hyd_file_thread_is_running();
		}
		else{
			this->check_hyd_thread_is_running();
		}
		if(this->hyd_calc->get_thread_type()==_hyd_thread_type::hyd_del_sz || this->hyd_calc->get_thread_type()==_hyd_thread_type::hyd_data_import
			|| this->hyd_calc->get_thread_type()==_hyd_thread_type::hyd_add_sz){
			emit send_delete2refresh_data_view();
		}

		//disconnect the thread when is finished
		QObject::disconnect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_calc_finished()));

		this->delete_multi_hydraulic_system();
		if (this->task_flag == true) {
			emit send_task_by_file_start();
		}

	}
}
//Catch the number of threads, which are launched from the multiple hydraulic system for calculation
void Main_Wid::catch_thread_number_hy_calc(QString number){
	this->status_wid->set_hydthread(true, number.toStdString());
}
//Set a new output flag for the hyd modul(menu hyd/common)
void Main_Wid::set_hyd_outputflag(void){
	Sys_Common_Output::output_hyd->set_detailflag_gui(this);
}
//Set a new output logfile for the hyd modul(menu hyd/common)
void Main_Wid::set_hyd_outputlogfile(void){
	Sys_Common_Output::output_hyd->set_new_logfile_gui(this);
}
//Reset the display HYD (menu hyd/common)
void Main_Wid::reset_hyd_outputtxt(void){
	//clear the display
	this->textEdit_hydtxt->clear();
	Sys_Common_Output::output_hyd->welcome_to_gui();
	Sys_Common_Output::output_hyd->reset_prefix_was_outputed();
	ostringstream cout;
	cout << "HYD-display is cleared" << endl;
	Sys_Common_Output::output_system->output_txt(cout.str(),false);
}
//Set hydraulic system properties (menu hyd/common)
void Main_Wid::set_hydraulic_system_properties(void){
	HydGui_System_State_Dia my_dia(this);
	my_dia.set_current_system_data(this->project_manager.get_stored_hydraulic_state());
	bool flag;
	if(Sys_Project::get_project_type()==_sys_project_type::proj_hyd_file){
		flag=false;
	}
	else{
		flag=true;
	}
	if(my_dia.start_dialog(flag)==true){
		this->project_manager.set_current_hydraulic_state(my_dia.get_new_system_data());
	}
}
//Create the database tables for the hydraulic system (menu hyd/common)
void Main_Wid::create_hyd_system_database_tables(void){
		//allocate the thread
		try{
			this->allocate_multi_hydraulic_system();
		}
		catch(Error msg){
			msg.output_msg(0);
			return;
		}
		//connect the thread when is finished
		QObject::connect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_create_tables_finished()));
		//set thread specific members
		this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_create_tab);
		this->hyd_calc->set_ptr2database(this->system_database->get_database());
		//start the thread
		this->hyd_calc->start();
		this->check_hyd_thread_is_running();
}
//Check for the normal end of the hydraulic create-tables thread
void Main_Wid::thread_hyd_create_tables_finished(void){
	if(this->hyd_calc->isRunning()==false){
		this->check_hyd_thread_is_running();
		//disconnect the thread when is finished
		QObject::disconnect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_create_tables_finished()));
		this->delete_multi_hydraulic_system();
		this->hyd_tables_created=true;
		emit send_table_creation_is_finished();
	}
}
//Check the database tables for the hydraulic system (menu hyd/common)
void Main_Wid::check_hyd_system_database_tables(void){
		//allocate the thread
		try{
			this->allocate_multi_hydraulic_system();
		}
		catch(Error msg){
			msg.output_msg(0);
			return;
		}
		//connect the thread when is finished
		QObject::connect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_check_tables_finished()));
		//set thread specific members
		this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_check_tab);
		this->hyd_calc->set_ptr2database(this->system_database->get_database());
		//start the thread
		this->hyd_calc->start();
		this->check_hyd_thread_is_running();
}
//Check for the normal end of the hydraulic check-datbase tables thread
void Main_Wid::thread_hyd_check_tables_finished(void){
	if(this->hyd_calc->isRunning()==false){
		this->check_hyd_thread_is_running();

		this->number_error_table_check=this->number_error_table_check+this->hyd_calc->get_error_number();
		//disconnect the thread when is finished
		QObject::disconnect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_check_tables_finished()));
		this->delete_multi_hydraulic_system();
		this->hyd_tables_created=true;
		emit send_table_check_is_finished();
		if (this->task_flag == true && (this->project_manager.get_project_type() == _sys_project_type::proj_hyd || this->project_manager.get_project_type() == _sys_project_type::proj_hyd_temp)) {
			emit send_task_by_file_start();
		}
	}
}
//Import the hydraulic base system per file to a database (menu hyd)
void Main_Wid::import_hyd_basesystem_file2database(void){
	try{
		bool flag2=false;
		if(Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database())==true){
			Sys_Diverse_Text_Dia dialog2;
			ostringstream txt;
			txt<<"All existing HYD-input data in the database will be deleted."<< endl;
			txt<<"Do you want to continue?" << endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.setIcon(QMessageBox::Warning);
			flag2=dialog2.start_dialog();
		}
		else{
			flag2=true;
		}

		if(flag2==true){
			HydGui_System_Filechooser_Dia dialog;
			ostringstream txt;
			txt<< "Choose the global file of the HYD-system (.ilm). This system "<< endl;
			txt <<"will be the base system for all HYD-boundary scenarios, which will be defined"<< endl;
			txt <<"later. The id of the boundary scenario will be 1."<< endl;
			dialog.set_txt(txt.str());
			bool flag=dialog.start_dialog();
			string buffer=dialog.get_file_name();

			if(flag==true){
				if(buffer!=label::not_set){
					//reset the dam-raster connection flags to the hydraulic system
					if(Sys_Project::get_project_type()==_sys_project_type::proj_all || Sys_Project::get_project_type()==_sys_project_type::proj_risk ||
						Sys_Project::get_project_type()==_sys_project_type::proj_dam_hyd || Sys_Project::get_project_type()==_sys_project_type::proj_dam ){
						this->setEnabled(false);
						emit send_txt2statusbar("Delete damage results/Reset DAM2HYD-connection...", 0);
						Dam_Damage_System::reset_raster_hyd_connection_flag(this->system_database->get_database());
						Dam_Damage_System::delete_data_in_erg_table(this->system_database->get_database());
						emit send_txt2statusbar("Ready", 0);
						this->setEnabled(true);
					}
					//reset combine-flag of the fpl-system
					if(Sys_Project::get_project_type()==_sys_project_type::proj_all || Sys_Project::get_project_type()==_sys_project_type::proj_risk){
						this->setEnabled(false);
						emit send_txt2statusbar("Reset FPL2HYD-connection...", 0);
						Fpl_Section::reset_combined2hyd_system(this->system_database->get_database(), this->system_state.get_sys_system_id());
						emit send_txt2statusbar("Ready", 0);
						this->setEnabled(true);
					}

					//allocate the thread
					try{
						this->allocate_multi_hydraulic_system();
					}
					catch(Error msg){
						msg.output_msg(0);
						return;
					}
					//connect the thread when is finished
					QObject::connect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_import_finished()));
					//enable menu and actions
					this->action_stop_hyd_import->setEnabled(true);

					//set thread specific members
					this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_data_import);
					this->hyd_calc->set_ptr2database(this->system_database->get_database());
					this->hyd_calc->set_one_system_filename(buffer);
					this->reset_exception_new_action();
					//start the thread
					this->hyd_calc->start();
					this->check_hyd_thread_is_running();
				}
				else{
					Sys_Diverse_Text_Dia dialog2(true);
					ostringstream txt;
					QIcon icon;
					icon.addFile(":hyd_icon");
					txt<<"No file is set for the base scenario!"<< endl;
					dialog2.set_dialog_question(txt.str());
					dialog2.set_window_icon(icon);
					dialog2.start_dialog();
				}
			}
		}
	}
	catch(Error msg){
		msg.output_msg(0);
	}
}
//Import and add the hydraulic boundary szenarios per file to a database (menu hyd/import files to database)
void Main_Wid::add_hyd_boundary_sz_file2database(void){
	try{
		if(Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database())==false){
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			txt<<"A HYD-base scenario must be set before any additional boundary scenario(s) can be imported!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return;
		}

		try{
			this->allocate_multi_hydraulic_system();
		}
		catch(Error msg){
			msg.output_msg(0);
			return;
		}
		this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_add_sz);
		this->hyd_calc->set_ptr2database(this->system_database->get_database());
		if(this->hyd_calc->set_system_number_file_gui()==false){
			this->delete_multi_hydraulic_system();
			return;
		}

		this->reset_exception_new_action();
		//connect the thread when is finished
		QObject::connect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_import_finished()));
		this->action_stop_hyd_import->setEnabled(true);

		//start calculation
		this->hyd_calc->start();
		this->check_hyd_thread_is_running();
	}
	catch(Error msg){
		msg.output_msg(0);
	}
}
//Import and add the hydraulic boundary szenarios per file to a database via task
void Main_Wid::add_hyd_boundary_sz_file2database_task(QStringList list_data) {
	try {
		if (Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database()) == false) {
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			txt << "A HYD-base scenario must be set before any additional boundary scenario(s) can be imported!" << endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return;
		}

		try {
			this->allocate_multi_hydraulic_system();
		}
		catch (Error msg) {
			msg.output_msg(0);
			return;
		}
		this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_add_sz);
		this->hyd_calc->set_ptr2database(this->system_database->get_database());
		if (this->hyd_calc->set_system_number_file_direct(list_data, &this->new_hyd_sc_list) == false) {
			this->delete_multi_hydraulic_system();
			return;
		}

		this->reset_exception_new_action();
		//connect the thread when is finished
		QObject::connect(this->hyd_calc, SIGNAL(finished()), this, SLOT(thread_hyd_import_finished()));
		this->action_stop_hyd_import->setEnabled(true);

		//start calculation
		this->hyd_calc->start();
		this->check_hyd_thread_is_running();
	}
	catch (Error msg) {
		msg.output_msg(0);
	}

}
//User aborted the hydraulic file-import process; the hyd-thread is stopped (menu hyd)
void Main_Wid::stop_hydimport_thread(void){
	//be sure that the thread is allocated and started
	if(this->hyd_calc!=NULL && this->hyd_calc->isRunning()==true){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"Do you want stop the HYD-thread?" << endl;
		txt<<"Import data will be lost" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			return;
		}
		else{
			this->hyd_calc->set_stop_thread_flag(true);
		}
	}
}
//Check for the normal end of the hydraulic file import thread
void Main_Wid::thread_hyd_import_finished(void){
	if(this->hyd_calc->isRunning()==false){
		this->action_stop_hyd_import->setEnabled(false);

		this->check_hyd_thread_is_running();

		//disconnect the thread when is finished
		QObject::disconnect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_import_finished()));
		emit send_delete2refresh_data_view();
		this->delete_multi_hydraulic_system();
		if (this->task_flag == true) {
			emit send_task_by_file_start();
		}
	}
}
//Check the the hydraulic system (data, geometrical interception, coupling) per file (menu hyd/Model check)
void Main_Wid::check_hyd_system_perfile(void){
	try{
		//allocate the thread
		try{
			this->allocate_multi_hydraulic_system();
		}
		catch(Error msg){
			msg.output_msg(0);
			return;
		}
		this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_check_system);
		if(this->hyd_calc->set_system_number_file_gui()==false){
			this->delete_multi_hydraulic_system();
			return;
		}

		//connect the thread when is finished
		QObject::connect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_systemcheck_finished()));
		//enable menu and actions
		this->action_stop_hyd_check->setEnabled(true);

		//set thread specific members
		
		this->reset_exception_new_action();
		//start the thread
		this->hyd_calc->start();
		if(Sys_Project::get_project_type()==_sys_project_type::proj_hyd_file){
			this->check_hyd_file_thread_is_running();
		}
		else{
			this->check_hyd_thread_is_running();
		}
	}
	catch(Error msg){
		msg.output_msg(0);
	}
}
//Check the the hydraulic system (data, geometrical interception, coupling) per database (menu hyd/Model check)
void Main_Wid::check_hyd_system_database(void){
	try{
		//allocate the thread
		try{
			this->allocate_multi_hydraulic_system();
		}
		catch(Error msg){
			msg.output_msg(0);
			return;
		}
		//connect the thread when is finished
		QObject::connect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_systemcheck_finished()));
		//enable menu and actions
		this->action_stop_hyd_check->setEnabled(true);

		//set thread specific members
		this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_check_system);
		this->hyd_calc->set_ptr2database(this->system_database->get_database());
		this->reset_exception_new_action();
		//start the thread
		this->hyd_calc->start();
		this->check_hyd_thread_is_running();
	}
	catch(Error msg){
		msg.output_msg(0);
	}
}
//User aborted the check of the hydraulic system thread (menu hyd)
void Main_Wid::stop_hyd_systemcheck_thread(void){
	//be sure that the thread is allocated and started
	if(this->hyd_calc!=NULL && this->hyd_calc->isRunning()==true){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"Do you want stop the HYD-thread?" << endl;
		txt<<"Data will be lost" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			return;
		}
		else{
			this->hyd_calc->set_stop_thread_flag(true);
		}
	}
}
//Check for the normal end of the check of the hydraulic system thread
void Main_Wid::thread_hyd_systemcheck_finished(void){
	if(this->hyd_calc->isRunning()==false){
		//enable menu and actions
		this->action_stop_hyd_check->setEnabled(false);
		if(Sys_Project::get_project_type()==_sys_project_type::proj_hyd_file){
			this->check_hyd_file_thread_is_running();
		}
		else{
			this->check_hyd_thread_is_running();
		}

		this->status_wid->set_hydthread(false);
		//disconnect the thread when is finished
		QObject::disconnect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_systemcheck_finished()));
		this->delete_multi_hydraulic_system();
	}
}
//Set a hydraulic calculation per database (menu hyd/calculation/per database)
void Main_Wid::set_hydcalc_per_db(void){
	try{
		this->allocate_multi_hydraulic_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_calculation);

	this->hyd_calc->set_ptr2database(this->system_database->get_database());
	this->hyd_calc->set_number_required_threads(this->project_manager.get_stored_hydraulic_state().number_threads);
	this->hyd_calc->set_required_output2file(this->project_manager.get_stored_hydraulic_state().file_output_required);

	if(this->hyd_calc->ask_boundary_scenarios_per_dialog(this->system_database->get_database(), this)==0){
		this->delete_multi_hydraulic_system();
		return;
	}
	else{
		//connect the thread when is finished
		QObject::connect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_calc_finished()));
		QObject::connect(this->hyd_calc,SIGNAL(emit_number_threads(QString )),this,SLOT(catch_thread_number_hy_calc(QString )));

		this->action_Close_Connection->setEnabled(false);
		this->action_stop_hyd_calc->setEnabled(true);

		this->reset_exception_new_action();
		//start calculation
		this->hyd_calc->start();
		this->check_hyd_thread_is_running();
	}
}
//Set a hydraulic temperature calculation per database (menu hyd/calculation/temp per database)
void Main_Wid::set_hydtempcalc_per_db(void) {
	try {
		this->allocate_multi_hydraulic_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_temp_calculation);

	this->hyd_calc->set_ptr2database(this->system_database->get_database());
	this->hyd_calc->set_number_required_threads(this->project_manager.get_stored_hydraulic_state().number_threads);
	this->hyd_calc->set_required_output2file(this->project_manager.get_stored_hydraulic_state().file_output_required);

	if (this->hyd_calc->ask_boundary_scenarios_per_dialog(this->system_database->get_database(), this) == 0) {
		this->delete_multi_hydraulic_system();
		return;
	}
	else {
		//connect the thread when is finished
		QObject::connect(this->hyd_calc, SIGNAL(finished()), this, SLOT(thread_hyd_calc_finished()));
		QObject::connect(this->hyd_calc, SIGNAL(emit_number_threads(QString)), this, SLOT(catch_thread_number_hy_calc(QString)));

		this->action_Close_Connection->setEnabled(false);
		this->action_stop_hyd_calc->setEnabled(true);

		this->reset_exception_new_action();
		//start calculation
		this->hyd_calc->start();
		this->check_hyd_thread_is_running();
	}




}
//Set a hydraulic calculation per task
void Main_Wid::set_hydcalc_per_task(QList<int> list_id) {
	try {
		this->allocate_multi_hydraulic_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_calculation);

	this->hyd_calc->set_ptr2database(this->system_database->get_database());
	this->hyd_calc->set_number_required_threads(list_id.count());
	this->hyd_calc->set_required_output2file(this->project_manager.get_stored_hydraulic_state().file_output_required);

	if (this->hyd_calc->set_boundary_scenarios_per_list(this->system_database->get_database(), list_id) == 0) {
		this->delete_multi_hydraulic_system();
		
		return;
	}
	else {
		//connect the thread when is finished
		QObject::connect(this->hyd_calc, SIGNAL(finished()), this, SLOT(thread_hyd_calc_finished()));
		QObject::connect(this->hyd_calc, SIGNAL(emit_number_threads(QString)), this, SLOT(catch_thread_number_hy_calc(QString)));

		this->action_Close_Connection->setEnabled(false);
		this->action_stop_hyd_calc->setEnabled(true);

		this->reset_exception_new_action();
		//start calculation
		this->hyd_calc->start();
		this->check_hyd_thread_is_running();
	}


}
//Delete selected boundary scenarios (menu hyd/)
void Main_Wid::delete_selected_bound_sc(void){
	try{
		this->allocate_multi_hydraulic_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_del_sz);

	this->hyd_calc->set_ptr2database(this->system_database->get_database());

	if(this->hyd_calc->ask_boundary_scenarios_per_dialog(this->system_database->get_database(), this)==0){
		this->delete_multi_hydraulic_system();
	}
	else{
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"The selected hydraulic boundary scenarios will be deleted!"<< endl;
		txt<<"Do you want to continue?" << endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.setIcon(QMessageBox::Warning);
		bool flag2=dialog2.start_dialog();
		if(flag2==true){
			//connect the thread when is finished
			QObject::connect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_calc_finished()));
			this->action_Close_Connection->setEnabled(false);

			//delete damages
			if(Sys_Project::get_project_type()==_sys_project_type::proj_all || Sys_Project::get_project_type()==_sys_project_type::proj_risk ||
				Sys_Project::get_project_type()==_sys_project_type::proj_dam_hyd || Sys_Project::get_project_type()==_sys_project_type::proj_dam){
				this->setEnabled(false);
				emit send_txt2statusbar("Delete damage results...", 0);
				for(int i=0; i< this->hyd_calc->sz_bound_manager.get_number_sz(); i++){
					if(this->hyd_calc->sz_bound_manager.get_ptr_sz(i)->get_is_selected()==true){
						Dam_Damage_System::delete_data_in_erg_table(this->system_database->get_database(), this->hyd_calc->sz_bound_manager.get_ptr_sz(i)->get_id());
					}
				}
				emit send_txt2statusbar("Ready", 0);
				this->setEnabled(true);
			}

			//start calculation
			this->hyd_calc->start();
			this->check_hyd_thread_is_running();
		}
		else{
			this->delete_multi_hydraulic_system();
			return;
		}
	}
}
//Delete selected boundary scenarios via task)
void Main_Wid::delete_selected_bound_sc_task(QList<int> list_id) {
	try {
		this->allocate_multi_hydraulic_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_del_sz);

	this->hyd_calc->set_ptr2database(this->system_database->get_database());
	//not the first scenario!
	list_id.removeFirst();

	if (this->hyd_calc->set_boundary_scenarios_per_list(this->system_database->get_database(), list_id) == 0) {
		this->delete_multi_hydraulic_system();
		Sys_Common_Output::output_system->rewind_userprefix();
		emit send_task_by_file_start();
	}
	else {



		//connect the thread when is finished
		QObject::connect(this->hyd_calc, SIGNAL(finished()), this, SLOT(thread_hyd_calc_finished()));
		this->action_Close_Connection->setEnabled(false);

		//delete damages
		if (Sys_Project::get_project_type() == _sys_project_type::proj_all || Sys_Project::get_project_type() == _sys_project_type::proj_risk ||
			Sys_Project::get_project_type() == _sys_project_type::proj_dam_hyd || Sys_Project::get_project_type() == _sys_project_type::proj_dam) {
			this->setEnabled(false);
			emit send_txt2statusbar("Delete damage results...", 0);
			for (int i = 0; i < this->hyd_calc->sz_bound_manager.get_number_sz(); i++) {
				if (this->hyd_calc->sz_bound_manager.get_ptr_sz(i)->get_is_selected() == true) {
					Dam_Damage_System::delete_data_in_erg_table(this->system_database->get_database(), this->hyd_calc->sz_bound_manager.get_ptr_sz(i)->get_id());
				}
			}
			emit send_txt2statusbar("Ready", 0);
			this->setEnabled(true);
		}

		//start calculation
		this->hyd_calc->start();
		this->check_hyd_thread_is_running();

	}


}
//Convert a given Gis-raster into a Prom-raster (menu hyd/tools)
void Main_Wid::convert_Gis2Prom_raster(void){
	try{
		this->allocate_multi_hydraulic_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_convert_Gis2Prom);
	this->hyd_calc->set_ptr2database(this->system_database->get_database());
	try{
		if(this->hyd_calc->ask_file_raster_conversion_dialog()==false){
			this->delete_multi_hydraulic_system();
			return;
		}
		else{
			QObject::connect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_calc_finished()));

			this->reset_exception_new_action();
			//start calculation
			this->hyd_calc->start();
			this->check_hyd_thread_is_running();

			//enable menu and actions
			this->action_stop_hyd_calc->setEnabled(true);
		}
	}
	catch(Error msg){
		msg.output_msg(2);
	}
}
//Convert profile data to a river DGM-W (menu hyd/tools)
void Main_Wid::convert_profile2dgmw(void){
	try{
		this->allocate_multi_hydraulic_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_convert_profil2dgmw);
	this->hyd_calc->set_ptr2database(this->system_database->get_database());
	try{
		if(this->hyd_calc->ask_file_profile_conversion_dialog()==false){
			this->delete_multi_hydraulic_system();
			return;
		}
		else{
			QObject::connect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_calc_finished()));

			this->reset_exception_new_action();
			//start calculation
			this->hyd_calc->start();
			this->check_hyd_thread_is_running();

			//enable menu and actions
			this->action_stop_hyd_calc->setEnabled(true);
		}
	}
	catch(Error msg){
		msg.output_msg(2);
	}

}
//Recieve if a module extern permits the use of the hydraulic module and disable the menus
void Main_Wid::enable_hyd_module_extern(bool flag){
	if(flag==true){
		this->menu_hyd_import->setEnabled(false);
		this->action_delete_boundary_scenario->setEnabled(false);
		this->menu_hyd_calc->setEnabled(false);
		this->menu_hyd_check->setEnabled(false);
		this->menu_fpl2hyd->setEnabled(false);
	}
	else{
		if(this->system_state.get_sys_system_id().area_state==0 && this->system_state.get_sys_system_id().measure_nr==0){
			this->menu_hyd_import->setEnabled(true);
			this->action_delete_boundary_scenario->setEnabled(true);
		}
		this->menu_hyd_calc->setEnabled(true);
		this->menu_hyd_check->setEnabled(true);
		this->menu_fpl2hyd->setEnabled(true);
	}
}
//Recieve if a module extern uses the hydraulic thread
void Main_Wid::hyd_module_extern_runs(bool flag){
	this->status_wid->set_hydthread(flag);
}
//Recieve if the hydraulic thread is sleeping
void Main_Wid::hydraulic_thread_sleeps(bool flag){
	this->status_wid->set_hydthread_sleep(flag);
}
//________
//menu DAM
//User aborted the damage calculation; the dam-thread is stopped (menu dam)
void Main_Wid::stop_damcalc_thread(void){
	//be sure that the thread is allocated and started
	if(this->dam_calc!=NULL && this->dam_calc->isRunning()==true){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"Do you want stop the DAM-thread?" << endl;
		txt<<"Result data will be lost" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			return;
		}
		else{
			this->dam_calc->set_stop_thread_flag(true);
			if(Sys_Project::get_project_type()==_sys_project_type::proj_dam){
				Hyd_Multiple_Hydraulic_Systems::set_stop_thread_flag(true);
			}
		}
	}
}
//Check for the normal end of the damage calculation thread
void Main_Wid::thread_dam_calc_finished(void){
	if(this->dam_calc!=NULL && this->dam_calc->isRunning()==false){
		this->action_stop_dam_calc->setEnabled(false);

		this->check_dam_thread_is_running();

		//disconnect the thread when is finished
		QObject::disconnect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
		if(this->dam_calc->get_thread_type()==_dam_thread_type::dam_del_ecn ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_del_eco_btype ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_del_eco_soil ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_del_pop ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_del_pys ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_add_ecn_dam_raster ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_add_eco_b_type_raster ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_add_eco_soil_raster ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_add_pop_raster ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_add_pys_raster ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_imp_ecn_dam_func ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_imp_eco_b_type ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_imp_eco_soil_cost ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_imp_pop_dam_func ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_imp_pys_category ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_imp_pys_dens_func ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_imp_hyd_res_raster_base_sc ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_add_hyd_res_raster_sc ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_sys_calc_no_break_sz ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_del_sc ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_imp_sc_point ||
			this->dam_calc->get_thread_type()==_dam_thread_type::dam_imp_sc_subcat){
			emit send_delete2refresh_data_view();
		}
		this->delete_damage_system();
		if (this->task_flag == true) {
			emit send_task_by_file_start();
		}
	}
}
//Recieve if a module extern uses the damage thread
void Main_Wid::dam_module_extern_runs(bool flag){
	this->status_wid->set_damthread(flag);
}
//Recieve if the damage thread is sleeping
void Main_Wid::dam_thread_sleeps(bool flag){
	this->status_wid->set_damthread_sleep(flag);
}
//Create the database tables for the damage system (menu dam/common)
void Main_Wid::create_dam_system_database_tables(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->dam_calc->set_thread_type(_dam_thread_type::dam_create_tab);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_create_tables_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
	this->dam_tables_created=true;
}
//Check for the normal end of the damage create-tables thread
void Main_Wid::thread_dam_create_tables_finished(void){
	if(this->dam_calc!=NULL && this->dam_calc->isRunning()==false){
		this->action_stop_dam_calc->setEnabled(false);

		this->check_dam_thread_is_running();

		//disconnect the thread when is finished
		QObject::disconnect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_create_tables_finished()));
		this->delete_damage_system();
		emit send_table_creation_is_finished();
	}
}
//Check the database tables for the damage system (menu dam/common)
void Main_Wid::check_dam_system_database_tables(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->dam_calc->set_thread_type(_dam_thread_type::dam_check_tab);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_check_tables_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
	this->dam_tables_created=true;
}
//Check for the normal end of the damage check-datbase tables thread
void Main_Wid::thread_dam_check_tables_finished(void){
	if(this->dam_calc!=NULL && this->dam_calc->isRunning()==false){
		this->action_stop_dam_calc->setEnabled(false);

		this->check_dam_thread_is_running();

		this->number_error_table_check=this->number_error_table_check+this->dam_calc->get_error_number();

		//disconnect the thread when is finished
		QObject::disconnect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_check_tables_finished()));
		this->delete_damage_system();
		emit send_table_check_is_finished();
		if (this->task_flag == true && this->project_manager.get_project_type() == _sys_project_type::proj_dam_hyd) {
			emit send_task_by_file_start();
		}
		
	}
}
//Set a new output flag for the dam modul(menu dam/common)
void Main_Wid::set_dam_outputflag(void){
	Sys_Common_Output::output_dam->set_detailflag_gui(this);
}
//Set a new output logfile for the dam modul(menu dam/common)
void Main_Wid::set_dam_outputlogfile(void){
	Sys_Common_Output::output_dam->set_new_logfile_gui(this);
}
//Reset the display DAM (menu dam/common)
void Main_Wid::reset_dam_outputtxt(void){
	//clear the display
	this->textEdit_damtxt->clear();
	Sys_Common_Output::output_dam->welcome_to_gui();
	Sys_Common_Output::output_dam->reset_prefix_was_outputed();
	ostringstream cout;
	cout << "DAM-display is cleared" << endl;
	Sys_Common_Output::output_system->output_txt(cout.str(),false);
}
//Import a hydraulic result raster from a file to database for a DAM-project as a base scenario (menu dam/Import HYD-results/Base scenario)
void Main_Wid::import_hyd_result_raster_base_sc(void){
	try{
		//check if base scenario already exists
		bool flag2=false;
		if(Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database())==true){
			Sys_Diverse_Text_Dia dialog2;
			ostringstream txt;
			txt<<"All imported HYD-result data and all damage result data in the database will be deleted."<< endl;
			txt<<"Do you want to continue?" << endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.setIcon(QMessageBox::Warning);
			flag2=dialog2.start_dialog();
		}
		else{
			flag2=true;
		}
		if(flag2==true){
			//Allocate dam-system
			try{
				this->allocate_damage_system();
			}
			catch(Error msg){
				msg.output_msg(0);
				return;
			}
			this->dam_calc->set_ptr2database(this->system_database->get_database());
			//Ask path to hyd results raster files
			if(this->dam_calc->ask_file_hyd_res_raster_multi_fp(true)==false){
				this->delete_damage_system();
				return;
			}
			//Read in rasters and store in hyd_database
			this->dam_calc->set_thread_type(_dam_thread_type::dam_imp_hyd_res_raster_base_sc);
			//connect the thread when is finished
			QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
			this->action_stop_dam_calc->setEnabled(true);
			//start calculation
			this->reset_exception_new_action();
			this->dam_calc->start();
			this->check_dam_thread_is_running();
		}
	}
	catch(Error msg){
		msg.output_msg(0);
	}
}
//Add a hydraulic result raster from a file to database for a DAM-project as a base scenario (menu dam/Import HYD-results/Add scenario)
void Main_Wid::add_hyd_result_raster_sc(void){
	try{
		//check if there is a base scenario
		if(Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database())==false){
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			txt<<"A HYD-base scenario must be set before any additional boundary scenario(s) can be imported!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return;
		}

		//Allocate dam-system
		try{
			this->allocate_damage_system();
		}
		catch(Error msg){
			msg.output_msg(0);
			return;
		}
		this->dam_calc->set_ptr2database(this->system_database->get_database());
		//Ask path to hyd results raster files
		if(this->dam_calc->ask_file_hyd_res_raster_multi_fp(false)==false){
			this->delete_damage_system();
			return;
		}
		//Read in rasters and store in hyd_database
		this->dam_calc->set_thread_type(_dam_thread_type::dam_add_hyd_res_raster_sc);
		//connect the thread when is finished
		QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
		this->action_stop_dam_calc->setEnabled(true);
		//start calculation
		this->reset_exception_new_action();
		this->dam_calc->start();
		this->check_dam_thread_is_running();
	}
	catch(Error msg){
		msg.output_msg(0);
	}
}
//Delete hydraulic resuts in the database for a DAM_project (menu dam/Delete Hyd-Results)
void Main_Wid::delete_hyd_results_dam(void){
	try{
		this->allocate_multi_hydraulic_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->hyd_calc->set_thread_type(_hyd_thread_type::hyd_del_sz);

	this->hyd_calc->set_ptr2database(this->system_database->get_database());

	if(this->hyd_calc->ask_boundary_scenarios_per_dialog(this->system_database->get_database(), this)==0){
		this->delete_multi_hydraulic_system();
	}
	else{
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"The selected hydraulic boundary scenarios will be deleted!"<< endl;
		txt<<"Do you want to continue?" << endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.setIcon(QMessageBox::Warning);
		bool flag2=dialog2.start_dialog();
		if(flag2==true){
			//connect the thread when is finished
			QObject::connect(this->hyd_calc,SIGNAL(finished()),this,SLOT(thread_hyd_calc_finished()));
			this->action_Close_Connection->setEnabled(false);

			//delete damages
			if( Sys_Project::get_project_type()==_sys_project_type::proj_dam){
				this->setEnabled(false);
				emit send_txt2statusbar("Delete damage results...", 0);
				for(int i=0; i< this->hyd_calc->sz_bound_manager.get_number_sz(); i++){
					if(this->hyd_calc->sz_bound_manager.get_ptr_sz(i)->get_is_selected()==true){
						Dam_Damage_System::delete_data_in_erg_table(this->system_database->get_database(), this->hyd_calc->sz_bound_manager.get_ptr_sz(i)->get_id());
					}
				}
				emit send_txt2statusbar("Ready", 0);
				this->setEnabled(true);
			}

			//start calculation
			this->hyd_calc->start();
			this->check_dam_thread_is_running();
		}
		else{
			this->delete_multi_hydraulic_system();
			return;
		}
	}
}
//Import the economic damage function from a file to database (menu dam/ECoNomic)
void Main_Wid::import_ecn_dam_function(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->ecn_sys.ask_file_damage_function(this)==false){
		this->delete_damage_system();
		return;
	};
	int number=0;
	try{
		QSqlTableModel results(0,*this->system_database->get_database());
		number=Dam_Ecn_Damage_Function::select_relevant_functions_database(&results,false);
	}
	catch(Error msg){
		msg.output_msg(4);
		this->delete_damage_system();
		return;
	}
	//Dam_Ecn_Damage_Function::close_function_table();
	if(number !=0){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<number<<" existing economic damage function in the database will be deleted."<< endl;
		txt<<"Do you want to continue?" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			this->delete_damage_system();
			return;
		}
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_imp_ecn_dam_func);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
///Add an economic raster from a files to database (menu dam/ECoNomic)
void Main_Wid::add_ecn_dam_rasters(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->ecn_sys.ask_file_damage_raster_multi()==false){
		this->delete_damage_system();
		return;
	}
	this->dam_calc->set_thread_type(_dam_thread_type::dam_add_ecn_dam_raster);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->reset_exception_new_action();
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Delete all economical data in the database to database (menu dam/ECoNomic/)
void Main_Wid::delete_all_ecn_dam(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->ecn_sys.ask_deleting_flag(this)==false){
		this->delete_damage_system();
		return;
	}
	//ask which raster to delete
	if(this->dam_calc->ecn_sys.get_raster2delete()==true){
		if(this->dam_calc->ecn_sys.ask_raster2handled_dialog(this->system_database->get_database(),this)==false){
			this->delete_damage_system();
			return;
		}
	}

	Sys_Diverse_Text_Dia dialog2;
	ostringstream txt;
	txt<<"The selected economic data in the database will be deleted:"<< endl<<endl;
	txt << this->dam_calc->ecn_sys.get_deleting_text() << endl;
	txt<<"Do you want to continue?" << endl;
	dialog2.set_dialog_question(txt.str());
	bool flag2=dialog2.start_dialog();
	if(flag2==false){
		this->delete_damage_system();
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_del_ecn);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Connect the econimical raster (menu dam/ECoNomic/)
void Main_Wid::connect_ecn_raster(void){
	if(Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"A HYD-base scenario must be set before a connection of the economical DAM-raster can be established!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	QSqlQueryModel model;
	if(Dam_Ecn_Raster::select_relevant_raster_database(&model, this->system_database->get_database(),false)<=0){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"There are no economical DAM-raster set in database for connection!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}

	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_sys_connect);
	this->dam_calc->set_specific_raster_type(_dam_raster_types::ecn_total);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Import the ecological biotope-types from a file to database (menu dam/ECOlogic/Import)
void Main_Wid::import_eco_b_type(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->eco_sys.ask_file_biotope_types(this)==false){
		this->delete_damage_system();
		return;
	};
	int number=0;
	try{
		QSqlTableModel results(0,*this->system_database->get_database());
		number=Dam_Eco_Btype::select_relevant_biotope_types_database(&results,false);
	}
	catch(Error msg){
		msg.output_msg(4);
		this->delete_damage_system();
		return;
	}
	if(number!=0){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<number <<" existing biotope-types in the database will be deleted."<< endl;
		txt<<"Do you want to continue?" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			this->delete_damage_system();
			return;
		}
	}
	this->dam_calc->set_thread_type(_dam_thread_type::dam_imp_eco_b_type);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Add an ecologic raster of the biotope-types from files to database (menu dam/ECoNomic/Import)
void Main_Wid::add_eco_b_type_rasters(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->eco_sys.ask_file_biotope_raster_multi()==false){
		this->delete_damage_system();
		return;
	}
	this->dam_calc->set_thread_type(_dam_thread_type::dam_add_eco_b_type_raster);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Delete all ecological biotope-type data in the database to database (menu dam/ECOlogic/)
void Main_Wid::delete_all_eco_b_type_dam(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->eco_sys.ask_deleting_biotope_flag(this)==false){
		this->delete_damage_system();
		return;
	}

	//ask which raster to delete
	if(this->dam_calc->eco_sys.get_biotope_raster2delete()==true){
		if(this->dam_calc->eco_sys.ask_biotope_raster2handled_dialog(this->system_database->get_database(),this)==false){
			this->delete_damage_system();
			return;
		}
	}

	Sys_Diverse_Text_Dia dialog2;
	ostringstream txt;
	txt<<"The selected ecological biotope-type data in the database will be deleted:"<< endl<<endl;
	txt << this->dam_calc->eco_sys.get_biotope_deleting_text() << endl;
	txt<<"Do you want to continue?" << endl;
	dialog2.set_dialog_question(txt.str());
	bool flag2=dialog2.start_dialog();
	if(flag2==false){
		this->delete_damage_system();
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_del_eco_btype);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Import the ecological soil-type cost function from a file to database (menu dam/ECOlogic/Import)
void Main_Wid::import_eco_soil_cost(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->eco_sys.ask_file_soil_type_cost_function(this)==false){
		this->delete_damage_system();
		return;
	};
	int number=0;
	try{
		QSqlTableModel results(0,*this->system_database->get_database());
		number=Dam_Eco_Soil_Cost_Function::select_relevant_functions_database(&results,false);
	}
	catch(Error msg){
		msg.output_msg(4);
		this->delete_damage_system();
		return;
	}
	if(number!=0){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<number <<" existing soil-cost function in the database will be deleted."<< endl;
		txt<<"Do you want to continue?" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			this->delete_damage_system();
			return;
		}
	}
	this->dam_calc->set_thread_type(_dam_thread_type::dam_imp_eco_soil_cost);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Import the ecological soil-type erosion function from a file to database (menu dam/ECOlogic/Import)
void Main_Wid::import_eco_soil_ero(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->eco_sys.ask_file_soil_type_erosion_function(this)==false){
		this->delete_damage_system();
		return;
	};
	int number=0;
	try{
		QSqlTableModel results(0,*this->system_database->get_database());
		number=Dam_Eco_Soil_Erosion_Function::select_relevant_functions_database(&results,false);
	}
	catch(Error msg){
		msg.output_msg(4);
		this->delete_damage_system();
		return;
	}
	if(number!=0){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<number<<" existing soil-erosion function in the database will be deleted."<< endl;
		txt<<"Do you want to continue?" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			this->delete_damage_system();
			return;
		}
	}
	this->dam_calc->set_thread_type(_dam_thread_type::dam_imp_eco_soil_ero);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Add an ecologic raster of the soil erosion-types from files to database (menu dam/ECOlogic/Import)
void Main_Wid::add_eco_soil_ero_rasters(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->eco_sys.ask_file_soil_type_erosion_raster_multi()==false){
		this->delete_damage_system();
		return;
	};
	this->dam_calc->set_thread_type(_dam_thread_type::dam_add_eco_soil_raster);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Delete all ecological soil-erosion data in the database to database (menu dam/ECOlogic/)
void Main_Wid::delete_all_eco_soil_dam(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->eco_sys.ask_deleting_soil_erosion_flag(this)==false){
		this->delete_damage_system();
		return;
	}
	//ask which raster to delete
	if(this->dam_calc->eco_sys.get_soil_erosion_raster2delete()==true){
		if(this->dam_calc->eco_sys.ask_soil_erosion_raster2handled_dialog(this->system_database->get_database(),this)==false){
			this->delete_damage_system();
			return;
		}
	}

	Sys_Diverse_Text_Dia dialog2;
	ostringstream txt;
	txt<<"The selected ecological soil-erosion data in the database will be deleted:"<< endl<<endl;
	txt << this->dam_calc->eco_sys.get_soil_erosion_deleting_text() << endl;
	txt<<"Do you want to continue?" << endl;
	dialog2.set_dialog_question(txt.str());
	bool flag2=dialog2.start_dialog();
	if(flag2==false){
		this->delete_damage_system();
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_del_eco_soil);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Connect the ecological raster (menu dam/ECOlogic/)
void Main_Wid::connect_eco_raster(void){
	if(Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"A HYD-base scenario must be set before a connection of the ecological DAM-raster can be established!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	QSqlQueryModel model;
	if(Dam_Eco_Btype_Raster::select_relevant_raster_database(&model, this->system_database->get_database(),false)<=0 &&
		Dam_Eco_Soil_Raster::select_relevant_raster_database(&model, this->system_database->get_database(),false)<=0){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"There are no ecological DAM-damage raster set in database for connection!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}

	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_sys_connect);
	this->dam_calc->set_specific_raster_type(_dam_raster_types::eco_all);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Import the people2risk damage function from a file to database (menu dam/PeOPle/Import)
void Main_Wid::import_pop_dam_function(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->people_sys.ask_file_damage_function(this)==false){
		this->delete_damage_system();
		return;
	};
	int number=0;
	try{
		QSqlTableModel results(0,*this->system_database->get_database());
		number=Dam_People_Damage_Function::select_relevant_functions_database(&results,false);
	}
	catch(Error msg){
		msg.output_msg(4);
		this->delete_damage_system();
		return;
	}
	if(number!=0){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<number <<" existing people2risk damage function in the database will be deleted."<< endl;
		txt<<"Do you want to continue?" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			this->delete_damage_system();
			return;
		}
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_imp_pop_dam_func);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Add an people2risk raster from files to database (menu dam/PeOPle/Import)
void Main_Wid::add_pop_rasters(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->people_sys.ask_file_people2risk_raster_multi()==false){
		this->delete_damage_system();
		return;
	};
	this->dam_calc->set_thread_type(_dam_thread_type::dam_add_pop_raster);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
    this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Delete all people2risk data in the database to database (menu dam/PeOPle/)
void Main_Wid::delete_all_pop_dam(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->people_sys.ask_deleting_flag(this)==false){
		this->delete_damage_system();
		return;
	}

	//ask which raster to delete
	if(this->dam_calc->people_sys.get_raster2delete()==true){
		if(this->dam_calc->people_sys.ask_raster2handled_dialog(this->system_database->get_database(),this)==false){
			this->delete_damage_system();
			return;
		}
	}

	Sys_Diverse_Text_Dia dialog2;
	ostringstream txt;
	txt<<"The selected people2risk data in the database will be deleted:"<< endl<<endl;
	txt << this->dam_calc->people_sys.get_deleting_text() << endl;
	txt<<"Do you want to continue?" << endl;
	dialog2.set_dialog_question(txt.str());
	bool flag2=dialog2.start_dialog();
	if(flag2==false){
		this->delete_damage_system();
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_del_pop);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Connect the people2risk raster (menu dam/PeOPle/)
void Main_Wid::connect_pop_raster(void){
	if(Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"A HYD-base scenario must be set before a connection of the people2risk DAM-raster can be established!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	QSqlQueryModel model;
	if(Dam_People_Raster::select_relevant_raster_database(&model, this->system_database->get_database(),false)<=0 ){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"There are no people2risk DAM-raster set in database for connection!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}

	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_sys_connect);
	this->dam_calc->set_specific_raster_type(_dam_raster_types::pop_total);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Import the people2risk category with the criteria score function from a file to database (menu dam/PsYchoSocial/Import)
void Main_Wid::import_pys_category(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->pys_sys.ask_file_categories(this)==false){
		this->delete_damage_system();
		return;
	};
	int number=0;
	try{
		QSqlTableModel results(0,*this->system_database->get_database());
		number=Dam_Pys_Category::select_relevant_category_database(&results,false);
	}
	catch(Error msg){
		msg.output_msg(4);
		this->delete_damage_system();
		return;
	}
	if(number!=0){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<number <<" existing psycho-social categories in the database will be deleted."<< endl;
		txt<<"Do you want to continue?" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			this->delete_damage_system();
			return;
		}
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_imp_pys_category);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Import the people2risk density function from a file to database (menu dam/PsYchoSocial/Import)
void Main_Wid::import_pys_density_function(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->pys_sys.ask_file_density_function(this)==false){
		this->delete_damage_system();
		return;
	};
	int number=0;
	try{
		QSqlTableModel results(0,*this->system_database->get_database());
		number=Dam_Pys_Density_Function::select_relevant_function_points_database(&results,false);
	}
	catch(Error msg){
		msg.output_msg(4);
		this->delete_damage_system();
		return;
	}
	if(number!=0){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"The existing psycho-social density function in the database will be deleted."<< endl;
		txt<<"Do you want to continue?" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			this->delete_damage_system();
			return;
		}
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_imp_pys_dens_func);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Add an psycho-social raster from files to database (menu dam/PsYchoSocial/Import)
void Main_Wid::add_pys_rasters(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->pys_sys.ask_file_damage_raster_multi()==false){
		this->delete_damage_system();
		return;
	};
	this->dam_calc->set_thread_type(_dam_thread_type::dam_add_pys_raster);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Delete all psycho-social data in the database to database (menu dam/PsYchoSocial/)
void Main_Wid::delete_all_pys_dam(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->pys_sys.ask_deleting_flag(this)==false){
		this->delete_damage_system();
		return;
	}

	//ask which raster to delete
	if(this->dam_calc->pys_sys.get_raster2delete()==true){
		if(this->dam_calc->pys_sys.ask_raster2handled_dialog(this->system_database->get_database(),this)==false){
			this->delete_damage_system();
			return;
		}
	}

	Sys_Diverse_Text_Dia dialog2;
	ostringstream txt;
	txt<<"The selected psycho-social data in the database will be deleted:"<< endl<<endl;
	txt << this->dam_calc->pys_sys.get_deleting_text() << endl;
	txt<<"Do you want to continue?" << endl;
	dialog2.set_dialog_question(txt.str());
	bool flag2=dialog2.start_dialog();
	if(flag2==false){
		this->delete_damage_system();
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_del_pys);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Connect the psycho-social raster (menu dam/PsYchoSocial/)
void Main_Wid::connect_pys_raster(void){
	if(Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"A HYD-base scenario must be set before a connection of the psycho-social DAM-raster can be established!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	QSqlQueryModel model;
	if(Dam_Pys_Raster::select_relevant_raster_database(&model, this->system_database->get_database(),false)<=0 ){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"There are no psycho-social DAM-raster set in database for connection!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_sys_connect);
	this->dam_calc->set_specific_raster_type(_dam_raster_types::pys_pys_total);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Import the simple counting points from a file to database (menu dam/SimpleCounting/Import)
void Main_Wid::import_sc_points(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->sc_sys.ask_file_damage_points(this)==false){
		this->delete_damage_system();
		return;
	};
	int number=0;
	try{
		QSqlQueryModel results;
		number=Dam_Sc_Point::select_relevant_points_database(&results, this->system_database->get_database(), this->system_state.get_sys_system_id(),false);
	}
	catch(Error msg){
		msg.output_msg(4);
		this->delete_damage_system();
		return;
	}
	if(number!=0){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<number <<" existing simple counting points in the database will be deleted."<< endl;
		txt<<"Do you want to continue?" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			this->delete_damage_system();
			return;
		}
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_imp_sc_point);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Import the simple counting subcategories from a file to database (menu dam/SimpleCounting/Import)
void Main_Wid::import_sc_subcategories(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->sc_sys.ask_file_subcategories(this)==false){
		this->delete_damage_system();
		return;
	};
	int number=0;
	try{
		QSqlQueryModel results;
		number=Dam_Sc_Subcategory::select_relevant_subcategory_database(&results, this->system_database->get_database());
	}
	catch(Error msg){
		msg.output_msg(4);
		this->delete_damage_system();
		return;
	}
	if(number!=0){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<number <<" existing simple counting subcategories in the database will be deleted."<< endl;
		txt<<"Do you want to continue?" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			this->delete_damage_system();
			return;
		}
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_imp_sc_subcat);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Delete all simple counting data in the database to database (menu dam/SimpleCounting/)
void Main_Wid::delete_all_sc_dam(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->dam_calc->sc_sys.ask_deleting_flag(this)==false){
		this->delete_damage_system();
		return;
	}

	Sys_Diverse_Text_Dia dialog2;
	ostringstream txt;
	txt<<"The selected simple counting data in the database will be deleted:"<< endl<<endl;
	txt << this->dam_calc->sc_sys.get_deleting_text() << endl;
	txt<<"Do you want to continue?" << endl;
	dialog2.set_dialog_question(txt.str());
	bool flag2=dialog2.start_dialog();
	if(flag2==false){
		this->delete_damage_system();
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_del_sc);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Connect the simple counting points (menu dam/SimpleCounting/)
void Main_Wid::connect_sc_points(void){
	if(Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"A HYD-base scenario must be set before a connection of SC-points can be established!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	QSqlQueryModel model;
	if(Dam_Sc_Point::select_relevant_points_database(&model, this->system_database->get_database(), this->system_state.get_sys_system_id(), false)<=0 ){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"There are no simple counting DAM-points set in database for connection!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_sys_connect);
	this->dam_calc->set_specific_raster_type(_dam_raster_types::sc_sc_total);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Import the CI data to database (menu dam/Critical Infrastructure/Import)
void Main_Wid::import_CI_data(void) {
	try {
		this->allocate_damage_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	//ask for input files
	if (this->dam_calc->ci_sys.ask_file_CI_data(this) == false) {
		this->delete_damage_system();
		return;
	};
	//todo
	int number = 0;
	try {
		QSqlQueryModel results;
		number = Dam_CI_Point::select_relevant_points_database(&results, this->system_database->get_database(), this->system_state.get_sys_system_id(), false);
	}
	catch (Error msg) {
		msg.output_msg(4);
		this->delete_damage_system();
		return;
	}
	if (number != 0) {
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt << number << " existing CI-points in the database will be deleted." << endl;
		txt << "Do you want to continue?" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2 = dialog2.start_dialog();
		if (flag2 == false) {
			this->delete_damage_system();
			return;
		}
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_imp_ci_data);

	//connect the thread when is finished
	QObject::connect(this->dam_calc, SIGNAL(finished()), this, SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();


}
//Delete all CI data in the database (menu dam/Critical Infrastructure/)
void Main_Wid::delete_all_CI_dam(void) {
	try {
		this->allocate_damage_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	
	if (this->dam_calc->ci_sys.ask_deleting_flag(this) == false) {
		this->delete_damage_system();
		return;
	}

	
	Sys_Diverse_Text_Dia dialog2;
	ostringstream txt;
	txt << "The selected CI data in the database will be deleted:" << endl << endl;
	txt << this->dam_calc->ci_sys.get_deleting_text() << endl;
	txt << "Do you want to continue?" << endl;
	dialog2.set_dialog_question(txt.str());
	bool flag2 = dialog2.start_dialog();
	if (flag2 == false) {
		this->delete_damage_system();
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_del_ci);

	//connect the thread when is finished
	QObject::connect(this->dam_calc, SIGNAL(finished()), this, SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Connect the CI data(menu dam /Critical Infrastructure/)
void Main_Wid::connect_CI_data(void) {
	if (Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database()) == false) {
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt << "A HYD-base scenario must be set before a connection of the CI-points can be established!" << endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	QSqlQueryModel model;
	
	if (Dam_CI_Point::select_relevant_points_database(&model, this->system_database->get_database(), this->system_state.get_sys_system_id(), false) <= 0) {
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt << "There are no CI data set in database for connection!" << endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	try {
		this->allocate_damage_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_sys_connect);
	
	this->dam_calc->set_specific_raster_type(_dam_raster_types::ci_ci_total);

	//connect the thread when is finished
	QObject::connect(this->dam_calc, SIGNAL(finished()), this, SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();

}
//Check the damage system (menu dam/System/)
void Main_Wid::check_damage_system(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_check_sys);
	this->dam_calc->set_ptr2database(this->system_database->get_database());
	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Output the the statistic of the damage system (menu dam/System/)
void Main_Wid::output_damage_system_statistic(void){
	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_sys_stat);
	this->dam_calc->set_ptr2database(this->system_database->get_database());
	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Connect the damage raster among each other and connect it to the hydraulic system (menu dam/System/)
void Main_Wid::connect_damage_rasters(void){
	if(Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"A HYD-base scenario must be set before a connection of the DAM-raster can be established!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	if(Dam_Damage_System::check_some_raster_set(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"There are no DAM-raster set in database for connection!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		this->status_wid->get_ptr_risk_state_check_box()->setChecked(false);
		return;
	}

	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->dam_calc->set_thread_type(_dam_thread_type::dam_sys_connect);

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->reset_exception_new_action();
	//start calculation
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Calculate the damages for selected nobreak-scenarios (menu dam/System/)
void Main_Wid::calculate_damage_nobreak_sz(void){
	this->setEnabled(false);
	this->statusBar()->showMessage("Check system for damage calculation...", 0);
	if(Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"A hydraulic base scenario must be set before any damages can be calculated"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		this->setEnabled(true);
		this->statusBar()->showMessage("Ready", 0);
		return;
	}
	if(Dam_Damage_System::check_some_raster_set(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"Damage raster have to be set before any damages can be calculated"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		this->status_wid->get_ptr_risk_state_check_box()->setChecked(false);
		this->setEnabled(true);
		this->statusBar()->showMessage("Ready", 0);
		return;
	}

	string text_buffer;
	if(Dam_Damage_System::check_all_raster_connected2hyd(this->system_database->get_database(), &text_buffer, this->system_state.get_sys_system_id())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"The DAM-raster have to be connected to the HYD-system before any damage calculations can be done!"<< endl;
		txt<<"Further informations: " << endl;
		txt<<text_buffer;
		txt<<endl;
		txt<<"Connect the DAM-raster(s)/-point(s) to the HYD-system!"<<endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.adjustSize();
		dialog2.start_dialog();
		this->setEnabled(true);
		this->statusBar()->showMessage("Ready", 0);
		return;
	}
	this->setEnabled(true);
	this->statusBar()->showMessage("Ready", 0);

	try{
		this->allocate_damage_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->dam_calc->set_thread_type(_dam_thread_type::dam_sys_calc_no_break_sz);
	if(this->dam_calc->ask_boundary_scenarios_per_dialog(this->system_database->get_database(), this)==0){
		this->delete_damage_system();
		return;
	}

	//connect the thread when is finished
	QObject::connect(this->dam_calc,SIGNAL(finished()),this,SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->dam_calc->start();
	this->check_dam_thread_is_running();
}
//Calculate the instationary damages for selected nobreak-scenarios (menu dam/System/)
void Main_Wid::calculate_instat_damage_nobreak_sz(void) {
	this->setEnabled(false);
	this->statusBar()->showMessage("Check system for instatationary damage calculation...", 0);
	if (Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(this->system_database->get_database()) == false) {
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt << "A hydraulic base scenario must be set before any instatationary damages can be calculated" << endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		this->setEnabled(true);
		this->statusBar()->showMessage("Ready", 0);
		return;
	}
	if (Dam_Damage_System::check_some_raster_set(this->system_database->get_database(), this->system_state.get_sys_system_id()) == false) {
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt << "Damage raster have to be set before any damages can be calculated" << endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		this->status_wid->get_ptr_risk_state_check_box()->setChecked(false);
		this->setEnabled(true);
		this->statusBar()->showMessage("Ready", 0);
		return;
	}

	string text_buffer;
	if (Dam_Damage_System::check_all_raster_connected2hyd(this->system_database->get_database(), &text_buffer, this->system_state.get_sys_system_id()) == false) {
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt << "The DAM-raster have to be connected to the HYD-system before any damage calculations can be done!" << endl;
		txt << "Further informations: " << endl;
		txt << text_buffer;
		txt << endl;
		txt << "Connect the DAM-raster(s)/-point(s) to the HYD-system!" << endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.adjustSize();
		dialog2.start_dialog();
		this->setEnabled(true);
		this->statusBar()->showMessage("Ready", 0);
		return;
	}
	if (Sys_Project::get_project_type() == _sys_project_type::proj_dam) {
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt << "The instationary damage calculation is currently not supported for DAM-projects!" << endl;
		txt << "Further informations: " << endl;
		txt << text_buffer;
		txt << endl;
		txt << "Please use the stationary damage calculation!" << endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.adjustSize();
		dialog2.start_dialog();
		this->setEnabled(true);
		this->statusBar()->showMessage("Ready", 0);
		return;

	}
	this->setEnabled(true);
	this->statusBar()->showMessage("Ready", 0);

	try {
		this->allocate_damage_system();
	}
	catch (Error msg) {
		msg.output_msg(0);
		return;
	}
	this->dam_calc->set_ptr2database(this->system_database->get_database());
	this->dam_calc->set_thread_type(_dam_thread_type::dam_sys_instat_calc_no_break_sz);
	if(this->dam_calc->ask_boundary_scenarios_per_dialog(this->system_database->get_database(), this) == 0) {
		this->delete_damage_system();
		return;
	}

	//connect the thread when is finished
	QObject::connect(this->dam_calc, SIGNAL(finished()), this, SLOT(thread_dam_calc_finished()));
	this->action_stop_dam_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->dam_calc->start();
	this->check_dam_thread_is_running();

}
//________
//menu RISK
//Recieve enabling/disabling the stop-at-next-step action (menu risk/calculation/catchment risk)
void Main_Wid::enable_stop_at_next_step_action(const bool flag){
	if(flag==true){
		this->menu_risk_calc->setEnabled(true);
		this->menu_catchment_risk->setEnabled(true);

		this->action_stop_next_HYD_calc->setEnabled(true);
		this->action_calc_nobreak_risk->setEnabled(false);
		this->action_calc_scenario_risk->setEnabled(false);
		this->action_calc_nobreak_state->setEnabled(false);
		this->action_risk_state_generation->setEnabled(false);
		this->action_calc_one_break_state->setEnabled(false);
		this->action_calc_user_defined->setEnabled(false);
		this->action_calc_automatic->setEnabled(false);
		this->action_risk_cumulation->setEnabled(false);
	}
	else{
		this->menu_risk_calc->setEnabled(false);
		this->menu_catchment_risk->setEnabled(false);

		this->action_stop_next_HYD_calc->setEnabled(false);
		this->action_calc_nobreak_risk->setEnabled(true);
		this->action_calc_scenario_risk->setEnabled(true);
		this->action_calc_nobreak_state->setEnabled(true);
		this->action_risk_state_generation->setEnabled(true);
		this->action_calc_one_break_state->setEnabled(true);
		this->action_calc_user_defined->setEnabled(true);
		this->action_calc_automatic->setEnabled(true);
		this->action_risk_cumulation->setEnabled(true);
	}
}
//User aborted the risk calculation thread (menu risk)
void Main_Wid::stop_riskcalc_thread(void){
	//be sure that the thread is allocated and started
	if(this->risk_calc!=NULL && this->risk_calc->isRunning()==true){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"Do you want stop the RISK-thread?" << endl;
		txt<<"Result data will be lost" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			return;
		}
		else{
			this->risk_calc->set_stop_thread_flag(true);
		}
	}
}
//Check for the normal end of the risk calculation thread
void Main_Wid::thread_risk_calc_finished(void){
	if(this->risk_calc!=NULL && this->risk_calc->isRunning()==false){
		this->action_stop_risk_calc->setEnabled(false);

		this->check_risk_thread_is_running();

		//disconnect the thread when is finished
		QObject::disconnect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_calc_finished()));
		if(this->risk_calc->get_thread_type()==_risk_thread_type::risk_del ||
			this->risk_calc->get_thread_type()==_risk_thread_type::risk_del_all_result ||
			this->risk_calc->get_thread_type()==_risk_thread_type::risk_del_risk_result ||
			this->risk_calc->get_thread_type()==_risk_thread_type::risk_calc_catchment_auto ||
			this->risk_calc->get_thread_type()==_risk_thread_type::risk_calc_catchment_hand ||
			this->risk_calc->get_thread_type()==_risk_thread_type::risk_calc_nobreak ||
			this->risk_calc->get_thread_type()==_risk_thread_type::risk_calc_scenario ||
			this->risk_calc->get_thread_type()==_risk_thread_type::risk_calc_catchment_cumulated ||
			this->risk_calc->get_thread_type()==_risk_thread_type::risk_calc_catchment_one_break ||
			this->risk_calc->get_thread_type()==_risk_thread_type::risk_generate_catchment_state ||
			this->risk_calc->get_thread_type()==_risk_thread_type::risk_calc_catchment_nobreak ||
			this->risk_calc->get_thread_type()==_risk_thread_type::risk_calc_serial_steps){
			emit send_delete2refresh_data_view();
		}
		this->delete_risk_system();
		if (this->task_flag == true) {
			emit send_task_by_file_start();
		}
	}
}
//Recieve if the risk thread is sleeping
void Main_Wid::risk_thread_sleeps(bool flag){
	this->status_wid->set_riskthread_sleep(flag);
}
//Create the database tables for the risk system
void Main_Wid::create_risk_system_database_tables(void){
	try{
		this->allocate_risk_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->risk_calc->set_thread_type(_risk_thread_type::risk_create_tab);

	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_create_tables_finished()));
	this->action_stop_risk_calc->setEnabled(true);

	this->risk_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->risk_calc->start();
	this->check_risk_thread_is_running();
	this->risk_tables_created=true;
}
//Check for the normal end of the risk create-tables thread
void Main_Wid::thread_risk_create_tables_finished(void){
	if(this->risk_calc!=NULL && this->risk_calc->isRunning()==false){
		this->action_stop_risk_calc->setEnabled(false);

		this->check_risk_thread_is_running();

		//disconnect the thread when is finished
		QObject::disconnect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_create_tables_finished()));
		this->delete_risk_system();
		emit send_table_creation_is_finished();
	}
}
//Check the database tables for the risk system (menu risk/common)
void Main_Wid::check_risk_system_database_tables(void){
	try{
		this->allocate_risk_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->risk_calc->set_thread_type(_risk_thread_type::risk_check_tab);

	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_check_tables_finished()));
	this->action_stop_risk_calc->setEnabled(true);

	this->risk_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->risk_calc->start();
	this->check_risk_thread_is_running();
	this->risk_tables_created=true;
}
//Check for the normal end of the risk check-datbase tables thread
void Main_Wid::thread_risk_check_tables_finished(void){
	if(this->risk_calc!=NULL && this->risk_calc->isRunning()==false){
		this->action_stop_risk_calc->setEnabled(false);

		this->check_risk_thread_is_running();

		this->number_error_table_check=this->number_error_table_check+this->risk_calc->get_number_error();

		//disconnect the thread when is finished
		QObject::disconnect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_check_tables_finished()));
		this->delete_risk_system();
		emit send_table_check_is_finished();
		if (this->task_flag == true && this->project_manager.get_project_type() == _sys_project_type::proj_risk) {
			emit send_task_by_file_start();
		}
	}
}
//Set a new output flag for the risk modul(menu risk/common)
void Main_Wid::set_risk_outputflag(void){
	Sys_Common_Output::output_risk->set_detailflag_gui(this);
}
//Set a new output logfile for the risk modul(menu risk/common)
void Main_Wid::set_risk_outputlogfile(void){
	Sys_Common_Output::output_risk->set_new_logfile_gui(this);
}
//Reset the display RISK (menu risk/common)
void Main_Wid::reset_risk_outputtxt(void){
	//clear the display
	this->textEdit_risktxt->clear();
	Sys_Common_Output::output_risk->welcome_to_gui();
	Sys_Common_Output::output_risk->reset_prefix_was_outputed();
	ostringstream cout;
	cout << "RISK-display is cleared" << endl;
	Sys_Common_Output::output_system->output_txt(cout.str(),false);
}
//Calculate nobreak risk  (menu risk/calculation)
void Main_Wid::calculate_nobreak_risk(void){
	try{
		this->allocate_risk_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->risk_calc->set_ptr2database(this->system_database->get_database());
	this->risk_calc->set_thread_type(_risk_thread_type::risk_calc_nobreak);

	_risk_thread_type buff_thread=_risk_thread_type::risk_calc_nobreak;
	try{
		this->setEnabled(false);
		emit send_txt2statusbar("Checking generated risk results...", 0);
		int number_selected=0;
		if(this->risk_calc->break_sc_manager.ask_boundary_scenarios_per_dialog(this->system_database->get_database(), this, &buff_thread, _risk_type::catchment_risk, &number_selected)==false){
			this->delete_risk_system();
			emit send_txt2statusbar("Ready", 0);
			this->setEnabled(true);
			return;
		}

		emit send_txt2statusbar("Ready", 0);
		this->setEnabled(true);
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_calc_finished()));
	this->action_stop_risk_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->risk_calc->start();
	this->check_risk_thread_is_running();
}
//Calculate scenario based risk (menu risk/calculation)
void Main_Wid::calculate_scenario_based_risk(void){
	if(Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"No FPL-section are set. A risk calculation via the"<< endl;
		txt<<"scenario based risk approach is not possible!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	try{
		this->allocate_risk_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->risk_calc->set_ptr2database(this->system_database->get_database());
	this->risk_calc->set_thread_type(_risk_thread_type::risk_calc_scenario);

	try{
		///ask for the boundary scenario and the fpl-section
		if(this->risk_calc->break_sc_manager.ask_sc2calc_per_dialog(this, this->system_database->get_database(), _risk_type::scenario_risk)==false){
			this->delete_risk_system();
			return;
		}
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_calc_finished()));
	this->action_stop_risk_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->risk_calc->start();
	this->check_risk_thread_is_running();
}
//Generate the risk states for the catchment area risk approach (menu risk/calculation/catchment risk)
void Main_Wid::generate_catchment_risk_states(void){
	if(Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"No FPL-section are set. A risk calculation via the"<< endl;
		txt<<"catchment based risk approach is not possible!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	try{
		this->allocate_risk_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->risk_calc->set_ptr2database(this->system_database->get_database());
	_risk_thread_type buff_thread=_risk_thread_type::risk_generate_catchment_state;
	this->risk_calc->set_thread_type(buff_thread);

	this->setEnabled(false);
	try{
		emit send_txt2statusbar("Checking generated risk results...", 0);
		int number_selected=0;
		this->risk_calc->break_sc_manager.ask_boundary_scenarios_per_dialog(this->system_database->get_database(), this, &buff_thread, _risk_type::catchment_risk, &number_selected );
		if(number_selected==0){
			this->delete_risk_system();
			emit send_txt2statusbar("Ready", 0);
			this->setEnabled(true);
			return;
		}
		emit send_txt2statusbar("Ready", 0);
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->setEnabled(true);
	this->risk_calc->set_thread_type(buff_thread);

	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_calc_finished()));
	this->action_stop_risk_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->risk_calc->start();
	this->check_risk_thread_is_running();
}
//Calculate catchment area risk for nobreak state (detailed risk result) (menu risk/calculation/catchment risk)
void Main_Wid::calculate_catchment_risk_nobreak(void){
	if(Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"No FPL-section are set. A risk calculation via the"<< endl;
		txt<<"catchment based risk approach is not possible!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	try{
		this->allocate_risk_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->risk_calc->set_ptr2database(this->system_database->get_database());
	_risk_thread_type buff_thread=_risk_thread_type::risk_calc_catchment_nobreak;
	this->risk_calc->set_thread_type(buff_thread);

	try{
		this->setEnabled(false);
		int number_selected=0;
		emit send_txt2statusbar("Checking generated risk results...", 0);
		this->risk_calc->break_sc_manager.ask_boundary_scenarios_per_dialog(this->system_database->get_database(), this, &buff_thread, _risk_type::catchment_risk, &number_selected);
		if(number_selected==0){
			this->delete_risk_system();
			emit send_txt2statusbar("Ready", 0);
			this->setEnabled(true);
			return;
		}
		emit send_txt2statusbar("Ready", 0);
		this->setEnabled(true);
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->risk_calc->set_thread_type(buff_thread);

	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_calc_finished()));
	this->action_stop_risk_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->risk_calc->start();
	this->check_risk_thread_is_running();
}
//Calculate catchment area risk for maximum break states (detailed risk result) (menu risk/calculation/catchment risk)
void Main_Wid::calculate_catchment_risk_one_break(void){
	if(Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"No FPL-section are set. A risk calculation via the"<< endl;
		txt<<"catchment based risk approach is not possible!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	try{
		this->allocate_risk_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->risk_calc->set_ptr2database(this->system_database->get_database());
	_risk_thread_type buff_thread=_risk_thread_type::risk_calc_catchment_one_break;
	this->risk_calc->set_thread_type(buff_thread);

	try{
		this->setEnabled(false);
		int number_selected=0;
		emit send_txt2statusbar("Checking generated risk results...", 0);
		this->risk_calc->break_sc_manager.ask_boundary_scenarios_per_dialog(this->system_database->get_database(), this, &buff_thread, _risk_type::catchment_risk, &number_selected);
		if(number_selected==0){
			this->delete_risk_system();
			emit send_txt2statusbar("Ready", 0);
			this->setEnabled(true);
			return;
		}
		emit send_txt2statusbar("Ready", 0);
		this->setEnabled(true);
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->risk_calc->set_thread_type(buff_thread);

	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_calc_finished()));
	this->action_stop_risk_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->risk_calc->start();
	this->check_risk_thread_is_running();
}
//Calculate catchment area risk user defind risk states (detailed risk result) (menu risk/calculation/catchment risk)
void Main_Wid::calculate_catchment_risk_user_defined(void){
	if(Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"No FPL-section are set. A risk calculation via the"<< endl;
		txt<<"catchment based risk approach is not possible!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	try{
		this->allocate_risk_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->risk_calc->set_ptr2database(this->system_database->get_database());
	this->risk_calc->set_thread_type(_risk_thread_type::risk_calc_catchment_hand);

	try{
		this->setEnabled(false);
		emit send_txt2statusbar("Checking generated risk results...", 0);
		if(this->risk_calc->break_sc_manager.ask_sc2calc_per_dialog(this, this->system_database->get_database(),_risk_type::catchment_risk)==false){
			this->delete_risk_system();
			emit send_txt2statusbar("Ready", 0);
			this->setEnabled(true);
			return;
		}
		emit send_txt2statusbar("Ready", 0);
		this->setEnabled(true);
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_calc_finished()));
	this->action_stop_risk_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->risk_calc->start();
	this->check_risk_thread_is_running();
}
//Calculate catchment area risk woth automatically generated risk states (detailed risk result) (menu risk/calculation/catchment risk)
void Main_Wid::calculate_catchment_risk_auto(void){
	if(Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"No FPL-section are set. A risk calculation via the"<< endl;
		txt<<"catchment based risk approach is not possible!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	try{
		this->allocate_risk_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->risk_calc->set_ptr2database(this->system_database->get_database());
	this->risk_calc->set_thread_type(_risk_thread_type::risk_calc_catchment_auto);

	if(this->risk_calc->ask_boundries_predict_risk(this)==false){
		this->delete_risk_system();
		return;
	}

	try{
		this->setEnabled(false);
		emit send_txt2statusbar("Checking generated risk results...", 0);
		Hyd_Boundary_Scenario_List buffer;
		this->risk_calc->break_sc_manager.set_risk_type(_risk_type::catchment_risk);
		_risk_thread_type buff_thread=_risk_thread_type::risk_calc_catchment_cumulated;
		if(this->risk_calc->break_sc_manager.make_list_hyd_bound_sc_calculation(this->system_database->get_database(),&buff_thread, &buffer)==false){
			this->delete_risk_system();
			emit send_txt2statusbar("Ready", 0);
			this->setEnabled(true);
			return;
		}
		emit send_txt2statusbar("Ready", 0);
		this->setEnabled(true);
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_calc_finished()));
	this->action_stop_risk_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->risk_calc->start();
	this->check_risk_thread_is_running();
}
//Calculate and cumulate the generate catchment area risk states to one risk result (menu risk/calculation/catchment risk)
void Main_Wid::cumulate_catchment_risk(void){
	if(Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"No FPL-section are set. A risk calculation via the"<< endl;
		txt<<"catchment based risk approach is not possible!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	try{
		this->allocate_risk_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->risk_calc->set_ptr2database(this->system_database->get_database());
	this->risk_calc->set_thread_type(_risk_thread_type::risk_calc_catchment_cumulated);

	try{
		this->setEnabled(false);
		emit send_txt2statusbar("Checking generated risk results...", 0);
		Hyd_Boundary_Scenario_List buffer;
		this->risk_calc->break_sc_manager.set_risk_type(_risk_type::catchment_risk);
		_risk_thread_type buff_thread=_risk_thread_type::risk_calc_catchment_cumulated;
		if(this->risk_calc->break_sc_manager.make_list_hyd_bound_sc_calculation(this->system_database->get_database(),&buff_thread, &buffer)==false){
			this->delete_risk_system();
			emit send_txt2statusbar("Ready", 0);
			this->setEnabled(true);
			return;
		}
		emit send_txt2statusbar("Ready", 0);
		this->setEnabled(true);
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_calc_finished()));
	this->action_stop_risk_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->risk_calc->start();
	this->check_risk_thread_is_running();
}
//Calculate and output the statistic of the catchment risk approach (menu risk/)
void Main_Wid::calc_output_catchment_risk_statistic(void){
	if(Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"No FPL-section are set. A risk calculation via the"<< endl;
		txt<<"catchment based risk approach is not possible!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	try{
		this->allocate_risk_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->risk_calc->set_ptr2database(this->system_database->get_database());
	this->risk_calc->set_thread_type(_risk_thread_type::risk_catchment_statistic);

	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_calc_finished()));
	this->action_stop_risk_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->risk_calc->start();
	this->check_risk_thread_is_running();
}
//Calculate steps of the risk analysis in serail (menu risk/catchment risk)
void Main_Wid::calc_risk_analysis_steps_serial(void){
	if(Fpl_Section::check_some_section_set_in_database(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"No FPL-section are set. A risk calculation via the"<< endl;
		txt<<"catchment based risk approach is not possible!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return;
	}
	try{
		this->allocate_risk_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->risk_calc->set_ptr2database(this->system_database->get_database());
	this->risk_calc->set_thread_type(_risk_thread_type::risk_calc_serial_steps);

	try{
		///ask for the boundary scenario and the fpl-section
		if(this->risk_calc->ask_steps_serial_calculation(this)==false){
			this->delete_risk_system();
			return;
		}
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_calc_finished()));
	this->action_stop_risk_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->risk_calc->start();
	this->check_risk_thread_is_running();
}
//Stop by next hydraulic calculation (menu risk/calculation/catchment risk)
void Main_Wid::stop_by_next_hyd_calc(void){
	Sys_Diverse_Text_Dia dialog2;
	ostringstream txt;
	txt<<"Do you want to the stop the calculation at the next appropriate step?"<< endl;
	dialog2.set_dialog_question(txt.str());
	if(dialog2.start_dialog()==false){
		return;
	}

	Risk_Hydraulic_Break_Calculation::set_stop_next_hyd_calc();
}
//Output the statistics of already generated detailed risk results to display/console (menu risk/)
void Main_Wid::output_statistics_detailed_risk_results(void){
	try{
		this->allocate_risk_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->risk_calc->set_ptr2database(this->system_database->get_database());
	this->risk_calc->set_thread_type(_risk_thread_type::risk_output_stat_result);

	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_calc_finished()));

	//start calculation
	this->risk_calc->start();
	this->check_risk_thread_is_running();
}
//Delete the risk data in database table (menu risk/delete results/restore)
void Main_Wid::delete_risk_data_database(void){
	try{
		this->allocate_risk_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	if(this->risk_calc->ask_delete_result_restore_database(this)==false){
		this->delete_risk_system();
		return;
	}

	this->risk_calc->set_thread_type(_risk_thread_type::risk_del);

	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_calc_finished()));
	//this->action_stop_risk_calc->setEnabled(true);

	this->risk_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->risk_calc->start();
	this->check_risk_thread_is_running();
}
//Delete the risk result data in database table
void Main_Wid::delete_risk_result_database(const bool del_hyd_dam_fpl){
	try{
		this->allocate_risk_system();
		this->risk_calc->set_del_hyd_dam_fpl_flag(del_hyd_dam_fpl);
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	//set flag
	this->risk_calc->set_thread_type(_risk_thread_type::risk_del_risk_result);
	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_calc_finished()));

	this->risk_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->risk_calc->start();
	this->check_risk_thread_is_running();
	do{

	}while(this->risk_calc->isFinished()==false);

}
//Delete the risk result data and the relvant result data of other modules (risk, fpl, dam, hyd) in database table
void Main_Wid::delete_risk_relevant_results_database(void){
	try{
		this->allocate_risk_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->risk_calc->set_thread_type(_risk_thread_type::risk_del_all_result);
	//connect the thread when is finished
	QObject::connect(this->risk_calc,SIGNAL(finished()),this,SLOT(thread_risk_calc_finished()));

	this->risk_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->risk_calc->start();
	this->check_risk_thread_is_running();
}
//______
//ALT-system
//Output the areastate and measure number (system id) to display ALT (menu ALT/)
void Main_Wid::show_systemid(void){
	ostringstream text;
	text << "CURRENT STATES (see also status bar)" << endl;
	text << " Areastate    : " <<  this->system_state.get_sys_system_id().area_state  << endl;
	text << " Measurestate : " << this->system_state.get_sys_system_id().measure_nr << endl ;
	Sys_Common_Output::output_alt->output_txt(&text);
	this->tabWidget_outtext->setCurrentWidget(this->tab_alttxt);
}
//Set a new measure state (menu ALT/new area-/measure state)
void Main_Wid::set_new_measure_state(void){
	if(this->check_switch_new_implement_measure(false)==false){
		return;
	}

	try{
		this->allocate_alt_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	//check if all criteria are set
	this->alt_calc->set_ptr2database(this->system_database->get_database());

	try{
		if(this->alt_calc->ask_new_measure_data(this, this->risk_flags.catchment_risk_applied)==false){
			this->delete_alt_system();
		}
		else{
			this->alt_calc->set_thread_type(_alt_thread_type::alt_insert);

			//connect the thread when is finished
			QObject::connect(this->alt_calc,SIGNAL(finished()),this,SLOT(thread_measure_implement_is_finished()));
			this->action_stop_measure_implementation->setEnabled(true);
			this->menu_edit_area_measure_state->setEnabled(false);
			this->action_new_measure_state->setEnabled(false);
			this->action_new_area_state->setEnabled(false);
			this->action_switch_measure_state->setEnabled(false);
			this->menu_RISK_system->setEnabled(false);
			this->action_delete_measure_state->setEnabled(false);
			this->tabWidget_treeview->widget(1)->setEnabled(false);

			//start calculation
			this->reset_exception_new_action();
			this->alt_calc->start();
			this->check_alt_thread_is_running();
		}
	}
	catch(Error msg){
		this->delete_alt_system();
		msg.output_msg(6);
		return;
	}
}
//Set a new area state (menu ALT/new area-/measure state)
void Main_Wid::set_new_area_state(void){
}
//User aborted the alternative implmentation thread (menu ALT/new area-/measure state)
void Main_Wid::stop_alt_implement_thread(void){
	//be sure that the thread is allocated and started
	if(this->alt_calc!=NULL && this->alt_calc->isRunning()==true){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"Do you want stop the ALT-thread?" << endl;
		txt<<"Result data will be lost" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			return;
		}
		else{
			this->alt_calc->set_stop_thread_flag(true);
		}
	}
}
//Set a new measure state (menu ALT/edit area-/measure state)
void Main_Wid::edit_measure_state(void){
	try{
		this->allocate_alt_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	//check if all criteria are set
	this->alt_calc->set_ptr2database(this->system_database->get_database());

	try{
		if(this->alt_calc->ask_edit_measure_data(this, this->risk_flags.catchment_risk_applied)==false){
			this->delete_alt_system();
		}
	}
	catch(Error msg){
		this->delete_alt_system();
		msg.output_msg(6);
		return;
	}
	this->delete_alt_system();
}
//Set a new area state (menu ALT/edit area-/measure state)
void Main_Wid::edit_area_state(void){
}
//Check for switching or implemeneting a new measure state
bool Main_Wid::check_switch_new_implement_measure(const bool switching){
	try{
		//check cost data availabe
		if(this->system_state.get_sys_system_id().measure_nr!=0 &&
			Cost_Ecn_Cost_System::check_data_in_cost_table(this->system_database->get_database(), this->system_state.get_sys_system_id())==false){
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			txt<<"There are no economical cost data set for the current measure!"<< endl;
			if(switching ==false){
				txt<<"Set first the cost data before you implement a new measure."<< endl;
			}
			else{
				txt<<"Set first the cost data before you switch to another measure."<< endl;
			}
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}

		bool nobreak=Risk_System::check_data_in_resulttable(this->system_database->get_database(), this->system_state.get_sys_system_id(), _risk_type::nobreak_risk);
		bool catchment=Risk_System::check_data_in_resulttable(this->system_database->get_database(), this->system_state.get_sys_system_id(), _risk_type::catchment_risk);

		if(catchment==true){
			this->allocate_risk_system();
			this->risk_calc->set_ptr2database(this->system_database->get_database());
			if(this->risk_calc->check_probability_boundary_is_reached()==false){
				Sys_Diverse_Text_Dia dialog2;
				dialog2.set_button_text("Yes", "No");
				ostringstream txt;
				txt<<"The given probability boundary for the catchment risk approach is not reached!"<< endl;
				txt<<"Check the statistics of risk calculations."<< endl;
				txt<<"A unreliable comparison of the alternatives is possible.  "<< endl;
				txt<<"Do you want to continue?"<<endl;
				dialog2.set_dialog_question(txt.str());
				if(dialog2.start_dialog()==false){
					this->delete_risk_system();
					return false;
				}
			}
			this->delete_risk_system();
		}

		if(nobreak==false && catchment==false){
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			if(this->system_state.get_sys_system_id().measure_nr==0){
				txt<<"There are no risk data set for the base measure state (0)!"<< endl;
			}
			else{
				txt<<"There are no risk data set for the current measure!"<< endl;
			}
			if(switching ==false){
				txt<<"Calculate first the risk data before you implement a new measure."<< endl;
			}
			else{
				txt<<"Calculate first the risk data before you switch to another measure."<< endl;
			}
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}

		if(this->system_state.get_sys_system_id().area_state==0 && this->system_state.get_sys_system_id().measure_nr==0){
			if(nobreak==false && catchment==false){
				Sys_Diverse_Text_Dia dialog2(true);
				ostringstream txt;

				txt<<"There are no risk data (nobreak/catchment) set for the base measure state (0)!"<< endl;
				if(switching ==false){
					txt<<"Calculate first the risk data (nobreak) before you implement a new measure."<< endl;
				}
				else{
					txt<<"Calculate first the risk data (nobreak) before you switch to another measure."<< endl;
				}
				dialog2.set_dialog_question(txt.str());
				dialog2.start_dialog();
				return false;
			}
			else if(nobreak==false && catchment==true){
				Sys_Diverse_Text_Dia dialog2;
				ostringstream txt;

				txt<<"There are just risk data available for the catchment risk approach for the base measure state (0)!"<<endl;
				txt<<"A further application of the nobreak risk approach won't be possible!"<< endl;
				txt<<"Do you want to continue?"<< endl;
				dialog2.set_dialog_question(txt.str());
				if(dialog2.start_dialog()==false){
					return false;
				}
				else{
					this->risk_flags.nobreak_risk_applied=false;
					return true;
				}
			}
			else if(nobreak==true && catchment==false){
				Sys_Diverse_Text_Dia dialog2;
				ostringstream txt;

				txt<<"There are just risk data available for the nobreak risk approach for the base measure state (0)!"<<endl;
				txt<<"A further application of the catchment risk approach won't be possible!"<< endl;
				txt<<"Do you want to continue?"<< endl;
				dialog2.set_dialog_question(txt.str());
				if(dialog2.start_dialog()==false){
					return false;
				}
				else{
					this->risk_flags.catchment_risk_applied=false;
					return true;
				}
			}
		}

		//check risk data availabe
		if(this->risk_flags.nobreak_risk_applied==true){
			if(nobreak==false){
				Sys_Diverse_Text_Dia dialog2(true);
				ostringstream txt;
				if(this->system_state.get_sys_system_id().measure_nr==0){
					txt<<"There are no risk data (nobreak) set for the base measure state (0)!"<< endl;
				}
				else{
					txt<<"There are no risk data (nobreak) set for the current measure!"<< endl;
				}
				if(switching ==false){
					txt<<"Calculate first the risk data (nobreak) before you implement a new measure."<< endl;
				}
				else{
					txt<<"Calculate first the risk data (nobreak) before you switch to another measure."<< endl;
				}
				dialog2.set_dialog_question(txt.str());
				dialog2.start_dialog();
				return false;
			}
		}

		if(this->risk_flags.catchment_risk_applied==true){
			if(catchment==false){
				Sys_Diverse_Text_Dia dialog2(true);
				ostringstream txt;
				if(this->system_state.get_sys_system_id().measure_nr==0){
					txt<<"There are no risk data (catchment) set for the base measure state (0)!"<< endl;
				}
				else{
					txt<<"There are no risk data (catchment) set for the current measure!"<< endl;
				}
				if(switching ==false){
					txt<<"Calculate first the risk data (catchment) before you implement a new measure."<< endl;
				}
				else{
					txt<<"Calculate first the risk data (catchment) before you switch to another measure."<< endl;
				}
				dialog2.set_dialog_question(txt.str());
				dialog2.start_dialog();
				return false;
			}
		}
	}
	catch(Error msg){
		msg.output_msg(0);
		return false;
	}
	return true;
}
//Switch the measure state to a another one, which is already calculated (menu ALT/switch measure state)
void Main_Wid::switch_measure_state(void){
	if(this->check_switch_new_implement_measure(true)==false){
		return;
	}

	try{
		this->allocate_alt_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	//set the database
	this->alt_calc->set_ptr2database(this->system_database->get_database());
	cout <<" test"<<endl;
	try{
		if(this->alt_calc->ask_measure_state2switch(this)==false){
			this->delete_alt_system();
		}
		else{
			this->alt_calc->set_thread_type(_alt_thread_type::alt_switch);

			//connect the thread when is finished
			QObject::connect(this->alt_calc,SIGNAL(finished()),this,SLOT(thread_measure_switch_is_finished()));
			this->menu_edit_area_measure_state->setEnabled(false);
			this->action_new_measure_state->setEnabled(false);
			this->action_new_area_state->setEnabled(false);
			this->menu_RISK_system->setEnabled(false);
			this->action_switch_measure_state->setEnabled(false);
			this->action_delete_measure_state->setEnabled(false);
			this->tabWidget_treeview->widget(1)->setEnabled(false);

			//start calculation
			this->reset_exception_new_action();
			this->alt_calc->start();
			this->check_alt_thread_is_running();
		}
	}
	catch(Error msg){
		this->delete_alt_system();
		msg.output_msg(6);
		return;
	}
}
//Delete a choosen measure state
void Main_Wid::delete_measure_state(void){
	try{
		this->allocate_alt_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	//set the database
	this->alt_calc->set_ptr2database(this->system_database->get_database());

	try{
		if(this->alt_calc->ask_measure_state2delete(this)==false){
			this->delete_alt_system();
		}
		else{
			///MADM_System (because of dependencies)
			Madm_Decision_Matrix::delete_data_in_table_matrix(this->system_database->get_database(), this->alt_calc->get_system_state_del_swi());
			_Madm_Solver::delete_data_in_table_ranking(this->system_database->get_database(), this->alt_calc->get_system_state_del_swi());

			this->alt_calc->set_thread_type(_alt_thread_type::alt_del);

			//connect the thread when is finished
			QObject::connect(this->alt_calc,SIGNAL(finished()),this,SLOT(thread_measure_switch_is_finished()));
			this->menu_edit_area_measure_state->setEnabled(false);
			this->action_new_measure_state->setEnabled(false);
			this->action_new_area_state->setEnabled(false);
			this->menu_RISK_system->setEnabled(false);
			this->action_switch_measure_state->setEnabled(false);
			this->action_delete_measure_state->setEnabled(false);

			//start calculation
			this->reset_exception_new_action();
			this->alt_calc->start();
			this->check_alt_thread_is_running();
		}
	}
	catch(Error msg){
		this->delete_alt_system();
		msg.output_msg(6);
		return;
	}
}
//Create the database tables for the alt system
void Main_Wid::create_alt_system_database_tables(void){
	try{
		this->allocate_alt_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->alt_calc->set_thread_type(_alt_thread_type::alt_create_tab);

	//connect the thread when is finished
	QObject::connect(this->alt_calc,SIGNAL(finished()),this,SLOT(thread_alt_create_tables_finished()));
	//this->action_stop_risk_calc->setEnabled(true);

	this->alt_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->alt_calc->start();
	this->check_alt_thread_is_running();
	this->alt_tables_created=true;
}
//Check for the normal end of the alt create-tables thread
void Main_Wid::thread_alt_create_tables_finished(void){
	if(this->alt_calc!=NULL && this->alt_calc->isRunning()==false){
		//this->action_stop_alt_calc->setEnabled(false);

		this->check_alt_thread_is_running();

		//disconnect the thread when is finished
		QObject::disconnect(this->alt_calc,SIGNAL(finished()),this,SLOT(thread_alt_create_tables_finished()));
		this->delete_alt_system();
		emit send_table_creation_is_finished();
	}
}
//Check the database tables for the alt system (menu alt/common)
void Main_Wid::check_alt_system_database_tables(void){
	try{
		this->allocate_alt_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->alt_calc->set_thread_type(_alt_thread_type::alt_check_tab);

	//connect the thread when is finished
	QObject::connect(this->alt_calc,SIGNAL(finished()),this,SLOT(thread_alt_check_tables_finished()));
	//this->action_stop_alt_calc->setEnabled(true);

	this->alt_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->alt_calc->start();
	this->check_alt_thread_is_running();
	this->alt_tables_created=true;
}
//Check for the normal end of the alt check-datbase tables thread
void Main_Wid::thread_alt_check_tables_finished(void){
	if(this->alt_calc!=NULL && this->alt_calc->isRunning()==false){
		//this->action_stop_alt_calc->setEnabled(false);

		this->check_alt_thread_is_running();

		this->number_error_table_check=this->number_error_table_check+this->alt_calc->get_number_error();

		//disconnect the thread when is finished
		QObject::disconnect(this->alt_calc,SIGNAL(finished()),this,SLOT(thread_alt_check_tables_finished()));
		this->delete_alt_system();
		if (this->task_flag == true && this->project_manager.get_project_type() == _sys_project_type::proj_all) {
			emit send_task_by_file_start();
		}
		emit send_table_check_is_finished();
	}
}
//Thread of the measure's implememtation is finished
void Main_Wid::thread_measure_implement_is_finished(void){
	this->action_stop_measure_implementation->setEnabled(false);
	this->menu_edit_area_measure_state->setEnabled(true);
	this->action_new_measure_state->setEnabled(true);
	this->action_new_area_state->setEnabled(true);
	this->action_switch_measure_state->setEnabled(true);
	this->action_delete_measure_state->setEnabled(true);
	this->menu_RISK_system->setEnabled(true);
	this->tabWidget_treeview->widget(1)->setEnabled(true);
	this->check_alt_thread_is_running();

	if(this->alt_calc->get_measure_implementation_ok()==true){
		if(this->system_state.get_sys_system_id().measure_nr==0 && Risk_System::check_data_in_resulttable(this->system_database->get_database(), this->system_state.get_sys_system_id(), _risk_type::nobreak_risk)==false){
			this->risk_flags.nobreak_risk_applied=false;
		}
		if(this->system_state.get_sys_system_id().measure_nr==0 && Risk_System::check_data_in_resulttable(this->system_database->get_database(), this->system_state.get_sys_system_id(), _risk_type::catchment_risk)==false){
			this->risk_flags.catchment_risk_applied=false;
		}

		//general
		this->system_state.set_systemid(this->alt_calc->get_sys_system_id());
		emit send_delete2refresh_data_view();
		this->project_manager.set_current_system_state(this->system_state.get_sys_system_id());
		this->status_wid->set_system_state(this->system_state.get_sys_system_id());

		this->risk_flags.risk_state_flag=true;
		this->risk_flags.reliability_dam_result=true;
		this->risk_flags.reliability_fpl_result=true;
		this->risk_flags.reliability_hyd_result=true;
		this->status_wid->get_ptr_risk_state_check_box()->setEnabled(false);

		this->set_risk_state_flag(this->risk_flags);
		this->save_project_parameter();
		//alt
		this->menu_edit_area_measure_state->setEnabled(true);
		//cost
		this->menu_COST_system->setEnabled(true);
		this->menu_cost_ecn->setEnabled(true);
		this->action_ecn_cost_user_defined->setEnabled(true);
		//hyd
		this->menu_hyd_import->setEnabled(false);
		this->action_delete_boundary_scenario->setEnabled(false);
		//dam
		this->menu_dam_ecn->setEnabled(false);
		this->menu_dam_eco->setEnabled(false);
		this->menu_dam_pys->setEnabled(false);
		this->menu_dam_pop->setEnabled(false);
		this->menu_dam_sc->setEnabled(false);
		this->menu_dam_CI->setEnabled(false);
		this->action_dam_connect->setEnabled(false);
		//fpl
		this->action_fpl_import_file->setEnabled(false);
		this->action_delete_fpl_section->setEnabled(false);

		try{
			string buff;
			//set results of fpl-sections by hand
			if(Fpl_Section::check_set_by_hand_section_result(this->system_database->get_database(), this->system_state.get_sys_system_id(), &buff)==true){
				//allocate the thread
				try{
					this->allocate_fpl_system();
				}
				catch(Error msg){
					throw msg;
				}
				//set thread specific members
				this->fpl_calc->set_ptr2database(this->system_database->get_database());
				this->fpl_calc->set_thread_type(_fpl_thread_type::fpl_frc_by_hand);
				try{
					this->fpl_calc->set_list_section_ids_current_set_by_hand();
					//start the not as a thread
					this->fpl_calc->run();
				}
				catch(Error msg){
					throw msg;
				}
			}
		}
		catch(Error msg){
			msg.output_msg(0);
			this->delete_fpl_system();
		}
	}
	else{
		ostringstream cout;
		cout << "Measure could not be implement into the system!"<<endl;
		Sys_Common_Output::output_alt->output_txt(&cout);
	}

	this->delete_alt_system();
}
//Thread to switch the measure state id finished
void Main_Wid::thread_measure_switch_is_finished(void){
	this->action_stop_measure_implementation->setEnabled(false);
	this->menu_edit_area_measure_state->setEnabled(true);
	this->action_new_measure_state->setEnabled(true);
	this->action_new_area_state->setEnabled(true);
	this->action_switch_measure_state->setEnabled(true);
	this->action_delete_measure_state->setEnabled(true);
	this->menu_RISK_system->setEnabled(true);
	this->tabWidget_treeview->widget(1)->setEnabled(true);
	this->check_alt_thread_is_running();

	//general
	this->system_state.set_systemid(this->alt_calc->get_sys_system_id());
	emit send_delete2refresh_data_view();
	this->project_manager.set_current_system_state(this->system_state.get_sys_system_id());
	this->status_wid->set_system_state(this->system_state.get_sys_system_id());

	this->action_switch_measure_state->setEnabled(true);
	this->action_madm_calc_database->setEnabled(true);
	this->save_project_parameter();
	//alt
	this->menu_edit_area_measure_state->setEnabled(true);
	//cost
	this->menu_COST_system->setEnabled(true);
	this->menu_cost_ecn->setEnabled(true);
	//hyd
	this->menu_hyd_import->setEnabled(false);
	this->action_delete_boundary_scenario->setEnabled(false);
	//dam
	this->menu_dam_ecn->setEnabled(false);
	this->menu_dam_eco->setEnabled(false);
	this->menu_dam_pys->setEnabled(false);
	this->menu_dam_pop->setEnabled(false);
	this->menu_dam_sc->setEnabled(false);
	this->menu_dam_CI->setEnabled(false);
	this->action_dam_connect->setEnabled(false);
	//fpl
	this->action_fpl_import_file->setEnabled(false);
	this->action_delete_fpl_section->setEnabled(false);

	this->delete_alt_system();
}
//Set a new output flag for the alt modul(menu alt/common)
void Main_Wid::set_alt_outputflag(void){
	Sys_Common_Output::output_alt->set_detailflag_gui(this);
}
//Set a new output logfile for the alt modul(menu alt/common)
void Main_Wid::set_alt_outputlogfile(void){
	Sys_Common_Output::output_alt->set_new_logfile_gui(this);
}
//Reset the display ALT (menu alt/common)
void Main_Wid::reset_alt_outputtxt(void){
	//clear the display
	this->textEdit_alttxt->clear();
	Sys_Common_Output::output_alt->welcome_to_gui();
	Sys_Common_Output::output_alt->reset_prefix_was_outputed();
	ostringstream cout;
	cout << "ALT-display is cleared" << endl;
	Sys_Common_Output::output_system->output_txt(cout.str(),false);
}
//______
//COST-system
//Set user-defined cost data (menu cost/ECoNomic/)
void Main_Wid::set_user_defined_ecn_cost(void){
	try{
		this->allocate_cost_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->cost_calc->set_ptr2database(this->system_database->get_database());

	try{
		if(this->cost_calc->ask_ecn_cost_per_dia(this)==false){
			this->delete_cost_system();
		}
	}
	catch(Error msg){
		msg.output_msg(7);
	}

	this->delete_cost_system();
}
//Create the database tables for the cost system
void Main_Wid::create_cost_system_database_tables(void){
	try{
		this->allocate_cost_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->cost_calc->set_thread_type(_cost_thread_type::cost_create_tab);

	//connect the thread when is finished
	QObject::connect(this->cost_calc,SIGNAL(finished()),this,SLOT(thread_cost_create_tables_finished()));
	//this->action_stop_risk_calc->setEnabled(true);

	this->cost_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->cost_calc->start();
	this->check_cost_thread_is_running();
	this->cost_tables_created=true;
}
//Check for the normal end of the cost create-tables thread
void Main_Wid::thread_cost_create_tables_finished(void){
	if(this->cost_calc!=NULL && this->cost_calc->isRunning()==false){
//		this->action_stop_cost_calc->setEnabled(false);

		this->check_cost_thread_is_running();

		//disconnect the thread when is finished
		QObject::disconnect(this->cost_calc,SIGNAL(finished()),this,SLOT(thread_cost_create_tables_finished()));
		this->delete_cost_system();
		emit send_table_creation_is_finished();
	}
}
//Check the database tables for the cost system (menu cost/common)
void Main_Wid::check_cost_system_database_tables(void){
	try{
		this->allocate_cost_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->cost_calc->set_thread_type(_cost_thread_type::cost_check_tab);

	//connect the thread when is finished
	QObject::connect(this->cost_calc,SIGNAL(finished()),this,SLOT(thread_cost_check_tables_finished()));
	//this->action_stop_cost_calc->setEnabled(true);

	this->cost_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->cost_calc->start();
	this->check_cost_thread_is_running();
	this->cost_tables_created=true;
}
//Check for the normal end of the cost check-datbase tables thread
void Main_Wid::thread_cost_check_tables_finished(void){
	if(this->cost_calc!=NULL && this->cost_calc->isRunning()==false){
		//this->action_stop_cost_calc->setEnabled(false);

		this->check_cost_thread_is_running();

		this->number_error_table_check=this->number_error_table_check+this->cost_calc->get_number_error();

		//disconnect the thread when is finished
		QObject::disconnect(this->cost_calc,SIGNAL(finished()),this,SLOT(thread_cost_check_tables_finished()));
		this->delete_cost_system();
		emit send_table_check_is_finished();
	}
}
//Set a new output flag for the cost modul(menu cost/common)
void Main_Wid::set_cost_outputflag(void){
	Sys_Common_Output::output_cost->set_detailflag_gui(this);
}
//Set a new output logfile for the cost modul(menu cost/common)
void Main_Wid::set_cost_outputlogfile(void){
	Sys_Common_Output::output_cost->set_new_logfile_gui(this);
}
//Reset the display COST (menu cost/common)
void Main_Wid::reset_cost_outputtxt(void){
	//clear the display
	this->textEdit_costtxt->clear();
	Sys_Common_Output::output_cost->welcome_to_gui();
	Sys_Common_Output::output_cost->reset_prefix_was_outputed();
	ostringstream cout;
	cout << "COST-display is cleared" << endl;
	Sys_Common_Output::output_system->output_txt(cout.str(),false);
}
//________
//menu MADM
//User aborted the madm calculation thread (menu madm)
void Main_Wid::stop_madmcalc_thread(void){
	//be sure that the thread is allocated and started
	if(this->madm_calc!=NULL && this->madm_calc->isRunning()==true){
		Sys_Diverse_Text_Dia dialog2;
		ostringstream txt;
		txt<<"Do you want stop the MADM-thread?" << endl;
		txt<<"Result data will be lost" << endl;
		dialog2.set_dialog_question(txt.str());
		bool flag2=dialog2.start_dialog();
		if(flag2==false){
			return;
		}
		else{
			this->madm_calc->set_stop_thread_flag(true);
		}
	}
}
//Import weigthing set via a file to database for the madm-analysis (menu madm/import)
void Main_Wid::import_weighting_set_file2database(void){
	try{
		this->allocate_madm_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->madm_calc->set_thread_type(_madm_thread_type::madm_imp_weight);

	//ask for the set
	try{
		if(this->madm_calc->set_weight_set_number_file_gui(this)==false){
			this->delete_madm_system();
			return;
		}
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	this->madm_calc->set_ptr2database(this->system_database->get_database());

	//connect the thread when is finished
	QObject::connect(this->madm_calc,SIGNAL(finished()),this,SLOT(thread_madm_calc_finished()));
	this->action_stop_madm_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->madm_calc->start();
	this->check_madm_thread_is_running();
}
//Delete selected weighting sets from database table (menu madm/)
void Main_Wid::delete_selcted_weighting_set_database(void){
	try{
		this->allocate_madm_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->madm_calc->set_thread_type(_madm_thread_type::madm_del_weight);
	this->madm_calc->set_ptr2database(this->system_database->get_database());

	//ask for the set
	try{
		if(this->madm_calc->ask_weighting_set2delete(this)==false){
			this->delete_madm_system();
			return;
		}
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	//connect the thread when is finished
	QObject::connect(this->madm_calc,SIGNAL(finished()),this,SLOT(thread_madm_calc_finished()));
	this->action_stop_madm_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->madm_calc->start();
	this->check_madm_thread_is_running();
}
//Perform a madm-analysis via for a file input (menu madm/calculation)
void Main_Wid::madm_analysis_file(void){
	try{
		this->allocate_madm_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->madm_calc->set_thread_type(_madm_thread_type::madm_calc_file);
	try{
		//ask for the file
		if(this->madm_calc->set_analysis_number_file_gui(this)==false){
			this->delete_madm_system();
			return;
		}
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}

	//connect the thread when is finished
	QObject::connect(this->madm_calc,SIGNAL(finished()),this,SLOT(thread_madm_calc_finished()));
	this->action_stop_madm_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->madm_calc->start();
	this->check_madm_thread_is_running();
}
//Perform a madm-analysis via for database input (menu madm/calculation)
void Main_Wid::madm_analysis_database(void){
	try{
		this->allocate_madm_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->madm_calc->set_thread_type(_madm_thread_type::madm_calc_database);
	this->madm_calc->set_ptr2database(this->system_database->get_database());
	//check set
	if(this->madm_calc->check_data_available_in_database(this->risk_flags.nobreak_risk_applied, this->risk_flags.catchment_risk_applied)==false){
		this->delete_madm_system();
		return;
	}

	//connect the thread when is finished
	QObject::connect(this->madm_calc,SIGNAL(finished()),this,SLOT(thread_madm_calc_finished()));
	this->action_stop_madm_calc->setEnabled(true);

	//start calculation
	this->reset_exception_new_action();
	this->madm_calc->start();
	this->check_madm_thread_is_running();
}
//Check for the normal end of the madm calculation thread
void Main_Wid::thread_madm_calc_finished(void){
	if(this->madm_calc!=NULL && this->madm_calc->isRunning()==false){
		this->action_stop_madm_calc->setEnabled(false);

		this->check_madm_thread_is_running();

		//disconnect the thread when is finished
		QObject::disconnect(this->madm_calc,SIGNAL(finished()),this,SLOT(thread_madm_calc_finished()));
		if(this->madm_calc->get_thread_type()==_madm_thread_type::madm_del_weight || this->madm_calc->get_thread_type()==_madm_thread_type::madm_imp_weight
			|| this->madm_calc->get_thread_type()==_madm_thread_type::madm_calc_database){
			emit send_delete2refresh_data_view();
		}
		this->delete_madm_system();
	}
}
//Create the database tables for the madm system
void Main_Wid::create_madm_system_database_tables(void){
	try{
		this->allocate_madm_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->madm_calc->set_thread_type(_madm_thread_type::madm_create_tab);

	//connect the thread when is finished
	QObject::connect(this->madm_calc,SIGNAL(finished()),this,SLOT(thread_madm_create_tables_finished()));
	this->action_stop_madm_calc->setEnabled(true);

	this->madm_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->madm_calc->start();
	this->check_madm_thread_is_running();
	this->madm_tables_created=true;
}
//Check for the normal end of the madm create-tables thread
void Main_Wid::thread_madm_create_tables_finished(void){
	if(this->madm_calc!=NULL && this->madm_calc->isRunning()==false){
		this->action_stop_madm_calc->setEnabled(false);

		this->check_madm_thread_is_running();

		//disconnect the thread when is finished
		QObject::disconnect(this->madm_calc,SIGNAL(finished()),this,SLOT(thread_madm_create_tables_finished()));
		this->delete_madm_system();
		emit send_table_creation_is_finished();
	}
}
//Check the database tables for the risk system (menu madm/common)
void Main_Wid::check_madm_system_database_tables(void){
	try{
		this->allocate_madm_system();
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
	this->madm_calc->set_thread_type(_madm_thread_type::madm_check_tab);

	//connect the thread when is finished
	QObject::connect(this->madm_calc,SIGNAL(finished()),this,SLOT(thread_madm_check_tables_finished()));
	this->action_stop_madm_calc->setEnabled(true);

	this->madm_calc->set_ptr2database(this->system_database->get_database());
	//start calculation
	this->madm_calc->start();
	this->check_madm_thread_is_running();
	this->madm_tables_created=true;
}
//Check for the normal end of the madm check-datbase tables thread
void Main_Wid::thread_madm_check_tables_finished(void){
	if(this->madm_calc!=NULL && this->madm_calc->isRunning()==false){
		this->action_stop_madm_calc->setEnabled(false);

		this->check_madm_thread_is_running();

		this->number_error_table_check=this->number_error_table_check+this->madm_calc->get_number_error();

		//disconnect the thread when is finished
		QObject::disconnect(this->madm_calc,SIGNAL(finished()),this,SLOT(thread_madm_check_tables_finished()));
		this->delete_madm_system();
		emit send_table_check_is_finished();
	}
}
//Set a new output flag for the madm modul(menu madm/common)
void Main_Wid::set_madm_outputflag(void){
	Sys_Common_Output::output_madm->set_detailflag_gui(this);
}
//Set a new output logfile for the madm modul(menu madm/common)
void Main_Wid::set_madm_outputlogfile(void){
	Sys_Common_Output::output_madm->set_new_logfile_gui(this);
}
//Reset the display MADM (menu madm/common)
void Main_Wid::reset_madm_outputtxt(void){
	//clear the display
	this->textEdit_madmtxt->clear();
	Sys_Common_Output::output_madm->welcome_to_gui();
	Sys_Common_Output::output_madm->reset_prefix_was_outputed();
	ostringstream cout;
	cout << "MADM-display is cleared" << endl;
	Sys_Common_Output::output_system->output_txt(cout.str(),false);
}
//______
//menu window
//Open/close explorer display
void Main_Wid::open_close_explorer_display(bool flag){
	if(flag==true){
		this->dockWidget_tree_view->show();
	}
	else{
		this->dockWidget_tree_view->close();
	}
}
//Open/close status window
void Main_Wid::open_close_status_display(bool flag){
	if(flag==true){
		this->status_wid->setMinimumHeight(85);
		this->status_wid->close_wid(false);
		
		
	}
	else{
		this->status_wid->close_wid(false);
		this->status_wid->setMinimumHeight(20);
		this->statusbar->adjustSize();
	}
}
//Open/close output display
void Main_Wid::open_close_output_display(bool flag){
	if(flag==true){
		this->dockWidget_outtxt->show();
	}
	else{
		this->dockWidget_outtxt->close();
	}
}
//Uncheck the explorer display action
void Main_Wid::uncheck_explorer_display(void){
	this->action_explorer_display->setChecked(false);
}
//Uncheck the output display action
void Main_Wid::uncheck_output_display(void){
	this->action_output_display->setChecked(false);
}
//Uncheck the status display action
void Main_Wid::uncheck_status_display(bool flag){
	this->action_Status_Display->setChecked(flag);
	if(flag==true){
		this->status_wid->setMinimumHeight(85);
	}
	else{
		this->status_wid->setMinimumHeight(20);
		this->statusbar->adjustSize();
	}
	
}
//________
//menu HELP
//Message with the "about"-text (menu help)
void Main_Wid::about(void){
	ostringstream text;
	text << "ProMaIDes (Protection Measures against Inundation Desicion Support). <br><br>" << endl;
    text << "This is version "<<Sys_Project::get_version_number()<<" produced at "<<Sys_Project::get_version_date() <<".<br><br>"<<endl;
    text << "Produced by the <a href ='https://tinyurl.com/agfrm77'>AG FRM</a> of the University of applied sciences Magdeburg-Stendal<br> and the <a href ='https://www.iww.rwth-aachen.de/go/id/lygz/?lidx=1'>IWW</a> of RWTH Aachen University.<br><br>"<< endl;
    text << "Copyright by AG FRM (HS-M), IWW (RWTH) 2023.<br>"<< endl;
	text << "It is distributed under <a href='https://promaides.myjetbrains.com/youtrack/articles/PMID-A-34/License'>a GPL-3.0-only license</a>." << endl;
	text << "This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under GPL-3.0-only conditions." << endl;
	QString buff;
	buff = text.str().c_str();

	QMessageBox msgBox(this);
	msgBox.setWindowTitle("About ProMaIDeS");
	msgBox.setTextFormat(Qt::RichText);   //this is what makes the links clickable
	msgBox.setText(buff);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.exec();
	

}
//Open the available online guides (menu help)
void Main_Wid::open_doku(void){

	ostringstream text;
	text << "ProMaIDes (Protection Measures against Inundation Desicion Support). <br><br>" << endl;
	text << "Please follow our online manuals:<br><br>" << endl;
	text << " - <a href ='https://promaides.myjetbrains.com/youtrack/articles/PMID-A-7/General'>General information</a> <br>" << endl;
	text << " - <a href ='https://promaides.myjetbrains.com/youtrack/articles/PMID-A-1/Theory-guide'>Theory guide</a> <br>" << endl;
	text << " - <a href ='https://promaides.myjetbrains.com/youtrack/articles/PMID-A-5/Application-guide'>Application guide</a> <br>" << endl;
	text << " - <a href ='https://promaides.myjetbrains.com/youtrack/articles/PMID-A-12/Development-guide'>Development guide</a> <br>" << endl;
	text << " - <a href ='https://promaides.myjetbrains.com/youtrack/articles/PMDP-A-31/General'>Guide to ProMaIDes Helpers</a> <br>" << endl;

	QString buff;
	buff = text.str().c_str();

	QMessageBox msgBox(this);
	msgBox.setWindowTitle("Help ProMaIDeS");
	msgBox.setTextFormat(Qt::RichText);   //this is what makes the links clickable
	msgBox.setText(buff);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.exec();

	//proc_developer_doku.close();

	//ostringstream path_prog;
	//path_prog<< "C:/Program Files (x86)/Mozilla Firefox/firefox.exe";
	//QStringList list_arg;

	//list_arg<<"file:///L:/daniel/work/promaides/doku/html/index.html";

	//proc_developer_doku.start(path_prog.str().c_str(), list_arg);
}
//Open the Community of users (menu help)
void Main_Wid::open_com_users(void) {
	ostringstream text;
	text << "ProMaIDes (Protection Measures against Inundation Desicion Support). <br><br>" << endl;
	text << "Interested in more information; please join our <a href ='https://promaides.myjetbrains.com/youtrack/articles/PMID-A-33/Community-of-users'>community of users</a>.<br>" << endl;

	QString buff;
	buff = text.str().c_str();

	QMessageBox msgBox(this);
	msgBox.setWindowTitle("Community of users ProMaIDeS");
	msgBox.setTextFormat(Qt::RichText);   //this is what makes the links clickable
	msgBox.setText(buff);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.exec();
}
//Allocate the table view for the database tables
void Main_Wid::allocate_db_table_view(void){
	this->delete_db_table_view();
	this->database_table_view=new Sys_Sql_Table_Wid;
	this->browser_database->set_ptr_table_model(this->database_table_view);
	this->tab_data_database->layout()->addWidget(this->database_table_view);
	this->database_table_view->show();
	//no editing allowod for the table view
	this->database_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
//Delete the table view for the database tables
void Main_Wid::delete_db_table_view(void){
	if(this->database_table_view!=NULL){
		delete this->database_table_view;
		this->database_table_view=NULL;
	}
}
//restore th widget settings
void Main_Wid::readSettings(void)
{
	QSettings settings("AG_FRM", "MyProMaIDes");
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("windowState").toByteArray());
}
//Handles the close event by the close-button of the main-widget
void Main_Wid::closeEvent(QCloseEvent *close){
	if(this->close_flag==false && this->closing_thread==NULL){
		this->terminate_threads_close_app();
		close->ignore();
	}
	else if(this->close_flag==false){
		close->ignore();
	}

	if(this->close_flag==true){
		


		QSettings settings("AG_FRM", "MyProMaIDes");
		settings.setValue("geometry", saveGeometry());
		settings.setValue("windowState", saveState());
		QMainWindow::closeEvent(close);

		close->accept();
		this->close();
	}
}
//Read task file
void Main_Wid::read_task_file(void) {
	int line_counter = 0;
	QFile ifile;

	ostringstream cout;
	ostringstream prefix;
	prefix << "INP" << "> ";
	Sys_Common_Output::output_system->set_userprefix(&prefix);
	cout << "Read task-file " << this->task_file_name<< endl;
	Sys_Common_Output::output_system->output_txt(&cout, false);

	//read in task file
	try {
		//open file
		ifile.setFileName(this->task_file_name.c_str());
		QIODevice::OpenMode my_flag;
		my_flag = my_flag | (QIODevice::ReadOnly);
		my_flag = my_flag | (QIODevice::Text);
		ifile.open(my_flag);
		if (ifile.isOpen() == false) {
			Error msg = this->set_error(2);
			ostringstream info;
			info << "Filename: " << this->task_file_name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	catch (Error msg) {
		throw msg;
	}

	string myline;
	QString qmyline;
	stringstream buffer1;


	//search for number of task
	do {
		qmyline = ifile.readLine();
		myline = qmyline.toStdString();
		(line_counter)++;
		functions::clean_string(&myline);
		if (myline.empty() ==false) {
			buffer1 << myline;
			buffer1 >> this->number_task;
			if (buffer1.fail() == true) {
				ostringstream info;
				info << "Reading number of task" << endl;
				info << "File                  : " << this->task_file_name << endl;
				info << "Wrong input sequenze  : " << buffer1.str() << endl;
				Error msg = this->set_error(3);
				msg.make_second_info(info.str());
				throw msg;
			}
			break;
		}
	} while (ifile.atEnd() == false);

	cout << "Found " << this->number_task <<" tasks"<< endl;
	Sys_Common_Output::output_system->output_txt(&cout, false);

	//read in tasks
	do {
		qmyline = ifile.readLine();
		myline = qmyline.toStdString();
		(line_counter)++;
		functions::clean_string(&myline);
		if (myline.empty() == false) {
			qmyline = myline.c_str();
			QStringList my_str_list;
			my_str_list = qmyline.split(",");



			QList<QVariant> buff_list;
			for (int i = 0; i < my_str_list.count(); i++) {
				string buff_space = my_str_list.at(i).toStdString();
				functions::clean_string(&buff_space);
				buff_list.append(buff_space.c_str());
			}
			this->task_list.append(buff_list);
		}
	} while (ifile.atEnd() == false);

	Sys_Common_Output::output_system->rewind_userprefix();
	ifile.close();

	if (this->number_task != this->task_list.count()) {
		ostringstream info;
		info << "Number of task to be found: " << this->number_task<<endl;
		info << "Number of task found      : " << this->task_list.count() << endl;
		info << "File                  : " << this->task_file_name << endl;
		Error msg = this->set_error(4);
		msg.make_second_info(info.str());
		throw msg;

	}

}
//Output task file list
void Main_Wid::output_task_list(void) {
	ostringstream cout;
	ostringstream prefix;
	prefix << "OUT" << "> ";
	Sys_Common_Output::output_system->set_userprefix(&prefix);
	cout << "Ouptut task-list " << endl;
	Sys_Common_Output::output_system->output_txt(&cout, false);
	for (int i = 0; i < this->task_list.count(); i++) {
		for (int j = 0; j < this->task_list.at(i).count(); j++) {
			cout << this->task_list.at(i).at(j).toString().toStdString() <<" ";
		}
		cout << endl;
	}

	Sys_Common_Output::output_system->output_txt(&cout, false);
	Sys_Common_Output::output_system->rewind_userprefix();
}
//Set error(s)
Error Main_Wid::set_error(const int err_type){
		string place="Main_Wid::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_...(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://tables check not successful
			place.append("tables_check_manager(void)");
			reason="The check of the database table was not successful: Can not open the project";
			help="Check the database tables";
			info <<"Check the exception file in the archiv; further information are given there"<<endl;
			info <<"Number of checking errors "<<this->number_error_table_check<< endl;
			type=9;
			fatal=true;
			break;
		case 2://can not open  file
			place.append("read_task_file(void)");
			reason = "Can not open the file of the tasks";
			help = "Check the file";
			type = 5;
			break;
		case 3://wrong input
			place.append("read_task_file(void)");
			reason = "There is a problem with the input; wrong sign is read in";
			help = "Check the task file";
			type = 1;
			break;
		case 4://wrong input
			place.append("read_task_file(void)");
			reason = "Number of task(s) found in file is not correct; check number of task at the beginning of file and number of tasks in file";
			help = "Check the task file";
			type = 1;
			break;
		default:
			place.append("set_error(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}
