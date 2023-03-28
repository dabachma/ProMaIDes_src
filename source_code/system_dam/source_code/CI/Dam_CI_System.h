#pragma once
/**\class _Dam_System
	\author produced by AG FRM HS-M
	\version 0.0.1
	\date 2021
*/

#ifndef DAM_CI_SYSTEM
#define DAM_CI_SYSTEM



//sys libs
#include "_Sys_Common_System.h"
#include "Sys_Multi_Filechooser_Dia.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Diverse_Text_Dia.h"
#include "Geo_Point.h"

//dam-libs

#include "Dam_CI_Point.h"
#include "Dam_CI_Polygon.h"
#include "Dam_CI_Element_List.h"

///Class for managing the CI-damage calculation  \ingroup dam
/**
This class manages the input, the calculation and the output of the CI-damage system.
*/
class Dam_CI_System : public _Dam_System
{
public:
	///Default constructor
	Dam_CI_System(void);
	///Default destructor
	~Dam_CI_System(void);


	//member
	///The CI polygons of the system
	Dam_CI_Polygon *dam_ci_polygon;
	///The CI points of the system
	Dam_CI_Point *dam_ci_point;
	///The CI connection of the system
	Dam_CI_Element_List dam_ci_connection;

	//method

	///Transfer CI data from file to database: points, polygons, connections
	void ci_data_file2database(QSqlDatabase *ptr_database, const _sys_system_id id);


	///Delete all CI data in the database: points, polygons, connections, results etc.
	void del_ci_data_database(QSqlDatabase *ptr_database);


	///Ask for the files of the CI-data per dialog
	bool ask_file_CI_data(QWidget *parent = NULL);
	///Ask for the flag, what should be deleted (points, subcategories)
	bool ask_deleting_flag(QWidget *parent);

	///Get a text for deleting CI information in database
	string get_deleting_text(void);

	///Output the members
	void output_members(void);
	///Output statistic of CI system
	void output_statistic(void);
	///Output the sum over the raster of the damage results to display/console
	void output_result_damage(void);
	///Check the damage system 
	void check_system(void);

	///Intercept CI-points with the hydraulic elements
	void intercept_hydraulic2damage(Hyd_Model_Floodplain *fp_models, const int number_fp, QSqlDatabase *ptr_database, const _sys_system_id id);
	///Check the connection to the hydraulic of the points
	static bool check_connection2hydraulic(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Read in CI-system per database
	void read_system_per_database(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Init the CI-system 
	void init_system(void);

	///Calculate the damages
	void calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number);
	///Calculate the instationary damages
	void calculate_instat_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number, const double time, const int count);

	///Delete the result members for a given system-id and a scenario (boundary-, break-)
	void delete_result_members_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);
	///Delete the instationary result members for a given system-id and a scenario (boundary-, break-)
	void delete_instat_result_members_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);

	///Output the result members to a database table
	void output_result_member2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output);
	///Output the instat_result members to a database table
	void output_instat_result_member2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const string date_time, bool *was_output);

	///Sum up the total damage results for a given system-id and scenario (boundary-, break-) from the database
	void sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);

	///Check if all points are active again after hydraulic
	bool check_points_active_again(void);


	///Make network statistics
	void make_network_statistics(QSqlDatabase *ptr_database);

	///Make network statistics of failed system per scenario
	void make_network_statistics_failed(void);

	///Get value from the result list
	double get_result_from_list(const int sec_id, const bool pop_t);


private:

	//members
	///Number of CI points in the system
	int no_ci_point;
	///Number of CI polygons in the system
	int no_ci_polygon;


	///Current filename of the damage function file
	/** It is set via dialog. \see ask_file_CI_data(QWidget *parent=NULL)*/
	string file_ci_point;
	///Current filename of the raster for immobile damages land-use categories
	/** It is set via dialog. \see ask_file_CI_data(QWidget *parent=NULL) */
	string file_ci_polygon;
	///Current filename of the raster for mobile damages land-use categories
	/** It is set via dialog. \see ask_file_CI_data(QWidget *parent=NULL) */
	string file_ci_connection;

	///Flag for deleting the CI data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_data_flag;
	///Flag for deleting the CI result data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_res_flag;


	///Results points: Direct failure in sector i
	QList<QList<int>> list_direct_failure_sec;
	///Results points: sectoral failure in sector i
	QList<QList<int>> list_sectoral_failure_sec;
	///Results points: transsectoral failure in sector i
	QList<QList<int>> list_transsectoral_failure_sec;
	///Results points: Total failure in sector i
	QList<QList<int>> list_total_failure_sec;
	///Results points: Total failure x level in sector i
	QList<QList<QVariant>> list_total_failure_level_sec;

	///Results points: activation of emergency CI-structures in sector i
	QList<QList<int>> list_emergency_active_sec;

	///Results points: Total direct failure 
	int tot_direct_failure;
	///Results points: Total sectoral failure 
	int tot_sectoral_failure;
	///Results points: Total transsectoral failure 
	int tot_transsectoral_failure;
	///Results points: Total emergency strcutures activated 
	int tot_emergency_active;


	///Results polygons: Total enduser affected by failure in sector i
	QList<QList<QVariant>> list_enduser_affected_sec;
	///Results polygons: Total enduser affected by failure x duration in sector i
	QList<QList<QVariant>> list_enduser_affected_duration_sec;


	//methods
	///Read in the CI point data from file
	void read_points_per_file(string fname);
	///Transfer the CI point data to a database
	void transfer_point_members2database(QSqlDatabase *ptr_database);
	///Input the CI point data per database: point information
	void input_perdatabase_point_data(QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output = true);

	///Set intercepted point to the indizes of the hydraulic floodplain
	void set_intercepted_point2floodplain_id(Hyd_Model_Floodplain *fp_model);
	///Transfer data evaluated by an interception to database: identifier of the floodplain, -floodplain element
	void transfer_intercepted_data2database(QSqlDatabase *ptr_database);
	///Transfer point statistic data, e.g. HUB-value, AUThority-value CV- and CP-value todatabse
	void transfer_point_statistic_data2database(QSqlDatabase *ptr_database);

	///Transfer polygon statistic data, e.g. AUThority-value CV-value todatabse
	void transfer_polygon_statistic_data2database(QSqlDatabase *ptr_database);

	///Output point member
	void output_point_member(void);
	///Sum up the total point damage results for a given system-id and scenario (boundary-, break-) from the database
	void sum_total_point_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);

	///Read in the CI polygon data from file
	void read_polygon_per_file(string fname);
	///Transfer the CI polygon data to a database
	void transfer_polygon_members2database(QSqlDatabase *ptr_database);
	///Input the CI point data per database: polygon information
	void input_perdatabase_polygon_data(QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output = true);
	///Output polygon member
	void output_polygon_member(void);
	///Sum up the total polygon damage results for a given system-id and scenario (boundary-, break-) from the database
	void sum_total_polygon_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);

	///Read in the CI connection data from file
	void read_connection_per_file(string fname);
	///Transfer the CI connection data to a database
	void transfer_connection_members2database(QSqlDatabase *ptr_database);
	///Input the CI connection data per database: polygon information
	void input_perdatabase_connection_data(QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output = true);
	///Output polygon member
	void output_connection_member(void);

	///Allocate the CI points
	void allocate_CI_point(void);
	///Delete the CI points
	void delete_CI_point(void);

	///Allocate the CI polygon
	void allocate_CI_polygon(void);
	///Delete the CI polygon
	void delete_CI_polygon(void);

	///Output the error_statistic of the system
	void output_error_statistic(void);

	///Find the midpoint of a CI-Element
	Geo_Point find_mid_point_CI_element(const int id, const int point_flag);
	///Find the CI-Element
	_Dam_CI_Element find_CI_element(const int id, const int point_flag);
	///Find the pointer to CI-Element
	_Dam_CI_Element* find_ptr_CI_element(const int id, const int point_flag);

	///Calculate the direct damages
	void calculate_direct_damage(Dam_Impact_Value_Floodplain *impact_fp, const int number);
	///Calculate the indirect damages
	void calculate_indirect_damage(void);

	///Calculate the direct damages instationary
	void calculate_instat_direct_damage(Dam_Impact_Value_Floodplain *impact_fp, const int number, const double time);
	///Calculate the indirect damages instationary
	void calculate_instat_indirect_damage(const double time);




	///Ouput CI-point results to database
	void output_point_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output);
	///Ouput CI-point instationary results to database
	void output_point_instat_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const string time_date, bool *was_output);
	///Ouput CI-polygon results to database
	void output_polygon_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output);
	///Ouput CI-polygon instationary results to database
	void output_polygon_instat_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const string time_date, bool *was_output);

	///Ouput CI-connection results to database
	void output_connection_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output);
	///Ouput CI-connection instationary results to database
	void output_connection_instat_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const string time_date, bool *was_output);

	///Check if the points are connected to the hydraulic
	int check_points_connected2hyd(void);

	///Check the CI-system
	void check_CI_system(void);

	///Reset results values
	void reset_result_values(void);

	///Calculate statistics of HUB-value
	void calc_stat_hub(double *max, double *average, double *std);
	///Calculate statistics of Authority-value
	void calc_stat_aut(double *max, double *average, double *std);

	///Calculate statistics per sector
	void calc_stat_per_sector(int *no_elem, double *max_hub, double *average_hub, double *std_hub, double *max_aut, double *average_aut, double *std_aut, const int sec_id);

	///Calculate CV (cascade vulnerability value) per final element and CP (cascade potential value) per point
	void calc_cv_cp(void);
	///Sum and reset the CP-value
	void sum_reset_cp(void);
	///Calculate the statistics of CP (cascade potential value)
	void calc_stat_cp(double *max, double *average, double *std);
	///Calculate the statistics of CV (cascade vulnerability value)
	void calc_stat_cv(double *max, double *average, double *std);

	///Calculate the statistics of CP per sector (cascade potential value)
	void calc_stat_cp_sec(const int sec_id,  double *max, double *average, double *std);
	///Calculate the statistics of CV per sector (cascade vulnerability value)
	void calc_stat_cv_sec(const int sec_id, double *max, double *average, double *std);

	///Calculate statistics total failure
	void calc_stat_tot_fail(double *max_hub, double *average_hub, double *std_hub, double *max_aut, double *average_aut, double *std_aut, double *max_cp, double *average_cp, double *std_cp, double *max_cv, double *average_cv, double *std_cv);

	///Calculate statistics direct failure
	void calc_stat_dir_fail(double *max_hub, double *average_hub, double *std_hub, double *max_aut, double *average_aut, double *std_aut, double *max_cp, double *average_cp, double *std_cp, double *max_cv, double *average_cv, double *std_cv);


	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif


