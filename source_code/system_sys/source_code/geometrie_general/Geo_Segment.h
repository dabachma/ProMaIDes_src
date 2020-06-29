#pragma once
/**\class Geo_Segment
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef GEO_SEGMENT_H
#define GEO_SEGMENT_H

//system classes
#include "Geo_Straight_Line.h" 
#include "Geo_Raster_Segment.h"


///Geometric class for a segment \ingroup sys
/**
A segment consists of two points and is in both directions restricted 

\see Geo_Point, Geo_Straight_Line, Geo_Polysegment
*/
class Geo_Segment : public Geo_Straight_Line
{
public:
	///Default constructor
	Geo_Segment(void);
	///Copy constructor
	Geo_Segment(const Geo_Segment& object);
	///Constructor where the two points are given (start- and end-point)
	Geo_Segment(Geo_Point *point1, Geo_Point *point2);
	///Default destructor
	virtual ~Geo_Segment(void);

	//methods
	///Set the segment with two given points (start- and end-point)
	void set_points(Geo_Point *point1, Geo_Point *point2);
	///Set the segment with the given coordinates of the start- and end-point
	void set_coordinates(const double x1, const double y1, const double x2, const double y2);
	///Set the segement with a Geo_Raster_Segement
	void set_segment(Geo_Raster_Segment *rast_seg);
	///Get the horizontal distance of the segment
	double get_horizontal_dist(void);
	///Get the vertical distance of the segment
	double get_vertical_dist(void);
	///Get the total distance of the segment
	double get_distance(void);
	///Get the distance along a segment from the beginning to a given point
	double get_distance_along_segment(Geo_Point * check_point);

	///Refresh the distances 
	void refresh_distances(void);


	///Calculate if a given interception point is on the segment
	bool calc_interception_point_on_segment(Geo_Point *check_point);
	///Calculate if a given point is on the segment
	bool calc_point_on_segment(Geo_Point *check_point);
	///Calculate and retrieve an interception point with a given straight line
	void calc_interception(Geo_Straight_Line *interception_line, _geo_interception_point *my_interception);
	///Calculate and retrieve an interception point with a given segment
	void calc_interception(Geo_Segment *interception_segment, _geo_interception_point *my_interception);
	///Get the mid point of the segment
	Geo_Point calc_mid_point(void); 

	///Calculate the interception with a square-root function (c1-(x-c3)*c2)^0.5+c4
	void calc_interception_square_root(Geo_Point_List *list, const double c1, const double c2, const double c3, const double c4);


	///Move the segment for a given distance in x-direction (positiv distance is to the right)
	void move_to_x_direction(const double distance);
	///Move the segment for a given distance in y-direction (positiv distance is upwards)
	void move_to_y_direction(const double distance);


	///Get a list of additional points on the segement specified by a given number
	void get_additional_points_on_segment(Geo_Point_List *list, const int number);
	///Get a list of additional points on the segement specified by a given distance
	void get_additional_points_on_segment(Geo_Point_List *list, const double interval);

	///Switch the points (point1->point2 v.v.)
	void switch_points(void);

	///Output members to display/console
	void output_members(ostringstream *cout);
	///Output point members to display/console
	void output_point_members(ostringstream *cout);

	///Copy operator
	Geo_Segment& operator=(const Geo_Segment& object);
	///Compare operator ==
	bool operator==(const Geo_Segment& object);
	///Compare operator !=
	bool operator!=(const Geo_Segment& object);

protected:
	//members
	///Horizontal distance of the segment
	double horizontal_dist;
	///Vertical distance of the segment
	double vertical_dist;
	///Total distance of the segment
	double distance;
	
	//methods

	///Calculate the horizontal distance of the segment
	void calc_horizontal_dist(void);
	///Calculate the vertical distance of the segment
	void calc_vertical_dist(void);
	///Calculate the total distance of the segment
	void calc_distance(void);
	
};
#endif