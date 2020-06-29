#pragma once
/**\class Fpl_Dike_Geo_Foreland
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/
#ifndef FPL_DIKE_GEO_FORELAND_H
#define FPL_DIKE_GEO_FORELAND_H

//fpl_libs
#include "Fpl_Dike_Segment.h"


///Geometric class representing the foreland of a dike section  \ingroup fpl
/**
 
*/
class Fpl_Dike_Geo_Foreland
{
public:
	///Default constructor
	Fpl_Dike_Geo_Foreland(void);
	///Default destructor
	~Fpl_Dike_Geo_Foreland(void);

	//method
	///Set the geometrie of the foreland geometry per database table
	void set_segments(Fpl_Section_Points *base_point, QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the geometrie of the foreland geometry per file
	void set_segments(Fpl_Section_Points *base_point, QFile *ifile, int *line_counter);

	///Transfer the geomtrical data of the foreland geometry to database
	void transfer_geometry2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database);


	///Output the geometrie of the foreland geometry to display/console
	void output_geometrie(void);

	///Get a pointer to the first point
	Geo_Point* get_ptr_first_point(void);


	///Get the number of segments
	int get_number_segments(void);
	///Get a pointer to a segments specified with an index
	Fpl_Dike_Segment *get_segment(const int index);  

	///Get the total horizontal length
	double get_tot_hori_length(void);
	///Get the total vertical length
	double get_tot_vert_length(void);


private:

	//member
	///Number of points representing the foreland geometry (number_foreland_points  + base point (0/0))
	int number_points;
	///Segments of the foreland geometry 
	Fpl_Dike_Segment *ptr_segments;
	///Number of segments of the foreland geometry
	int number_segments;

	///Total length of the foreland geometry
	double total_length;
	///Horizontal length of the foreland geometry
	double total_horizontal;
	///Vertical length of the foreland geometry
	double total_vertical;


	//methods
	///Allocate the segments of the foreland geometry
	void allocate_segments(void);
	///Delete the segments of the foreland geometry
	void delete_segments(void);

	///Calculate the total length as well as the vertical and horizontal distance 
	void calc_total_length(void);
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
