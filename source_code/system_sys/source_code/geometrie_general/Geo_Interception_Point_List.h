#pragma once
/**\class Geo_Interception_Point_List
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef GEO_INTERCEPTION_POINT_LIST_H
#define GEO_INTERCEPTION_POINT_LIST_H

//geo_classes
#include "Geo_Point.h"

//struct
///Datastructure for an interception point \ingroup sys
struct _geo_interception_point{
	///Flag if there was an interception
	bool interception_flag;
	///Flag if the interception is indefinite
	bool indefinite_flag;
	///Point of interception
	Geo_Point interception_point;
	///Name of the first object for interception
	string name_1;
	///Name of the second object of interception
	string name_2;
};
///Datastructure for an interception point for interception between polysegments and/or polygons \ingroup sys
struct _geo_multisegment_interception{
	///Interception point \see _geo_interception_point
	_geo_interception_point line_interception;
	///Index of the segment of the geometrical structure which intercepts
	int index_intercepts;
	///The distance along the structure which intercepts between their beginning and the interception point
	double distance;
	///Index of the segment of the geometrical structure which is intercepted
	int index_is_intercept;
	///Point index if a point of the the geometrical structure which is intercepted is hitten (-1 => no point hit)
	int index_point_is_intercepted;
};
///Class for the gathering of geometrical interception points in a list \ingroup sys
/**
This list is used for the interception between line and polyline, where
multiple interception points are possible. This list gathers this points.

\see Geo_Point
*/
class Geo_Interception_Point_List
{
public:
	///Default constructor
	Geo_Interception_Point_List(void);
	///Default destructor
	~Geo_Interception_Point_List(void);

	//methods
	///Get the number of interception_points
	int get_number_points(void);
	///Get the total point list as a pointer
	_geo_multisegment_interception* get_total_list(void);
	///Get a specific point with the given index
	_geo_multisegment_interception get_point(const int index);
	///Change the Index of the geometrical structure, which is intercepted for a given point
	void change_index_is_intercepted(const int index, const int is_intercepted_index);
	//Change the index of a point if one is hit for the structure which is intercepted
	void change_point_index_is_intercepted(const int index, const int is_intercepted_point_index);
	
	///Set a new point (no indefinite interception are taken; it is a point list)
	void set_new_point(_geo_multisegment_interception *new_interception_point);
	///Sort the interception points after the distance to the beginning of the structure which intercepts
	void sort_distance(void);
	///Output members
	void output_members(void);
	///Get the first point of the list
	_geo_multisegment_interception get_first_point(void);
	///Get the last point of the list
	_geo_multisegment_interception get_last_point(void);
	///Delete the list
	void delete_list(void);
	///Reset the list
	void reset_list(void);

	///Get the maximum y-coordinate
	double get_max_y_coordinate(void);
	///Get the minimum y-coordinate
	double get_min_y_coordinate(void);


private:

	//members
	///Number of interception
	int number_interception;
	///Number of points allocated
	int number_points_alloc;

	///Number of list-elements in a block for allocation
	const int block_elems;

	///List of interception points 
	_geo_multisegment_interception *intercept_points;

	//methods
	///Delete the allocated interception points
	void delete_points(void);
	///Allocate the points
	void allocate_points(void);

	///Set error(s)
	Error set_error(const int err_type);
};

#endif