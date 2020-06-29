#pragma once
/**\class Geo_Raster_Segment
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2012
*/
#ifndef GEO_RASTER_SEGMENT_H
#define GEO_RASTER_SEGMENT_H

//system sys-geo libs
#include "Geo_Raster_Straight_Line.h"

//forward
class Geo_Straight_Line;
class Geo_Segment;
///Geometrie class for a segement of a raster element \ingroup sys
/**

*/

class Geo_Raster_Segment : public Geo_Raster_Straight_Line
{
public:

	///Default constructor
	Geo_Raster_Segment(void);
	///Copy constructor
	Geo_Raster_Segment(const Geo_Raster_Segment& object);
	///Constructor where the two points are given (start- and end-point)
	Geo_Raster_Segment(Geo_Point *point1, Geo_Point *point2);
	///Default destructor
	virtual ~Geo_Raster_Segment(void);

	//methods
	///Set the segment with two given points (start- and end-point)
	void set_points(Geo_Point *point1, Geo_Point *point2);
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
	void calc_interception(Geo_Raster_Straight_Line *interception_line, _geo_interception_point *my_interception);
	///Calculate and retrieve an interception point with a given segment
	void calc_interception(Geo_Raster_Segment *interception_segment, _geo_interception_point *my_interception);
	///Calculate and retrieve an interception point with a given straight line
	void calc_interception(Geo_Straight_Line *interception_line, _geo_interception_point *my_interception);
	///Calculate and retrieve an interception point with a given segment
	void calc_interception(Geo_Segment *interception_segment, _geo_interception_point *my_interception);

	///Get the mid point of the segment
	Geo_Point calc_mid_point(void); 


	///Switch the points (point1->point2 v.v.)
	void switch_points(void);

	///Output members to display/console
	void output_members(ostringstream *cout);
	///Output point members to display/console
	void output_point_members(ostringstream *cout);

	///Copy operator
	Geo_Raster_Segment& operator=(const Geo_Raster_Segment& object);
	///Compare operator ==
	bool operator==(const Geo_Raster_Segment& object);
	///Compare operator !=
	bool operator!=(const Geo_Raster_Segment& object);

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
