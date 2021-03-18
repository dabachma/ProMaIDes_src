#pragma once
/**\class Dam_Sc_Point
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2012
*/
#ifndef DAM_SC_POINT_H
#define DAM_SC_POINT_H

//dam-libs
//base class
#include "Geo_Point.h"
//sys-libs
#include "_Sys_Common_System.h"

///Enumerator for specifiying the categories of the simple counting \ingroup dam
enum _dam_sc_category{
	///Public_buildings
	pub_build = 1,
	///Ecologic perilous sites or buildings
	eco_build = 2,
	///Cultural heritage
	cult_build = 3,
	///Buildings with highly vulnerable persons
	person_build = 4,
	///Not set
	nodefined_build = 0,
};

///Information of the simple-counting damage calculation provided in the element of the simple-counting damage raster \ingroup dam
/**

*/
class Dam_Sc_Point : public Geo_Point, public _Sys_Common_System
{
public:
	///Default constructor
	Dam_Sc_Point(void);
	///Default destructor
	~Dam_Sc_Point(void);

	//members
	///Pointer to the table for the simple counting point information in a database
	static Tables *point_table;
	///Pointer to the table for the results of the damage calculation for the simple counting points in a database
	static Tables *point_erg_table;

	//methods
	///Input the simple counting point data per database
	void input_point_perdatabase(const QSqlQueryModel *results, const int glob_index);

	///Get a string to set the point data from the raster interception to the database table: identifier of the floodplain, -floodplain element
	bool get_string_interception_point_data2database(ostringstream *text);

	///Create the database table for the simple counting points
	static void create_point_table(QSqlDatabase *ptr_database);
	///Set the database table for the simple counting points: it sets the table name and the name of the columns and allocate them
	static void set_point_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Delete all data in the database table for the simple counting points
	static void delete_data_in_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the points of the given system state
	static void delete_data_in_point_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Close and delete the database table for the simple counting points
	static void close_point_table(void);
	///Get the maximum value of the global index of the point database table
	static int get_max_glob_id_point_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the point data to database table
	static string get_insert_header_point_table(QSqlDatabase *ptr_database);

	///Select and count the number of relevant damage points in a database table
	static int select_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output = true);
	///Select and count the number of relevant damage points in a database table (just part of it)
	static int select_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int offset, const int number_rows, const bool with_output = true);
	///Count the number of relevant damage points in a database table
	static int count_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output = true);
	///Count number of not connected points to the hydraulic model
	static int count_not_connected_points2hyd(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Create the database table for the results of the damage calculation per point
	static void create_erg_table(QSqlDatabase *ptr_database);
	///Set the database table for the results of the damage calculation per point: it sets the table name and the name of the columns and allocate them
	static void set_erg_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the results of the damage calculation per point
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database);
	///Delete the data in the database table for the results of the damage calculation specified by the system state
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete the data in the database table for the results of the damage calculation specified parameters
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag=false);
	///Delete the data in the database table for the results of the damage calculation specified by the system state and the scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz );
	///Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz);

	///Close and delete the database table for the results of the damage calculation per point
	static void close_erg_table(void);
	///Get the maximum value of the global index of the result point database table
	static int get_max_glob_id_point_erg_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the point result data to database table
	static string get_insert_header_erg_table(QSqlDatabase *ptr_database);

	///Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static int select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz );
	///Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (just part of it)
	static int select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_row );
	///Count data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static int count_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz );

	///Switch the applied-flag for the simple counting damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the simple counting damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);

	///Switch the applied-flag for the simple counting damage points in the database table for a defined system state
	static void switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the simple counting damage points in the database table for a defined system state
	static void switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int point_id, const bool flag);
	///Reswitch the applied-flag for the simple counting damage points in the database table for the base state; a defined system state and his applied elements are decisive
	static void reswitch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);

	///Copy the results of a given system id to another one in database tables
	static void copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);

	///Convert category (_dam_sc_category) to string
	static string convert_category2txt(const _dam_sc_category cat);

	///Get the category id
	int get_cat_id(void);
	///Get the subcategory id
	int get_subcat_id(void);
	///Get the score value
	double get_score_value(void);
	///Get the affected score value
	double get_affected_score_value(void);
	///Get the boundary value
	double get_boundary_value(void);

	///Set the category id, the subcategory id, the score value and the boundary value
	void set_members(const int cat_id, const double score, const double bound, const int subcat_id=-1);

	///Get the data-string to complete a insert-string for inserting the data of the point to database
	string get_datastring_members2database(const int global_id);
	///Get the data-string to complete a insert-string for inserting the data of the point results to database
	string get_datastring_results2database(const int global_id, const int bound_sz, const string break_sz, const bool must_output);

	///Set the index of the connected floodplain
	void set_index_floodplain(const int index);
	///Get the index of the connected floodplain
	int get_index_floodplain(void);
	///Set the index of the connected floodplain element
	void set_index_floodplain_element(const int index);
	///Get the index of the connected floodplain element
	int get_index_floodplain_element(void);
	///get the flag if the point is connected to the hydraulic
	bool get_connected_flag(void);

	///Reset result values
	void reset_result_value(void);

	///Calculate the simple counting damages per point
	void calculate_damages(Dam_Impact_Values *impact);

private:

	///Identifier of the category
	int cat_id;
	///Identifier of the category
	_dam_sc_category category;
	///Identifier of the subcategory
	int subcat_id;
	///Score value
	double score_value;
	///Boundary value (waterlevel), when the point gets active
	double boundary_value;

	///Index of the floodplain, to which the damage point is connected
	int index_fp;
	///Index of the floodplain element, to which the damage point is connected
	int index_fp_elem;
	///Flag if the point is connected to the hydraulic
	bool is_connected;
	///Global index of the point in the database
	int global_index;

	///Flag if the point is affected
	bool is_affected;

	//method
	///Transfer the category id in the enum _dam_sc_category
	_dam_sc_category convert_id2enum(const int id);

	///Check the points
	void check_members(void);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif