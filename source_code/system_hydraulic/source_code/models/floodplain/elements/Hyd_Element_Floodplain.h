#pragma once
/**\class Hyd_Element_Floodplain
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_ELEMENT_FLOODPLAIN_H
#define HYD_ELEMENT_FLOODPLAIN_H

//system_hyd classes
#include "_Hyd_Element_Floodplain_Type.h"
#include "Hyd_Element_Floodplain_Type_Standard.h"
#include "Hyd_Element_Floodplain_Type_Noflow.h"
#include "Hyd_Element_Floodplain_Type_Coast.h"
#include "Hyd_Element_Floodplain_Type_River.h"
#include "Hyd_Element_Floodplain_Type_Dam.h"
//info class about the hydraulic boundary szenario data
#include "Hyd_Boundary_Szenario.h"

//system_sys_ classes
#include "_Sys_Common_System.h" 
#include "Geo_Point.h"
#include "Geo_Raster_Polygon.h"

using namespace std;

///Enumerator for the element types \ingroup hyd
enum _hyd_elem_type{
	///Standard element type
	STANDARD_ELEM,
	///Coast element type; the waterlevel is set by the coast model (Hyd_Coast_Model)
	COAST_ELEM,
	///Noflow element type (Hyd_Floodplain_Polygon)
	NOFLOW_ELEM,
	///Noinfo element type; there are no data available for this element 
	NOINFO_ELEM,
	///River element type; the element is intersected by or is in a river polygon (Hyd_Model_River)
	RIVER_ELEM,
	///Other floodplain element type; the element is intersected by another floodplain (Hyd_Model_Floodplain)
	OTHER_FP_ELEM,
	///Dikeline element type; the element is intersected by a dikeline (Hyd_Floodplain_Polysegment)
	DIKELINE_ELEM,
};
class Hyd_Element_Floodplain;
///Structure to gather pointer to the neighbouring elements \ingroup hyd
struct _hyd_neighbouring_elems{
	///Neighbouring element in y-direction 
	Hyd_Element_Floodplain *y_direction;
	///Neighbouring element in x-direction 
	Hyd_Element_Floodplain *x_direction;
	///Neighbouring element in y-direction (negative)
	Hyd_Element_Floodplain *minus_y_direction;
	///Neighbouring element in x-direction (negative)
	Hyd_Element_Floodplain *minus_x_direction;
};

///Class for the elements of the floodplain-models \ingroup hyd
/**
The elements are the representation of the floodplain. Geometrically they are rectangles, with a special
element type. The element type is managed in this class.
\see _Hyd_Element_Floodplain_Type
*/
class Hyd_Element_Floodplain : public _Sys_Common_System
{
public:
	///Default constructor
	Hyd_Element_Floodplain(void);
	///Default destructor
	~Hyd_Element_Floodplain(void);

	//members

	///Pointer to the element type
	_Hyd_Element_Floodplain_Type *element_type;
	///Pointer to the database table for the results of an hydraulic simulation for the floodplain elements
	/**This pointer is allocated with set_erg_table(QSqlDatabase *ptr_database) and deleted with close_erg_table(void) */
	static Tables *erg_table;
	/**This pointer is allocated with set_erg_instat_table(QSqlDatabase *ptr_database) and deleted with close_erg_instat_table(void) */
	static Tables *erg_instat_table;
	///Pointer to the database table for the element data in a database
	/**This pointer is allocated with set_table(QSqlDatabase *ptr_database) and deleted with close_table(void) */
	static Tables *elem_table;
	///Pointer to the database table for the boundary data in a database
	/**This pointer is allocated with set_element_boundary_table(QSqlDatabase *ptr_database) and deleted with void close_boundary_table(void) */
	static Tables *boundary_table;
	///Member for storing the hydraulic boundary szenario information
	Hyd_Boundary_Szenario *hyd_sz;


	//methods

	///Set the input from a string which is read from a file
	void input_value(const string line_of_file);
	///Transfer the floodplain element data to a database
	void transfer_element_members2database(const int glob_elem_id, QSqlQuery *model, QSqlDatabase *ptr_database, const int fp_number, const _hyd_floodplain_geo_info geo_info);
	///Get a string for transfering the data to database 
	string get_datastring2database(QSqlDatabase *ptr_database, const int glob_elem_id, const int fp_number, const _hyd_floodplain_geo_info geo_info);
	///Get the header for inserting the element data to database table (static)
	static string get_insert_header_data_table(QSqlDatabase *ptr_database);
	///Transfer the floodplain element boundary data of an hydraulc boundary scenario to a database
	void transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database, const int fp_number);
	///Input the floodplain element data with the index from the selected data set of a database table
    void input_members(const int index, const QSqlQueryModel *query_result, QSqlQueryModel *bound_result, const int bound_number, const bool just_elems, int *last_index);
	///Set members directly
	void set_members_directly(const int elem_number, Geo_Point* midpoint, const double z_value, const _hyd_elem_type type);

	///Create the database table for the element data of the floodplain model
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the element data of the floodplain model: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the floodplain element data; also for the appending boundary table
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the floodplain element data for the given system state
	static void delete_data_in_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Close and delete the database table for the floodplain element data; also for the appending boundary table 
	static void close_table(void);
	///Select and count the number of relevant floodplain elements for one floodplain model in a database table
	static int select_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const bool with_output = true);
	///Select and count the number of relevant floodplain elements for one floodplain model in a database table (just parts of it)
	static int select_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int offset, const int number_rows, const bool with_output = true);

	///Count the number of relevant elements
	static int count_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const bool with_output = true);

	///Create the database table for the boundary element data
	static void create_element_boundary_table(QSqlDatabase *ptr_database);
	///Create the database view for the connection of boundary ids and the floodplain elements
	static void create_bound2elems_view(QSqlDatabase *ptr_database);
	///Check if the view exists already 
	static bool check_bound2elems_view_exists(QSqlDatabase *ptr_database);

	///Set the database table for the boundary element data: it sets the table name and the name of the columns and allocate them
	static void set_element_boundary_table(QSqlDatabase *ptr_database);
	///Delete the boundary condition data in the database table for a given boundary scenario
	static void delete_boundary_cond_by_scenario(QSqlDatabase *ptr_database, const int sc_id);
	///Delete the boundary condition data in the database table for a given system state
	static void delete_boundary_cond_by_system_state(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Select and count the number of relevant boundary_condition in a database table
	static int select_relevant_boundary_cond_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sc_bound_id, const bool with_output = true);
	///Get a string for transfering the boundary data to database 
	string get_bound_datastring2database(QSqlDatabase *ptr_database,const int fp_number);
	///Get the header for inserting the element boundary data to database table (static)
	static string get_insert_header_bound_table(QSqlDatabase *ptr_database);

	///Create the database table for the results of an hydraulic simulation for the floodplain elements
	static void create_erg_table(QSqlDatabase *ptr_database);
	///Set the database table for the results of an hydraulic simulation for the floodplain elements: it sets the table name and the name of the columns and allocate them
	static void set_erg_table(QSqlDatabase *ptr_database, const bool not_close=false);
	///Select and count the number of relevant results per floodplain elements for one floodplain model in a database table
	static int select_relevant_results_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int bound_sz, const string break_sz, const bool with_output = true);
	///Select and count the number of relevant results per floodplain elements for one floodplain model in a database table (just part of it)
	static int select_relevant_results_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int bound_sz, const string break_sz, const int offset, const int number_rows, const bool with_output = true);
	///Count the number of relevant results per floodplain elements for one floodplain model in a database table
	static int count_relevant_results_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int bound_sz, const string break_sz, const bool with_output = true);

	///Select the data in the database table for the maximum results of the river profiles specified by the system id and the scenario-ids (static)
	static int select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz , const bool like_flag);
    ///Select the data if a flood szenario is calculated specified by the system id and the scenario-ids (static)
    static bool check_calc_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz , const bool like_flag);

	///Select the data in the database table for the instationary results of the river profiles specified by the system id and the scenario-ids (static)
	static int select_data_in_instat_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag);
	///Select the data if a flood szenario is calculated specified by the system id and the scenario-ids (static)
	static bool check_calc_in_instat_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag);

	
	///Delete the results data in the result database table for a given boundary scenario
	static void delete_results_by_scenario(QSqlDatabase *ptr_database, const int sc_id);
	///Delete the results data in the result database table for a the specified parameters
	static void delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag=false);
	///Delete the results data in the result database table for a the specified parameters
	static void delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int sc_id, const string break_sz, const bool like_flag=false);

	///Close and delete the database table for the results of an hydraulic simulation for the floodplain elements
	static void close_erg_table(void);
	///Get the maximum value of the global index of the element result database table
	static int get_max_glob_id_erg_table(QSqlDatabase *ptr_database);

	///Delete all data in the database table for the results of an hydraulic simulation for the floodplain elements
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database);
	///Delete the data in the database table for the results of the hydraulic calculation for a floodplain specified by the system state 
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete the data in the database table for the results of the hydraulic calculation for a floodplain specified by the system state and the scenario-ids 
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz );
	///Delete the data in the database table for the instationaryresults of the hydraulic calculation for a floodplain specified by the system id and the scenario-ids (static)
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);


	///Switch the applied-flag for the 2-d results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the 2-d results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);
	///Switch the applied-flag for the boundary condition in the database table for a defined system state
	static void switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the boundary condition in the database table for a defined system state
	static void switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);

	///Copy the results of a given system id to another one in database table
	static void copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);


	///Create the database table for the instationary results of an hydraulic simulation for the floodplain elements
	static void create_erg_instat_table(QSqlDatabase *ptr_database);
	///Create the database view for the instationary results of an hydraulic simulation for the floodplain elements
	static void create_erg_instat_view1(QSqlDatabase *ptr_database);
	///Set the database table for the instationary results of an hydraulic simulation for the floodplain elements: it sets the table name and the name of the columns and allocate them
	static void set_erg_instat_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Close and delete the database table for the instationary results of an hydraulic simulation for the floodplain elements
	static void close_erg_instat_table(void);
	///Get the maximum value of the global index of the element instationary result database table
	static int get_max_glob_id_erg_instat_table(QSqlDatabase *ptr_database);

	///Select and count the number of relevant instationary results per floodplain elements for one floodplain model in a database table
	static int select_relevant_instat_results_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int bound_sz, const string break_sz, const bool with_output = true);
	///Select and count the number of relevant instationary results per floodplain elements for one floodplain model in a database table (just part of it)
	static int select_relevant_instat_results_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int bound_sz, const string break_sz, const int offset, const int number_rows, const string time_date, const bool with_output = true);
	///Count the number of relevant instationary results per floodplain elements for one floodplain model in a database table
	static int count_relevant_instat_results_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int bound_sz, const string break_sz, const string time_date, const bool with_output = true);
	///Get a list of dicstinct date-time strings from database table
	static int get_distinct_date_time_instat_results_elements_database(QStringList *list, QSqlDatabase *ptr_database, const _sys_system_id id,  const int bound_sz, const string break_sz, const bool with_output = true);



	///Copy the instationary results of a given system id to another one in database table
	static void copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);


	///Clone the members of the floodplain elements
	void clone_element(Hyd_Element_Floodplain *elements);

	///Initialize the _Hyd_Element_Floodplain_Type; decide which type is chosen
	void init_element_type(_hyd_neighbouring_elems neigh_elem, double *width_x, double *width_y, double *area, Hyd_Param_Material *mat_table, const bool clone);
	
	///Set the element number
	void set_elem_number(const int number);
	///Set the mid_point of the element
	void set_mid_point(Geo_Point *mid_point);
	///Set the element type
	void set_element_type(_hyd_elem_type type);

	///Set element flag for flow in x-direction
	void set_x_noflow_flag(const bool flag);
	///Set element flag for flow in y-direction
	void set_y_noflow_flag(const bool flag);
	///Set the absolute element border height z (m) in x-direction and the Poleni-factor
	void set_x_border_z(const double abs_height, const double pol_factor);
	///Set the absolute element border height z (m) in y-direction and the Poleni-factor
	void set_y_border_z(const double abs_height, const double pol_factor);
	///Set if there are no data available for an element
	void set_no_info_element(const int no_info_value);

	///Get a pointer to the flow data
	_hyd_flowdata_floodplain_elem get_flow_data(void);
	///Get a pointer to the boundary data
	_hyd_boundarydata_floodplain_elem get_boundary_data(void);
	///Get the mid point
	Geo_Point* get_mid_point(void);
	///Get element number
	int get_elem_number(void);
	///Get the coupling relevant element index
	int get_coupling_relevant_elem_index(void);
	///Get element type
	_hyd_elem_type get_elem_type(void);
	///Get the global geodetic height (z_value)
    double get_z_value(void);

	///Output the members
	void output_members(ostringstream *cout);
	///Output the header for the members output
	static void output_member_header(ostringstream *cout);

	///Output setted members
	void output_setted_members(ostringstream *cout);
	///Output the header for the setted members output
	static void output_setted_header(ostringstream *cout);

	///Ouput the result members per internal timestep
	void output_result_values(ostringstream *cout);
	///Ouput the header for the maximum calculated values output
	static void output_header_result_values(ostringstream *cout);

	///Ouput the maximum calculated results
	void output_maximum_calculated_values(ostringstream *cout);
	///Ouput the header for the maximum calculated results output
	static void output_header_maximum_calculated_values(ostringstream *cout);

	///Get the header for inserting the element result data to database table (static)
	static string get_insert_header_erg_data_table(QSqlDatabase *ptr_database);
	///Get a string for transfering the result data to database 
	string get_datastring_erg2database(const int glob_elem_id, const int fp_number, const _hyd_floodplain_geo_info geo_info, const string break_sz, const bool must_output);

	///Get the header for inserting the element instationary result data to database table (static)
	static string get_insert_header_erg_instat_data_table(QSqlDatabase *ptr_database);
	///Get a string for transfering the instationary result data to database 
	string get_datastring_erg_instat_2database(const int id, const int fp_number, const int elem_id, const string break_sz, const string time, const _hyd_floodplain_geo_info geo_info);


	///Output the maximum calculated results to the database table (erg_table)
	void output_maximum_calculated_values(int *id_glob, QSqlQuery *model, QSqlDatabase *ptr_database, const int fp_no, const _hyd_floodplain_geo_info geo_info, const string break_sz);

	///Output the boundaries of the element as a geometry to tecplot
	void output_boundaries2tecplot(_hyd_floodplain_geo_info *geo_info, ofstream *output_file);

	///Conversion function for output (element type from enumerator to text)
	static string convert_elem_type2txt(_hyd_elem_type type);

	///Reset the boundary data of the floodplain element
	void reset_boundary_data(void);

	///Input the floodplain element boundary data from a database table
    void input_element_boundarydata2database(QSqlQueryModel *results, const int number, const int glob_id, int *last_index);

	///Set the boundary data buffer to the element type and delete it after
	void set_boundary_data_buffer2type(void);

	///Set the global element id
	void set_glob_elem_id(const int id);
	///Get the global element id
	int get_glob_elem_id(void);

private:
	//members



	///Number of the elements representing the floodplain as global id in the database
	int glob_elem_number;
	///Number of the elements representing the floodplain
	int elem_number;
	///Type of the element (_hyd_elem_type)
	_hyd_elem_type type;
	///Midpoint coordinates of the element
	Geo_Point mid_point;
	///Global geodetic height of the element
    double z_value;

	///Read-in buffer of the boundary data of the element (_hyd_boundarydata_floodplain_elem)
	_hyd_boundarydata_floodplain_elem *buffer_boundary_data;
	///Read-in buffer of the flow data of the element (_hyd_flowdata_floodplain_elem)
	_hyd_flowdata_floodplain_elem *buffer_flow_data;

	///Polygon-string for inserting the geometrical data of the element (polygon to database)
	string polygon_string;

	//methods

	///Allocate the element type
	void allocate_element_type(void);
	///Delete the element type
	void delete_element_type(void);
	///Conversion function for input (element type from text to enumerator)
	_hyd_elem_type convert_txt2elem_type(string txt);
	///Allocate the data buffers (_hyd_boundarydata_floodplain_elem and _hyd_flowdata_floodplain_elem)
	void allocate_data_buffer(void);
	///Delete the data buffers (_hyd_boundarydata_floodplain_elem and _hyd_flowdata_floodplain_elem)
	void delete_data_buffer(void);
	///Decide for the value of the boundary conditon (stationary => double value; instationary => integer value for the curve number)
	void decide_boundary_value(const double value);

	///Convert the midpoint of the element to a corner point with a given direction (Y_DIR:=up left; X_DIR:=up right; MINUS_Y_DIR:=down right; MINUS_X_DIR:=down left)
	Geo_Point convert_midpoint2corner(_hyd_floodplain_geo_info *geo_info, _hyd_neighbouring_direction dir);

	///Delete all data in the database table for the boundary element data
	static void delete_data_in_boundary_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the boundary element data
	static void close_boundary_table(void);

	


	///Transfer the floodplain element boundary data to a database table
	void transfer_element_boundarydata2database(QSqlDatabase *ptr_database, const int glob_id);


	///Delete the maximum result row in a database table for this element
	void delete_max_result_row_in_table(QSqlDatabase *ptr_database, const int fp_no, const string break_sz);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);

};
#endif
