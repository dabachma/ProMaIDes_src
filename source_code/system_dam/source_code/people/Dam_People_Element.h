#pragma once
/**\class Dam_People_Element
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/
#ifndef DAM_PEOPLE_ELEMENT_H
#define DAM_PEOPLE_ELEMENT_H

//dam-libs
//base class
#include "_Dam_Element.h"
#include "Dam_People_Damage_Function.h"

///Information of the people-damage calculation provided in the element of the people-damage raster \ingroup dam
/**

*/
class Dam_People_Element : public _Dam_Element
{
public:
	///Default constructor
	Dam_People_Element(void);
	///Default destructor
	~Dam_People_Element(void);

	//members
	///Pointer to the table for the raster element information in a database
	static Tables *elem_table;
	///Pointer to the table for the results of the damage calculation for the raster elements in a database
	static Tables *elem_erg_table;

	//method
	///Transfer the people2risk damage raster element data to a database (inadequate performance)
	void transfer_input_members2database(QSqlDatabase *ptr_database, const int raster_index, const double cellsize,  const double x_origin, const double y_origin);
	///Get the data-string to complete a insert-string for inserting the data of the element to database
	string get_datastring_members2database(const int global_id, const int raster_index, const double cellsize, const double x_origin, const double y_origin);

	///Input the people2risk damage raster element data per database
	void input_element_perdatabase(const QSqlQueryModel *results, const int glob_index);

	///Set the element data from the raster interception to the database table: identifier of the floodplain, -floodplain element and the reduced area
	void set_interception_elem_data2database(QSqlDatabase *ptr_database, QSqlQuery *elem_results, const int raster_id);
	///Get a string to set the element data from the raster interception to the database table: identifier of the floodplain, -floodplain element and the reduced area
	bool get_string_interception_elem_data2database(ostringstream *text);

	///Set the index of the damage function for the people2risk type
	void set_id_people2risk_type(const int id);
	///Set the individual stock value of the population density
	void set_indiv_pop_stock_value(const double stock_pop);

	///Get the index of the damage function for the people2risk type
	int get_id_people2risk_type(void);
	///Get the individual stock value of the population density
	double get_indiv_pop_stock_value(void);

	///Get the result of the affected people calculation for the element
	double get_affected_pop_result(void);
	///Get the total results of the endangered people calculation for the element
	double get_endangered_pop_result(void);

	///Output the damage members
	void output_damage_members(ostringstream *txt);

	///Connect the element to the people2risk damage function
	bool connect_people2risk_type2damage_function(const int no_info, Dam_People_Damage_Function *function);
	///Get a flag if the people2risk damage function is found
	bool people2risk_type_is_found(void);

	///Calculate the people2risk damages per element
	void calculate_damages(Dam_Impact_Values *impact);
	///Output the result memebrs to database
	void output_result_members2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int raster_id, const double cellsize, const double x_origin, const double y_origin);
	///Get the data-string to complete a insert-string for inserting the data of the element results to database
	string get_datastring_results2database(const int global_id, const int bound_sz, const string break_sz, const int raster_id, const double cellsize, const double x_origin, const double y_origin,  const bool must_output);

	///Set the element to a no-info element concerning people2risk type
	void set_noinfo_elem_type(void);
	///Set the element to a no-info element concerning population density
	void set_noinfo_elem_density(void);

	///Create the database table for the raster elements
	static void create_elem_table(QSqlDatabase *ptr_database);
	///Set the database table for the raster elements: it sets the table name and the name of the columns and allocate them
	static void set_elem_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the raster elements
	static void delete_data_in_elem_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the raster elements
	static void close_elem_table(void);
	///Get the maximum value of the global index of the element database table
	static int get_max_glob_id_elem_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the element data to database table
	static string get_insert_header_elem_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the raster elements of the given raster index
	static void delete_data_in_elem_table(QSqlDatabase *ptr_database, const int raster_id);
	///Delete all data in the database table for the raster elements of the given system id
	static void delete_data_in_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Create the database table for the results of the damage calculation per element
	static void create_erg_table(QSqlDatabase *ptr_database);
	///Set the database table for the results of the damage calculation per element: it sets the table name and the name of the columns and allocate them
	static void set_erg_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the results of the damage calculation per element
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database);
	///Delete the data in the database table for the results of the damage calculation specified by the system state
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete the data in the database table for the results of the damage calculation specified parameters
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag=false);
	///Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz);
	///Delete the data in the database table for the results of the damage calculation specified by the raster id
	static void delete_data_in_erg_table_per_raster(QSqlDatabase *ptr_database, const int raster_id);

	///Delete the data in the database table for the results of the damage calculation specified by the system state and the scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz );
	///Close and delete the database table for the results of the damage calculation per element
	static void close_erg_table(void);
	///Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static int select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz );
	///Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (just part of it)
	static int select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_row );
	///Count data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static int count_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz );

	///Get the maximum value of the global index of the element result database table
	static int get_max_glob_id_erg_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the element result data to database table
	static string get_insert_header_erg_table(QSqlDatabase *ptr_database);

	///Switch the applied-flag for the people2risk damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the people2risk damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);

	///Switch the applied-flag for the people2risk damage elements in the database table for a defined system state
	static void switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the people2risk damage elements in the database table for a defined system state
	static void switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int elem_id, const int raster_id, const bool flag);

	///Reswitch the applied-flag for the people2risk damage elements in the database table for the base state; a defined system state and his applied elements are decisive
	static void reswitch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);

	///Copy the results of a given system id to another one in database tables
	static void copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);

	///Select and count the number of relevant damage raster elements in a database table
	static int select_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int raster_id, const bool with_output = true);
	///Select and count the number of relevant damage raster elements in a database table (just part of it)
	static int select_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int raster_id,  const int offset, const int number_rows, const bool with_output = true);
	///Count the number of relevant damage raster elements in a database table
	static int count_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int raster_id, const bool with_output = true);

private:

	//members
	///Index of the damage function for the people2risk type
	int id_people2risk_type;
	///Pointer to the damage function for the people2risk type
	Dam_People_Damage_Function *ptr_people2risk_type;

	///Value for an individual stock of the population density
	double indiv_pop_stock_value;

	///Flag if it is a no-info element concerning people2risk type
	bool no_info_elem_type;
	///Flag if it is a no-info element concerning population density
	bool no_info_elem_density;

	///Index of the endangered zone, where the element belongs to
	int id_endang_zone;

	///Result of the affected people calculation
	double result_affected_pop;
	///Result of the endangered people calculation
	double result_endangered_pop;

	//method
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif