#pragma once
/**\class Dam_Pys_Element
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/

#ifndef DAM_PYS_ELEMENT_H
#define DAM_PYS_ELEMENT_H

//base class
#include "_Dam_Element.h"
#include "Dam_Pys_Category.h"
#include "Dam_Pys_Density_Function.h"

///Data structure for gathering the result data of the psycho-social damage calculation
struct _dam_pys_result{
	///Score value
	double score_value;
	///Score value multiplied with the population density factor
	double score_pop_value;
};

///Information of the psycho-social damage calculation provided in the element of the raster \ingroup dam
/**

*/
class Dam_Pys_Element : public _Dam_Element
{
public:
	///Default constructor
	Dam_Pys_Element(void);
	///Default destructor
	~Dam_Pys_Element(void);

	//members
	///Pointer to the table for the raster element information in a database
	static Tables *elem_table;
	///Pointer to the table for the results of the damage calculation for the raster elements in a database
	static Tables *elem_erg_table;

	//method
	///Transfer the psycho-social raster element data to a database
	void transfer_input_members2database(QSqlDatabase *ptr_database, const int raster_index, const double cellsize, const double x_origin, const double y_origin);

	///Get the data-string to complete a insert-string for inserting the data of the element to database
	string get_datastring_members2database(const int global_id, const int raster_index, const double cellsize, const double x_origin, const double y_origin);

	///Input the psycho-social damage raster element data per database
	void input_element_perdatabase(const QSqlQueryModel *results, const int glob_index);

	///Set the element data from the raster interception to the database table: identifier of the floodplain, -floodplain element and the reduced area
	void set_interception_elem_data2database(QSqlDatabase *ptr_database, QSqlQuery *elem_results, const int raster_id);
	///Get a string to set the element data from the raster interception to the database table: identifier of the floodplain, -floodplain element and the reduced area
	bool get_string_interception_elem_data2database(ostringstream *text);

	///Set the index of the psycho-social category
	void set_id_category(const int id);
	///Set the individual stock value of the population density
	void set_population_density(const double stock_pop);
	///Set the value of the psycho-social criteria age class 50 to 59
	void set_criteria_age_50_59(const double value);
	///Set the value of the psycho-social criteria age class greater 80
	void set_criteria_age_80(const double value);
	///Set the value of the psycho-social criteria female gender
	void set_criteria_female(const double value);
	///Set the value of the psycho-social criteria home owner
	void set_criteria_owner(const double value);

	///Get the index of the psycho-social category
	int get_id_category(void);
	///Get the individual stock value of the population density
	double get_population_density(void);
	///Get the value of the psycho-social criteria age class 50 to 59
	double get_criteria_age_50_59(void);
	///Get the value of the psycho-social criteria age class greater 80
	double get_criteria_age_80(void);
	///Get the value of the psycho-social criteria age female gender
	double get_criteria_female(void);
	///Get the value of the psycho-social criteria home owner
	double get_criteria_owner(void);

	///Output the damage members
	void output_damage_members(ostringstream *txt);

	///Connect the element to the psycho-social category
	bool connect_element2category(const int no_info, Dam_Pys_Category *category);
	///Get a flag if the psycho-social category is found
	bool category_function_is_found(void);

	///Set the pointer to the density function
	void set_ptr_density_function(Dam_Pys_Density_Function *ptr_function);

	///Set the element to a no-info element concerning psycho-social damage calculation
	void set_noinfo_elem(void);

	///Calculate the psycho-social damages per element
	void calculate_damages(Dam_Impact_Values *impact);
	///Get the data-string to complete a insert-string for inserting the data of the element results to database
	string get_datastring_results2database(const int global_id, const int bound_sz, const string break_sz, const int raster_id, const double cellsize, const double x_origin, const double y_origin, const bool must_output);

	///Get the results for the psycho-social criteria age class 50 to 59
	_dam_pys_result get_result_crit_age50_59(void);
	///Get the results for the psycho-social criteria age class greater 80
	_dam_pys_result get_result_crit_age80(void);
	///Get the results for the psycho-social criteria female gender
	_dam_pys_result get_result_crit_female(void);
	///Get the results for the psycho-social criteria home owner
	_dam_pys_result get_result_crit_owner(void);

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
	///Delete all data in the database table for the raster elements of the given system state
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
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz);
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

	///Switch the applied-flag for the psycho-social damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the psycho-social damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);

	///Switch the applied-flag for the psycho-social damage elements in the database table for a defined system state
	static void switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the psycho-social damage elements in the database table for a defined system state
	static void switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int elem_id, const int raster_id, const bool flag);

	///Reswitch the applied-flag for the psycho-social damage elements in the database table for the base state; a defined system state and his applied elements are decisive
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
	///Index of the category
	int id_category;
	///Pointer to the psycho-social category
	Dam_Pys_Category *ptr_category;

	///Pointer to the density function
	Dam_Pys_Density_Function *ptr_density_func;

	///Value of the population density
	double pop_density;

	///Value of the psycho-social criteria age class 50 to 59; input value of the corresponding score function
	double crit_age50_59;
	///Value of the psycho-social criteria age class greater 80; input value of the corresponding score function
	double crit_age80;
	///Value of the psycho-social criteria female gender; input value of the corresponding score function
	double crit_female;
	///Value of the psycho-social criteria home owner; input value of the corresponding score function
	double crit_owner;

	///Flag if it is a no-info element; it must be setted by psycho-social category
	bool no_info_elem;

	///Results for the psycho-social criteria age class 50 to 59
	_dam_pys_result res_crit_age50_59;
	///Results for the psycho-social criteria age class greater 80
	_dam_pys_result res_crit_age80;
	///Results for the psycho-social criteria female gender
	_dam_pys_result res_crit_female;
	///Results for the psycho-social criteria home owner
	_dam_pys_result res_crit_owner;

	//method
	///Reset the result value of the element
	void reset_result_value(void);

	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif