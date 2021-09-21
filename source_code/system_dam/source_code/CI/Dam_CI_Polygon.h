#pragma once
/**\class _Dam_System
	\author produced by AG FRM HS-M
	\version 0.0.1
	\date 2021
*/

#ifndef DAM_CI_POLYGON
#define DAM_CI_POLYGON



//sys libs
#include "_Sys_Common_System.h"
#include "Sys_Multi_Filechooser_Dia.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Diverse_Text_Dia.h"
#include "Geo_Simple_Polygon.h"

//dam-libs
#include "_Dam_CI_Element.h"


///Class for the polygons as CI elements points which are used in the CI-damage calculation  \ingroup dam
/**
The CI-elements, points and polygons, are connected by lists (\ref Dam_CI_Element_List) with each other. In- and outcomming list are available.
*/
class Dam_CI_Polygon : public _Dam_CI_Element, public Geo_Simple_Polygon
{
public:
	///Default constructor
	Dam_CI_Polygon(void);
	///Default destructor
	~Dam_CI_Polygon(void);

	//members
	///Pointer to the table for the CI polygon information in a database
	static Tables *polygon_table;
	///Pointer to the table for the results of the damage calculation for the CI polygon in a database
	static Tables *polygon_erg_table;
	///Pointer to the table for the instationary results of the damage calculation for the CI polygon in a database
	static Tables *polygon_instat_erg_table;

	///Midpoint and saving the name
	Geo_Point mid_point;

	//method
	///Input the CI polygon data per database
	void input_polygon_perdatabase(const QSqlQueryModel *results, const int glob_index);

	///Create the database table for the CI polygons
	static void create_polygon_table(QSqlDatabase *ptr_database);
	///Set the database table for the CI polygon: it sets the table name and the name of the columns and allocate them
	static void set_polygon_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Delete all data in the database table for the CI polygon
	static void delete_data_in_polygon_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the CI polygon of the given system state
	static void delete_data_in_polygon_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Close and delete the database table for the CI polygon
	static void close_polygon_table(void);
	///Get the maximum value of the global index of the CI polygon database table
	static int get_max_glob_id_polygon_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the CI polygon data to database table
	static string get_insert_header_polygon_table(QSqlDatabase *ptr_database);


	///Select and count the number of relevant CI-polygons in a database table
	static int select_relevant_polygon_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output = true);
	///Select and count the number of relevant CI-polygon in a database table (just part of it)
	static int select_relevant_polygon_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int offset, const int number_rows, const bool with_output = true);
	///Count the number of relevant CI-polygon in a database table
	static int count_relevant_polygon_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output = true);
	///Get a string to set the polygon data from the statistical calculation to the database table
	bool get_string_stat_polygon_data2database(ostringstream *text);





	///Create the database table for the results of the damage calculation per CI-polygon
	static void create_erg_table(QSqlDatabase *ptr_database);
	///Set the database table for the results of the damage calculation per CI-polygon: it sets the table name and the name of the columns and allocate them
	static void set_erg_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Delete all data in the database table for the results of the damage calculation per CI-polygon
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database);
	///Delete the data in the database table for the results of the damage calculation specified by the system state
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete the data in the database table for the results of the damage calculation specified parameters
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag = false);
	///Delete the data in the database table for the results of the damage calculation specified by the system state and the scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);
	///Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz);

	///Close and delete the database table for the results of the damage calculation per CI-polygon
	static void close_erg_table(void);
	///Get the maximum value of the global index of the result CI-polygon database table
	static int get_max_glob_id_polygon_erg_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the CI-polygon result data to database table
	static string get_insert_header_erg_table(QSqlDatabase *ptr_database);

	///Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static int select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);
	///Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (just part of it)
	static int select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_row);
	///Count data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static int count_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);

	///Switch the applied-flag for the CI-polygon damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the CI-polygon damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);

	///Switch the applied-flag for the CI-polygon in the database table for a defined system state
	static void switch_applied_flag_polygon_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the CI-polygon in the database table for a defined system state
	static void switch_applied_flag_polygon_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int polygon_id, const bool flag);

	///Reswitch the applied-flag for the CI-polygon in the database table for the base state; a defined system state and his applied elements are decisive
	static void reswitch_applied_flag_polygon_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);

	///Copy the results of a given system id to another one in database tables
	static void copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);

	///Create the database table for the instationary results of the damage calculation per CI-polygon
	static void create_instat_erg_table(QSqlDatabase *ptr_database);
	///Set the database table for the instationary results of the damage calculation per CI-polygon: it sets the table name and the name of the columns and allocate them
	static void set_instat_erg_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Delete all data in the database table for the instationary results of the damage calculation per CI-polygon
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database);
	///Delete the data in the database table for the instationary results of the damage calculation specified by the system state
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete the data in the database table for the instationary results of the damage calculation specified parameters
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag = false);
	///Delete the data in the database table for the instationary results of the damage calculation specified by the system state and the scenario-ids
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);
	///Delete the data in the database table for the instationary results of the damage calculation specified by the hydraulic scenario-ids
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const int bound_sz);

	///Close and delete the database table for the instationary results of the damage calculation per CI-polygon
	static void close_instat_erg_table(void);
	///Get the maximum value of the global index of the instationary result CI-polygon database table
	static int get_max_glob_id_polygon_instat_erg_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the CI-point instationary result data to database table
	static string get_insert_header_instat_erg_table(QSqlDatabase *ptr_database);

	///Copy the instationaryresults of a given system id to another one in database tables per CI-polygon
	static void copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);









	///Set members
	void set_members(const int sector_id, const double endusers, const string name, const int id);

	///Get endusers
	double get_enduser(void);

	///Output point members
	void output_point(ostringstream *cout);

	///Polygon mid to mid point
	void set_polygon_mid2mid_point(void);

	///Get the data-string to complete a insert-string for inserting the data of the polygon to database
	string get_datastring_members2database(const int global_id);
	///Get the data-string to complete a insert-string for inserting the data of the polygon to database
	string get_datastring_results2database(const int global_id, const int bound_sz, const string break_sz, const bool must_output);
	///Get the data-string to complete a insert-string for inserting the data of the polygon instationary results to database
	string get_datastring_instat_results2database(const int global_id, const int bound_sz, const string break_sz, string date_time, const bool must_output);


	
	///Transfer the polygon point data of a file into a database table
	void transfer_polygon_point2database_table(QSqlDatabase *ptr_database, const int poly_id);

	///Reset result values
	void reset_result_values(void);


	///Finalize results
	void finalize_results(void);

	///Check polygon incoming 
	void check_polygon_incoming(void);
	///Check the polygons
	void check_members(void);



private:

	//members
	///Pointer to the table for the polygon point data in a database
	static Tables *polygon_point_table;

	///End-user: who is affected in the polygon, e.g. persons without electricity
	double enduser;

	///End-user duration: who is how long affected in the polygon, e.g. persons without electricity for 10 d
	double enduser_duration;

	///String for the polygon
	string polygon_str;



	//methods

	///Create the database table for the polygon point data in a database
	static void create_point_table(QSqlDatabase *ptr_database);
	///Set the database table for the polygon point data: it sets the table name and the name of the columns and allocate them
	static void set_point_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Delete all data in the database table for the polygon point data
	static void delete_point_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the polygon point data
	static void close_point_table(void);
	
	///Select and count the number of relevant polygon points in a database table
	static int select_relevant_points_database(QSqlTableModel *results, const int global_polygon_id);



	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
