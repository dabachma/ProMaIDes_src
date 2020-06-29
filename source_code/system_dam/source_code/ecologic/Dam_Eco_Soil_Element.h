#pragma once
/**\class Dam_Eco_Soil_Element
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/

#ifndef DAM_ECO_SOIL_ELEMENT_H
#define DAM_ECO_SOIL_ELEMENT_H

//dam-libs
#include "_Dam_Element.h"
#include "Dam_Eco_Soil_Cost_Function.h"
#include "Dam_Eco_Soil_Erosion_Function.h"

///Information of the ecological soil-damage calculation provided in the element of the ecological soil raster \ingroup dam
/**

*/
class Dam_Eco_Soil_Element : public _Dam_Element
{
public:
	///Default constructor
	Dam_Eco_Soil_Element(void);
	///Default destructor
	~Dam_Eco_Soil_Element(void);

	//members
	///Pointer to the table for the raster element information in a database
	static Tables *elem_table;
	///Pointer to the table for the results of the damage calculation for the raster elements in a database
	static Tables *elem_erg_table;

	//methods
	///Transfer the soil-erosion raster element data to a database (inadequate performance)
	void transfer_input_members2database(QSqlDatabase *ptr_database, const int raster_index, const double cellsize, const double x_origin, const double y_origin);
	///Get the data-string to complete a insert-string for inserting the data of the element to database
	string get_datastring_members2database(const int global_id, const int raster_index, const double cellsize, const double x_origin, const double y_origin);

	///Input the soil-erosion raster element data per database
	void input_element_perdatabase(const QSqlQueryModel *results, const int glob_index);

	///Set the element data from the raster interception to the database table: identifier of the floodplain, -floodplain element and the reduced area
	void set_interception_elem_data2database(QSqlDatabase *ptr_database, QSqlQuery *elem_results, const int raster_id);
	///Get a string to set the element data from the raster interception to the database table: identifier of the floodplain, -floodplain element and the reduced area
	bool get_string_interception_elem_data2database(ostringstream *text);

	///Set the index of the soil type
	void set_index_soil_type(const int index);
	///Set the index of the erosion type
	void set_index_ero_type(const int index);

	///Get the index of the soil type
	int get_id_soil_type(void);
	///Get the index of the erosion type
	int get_id_erosion_type(void);

	///Output the soil members
	void output_soil_members(ostringstream *txt);

	///Connect the element to the soil type
	bool connect_soiltype2damage_function(const int no_info, Dam_Eco_Soil_Cost_Function *function);
	///Connect the element to the erosion
	bool connect_erotype2damage_function(const int no_info, Dam_Eco_Soil_Erosion_Function *function);
	///Get a flag if the soil type is found
	bool soil_type_is_found(void);
	///Get a flag if the erosion type is found
	bool erotype_is_found(void);

	///Set the element to a no-info element concerning soil type
	void set_noinfo_soiltype(void);
	///Set the element to a no-info element concerning the erosion type
	void set_noinfo_erotype(void);

	///Get the result of the damage calculation
	double get_damage_result(void);

	///Calculate the economical damages per element
	void calculate_damages(Dam_Impact_Values *impact);

	///Output the result memebrs to database (inadequate performance)
	void output_result_members2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int raster_id, const double cellsize, const double x_origin, const double y_origin);
	///Get the data-string to complete a insert-string for inserting the data of the element results to database
	string get_datastring_results2database(const int global_id, const int bound_sz, const string break_sz, const int raster_id, const double cellsize, const double x_origin, const double y_origin, const bool must_output);

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
	///Delete the data in the database table for the results of the damage calculation specified by the system state and the scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz );
	///Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz );
	///Delete the data in the database table for the results of the damage calculation specified by the raster id
	static void delete_data_in_erg_table_per_raster(QSqlDatabase *ptr_database, const int raster_id);

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

	///Switch the applied-flag for the ecological soil-erosion damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the ecological soil-erosion damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);

	///Switch the applied-flag for the ecological soil-erosion damage elements in the database table for a defined system state
	static void switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the ecological soil-erosion damage elements in the database table for a defined system state
	static void switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int elem_id, const int raster_id, const bool flag);

	///Reswitch the applied-flag for the ecological soil-erosion damage elements in the database table for the base state; a defined system state and his applied elements are decisive
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
	///Index of the soil type
	int id_soil_type;
	///Pointer to the soil type
	Dam_Eco_Soil_Cost_Function *ptr_soil_type;

	///Index of the erosion type
	int id_erosion_type;
	///Pointer to the erosion type
	Dam_Eco_Soil_Erosion_Function *ptr_erosion_type;

	///Flag if it is a no-info element concerning the soil types
	bool no_info_soil_type;
	///Flag if it is a no-info element concerning the erosion types
	bool no_info_erosion_type;

	///Result of the damage calculation
	double result_dam;
	///Zone of the soil-erosion damage
	int soil_erosion_zone;

	//method
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif