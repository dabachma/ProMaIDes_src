#pragma once
/**\class _Dam_System
	\author produced by AG FRM HS-M
	\version 0.0.1
	\date 2021
*/

#ifndef DAM_CI_POINT
#define DAM_CI_POINT



//sys libs
#include "_Sys_Common_System.h"
#include "Sys_Multi_Filechooser_Dia.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Diverse_Text_Dia.h"
#include "Geo_Point.h"

//dam-libs
#include "_Dam_CI_Element.h"





///Class for the points as CI (Critical Infrastructure) elements points which are used in the CI-damage calculation  \ingroup dam
/**
The CI-elements, points and polygons, are connected by lists (\ref Dam_CI_Element_List) with each other. In- and outcomming list are available.
*/


class Dam_CI_Point : public _Dam_CI_Element , public Geo_Point
{
public:
	///Default constructor
	Dam_CI_Point(void);
	///Default destructor
	~Dam_CI_Point(void);

	//members
	///Pointer to the table for the CI point information in a database
	static Tables *point_table;
	///Pointer to the table for the results of the damage calculation for the CI points in a database
	static Tables *point_erg_table;
	///Pointer to the table for the instationary results of the damage calculation for the CI point in a database
	static Tables *point_instat_erg_table;


	//methods
	///Input the CI point data per database
	void input_point_perdatabase(const QSqlQueryModel *results, const int glob_index);

	///Get a string to set the CI point data from the raster interception to the database table: identifier of the floodplain, -floodplain element
	bool get_string_interception_point_data2database(ostringstream *text);

	///Get a string to set the point data from the statistical calculation to the database table
	bool get_string_stat_point_data2database(ostringstream *text);

	///Create the database table for the CI points
	static void create_point_table(QSqlDatabase *ptr_database);
	///Set the database table for the CI points: it sets the table name and the name of the columns and allocate them
	static void set_point_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Delete all data in the database table for the CI points
	static void delete_data_in_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the CI points of the given system state
	static void delete_data_in_point_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Close and delete the database table for the CI points
	static void close_point_table(void);
	///Get the maximum value of the global index of the CI point database table
	static int get_max_glob_id_point_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the CI point data to database table
	static string get_insert_header_point_table(QSqlDatabase *ptr_database);

	///Select and count the number of relevant damage points in a database table
	static int select_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output = true);
	///Select and count the number of relevant damage points in a database table (just part of it)
	static int select_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int offset, const int number_rows, const bool with_output = true);
	///Count the number of relevant damage points in a database table
	static int count_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output = true);
	///Count number of not connected points to the hydraulic model
	static int count_not_connected_points2hyd(QSqlDatabase *ptr_database, const _sys_system_id id);




	///Create the database table for the results of the damage calculation per CI-point
	static void create_erg_table(QSqlDatabase *ptr_database);
	///Set the database table for the results of the damage calculation per CI-point: it sets the table name and the name of the columns and allocate them
	static void set_erg_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Delete all data in the database table for the results of the damage calculation per CI-point
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database);
	///Delete the data in the database table for the results of the damage calculation specified by the system state
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete the data in the database table for the results of the damage calculation specified parameters
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag = false);
	///Delete the data in the database table for the results of the damage calculation specified by the system state and the scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);
	///Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz);

	///Close and delete the database table for the results of the damage calculation per CI-point
	static void close_erg_table(void);
	///Get the maximum value of the global index of the result CI-point database table
	static int get_max_glob_id_point_erg_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the CI-point result data to database table
	static string get_insert_header_erg_table(QSqlDatabase *ptr_database);

	///Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static int select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);
	///Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (just part of it)
	static int select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_row);
	///Count data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static int count_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);

	///Switch the applied-flag for the CI-points damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the CI-points damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);

	///Switch the applied-flag for the CI-points in the database table for a defined system state
	static void switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the CI-points in the database table for a defined system state
	static void switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int point_id, const bool flag);

	///Reswitch the applied-flag for the CI-points in the database table for the base state; a defined system state and his applied elements are decisive
	static void reswitch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);

	///Copy the results of a given system id to another one in database tables
	static void copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);

	///Create the database table for the instationary results of the damage calculation per CI-point
	static void create_instat_erg_table(QSqlDatabase *ptr_database);
	///Set the database table for the instationary results of the damage calculation per CI-point: it sets the table name and the name of the columns and allocate them
	static void set_instat_erg_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Delete all data in the database table for the instationary results of the damage calculation per CI-point
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database);
	///Delete the data in the database table for the instationary results of the damage calculation specified by the system state
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete the data in the database table for the instationary results of the damage calculation specified parameters
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag = false);
	///Delete the data in the database table for the instationary results of the damage calculation specified by the system state and the scenario-ids
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);
	///Delete the data in the database table for the instationary results of the damage calculation specified by the hydraulic scenario-ids
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const int bound_sz);

	///Close and delete the database table for the instationary results of the damage calculation per CI-point
	static void close_instat_erg_table(void);
	///Get the maximum value of the global index of the instationary result CI-point database table
	static int get_max_glob_id_point_instat_erg_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the CI-point instationary result data to database table
	static string get_insert_header_instat_erg_table(QSqlDatabase *ptr_database);

	///Copy the instationaryresults of a given system id to another one in database tables per CI-point
	static void copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);





	///Get the boundary value
	double get_boundary_value(void);
	///Get sector level
	int get_sec_level(void);



	///Set the sector id, sector level, boundary value, recovery time, final flag
	void set_members(const int sector_id, const int sector_level, const double boundary, const double recovery_time, const bool final_flag, const bool regular, const double activation_time);

	///Get the data-string to complete a insert-string for inserting the data of the point to database
	string get_datastring_members2database(const int global_id);
	///Get the data-string to complete a insert-string for inserting the data of the point results to database
	string get_datastring_results2database(const int global_id, const int bound_sz, const string break_sz, const bool must_output);
	///Get the data-string to complete a insert-string for inserting the data of the point instationary results to database
	string get_datastring_instat_results2database(const int global_id, const int bound_sz, const string break_sz, string date_time, const bool must_output);

	///Get the data-string to complete a insert-string for inserting the data of the connection results to database
	string get_datastring_conect_results2database(int *global_id, const int bound_sz, const string break_sz, const bool must_output);
	///Get the data-string to complete a insert-string for inserting the data of the connection instationary results to database
	string get_datastring_conect_instat_results2database(int *global_id, const int bound_sz, const string break_sz, string date_time, const bool must_output);

	///Calculate direct damages
	void calculate_direct_damages(Dam_Impact_Values *impact);
	///Calculate direct damages instationary
	void calculate_direct_damages_instationary(Dam_Impact_Values *impact, const double time);

	///Check the points
	void check_members(void);




private:

	//members

	///Level within the sector
	int sector_level;


	///Global index of the point in the database
	int global_index;






	//method


	
	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};

#endif

