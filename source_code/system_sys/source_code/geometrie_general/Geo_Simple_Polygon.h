#pragma once
/**\class Geo_Simple_Polygon
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef GEO_SIMPLE_POLYGON_H
#define GEO_SIMPLE_POLYGON_H

//geo libs
#include "_Geo_Polygon.h"



///Geometric class for a simple polygon.  \ingroup sys
/**
A simple polygon is a polygon, whose sides do not cross. It consits of segments.
\see Geo_Point, Geo_Segment
*/
class Geo_Simple_Polygon : public _Geo_Polygon
{
public:
	///Default constructor
	Geo_Simple_Polygon(void);
	///Default destructor
	virtual ~Geo_Simple_Polygon(void);

	//methods
	///Set the number of points of the polygon
	void set_number_points(const int number);
	///Set the points of the polygon
	void set_points(Geo_Point *points, const bool check = true);
	///Set the points of the polygon
	void set_points(Geo_Point_List *point_list);
	///Get a segments of the polygon with the given index
	Geo_Segment* get_segment(const int index);
	///Get a point of the polygon with the given index
	Geo_Point* get_point(const int index);

	///Check if points are clockwise (true) or counter clockwise (false) sorted
	bool check_clockwise(void);

	///Get mid-point of the polagon
	Geo_Point get_mid_point(void);

	///Change point coordinates by a given point index
	void change_point_coordinates(const int index, const double x, const double y);

	///Set a simple polygon with a given polysegment
	void set_polygon(Geo_Polysegment *poly_seg);

	///Move the polygon for a given distance in x-direction (positiv distance is to the right)
	void move_to_x_direction(const double distance);
	///Move the polygon for a given distance in y-direction (positiv distance is upwards)
	void move_to_y_direction(const double distance);


	///Check if the polygon is a simple one=> no interception of the segments
	void check_simple_polygon(void);

private:
	//member

	//method
	///Set error(s)
	Error set_error(const int err_type);


};
#endif