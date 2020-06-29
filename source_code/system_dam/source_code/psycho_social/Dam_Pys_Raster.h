#pragma once
/**\class Dam_Pys_Raster
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/
#ifndef DAM_PYS_RASTER_H
#define DAM_PYS_RASTER_H

//dam-libs
#include "Dam_Raster.h"
#include "Dam_Pys_Element.h"
#include "Dam_Pys_Category.h"
#include "Dam_Pys_Function.h"
#include "Dam_Pys_Density_Function.h"

///Class for the geometrical raster of the psycho-social damages \ingroup dam
/**

*/
class Dam_Pys_Raster : public Dam_Raster
{
//class for displaying the data
friend class Sys_Data_Tree_Wid;

public:
	///Default constructor
	Dam_Pys_Raster(void);
	///Default destructor
	~Dam_Pys_Raster(void);

	//methods
	///Read in the psycho-social category data from file: population density, type of score function and the files for the criteria
	void read_all_damage_raster_per_file(string fname_pop, string fname_cat, string fname_crit_50_59, string fname_crit_80, string fname_crit_female, string fname_crit_owner);
	///Transfer the psycho-social damage raster data to a database: the general settings of the raster (geometry etc.) as well as the element information
	void transfer_input_members2database(QSqlDatabase *ptr_database);

	///Input the psycho-social damage raster data per database: the general settings of the raster (geometry etc.)
	void input_raster_perdatabase_general_data(const QSqlQueryModel *results, const int glob_index);
	///Input the psycho-social damage raster element data per database: element information, the general information has to be set before
	void input_raster_perdatabase_element_data(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Output the members of geometrical damage raster to display/console
	void output_member(void);
	///Output the sum of the damage results to display/console
	void output_result_damage(void);

	///Set intercepted elements to a no-info element
	void set_intercepted_elem2no_info(Dam_Pys_Raster *raster);

	///Set intercepted elements to a reduced area
	void set_intercepted_elem2reduced_area(Dam_Pys_Raster *raster, QSqlDatabase *ptr_database, const _sys_system_id id);
	///Set intercepted elements to the indizes of the hydraulic floodplain
	void set_intercepted_elem2floodplain_id(Hyd_Model_Floodplain *fp_model);
	///Transfer element data evaluated by an interception to database: identifier of the floodplain, -floodplain element and the reduced area
	void transfer_intercepted_elem_data2database(QSqlDatabase *ptr_database);

	///Set intercepted elements with polygons to the new category id
	void set_intercepted_elem2new_category(QSqlDatabase *ptr_database, const _sys_system_id id, Dam_Polygon *polys, QList <int> current_id, QList <int> new_id);

	///Calculate the psycho-social damages for the raster
	void calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number);
	///Output the psycho-social damages for the raster to database
	void output_results2database(QSqlDatabase *ptr_database,const int bound_sz, const string break_sz, bool *was_output, const bool must_output);

	///Connect the psycho-social category to the elements
	void connect_category2elems(const int number, Dam_Pys_Category *category);
	///Ceck if all elements could be connected to the psycho-socia
	void check_category_connection(void);
	///Set pointer of the population density function
	void set_ptr_density_func(Dam_Pys_Density_Function *ptr_density_func);

	///Create the database table for the general parameters of the raster
	static void create_raster_table(QSqlDatabase *ptr_database);
	///Set the database table for the general parameters of the raster: it sets the table name and the name of the columns and allocate them
	static void set_raster_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the general raster parameters
	static void delete_data_in_raster_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the general raster parameters as well as the raster elements data of the given id (static)
	static void delete_data_in_raster_table(QSqlDatabase *ptr_database, const int id);
	///Close and delete the database table for the general parameters of raster
	static void close_table(void);
	///Check if all psycho-social raster are connected to the hydraulic system
	static bool check_all_raster_connected2hyd(QSqlDatabase *ptr_database);
	///Reset the flag, that the hydraulic connection is set
	static void reset_hydraulic_connection_flag(QSqlDatabase *ptr_database);
	///Reset the hydraulic connection in elements (static)
	static void reset_hydraulic_connection_elements(QSqlDatabase *ptr_database);
	///Select and count the number of relevant damage raster in a database table
	static int select_relevant_raster_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const bool with_output = true);
	///Select and count the number of relevant damage raster in a database table by a given raster number
	static int select_relevant_raster_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int raster_no);

	///Sum up the total damage results from database
	void sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);

	///Get the results for the psycho-social criteria age class 50 to 59
	_dam_pys_result get_result_crit_age50_59(void);
	///Get the results for the psycho-social criteria age class greater 80
	_dam_pys_result get_result_crit_age80(void);
	///Get the results for the psycho-social criteria female gender
	_dam_pys_result get_result_crit_female(void);
	///Get the results for the psycho-social criteria home owner
	_dam_pys_result get_result_crit_owner(void);

private:
	//members
	///Pointer to the psycho-social category information of each raster element
	Dam_Pys_Element *element;

	///Flag if the raster is set
	bool raster_is_set;

	///Pointer to the population density function
	Dam_Pys_Density_Function *ptr_dens_function;

	///Pointer to the table for the general raster parameters in a database
	static Tables *raster_table;

	///Results for the psycho-social criteria age class 50 to 59
	_dam_pys_result res_crit_age50_59;
	///Results for the psycho-social criteria age class greater 80
	_dam_pys_result res_crit_age80;
	///Results for the psycho-social criteria female gender
	_dam_pys_result res_crit_female;
	///Results for the psycho-social criteria home owner
	_dam_pys_result res_crit_owner;

	//methods
	///Allocate the psycho-social category information of each raster element
	void allocate_raster_elem_info(void);
	///Delete the psycho-social category information of each raster element
	void delete_raster_elem_info(void);

	///Allocate a damage raster as buffer for an input per file
	void allocate_raster_buffer(Dam_Pys_Raster **buffer);
	///Delete a damage raster as buffer for an input per file
	void delete_raster_buffer(Dam_Pys_Raster **buffer);

	///Extract the psycho-social category index to the raster element information
	void extract_category2raster_elem(Dam_Raster *raster);
	///Extract the psycho-social criteria age class 50 to 59 to the raster element information
	void extract_criteria_age50_59_2raster_elem(Dam_Raster *raster);
	///Extract the psycho-social criteria age class greater 80 to the raster element information
	void extract_criteria_age80_2raster_elem(Dam_Raster *raster);
	///Extract the psycho-social criteria female gender to the raster element information
	void extract_criteria_female_2raster_elem(Dam_Raster *raster);
	///Extract the psycho-social criteria home owner to the raster element information
	void extract_criteria_owner_2raster_elem(Dam_Raster *raster);
	///Extract the psycho-social population density to the raster element information
	void extract_popdens2raster_elem(Dam_Raster *raster);

	///Set the system-id to the element information
	void set_sys_system_id(const _sys_system_id id);
	///Set the area of an element to the given area
	void set_area_element2given(const int index, const double area);
	///Set the element to a no-info element
	void set_no_info_element(const int index);

	///Reset the result value of the raster
	void reset_result_value(void);

	///Transfer the midpoint information and a polygon string from the raster element to the raster element information
	void transfer_midpoint2elem_info(void);

	///Set the connection flag to database
	void set_connection_flag2database(const bool flag, QSqlDatabase *ptr_database);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif