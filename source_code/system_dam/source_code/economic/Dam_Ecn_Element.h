#pragma once
/**\class Dam_Ecn_Element
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

//sys-libs
#include "_Sys_Common_System.h"

//dam libs
#include "_Dam_Element.h"
#include "Dam_Ecn_Damage_Function.h"
#include "Dam_Impact_Values.h"




#ifndef DAM_ECN_ELEMENT_H
#define DAM_ECN_ELEMENT_H

///Information of the economical damage calculation provided in the element of the economical raster \ingroup dam
/**

*/
class Dam_Ecn_Element : public _Dam_Element
{
public:
	///Default constructor
	Dam_Ecn_Element(void);
	///Default destructor
	~Dam_Ecn_Element(void);

	//members
	///Pointer to the table for the raster element information in a database
	static Tables *elem_table;
	///Pointer to the table for the results of the damage calculation for the raster elements in a database
	static Tables *elem_erg_table;

	//methods
	///Transfer the economic damage raster element data to a database (inadequate performance)
	void transfer_input_members2database(QSqlDatabase *ptr_database, const int raster_index, const double cellsize, const double x_origin, const double y_origin);
	///Get the data-string to complete a insert-string for inserting the data of the element to database
	string get_datastring_members2database(const int global_id, const int raster_index, const double cellsize, const double x_origin, const double y_origin); 
	
	///Input the economic damage raster element data per database
	void input_element_perdatabase(const QSqlQueryModel *results, const int glob_index);

	///Set the element data from the raster interception to the database table: identifier of the floodplain, -floodplain element and the reduced area
	void set_interception_elem_data2database(QSqlDatabase *ptr_database, QSqlQuery *elem_results, const int raster_id);
	///Get a string to set the element data from the raster interception to the database table: identifier of the floodplain, -floodplain element and the reduced area
	bool get_string_interception_elem_data2database(ostringstream *text);



	///Set the index of the damage function for the land use category, calculating immobile damages
	void set_index_immob_dam(const int index);
	///Set the index of the damage function for the land use category, calculating mobile damages
	void set_index_mob_dam(const int index);
	///Set the individual stock value for this element, calculating mobile damages
	void set_indiv_mob_stock_value(const double stockvalue);
	///Set the individual stock value for this element, calculating immobile damages
	void set_indiv_immob_stock_value(const double stockvalue);

	///Get the index of the mobile damage function
	int get_id_mobile_dam_func(void);
	///Get the index of the immobile damage function
	int get_id_immobile_dam_func(void);


	///Get the result of the mobile damage calculation
	_dam_ecn_result get_mob_damage_result(void);
	///Get the result of the immobile damage calculation
	_dam_ecn_result get_immob_damage_result(void);
	///Get the total results of the damage calculation
	_dam_ecn_result get_total_damage_result(void);

	///Output the damage members
	void output_damage_members(ostringstream *txt);

	///Connect the element to the mobile damage function
	bool connect_mob_dam2damage_function(const int no_info, Dam_Ecn_Damage_Function *function);
	///Connect the element to the immobile damage function
	bool connect_immob_dam2damage_function(const int no_info, Dam_Ecn_Damage_Function *function); 
	///Get a flag if the immobile damage function is found
	bool immob_dam_func_is_found(void);
	///Get a flag if the mobile damage function is found
	bool mob_dam_func_is_found(void);

	///Set the element to a no-info element concerning immobile damages
	void set_noinfo_immob(void);
	///Set the element to a no-info element concerning mobile damages
	void set_noinfo_mob(void);

	///Calculate the economical damages per element
	void calculate_damages(Dam_Impact_Values *impact);

	///Output the result members to database (inadequate performance)
	void output_result_members2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int raster_id, const double cellsize, const double x_origin, const double y_origin);
	///Get the data-string to complete a insert-string for inserting the data of the element results to database
	string get_datastring_results2database(const int global_id, const int bound_sz, const string break_sz, const int raster_id, const double cellsize, const double x_origin, const double y_origin, const bool must_output); 

	///Create the database table for the raster elements
	static void create_elem_table(QSqlDatabase *ptr_database);
	///Set the database table for the raster elements: it sets the table name and the name of the columns and allocate them
	static void set_elem_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the raster elements
	static void delete_data_in_elem_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the raster elements with a given system id
	static void delete_data_in_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete all data in the database table for the raster elements of the given raster index
	static void delete_data_in_elem_table(QSqlDatabase *ptr_database, const int raster_id);

	///Close and delete the database table for the raster elements
	static void close_elem_table(void);
	///Get the maximum value of the global index of the element database table
	static int get_max_glob_id_elem_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the element data to database table
	static string get_insert_header_elem_table(QSqlDatabase *ptr_database);

	///Create the database table for the results of the damage calculation per element
	static void create_erg_table(QSqlDatabase *ptr_database);
	///Set the database table for the results of the damage calculation per element: it sets the table name and the name of the columns and allocate them
	static void set_erg_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the results of the damage calculation per element
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database);
	///Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario id
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database,  const int bound_sz);

	///Delete the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz );
	///Delete the data in the database table for the results of the damage calculation specified by the raster id
	static void delete_data_in_erg_table_per_raster(QSqlDatabase *ptr_database, const int raster_id);

	///Delete the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete the data in the database table for the results of the damage calculation specified parameters
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag=false);
	///Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static int select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz );
	///Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (just part of it)
	static int select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_row );
	///Count data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
	static int count_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz );

	
	
	
	///Close and delete the database table for the results of the damage calculation per element
	static void close_erg_table(void);
	///Get the maximum value of the global index of the element result database table
	static int get_max_glob_id_erg_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the element result data to database table
	static string get_insert_header_erg_table(QSqlDatabase *ptr_database);

	///Switch the applied-flag for the economic damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the economic damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);

	///Switch the applied-flag for the economic damage elements in the database table for a defined system state
	static void switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the economic damage elements in the database table for a defined system state
	static void switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int elem_id, const int raster_id, const bool flag);
	///Reswitch the applied-flag for the economic damage elements in the database table for the base state; a defined system state and his applied elements are decisive (static)
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
	///Index of the damage function for the land use category, calculating immobile damages
	int id_immob_dam;
	///Pointer to the damage function for the land use category, calculating immobile damages
	Dam_Ecn_Damage_Function *ptr_immob_dam;

	///Index of the damage function for the land use category, calculating mobile damages
	int id_mob_dam;
	///Pointer to the damage function for the land use category, calculating mobile damages
	Dam_Ecn_Damage_Function *ptr_mob_dam;

	///Value for an individual stock value for mobile damages
    float indiv_mob_stock_value;
	///Value for an individual stock value for immobile damages
    float indiv_immob_stock_value;

	///Flag if it is a no-info element concerning immobile damages
	bool no_info_immob_elem;
	///Flag if it is a no-info element concerning mobile damages
	bool no_info_mob_elem;

	///Results of the mobile damage calculation
	_dam_ecn_result result_mob_dam;
	///Result of the immobile damage calculation
	_dam_ecn_result result_immob_dam;
	///Result total of the damage calculation
	_dam_ecn_result result_total;


	//method
	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
	

};

#endif
