#pragma once
/**\class Dam_Damage_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/
#ifndef DAM_DAMAGE_SYSTEM_H
#define DAM_DAMAGE_SYSTEM_H

//Hyd-libs
#include "Hyd_Multiple_Hydraulic_Systems.h"
#include "Hyd_Boundary_Szenario.h"

//dam libs
#include "Dam_Eco_System.h"
#include "Dam_CI_System.h"
#include "Dam_Ecn_System.h"
#include "Dam_People_System.h"
#include "Dam_Sc_System.h"
#include "Dam_Pys_System.h"
#include "Dam_Impact_Value_Floodplain.h"
#include "Dam_Polygon.h"
#include "Dam_Hyd_Result_Raster.h"

//qt libs
#include <QObject>

///Enumerator to distinguish the type of the thread, e.g. peforming the importing of file data to a database etc. \ingroup dam
enum _dam_thread_type{
	///Thread for an import of the economic damage function from file to database
	dam_imp_ecn_dam_func,
	///Thread for adding economic raster from file to database
	dam_add_ecn_dam_raster,
	///Thread for deleting all economic data in database
	dam_del_ecn,
	///Thread for an import of the ecologic biotope-types from file to database
	dam_imp_eco_b_type,
	///Thread for adding the ecologic raster of the biotope-types from file to database
	dam_add_eco_b_type_raster,
	///Thread for deleting all ecologic biotope-type data in database
	dam_del_eco_btype,
	///Thread for an import of the ecologic soil-types cost function from file to database
	dam_imp_eco_soil_cost,
	///Thread for an import of the ecologic soil-types erosion function from file to database
	dam_imp_eco_soil_ero,
	///Thread for adding the ecologic raster of the soil erosion-types from file to database
	dam_add_eco_soil_raster,
	///Thread for deleting all ecologic soil-erosion data in database
	dam_del_eco_soil,
	///Thread for an import of the people2risk damage function(s) from file to database
	dam_imp_pop_dam_func,
	///Thread for adding the people2risk raster from file to database
	dam_add_pop_raster,
	///Thread for deleting all people2risk data in database
	dam_del_pop,
	///Thread for an import of the psycho-social categories and their criteria score functions from file to database
	dam_imp_pys_category,
	///Thread for an import of the psycho-social density functions from file to database
	dam_imp_pys_dens_func,
	///Thread for adding the psycho-social raster from file to database
	dam_add_pys_raster,
	///Thread for deleting all psycho-social data in database
	dam_del_pys,

	///Thread for an import of the simple counting subcategories from file to database
	dam_imp_sc_subcat,
	///Thread for import the simple counting points from file to database
	dam_imp_sc_point,
	///Thread for deleting all simple counting data in database
	dam_del_sc,

	///Thread for an import of the CI data from file to database
	dam_imp_ci_data,
	///Thread for deleting all CI-data in database
	dam_del_ci,

	///Thread to output the statistic of the damage system
	dam_sys_stat,
	///Thread for connecting the damage raster among each other and to the hydraulic system
	dam_sys_connect,
	///Thread for calculating the damages for the nobreak scenarios
	dam_sys_calc_no_break_sz,
	///Thread for calculating the instationary damages for the nobreak scenarios
	dam_sys_instat_calc_no_break_sz,
	///Thread for calculating the damages for all break scenarios
	dam_sys_calc_break_sz,
	///Thread for calculating the instationary damages for all break scenarios
	dam_sys_instat_calc_break_sz,

	///Thread is creating the damage tables for a database project
	dam_create_tab,
	///Thread is checking the damage tables of a database project
	dam_check_tab,

	///Thread is checking the damage system of a database project
	dam_check_sys,

	///Thread for importing hydraulic result raster for a base scenario (used in a DAM_HYD-project)
	dam_imp_hyd_res_raster_base_sc,

	///Thread for adding hydraulic result rasters as additional scenarios (used in a DAM_HYD-project)
	dam_add_hyd_res_raster_sc,

	///Unknown thread, nothings happen
	dam_thread_unknown
};

///Data structure for gathering the damage results. It is mainly used in the Risk modul (\ref risk). \ingroup dam
struct _dam_damage_results{
	///Identifier for the hydraulic boundary scenario
	int id_hyd_bound_sc;
	///Name for the hydraulic boundary scenario
	string hyd_bound_sc_name;
	///Identifier for the break scenario
	string id_break_sc;
	///Damage result economic immobile damages
	double ecn_immob_dam;
	///Damage result economic immobile damages 95% quantile
	double ecn_immob_dam_95;
	///Damage result economic immobile damages 5% quantile
	double ecn_immob_dam_5;
	///Damage result economic mobile damages
	double ecn_mob_dam;
	///Damage result economic mobile damages 95% quantile
	double ecn_mob_dam_95;
	///Damage result economic mobile damages 5% quantile
	double ecn_mob_dam_5;
	///Damage result ecologic biotope-type damages
	double eco_biotype;
	///Damage result ecologic soil-erosion damages
	double eco_soil_erosion;
	///Damage result people2risk population affected
	double pop_affected;
	///Damage result people2risk population endangered
	double pop_endangered;
	///Damage result psycho-social criteria age 50 to 59
	double pys_age_50_59;
	///Damage result psycho-social criteria age 50 to 59 with the application of a population density function
	double pys_age_50_59_dens;
	///Damage result psycho-social criteria age 80
	double pys_age_80;
	///Damage result psycho-social criteria age 80 with the application of a population density function
	double pys_age_80_dens;
	///Damage result psycho-social criteria female
	double pys_female;
	///Damage result psycho-social criteria female with the application of a population density function
	double pys_female_dens;
	///Damage result psycho-social criteria home owner
	double pys_owner;
	///Damage result psycho-social criteria home owner with the application of a population density function
	double pys_owner_dens;

	///Damage result simple counting for category public building
	double sc_pub_build;
	///Damage result simple counting for category ecologic perilous sites or buildings
	double sc_eco_build;
	///Damage result simple counting for category cultural heritage
	double sc_cult_build;
	///Damage result simple counting for category buildings with highly vulnerable person
	double sc_person_build;



	///Damage result ci for population time sector electricity
	double ci_elect_pt;
	///Damage result ci for population time sector information technology
	double ci_info_tec_pt;
	///Damage result ci for population time sector water supply
	double ci_water_sup_pt;
	///Damage result ci for population time sector water treatment
	double ci_water_treat_pt;
	///Damage result ci for population time sector energy
	double ci_energy_pt;
	///Damage result ci for population time cummulated sector health
	double ci_health_pt;
	///Damage result ci for population time cummulated sector social
	double ci_social_pt;
	///Damage result ci for population time cummulated sector material
	double ci_mat_pt;



	///Damage result ci for population sector electricity
	double ci_elect_p;
	///Damage result ci for population sector information technology
	double ci_info_tec_p;
	///Damage result ci for population sector water supply
	double ci_water_sup_p;
	///Damage result ci for population sector water treatment
	double ci_water_treat_p;
	///Damage result ci for population sector energy
	double ci_energy_p;
	///Damage result ci for population cummulated sector health
	double ci_health_p;
	///Damage result ci for population cummulated sector social
	double ci_social_p;
	///Damage result ci for population cummulated sector material
	double ci_mat_p;


};

///Data structure for combining the hydraulic boundary scenario data with the break scenario data (\see Risk_Break_Scenario) \ingroup dam
struct _dam_break2nobreak_sc{
	///Name of the hydraulic boundary scenario
	string hyd_sc_name;
	///Identifier string of the break scenario
	string break_sc;
	///Id of the hydraulic boundary scenario
	int id_hyd_sc;
	///Combined name for output reasons
	string combi_name;
	///Flag if the scenario have to be calculated in the damage calculation
	bool must_calc;
	///Flag if the damage result was output
	bool dam_was_output;
};

///Structure for the Dam-system parameters
struct _dam_system_param {
	///Flag if the instationary calculation of CI-failure should be done until "all active" is reached again (:=true); if this is set to false the end time of the hydraulic calculatrion is used
	bool until_all_active;
	///Maximum time steps for instationary damage calculation
	int max_timesteps;

};

///Class for the calculation of all types of damages, like economic, ecologic, psycho-social and simple consequences \ingroup dam
/**

*/
class Dam_Damage_System : public QThread , public _Sys_Common_System
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	Dam_Damage_System(void);
	///Default destructor
	~Dam_Damage_System(void);

	//members
	///Economical damage system
	Dam_Ecn_System ecn_sys;

	///Ecological damage system
	Dam_Eco_System eco_sys;

	///People damage system
	Dam_People_System people_sys;

	///Simple-counting damage system
	Dam_Sc_System sc_sys;

	///CI damage system
	Dam_CI_System ci_sys;

	///Psycho-social damage system
	Dam_Pys_System pys_sys;

	///Manager of the hydraulic boundary szenarios
	Hyd_Boundary_Szenario_Management sz_bound_manager;

	///Impact values for the damage calculation from the hydraulic system for each floodplain
	Dam_Impact_Value_Floodplain *impact_floodplain;

	///Pointer to the table for the results of the damage calculation in a database
	static Tables *erg_table;


	///Pointer to the table for the DAM-system information in a database
	static Tables *system_table;

	//methods
	///Output the members
	void output_member(void);

	///Connect via interception the raster with the raster of the same damage type and the hydraulic raster
	void connect_damage_raster(void);

	///Run the thread; the type of thread, which will be performed, has to be set before with set_thread_type(_dam_thread_type type)
	void run(void);

	///Set the thread type for specifiying the task of the thread
	void set_thread_type(_dam_thread_type type);
	///Get the thread type for specifiying the task of the thread
	_dam_thread_type get_thread_type(void);
	///Set a pointer to the database; the copy of the database is made in this method
	void set_ptr2database(QSqlDatabase *ptr_database);
	///Set a specific raster type before the thread-calculation
	void set_specific_raster_type(_dam_raster_types types);

	///Set the stop thread flag
	static void set_stop_thread_flag(const bool flag);
	///Check the stop thread flag
	static void check_stop_thread_flag(void);
	///Get the stop thread flag
	static bool get_stop_thread_flag(void);

	///Get the number of errors
	int get_error_number(void);

	///Create a whole set of damage database tables
	void create_dam_database_tables(void);
	///Check all damage database tables, for their existence in the database and their declaration in the database table-file
	void check_dam_database_tables(void);
	///Delete the data of all damage database tables
	void delete_data_dam_database_tables(void);
	///Close all damage database tables
	static void close_dam_database_tables(void);

	///Check if some damage raster are set in the database in the damage system
	static bool check_some_raster_set(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Calculate the damage for the nobreak scenario
	void calc_damage_nobreak_sz(void);
	///Calculate the damage for the break scenario
	void calc_damage_break_sz(void);

	///Calculate the instationary damage for the nobreak scenario
	void calc_instat_damage_nobreak_sz(void);
	///Calculate the instationary damage for the break scenario
	void calc_instat_damage_break_sz(void);

	///Output the statistic of the damage system
	void output_statistic(void);

	////Output the result members to database
	void output_result_members2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz);

	///Create the database table for the results of the damage calculation
	static void create_erg_table(QSqlDatabase *ptr_database);
	///Set the database table for the results of the damage calculation: it sets the table name and the name of the columns and allocate them
	static void set_erg_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Delete all data in the database table for the results of the damage calculation
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database);
	///Delete the data in the database table for the results of the damage calculation specified by the system id
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario-id
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const int hyd_sc);
	///Delete the data in the database table for the results of the damage calculation specified by the scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag=false);
	///Delete the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz );
	///Close and delete the database table for the results of the damage calculation
	static void close_erg_table(void);
	///Select the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static int select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz );
	///Select the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static int select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const int glob_id);
	///Select the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static int select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const string like_risk_id);


	///Create the database table for the Dam-system parameters
	static void create_Dam_system_table(QSqlDatabase *ptr_database);
	///Insert a row in the Dam-system table
	static void insert_row_dam_system(QSqlDatabase *ptr_database);
	///Set the database table for the Dam-system parameters: it sets the table name and the name of the columns and allocate them
	static void set_Dam_system_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Close and delete the database table for the Dam-system parameters
	static void close_Dam_system_table(void);
	///Set system parameters by database table
	void set_Dam_system_parameters_db(void);


	///Check if all damage raster are connected to the hydraulic system
	static bool check_all_raster_connected2hyd(QSqlDatabase *ptr_database, string *txt, _sys_system_id id);
	///Reset the connection to the hydraulic system; (just the connected flag is set to false)
	static void reset_raster_hyd_connection_flag(QSqlDatabase *ptr_database);
	///Reset the connection to the hydraulic system in elements (static)
	static void reset_raster_hyd_connection_elements(QSqlDatabase *ptr_database, _dam_raster_types type);

	///Switch the applied-flag for the damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);

	///Copy the results of a given system id to another one in database tables
	static void copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);

	///Select hydraulic boundary scenarios, which damage results have not yet calculated
	int select_scenarios_without_dam_result(QSqlDatabase *ptr_database, const bool just_hydraulic);

	///Set the system-id (_sys_system_id) of the object
	void set_systemid(const _sys_system_id actuel_id);
	///Set identifier string for the break scenario
	void set_string_break_sc(const string break_sc);
	///Set an array of break scenarios, which are calculated
	void set_break_sc_array(const int number, _dam_break2nobreak_sc *ptr_break_sc);

	///Ask per dialog (HydGui_Boundary_Scenario_Dia), which of the availabe hydraulic boundary scenarios should be handled and return the number
	int ask_boundary_scenarios_per_dialog(QSqlDatabase *ptr_database, QWidget *parent);

	///Get the flag if the thread has started
	bool get_thread_has_started(void);

	///Input the polygons for raster interception per file
	void input_polygons_per_file(const string file_name);

	///Intercept the damage raster with the polygons for the measure "resettlement"
	void intercept_raster_resettlement(QList <int> btype_replace);

	///Intercept the economical damage raster with the polygons for the measure "change ECN-damage function"
	void intercept_ecn_raster(QList <int> current_id, QList <int> new_id);
	///Intercept the ecological biotope type raster with the polygons for the measure "change ECO-biotope type"
	void intercept_eco_btype_raster(QList <int> current_id, QList <int> new_id);
	///Intercept the ecological soil type raster with the polygons for the measure "change ECO-soil type"
	void intercept_eco_soil_raster(QList <int> current_id, QList <int> new_id);
	///Intercept the people2risk category raster with the polygons for the measure "change POP category"
	void intercept_pop_category_raster(QList <int> current_id, QList <int> new_id);
	///Intercept the psycho-social category raster with the polygons for the measure "change PYS category"
	void intercept_pys_category_raster(QList <int> current_id, QList <int> new_id);

	///Ask for the file(s) of the hydraulic result raster(s) per dialog for multiple floodplains
	bool ask_file_hyd_res_raster_multi_fp(const bool base_sc);

signals:
	///Send if the hydraulic thread is used
	void send_hyd_thread_runs(bool flag);
	///Send if the hydraulic thread will run or permit hydraulic module application
	void send_hyd_thread_enable(bool flag);
	///Emit that the dam thread sleeps
	void send_dam_thread_sleeps(bool flag);

private:

	//members
	///Type of thread specifiying the task of the thread \see _dam_thread_type
	_dam_thread_type thread_type;

	///Flag if the thread are aborted by the user
	static bool abort_thread_flag;

	///Dam-system parameters
	_dam_system_param system_param;

	///Copy of the database
	QSqlDatabase qsqldatabase;

	///Number of errors occurred
	int number_error;
	///Number warning at the beginning of an action
	int number_warning_begin;
	///Number warning at the end of an action
	int number_warning;

	///Number warnings, which occurred in the HYD-modul (connecting DAM-system to HYD-system)
	int number_hyd_warnings;
	///Number errors, which occurred in the HYD-modul (connecting DAM-system to HYD-system)
	int number_hyd_errors;

	///Time classes for the real time calculation (start time point)
	time_t start_time;
	///Time classes for the real time calculation (actual time point)
	time_t actual_time;

	///Number of impact values for the floodplains from the hydraulic system
	int number_floodplain_impact;
	///Flag if the impact values for the floodplains are set
	bool impact_floodplain_set;

	///Identifier string for the break scenario
	string break_sz;
	///Number of break scenarios
	int number_break_sc;
	///Number of break scenarios to calculate
	int number_break_sc2calc;
	///Pointer to the break scenario data
	_dam_break2nobreak_sc *ptr_break_sc_data;

	///If required a specific raster type can set before launching the thread
	_dam_raster_types specific_raster_type;

	///Flag if the HYD-thread is on the way
	static bool hyd_thread_runs;

	///Flag if the threads has started
	bool thread_is_started;

	///Polygons, for intercepting with damage raster
	Dam_Polygon *polygons;
	///Number of polygons
	int number_polygons;

	///Dialog for multiple hyd raster import
	//DamGui_Number_Hyd_Raster_Dia *number_hyd_raster_dia;

	///Flag if a base scenario of the hydraulic results is imported
	bool hyd_base_sc_import;

	///Dialog for hydraulic result raster import for multiple floodplains
	Sys_Multi_Filechooser_Dia *multi_fp_hyd_raster_import_dia;
	///Number of dialogs for hydraulic result raster import
	int number_hyd_imp_dia;

	//methods
	///Read in the damage calculation data from database
	void read_damage_data_per_database(void);

	///Set the number of warnings before an action
	void set_start_warnings_number(void);
	///Set the warning number, which occured during an action
	void set_warning_number(void);
	///Get the warning number, which occurred during an action
	int get_occured_warning(void);

	///Output the error statistic of the damage system
	void output_error_statistic(void);
	///Output the error statistic and the needed time of the damage system
	void output_error_time_statistic(void);
	///Output the statistic of the damage calculation for selected nobreak-scenarios
	void output_final_statistic_multi_nobreak_hydsz(void);
	///Output the statistic of the damage calculation for generated nbreak-scenarios
	void output_final_statistic_multi_break_sz(void);

	///Check the damage system
	void check_damage_system(void);

	///Set the impact values of the floodplains from database
	void set_impact_values_hyd(const int bound_sz, const string break_sz);
	///Set the instationary impact values of the floodplains from database
	void set_instat_impact_values_hyd(const int bound_sz, const string break_sz, const string time_date);

	///Import multi hydraulic result raster from file to database for multiple floodplains
	void import_multi_fp_hyd_res_raster_file2database(void);
	///Import multi hydraulic result raster from file to database for one specified floodplain
	void import_fp_hyd_res_raster_file2database(const int fp_index, string h_file, string z_file, string v_file, string d_file, string dh_dt_file, Hyd_Boundary_Szenario *sc);

	///Import multi hydraulic result raster from file to database for one specified floodplain
	void check_fp_hyd_res_raster_file2database(const int fp_index, string h_file, string z_file, string v_file, string d_file, string dh_dt_file);

	///Allocate the impact values of the floodplains from the hydraulic system
	void allocate_impact_floodplain(void);
	///Delete the impact values of the floodplains from the hydraulic system
	void delete_impact_floodplain(void);

	///Allocate the polygons for an interception
	void allocate_polygons(void);
	///Delete the polygons for an interception
	void delete_polygons(void);

	///Allocate the dialog for importing multiple hydraulic results for multiple floodplains
	void allocate_hyd_res_import_dia_multi_fp(void);
	///Delete the dialog for importing multiple hydraulic results for multiple floodplains
	void delete_hyd_res_import_dia_multi_fp(void);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif