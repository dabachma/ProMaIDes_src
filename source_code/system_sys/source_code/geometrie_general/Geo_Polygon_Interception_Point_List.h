#pragma once
/**\class Geo_Polygon_Interception_Point_List
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010
*/
#ifndef GEO_POLYGON_INTERCEPTION_POINT_LIST_H
#define GEO_POLYGON_INTERCEPTION_POINT_LIST_H

//geo libs
#include "Geo_Polygon_Interception_Point.h"



///List-class for gathering the points of a polygon-polygon interception  \ingroup sys
/**
\see Geo_Polygon_Interception_Point
*/


class Geo_Polygon_Interception_Point_List
{
public:
	///Default constructor
	Geo_Polygon_Interception_Point_List(void);
	///Default destructor
	~Geo_Polygon_Interception_Point_List(void);

	//methods
	///Get the number of points
	int get_number_points(void);
	///Get the total point list as a pointer
	Geo_Polygon_Interception_Point* get_total_list(void);
	///Get a specific point with the given index
	Geo_Polygon_Interception_Point get_point(const int index);
	///Get the first point of the list
	Geo_Polygon_Interception_Point get_first_point(void);
	///Get the last point of the list
	Geo_Polygon_Interception_Point get_last_point(void);
	///Get a pointer to specific point with the given index
	Geo_Polygon_Interception_Point *get_ptr_point(const int index);
	///Get a pointer to the next point with the given index
	Geo_Polygon_Interception_Point *get_ptr_next_point(int *index);


	///Get the number of inside points
	int get_number_inside_points(void);
	///Get the number of at boundary points
	int get_number_atboundary_points(void);
	///Get the number of corresponding points
	int get_number_corresponding_points(void);

	///Output members
	void output_members(ostringstream *cout);

	///Set a new point
	void set_new_point(Geo_Polygon_Interception_Point *new_point);

	///Delete the list
	void delete_list(void);

	///Reset the list
	void reset_list(void);

	///Set the indices for the corresponding points in the other listand vice versa
	void set_corresponding_indices(Geo_Polygon_Interception_Point_List *list);

	///Set the direction of pure interception points
	void set_interception_point_direction(_Geo_Polygon *poly);

	

private:

	//members
	///Number of points in list
	int number;
	///Number of points allocated
	int number_points_alloc;
	///Points in list
	Geo_Polygon_Interception_Point *points;

	///Number of list-elements in a block for allocation
	const int block_elems;



	//methods
	///Delete the allocated points
	void delete_points(void);
	///Allocate the points
	void allocate_points(void);

	///Set error(s)
	Error set_error(const int err_type);
};
#endif