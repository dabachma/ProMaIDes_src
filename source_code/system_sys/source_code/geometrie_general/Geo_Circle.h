#pragma once
/**\class Geo_Circle
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef GEO_CIRCLE_H
#define GEO_CIRCLE_H

//system class
#include "_Geo_Geometrie.h"
#include "Geo_Point.h"
#include "Geo_Point_List.h"

///Geometric class for a circle \ingroup sys
/**
This class is not yet implemented.
*/
class Geo_Circle
{
public:
	///Default constructor
	Geo_Circle(void);
	///Default destructor
	~Geo_Circle(void);


	//methods
	///Set the radius
	void set_radius(const double rad);
	///Get the radius
	double get_radius(void);
	///Set the midpoint
	void set_midpoint(Geo_Point *point);
	///Set the midpoint
	void set_midpoint(const double x_coor, const double y_coor);
	///Get the midpoint
	Geo_Point *get_mid_point(void);

	///Get the area of the circle
	double get_area(void);
	///Get the perimeter of the circle
	double get_perimeter(void);

	///Check if a point is inside a circle
	bool check_point_inside(Geo_Point *point);
	///Check if a point is on a circle
	bool check_point_atboundary(Geo_Point *point);

	///Check an interception between a straight line and a circle; tangent points are not taken into account
	void check_interception2line(Geo_Point_List *list, Geo_Straight_Line *line);
	///Check an interception between a segment and a circle; tangent points are not taken into account
	void check_interception2segment(Geo_Point_List *list, Geo_Segment *segment);
	///Check an interception between a polysegment and a circle; tangent points are not taken into account
	void check_interception2polysegment(Geo_Point_List *list, Geo_Polysegment *poly_segment);
	///Check an interception between a polygon and a circle; tangent points are not taken into account
	void check_interception2polygon(Geo_Point_List *list, Geo_Simple_Polygon *poly);

	///Get the minimum y-coordinate of a point of the circle by a given x-coordinate; if no coordinate is found false is returned
	bool get_y_coordinate_point_at_circle(const double x_coor, double *y_coor);





protected:

	//member
	//Radius of the circle
	double radius;
	//Midpoint of the Circle
	Geo_Point mid_point;

};
#endif