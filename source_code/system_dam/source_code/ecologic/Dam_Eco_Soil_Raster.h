#pragma once
/**\class Dam_Eco_Soil_Raster
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/

#ifndef DAM_ECO_SOIL_RASTER_H
#define DAM_ECO_SOIL_RASTER_H

//dam-libs
//base class
#include "Dam_Raster.h"
#include "Dam_Eco_Soil_Erosion_Function.h"
#include "Dam_Eco_Soil_Cost_Function.h"
#include "Dam_Eco_Soil_Element.h"

///This class provides the geometrical raster as well as the specific soil-erosion information for the ecological soil-damage calculation \ingroup dam
/**

*/
class Dam_Eco_Soil_Raster : public Dam_Raster
{
//class for displaying the data
friend class Sys_Data_Tree_Wid;

public:
	///Default constructor
	Dam_Eco_Soil_Raster(void);
	///Default destructor
	~Dam_Eco_Soil_Raster(void);

	//methods
	///Read in the ecologic soil-erosion damage raster from file: erosion-type, soil-type
	void read_all_soil_erosion_raster_per_file(string fname_ero, string fname_soil);
	///Transfer the ecologic soil-erosion damage raster data to a database: the general settings of the raster (geometry etc.) as well as the element information
	void transfer_input_members2database(QSqlDatabase *ptr_database);
	///Input the ecologic soil-erosion damage raster data per database: the general settings of the raster (geometry etc.)
	void input_raster_perdatabase_general_data(const QSqlQueryModel *results, const int glob_index);
	///Input the ecologic soil-erosion damage raster data per database: element information, the general information has to be set before
	void input_raster_perdatabase_element_data(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Output the members of geometrical soil raster to display/console
	void output_member(void);
	///Output the number of floodplain connected elements
	void output_number_fp_connected_elem(ostringstream *cout);
	///Output the sum of the damage results to display/console
	void output_result_damage(void);

	///Get the results of the ecological soil-erosion damage calculation for the raster
	double get_soil_ero_damage_results(void);

	///Calculate the economical damages for the raster
	void calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number);
	///Output the economical damages for the raster to database
	void output_results2database(QSqlDatabase *ptr_database,const int bound_sz, const string break_sz, bool *was_output, const bool must_output);

	///Connect the soil-type cost-functions to the elements
	void connect_soil_type2elems(const int number, Dam_Eco_Soil_Cost_Function *functions);
	///Ceck if all soil elements could be connected to the soil-type cost-functions
	void check_soil_cost_function_connection(void);

	///Connect the soil-erosion functions to the elements
	void connect_soil_erosion2elems(const int number, Dam_Eco_Soil_Erosion_Function *functions);
	///Ceck if all soil elements could be connected to the soil-erosion functions
	void check_soil_erosion_function_connection(void);

	///Set intercepted elements to a reduced area
	void set_intercepted_elem2reduced_area(Dam_Eco_Soil_Raster *raster, QSqlDatabase *ptr_database, const _sys_system_id id);
	///Set intercepted elements to the indizes of the hydraulic floodplain
	void set_intercepted_elem2floodplain_id(Hyd_Model_Floodplain *fp_model);

	///Set intercepted elements with polygons to the new soil-type (cost) id
	void set_intercepted_elem2new_soil_type(QSqlDatabase *ptr_database, const _sys_system_id id, Dam_Polygon *polys, QList <int> current_id, QList <int> new_id);

	///Transfer element data evaluated by an interception to database: identifier of the floodplain, -floodplain element and the reduced area
	void transfer_intercepted_elem_data2database(QSqlDatabase *ptr_database);

	///Create the database table for the general parameters of the raster
	static void create_raster_table(QSqlDatabase *ptr_database);
	///Set the database table for the general parameters of the raster: it sets the table name and the name of the columns and allocate them
	static void set_raster_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the general raster parameters
	static void delete_data_in_raster_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the general raster parameters as well as the raster elements data of the given id
	static void delete_data_in_raster_table(QSqlDatabase *ptr_database, const int id);
	///Close and delete the database table for the general parameters of raster
	static void close_table(void);
	///Check if all ecological soil-erosion raster are connected to the hydraulic system
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

private:
	///Pointer to the soil-type, -erosion information of each raster element
	Dam_Eco_Soil_Element *soil_elem;

	///Flag if the raster is set
	bool raster_is_set;

	///Pointer to the table for the general raster parameters in a database
	static Tables *raster_table;
	///Sum over the elements of the soil-erosion damage results
	double result_soil_ero_dam;

	//methods
	///Set the area of an element to the given area
	void set_area_element2given(const int index, const double area);
	///Allocate the economic information of each raster element
	void allocate_raster_elem_info(void);
	///Delete the economic information of each raster element
	void delete_raster_elem_info(void);

	///Extract the soil-type to the raster element information
	void extract_soil_type2raster_elem(Dam_Raster *raster);
	///Extract the erosion-type to the raster element information
	void extract_erosion_type2raster_elem(Dam_Raster *raster);

	///Transfer the midpoint information from the raster element to the raster element information
	void transfer_midpoint2elem_info(void);

	///Set the system-id to the element information
	void set_sys_system_id(const _sys_system_id id);

	///Count the number of the floodplain connected elements
	int count_number_fp_connected_elem(void);

	///Set the connection flag to database
	void set_connection_flag2database(const bool flag, QSqlDatabase *ptr_database);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif