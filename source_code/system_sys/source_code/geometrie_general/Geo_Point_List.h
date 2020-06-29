#pragma once
/**\class Geo_Point_List
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef GEO_POINT_LIST_H
#define GEO_POINT_LIST_H

//system class
#include "Geo_Point.h"

//forward declaration
class _Geo_Polygon;

///Class for the gathering of geometrical points in a list \ingroup sys
/**
This list is used for store multiple points in a list-schema.

\see Geo_Point
*/
class Geo_Point_List
{
public:
	///Default constructor
	Geo_Point_List(void);
	///Default destructor
	~Geo_Point_List(void);

	//methods
	///Get the number of points
	int get_number_points(void);
	///Get the total point list as a pointer
	Geo_Point* get_total_list(void);
	///Get a specific point with the given index
	Geo_Point get_point(const int index);
	///Get the first point of the list
	Geo_Point get_first_point(void);
	///Get the point with the maximum y-coordinate
	Geo_Point *get_ptr_point_max_y(void);
	///Get the last point of the list
	Geo_Point get_last_point(void);
	///Get a pointer to specific point with the given index
	Geo_Point *get_ptr_point(const int index);

	///Output members
	void output_members(ostringstream *cout);

	///Set a new point
	void set_new_point(Geo_Point *new_point);
	///Set a new points with a polygon
	void set_new_points(_Geo_Polygon *new_points);
	///Set a new points with a polysegment
	void set_new_points(Geo_Polysegment *new_points);

	///Get the index of the point with the smallest y-coordinate in the list
	int get_index_smallest_y(void);
	///Get the the smallest y-coordinate in the list
	double get_smallest_y(void);

	///Delete the list
	void delete_list(void);
	///Reset the list
	void reset_list(void);

	///Sort the point list after the x-coordinates of the points 
	void sort_list_x_coordinates(const bool ascending);

	///Set the flag if double points are allowed
	void set_double_point_allowed(const bool flag);

	///Calculate area
	double calculate_area(void);

private:

	//members
	//Number of points in list
	int number;
	///Points in list
	Geo_Point *points;
	///Number of list-elements in a block for allocation
	const int block_elems;
	///Number of points allocated
	int number_points_alloc;

	///Flag if double points are allowed
	bool double_allowed;

	//methods
	///Delete the allocated points
	void delete_points(void);
	///Allocate the points
	void allocate_points(void);

	///Set error(s)
	Error set_error(const int err_type);


};

#endif