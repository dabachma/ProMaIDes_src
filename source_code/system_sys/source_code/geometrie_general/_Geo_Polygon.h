#pragma once
/**\class _Geo_Polygon
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _GEO_POLYGON_H
#define _GEO_POLYGON_H


//system libs
#include "Geo_Segment.h"
#include "Geo_Raster_Segment.h"
#include "Geo_Ray.h"
#include "Geo_Interception_Point_List.h"
#include "Geo_Polysegment.h"
#include "Geo_Point_List.h"
#include "Geo_Polygon_Interception_Point_List.h"


///Enumarator for an interception between two polygons
enum _polygon_intercept{
	///One polygon is completly inside the other
	complete_inside,
	///One polygon is completly inside the other, but tangent the boundary
	inside_with_boundary,
	///One polygon is completly outside the other
	complete_outside,
	///One polygon is completly outside the other, but tangent the boundary
	outside_with_boundary,
	///The two polygons are partly intercepting each other
	partly_intercepted
};



///Virtual geometric class for a polygon.  \ingroup sys
/**
A polygon consits of segments. One child-class is the simple polygon, which is already implemented.
Another child-class could be convex polygons etc.

\see Geo_Simple_Polygon, Geo_Segment
*/
class _Geo_Polygon
{
public:
	///Default constructor
	_Geo_Polygon(void);

	///Default destructor
	virtual ~_Geo_Polygon(void);

	//member


	//methods
	///Get the number of segments of the polygon
	int get_number_segments(void);
	///Get the number of points
	int get_number_points(void);
	///Get a segments of the polygon with the given index
	virtual Geo_Segment* get_segment(const int index)=0;
	///Get a point of the polygon with the given index
	virtual Geo_Point* get_point(const int index)=0;
	///Calculate the area of the polygon
	double calculate_area(void);
	///Calculate the perimeter of the polygon
	double calculate_perimeter(void);

	///Check if a point is inside the polygon
	bool check_point_inside(Geo_Point *check_point);
	//Check if a point is inside or at boundary the polygon
	bool check_point_inside_atboundary(Geo_Point *check_point);
	///Check if a point is at the polygon boundary polygon
	bool check_point_atboundary(Geo_Point *check_point);
	///Check if a point is completly outside the polygon
	bool check_point_outside(Geo_Point *check_point);
	///Check if a segment is at the boundary of the polygon
	bool check_segment_at_boundary(Geo_Segment *check_segment);
	///Check if a segment is completly inside or at boundary
	bool check_segment_is_inside(Geo_Segment *check_segment);
	///Check if a segment is completly inside or at boundary
	bool check_segment_is_inside(Geo_Raster_Segment *check_segment);
	///Check if a polygon is completly inside, outside ot partly intercepted
	_polygon_intercept check_polygon_interception(_Geo_Polygon *check_polygon);
	///Calculate the interception points with a Geo_Segment and fill the Geo_Interception_Point_List 
	void calculate_segment_interception(Geo_Interception_Point_List *intercept_list, Geo_Segment *check_segment);
	///Calculate the interception points with a Geo_Segment and fill the Geo_Interception_Point_List 
	void calculate_segment_interception(Geo_Interception_Point_List *intercept_list, Geo_Raster_Segment *check_segment);

	///Calculate the interception points with a Geo_Poly_Segment and fill the Geo_Interception_Point_List 
	void calculate_polysegment_interception(Geo_Interception_Point_List *intercept_list, Geo_Polysegment *check_polysegment);
	///Check if a polysegement is partly inside a polygon 
	bool check_polysegment_partly_inside(Geo_Polysegment *check_polysegment);

	///Calculate the common area of two polygons with the Weiler-Atherton algorithm
	double polygon_interception(_Geo_Polygon *interception);

	///Calculate the common area of two polygons with the Weiler-Atherton algorithm
	double polygon_interception(_Geo_Polygon *interception, Geo_Polygon_Interception_Point_List *poly1, Geo_Polygon_Interception_Point_List *poly2, Geo_Interception_Point_List *buffer_list, Geo_Point_List *polygon_list, Geo_Polygon_Interception_Point *buffer);

	///Calculate the length of segment, which is in a polygon
	double segment_interception(Geo_Segment *seg, Geo_Interception_Point_List *list);

	///Convert the enum _polygon_intercept into string
	static string convert_ploygon_intercept2txt(_polygon_intercept given_enum);
	///Get a point wich is inside a polgon
	bool get_point_inside(Geo_Point *point_inside);

	///Get a string for inserting a polygon data to a sql-database
	string get_polygon2sql_string(void);

	///Clear polygon, the segments of the polygon are deleted
	void clear_polygon(void);

	///Output point members to display/console
	void output_point_members(ostringstream *cout);

protected:


	//members
	///Number of segments for this polygon
	int number_segments;
	///Segments of the polygon
	Geo_Segment *segments;

	//methods
	///Allocate the segments
	void allocate_segments(void);
	///Delete the segments
	void delete_segments(void);
	///Check the points if there are same sequentiell points
	void check_points(Geo_Point *points);

private:

	//members
	///Flag if segments are allocated in this class
	bool segment_alloc_here;


	//methods
	///Check an intersection with another polygon (just true or false)
	bool check_interception(_Geo_Polygon *check_polygon);

	///Set error(s)
	Error set_error(const int err_type);

};

#endif