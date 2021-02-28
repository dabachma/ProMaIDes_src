#pragma once
/**\class Fpl_Cub_Dike_Landside
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/

#ifndef FPL_CUB_DIKE_LANDSIDE_H
#define FPL_CUB_DIKE_LANDSIDE_H

//system sys

//system fpl
#include "Fpl_Cub_Dike_Segment_Landside.h"


///Cubature of the landside of a dike section as segments \ingroup fpl
/**


*/
class Fpl_Cub_Dike_Landside
{
public:
	///Default constructor
	Fpl_Cub_Dike_Landside(void);
	///Default destructor
	~Fpl_Cub_Dike_Landside(void);


	//method
	///Set the geometrie of the landside cubature per database table
	void set_segments(Fpl_Section_Points *base_point, Fpl_Section_Points *crest_point, QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the geometrie of the landside cubature per file
	void set_segments(Fpl_Section_Points *base_point, Fpl_Section_Points *crest_point, QFile *ifile, int *line_counter);

	///Transfer the geomtrical data of the landside dike cubature to database
	void transfer_geometry2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database);


	///Output the geometrie of the landside to display/console
	void output_geometrie(void);

	///Get a pointer to the last point (:= base landside)
	Geo_Point *get_ptr_last_point(void);
	///Get a pointer to the last point (:= crest landside)
	Geo_Point *get_ptr_first_point(void);


	///Get the total length
	double get_total_length(void);
	///Get the number of segments
	int get_number_segments(void);
	///Get a pointer to a segments specified with an index
	Fpl_Cub_Dike_Segment_Landside *get_segment(const int index);  

	//Calculate the middle of the gradient at the landside cubature
	double calculate_mid_gradient(void);


private:

	//member
	///Pointer to the landside base point
	Fpl_Section_Points *ptr_base_point;
	///Pointer to the landside crest point
	Fpl_Section_Points *ptr_crest_point;
	///Number of points representing the landside cubature (number_landside_points + crest point + base point)
	int number_points;
	///Segments of the landside cubature 
	Fpl_Cub_Dike_Segment_Landside *ptr_segments;
	///Number of segments of the landside cubature
	int number_segments;

	///Total length of the landside cubature
	double total_length;
	///Horizontal length of the landside cubature
	double total_horizontal;
	///Vertical length of the landside cubature
	double total_vertical;


	//methods
	///Allocate the segments of the landside cubature
	void allocate_segments(void);
	///Delete the segments of the landside cubature
	void delete_segments(void);

	///Calculate the total length as well as the vertical and horizontal distance 
	void calc_total_length(void);
	///Set the pointer to the base point and the crest point
	void set_base_crest_point(Fpl_Section_Points *base_point, Fpl_Section_Points *crest_point);
	///Check the cubature segments	
	void check_cubature(void);

	///Find the key-values for the file input of the geometrical points
	void find_key_values_file(const string myline);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif
