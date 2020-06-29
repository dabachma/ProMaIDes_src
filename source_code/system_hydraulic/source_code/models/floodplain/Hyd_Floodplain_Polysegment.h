#pragma once
/**\class Hyd_Floodplain_Polysegment
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_FLOODPLAIN_POLYSEGMENT_H
#define HYD_FLOODPLAIN_POLYSEGMENT_H

//system sys class
//base class
#include "Geo_Polysegment.h"
#include "Geo_Interception_Point_List.h" 
#include "Common_Const.h"
#include "_Sys_Common_System.h"

//Hyd libs
//for the input per file
#include "_Hyd_Parse_IO.h"
#include "Hyd_Element_Floodplain.h"
#include "Hyd_Floodplain_Dikeline_Point.h"
#include "Hyd_Floodplain_Dikeline_Point_List.h"

///Enumerator to specifies the type of the Hyd_Floodplain_Polysegment \ingroup hyd
enum _hyd_floodplain_polysegment_type{
	///Polysegment type of dikelines 
	DIKELINE,
	///Polysegment type of the coastline
	COASTLINE,
	///Polysegment type of the river midline
	RIVERLINE,
	///Polysegment type of the river bankline
	RIVERBANKLINE,
	///Polysegment type of the floodplain boundary
	FP_BOUNDARYLINE
};

//forward declaration
struct _hyd_breach_start_parameters;
struct _hyd_idealisation_parameter;


///Geometrical class for the polysegments in a floodplain model, like dikeline, coastline or the river bankline \ingroup hyd
/**

\see Hyd_Model_Floodplain
*/
class Hyd_Floodplain_Polysegment : public Geo_Polysegment, public _Sys_Common_System
{
public:
	///Default constructor
	Hyd_Floodplain_Polysegment(void);
	///Default destructor
	~Hyd_Floodplain_Polysegment(void);

	//members

	///A list with interception points
	Geo_Interception_Point_List intercept_list;

	///Pointer to the table for the polysegment data in a database
	static Tables *polysegment_table;
	///Pointer to the table for the polysegment point data in a database
	static Tables *polysegment_point_table;

	//methods

	///Input the members per file
	void input_members(const int index, const string filename, const double offset_x=0.0, const double offset_y=0.0);

	///Create the database table for the polysegment data in a database
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the polysegment data: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the polysegment data
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Delete data in the database table for the polysegment data by a given system id
	static void delete_data_in_table(QSqlDatabase *ptr_database, _sys_system_id id);
	///Close and delete the database table for the polysegment data
	static void close_table(void);
	///Check if a valid coast-line is set in the database table, for applying the coast model
	static bool check_coast_model_applied_in_database(QSqlDatabase *ptr_database, _sys_system_id id);
	///Select and count the number of relevant polysegments in a database table 
	static int select_relevant_polysegments_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int model_id, const bool coast_flag , const bool with_output);
	///Select and count the number of relevant polysegments in a database table (static)
	static int select_relevant_polysegments_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool coast_flag , const bool with_output);
	///Select and count the number of polysegments in a database table (static)
	static int select_polysegments_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool coast_flag , const bool with_output);

	
	///Switch the applied-flag for the section in the database table for a defined system state and id (static)
	static void switch_applied_flag_polysegment_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int glob_id);
	///Switch the applied-flag for the section in the database table for a defined system state and id (static)
	static void switch_applied_flag_polysegment_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);


	///Transfer the polysegment data of a file into a database table
	void transfer_polysegment2database_table(QSqlDatabase *ptr_database, const int model_id, const bool coast_flag);
	///Set the polysegment data from a database selection 
	void input_members(const QSqlQueryModel *results, QSqlDatabase *ptr_database, const int index);

	///Set number of points
	void set_number_of_points(const int number);

	///Set dikeline polysegment by point list
	void set_dike_line_by_list(Hyd_Floodplain_Dikeline_Point_List *list);
	///Fill marked points with heights
	void fill_marked_points_heights(const double marker);

	///Get pointer to the dike line points
	Hyd_Floodplain_Dikeline_Point* get_ptr_dike_line_points(const int index);
	///Set the dikeline points to Geo_Segement
	void set_dikelinepoints_to_segment(void);

	///Set the line index 
	void set_index(const int index);
	///Get the line index 
	int get_index(void);

	///Get the name of the polysegment
	string get_name(void);

	///Set the name of the polygon
	void set_name(string nam);

	///Output the members
	void output_members(void);
	///Output the polysegment to tecplot
	void output2tecplot(ofstream *output_file);

	///Set the no_flow_flag
	void set_no_flow_flag(const bool flag);
	///Get the no_flow_flag
	bool get_no_flow_flag(void);
	///Get the height of an segment for a given index
	double get_segments_height(const int index);
	///Get the Poleni-factor of an segment for a given index
	double get_segments_poleni(const int index);
	///Get a no overflow flag of an segment for a given index
	bool get_segments_no_overflow(const int index);

	///Set the type of the line (_hyd_floodplain_polysegment_type)
	void set_line_type(_hyd_floodplain_polysegment_type type);
	///Get the type of the line (_hyd_floodplain_polysegment_type)
	_hyd_floodplain_polysegment_type get_line_type(void);

	///Set the element type (_hyd_elem_type) to set to elements which are intercepted by this line
	void set_element_type(_hyd_elem_type type);
	///Get the element type (_hyd_elem_type) to set to elements which are intercepted by this line
	_hyd_elem_type get_element_type(void);

	//Set if the line is closed (true) or not (false) per string
	void set_is_closed_str(string txt);

	///Conversion function for output from enumerator _hyd_floodplain_polysegment_type to text
	static string convert_polysegment_type2txt(_hyd_floodplain_polysegment_type type);

	///Clone the floodplain-polysegment
	void clone_polysegment(Hyd_Floodplain_Polysegment *seg);

	///Get the pointer to the upwards dikeline point
	Hyd_Floodplain_Dikeline_Point* get_ptr_upwards_point(const int segment_id);
	///Get the pointer to the downwards dikeline point
	Hyd_Floodplain_Dikeline_Point* get_ptr_downwards_point(const int segment_id);

	///Find the pointer to the dikeline points up and down the dikeline to a given station
	bool find_ptr_up_down_point(const double station, Hyd_Floodplain_Dikeline_Point **up, Hyd_Floodplain_Dikeline_Point **down);

	///Get a polyline of the dikeline points, which are represented by an fpl-section
	void get_polyline_fpl_section(Geo_Polysegment *polyline,Hyd_Floodplain_Dikeline_Point *start, Hyd_Floodplain_Dikeline_Point *end);

	///Idealise the points of the dikeline, which are represented by an fpl-section, with the data of the fpl-section
	void idealise_points_by_fpl(Hyd_Floodplain_Dikeline_Point *start, Hyd_Floodplain_Dikeline_Point *end, _hyd_idealisation_parameters ideal_params, _hyd_breach_start_parameters *start_params, const int section_id);

	///Get the number of involved points to a fpl-section
	int get_number_involved_points(void);
	///Get a pointer to the involved points by a given index
	Hyd_Floodplain_Dikeline_Point* get_ptr_involved_points(const int index);

	///Set distances of the dikeline points to the beginning of the polysegement
	void set_distances2begin(void);

private:

	//members


	///Name of the polysegment
	string name;

	///Index of the polysegment
	int index;
	///The dikeline points 
	Hyd_Floodplain_Dikeline_Point *line_points;
	///Flag to set noflow boundaries; if this flag is set, the height is not more important
	bool no_flow_flag;
	///Specifies the type of the line (_hyd_floodplain_polysegment_type)
	_hyd_floodplain_polysegment_type line_type;
	///Specifies the element type (_hyd_elem_type) which are intercepted by this line
	_hyd_elem_type intercepted_elem_type;

	///A list of pointer to a part of the dikeline points
	Hyd_Floodplain_Dikeline_Point **list_part_points;
	///Number of the points stored in the list list_part_points
	int number_part_points;



	//methods

	///Allocate the points of the dikeline
	void allocate_dikeline_points(void);
	///Delete the points of the dikeline
	void delete_dikeline_points(void);



	///Allocate the list of the pointer of a part of the dikeline points
	void allocate_part_point_list(void);
	///Delete the list of pointer of a part of the dikeline points
	void delete_part_point_list(void);
	///Set the list of the pointer of a part of the dikeline points by two given points
	void set_part_point_list(Hyd_Floodplain_Dikeline_Point *up, Hyd_Floodplain_Dikeline_Point *down);

	///Create the database table for the polysegment point data in a database
	static void create_point_table(QSqlDatabase *ptr_database);
	///Set the database table for the polysegment point data: it sets the table name and the name of the columns and allocate them 
	static void set_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the polysegment point data 
	static void delete_point_data_in_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the polysegment point data 
	static void delete_point_data_in_table(QSqlDatabase *ptr_database, const int glob_id);
	///Transfer the polysegment point data of a file into a database table
	void transfer_polysegment_point2database_table(QSqlDatabase *ptr_database, const int poly_id);
	///Close and delete the database table for the polysegment point data 
	static void close_point_table(void);
	///Select and count the number of relevant polysegment points in a database table
	static int select_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int global_segment_id);

	///Check the dikeline points
	void check_dikeline_points(void);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};

#endif
