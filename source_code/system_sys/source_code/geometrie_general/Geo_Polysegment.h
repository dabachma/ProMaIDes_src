#pragma once
/**\class Geo_Polysegment
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef GEO_POLYSEGMENT_H
#define GEO_POLYSEGMENT_H

//system libs
#include "Geo_Segment.h"

//forward declaration
class Geo_Simple_Polygon;
///Geometric class for a polysegment \ingroup sys
/**
A polysegment consists of multiple connected segments.

\see Geo_Segment
*/
class Geo_Polysegment
{
public:
	///Default constructor
	Geo_Polysegment(void);
	///Default destructor
	virtual ~Geo_Polysegment(void);

	//member
	///Segments of the polysegment
	Geo_Segment *my_segment;
	

	//method
	///Set the points of the polysegment and the number of points; the segments are here generated
	void set_segments(const int number, Geo_Point *points);
	///Set number of points
	void set_number_of_points(const int number);
	///Get number of points
	int get_number_of_points(void);

	///Set the points (corresponding to the before set number; the segments are here generated)
	void set_points(Geo_Point *points);
	///Set a closed polysegment with a Geo_Simple_Polygon
	void set_polysegment(Geo_Simple_Polygon *polygon);


	///Get the number of segments
	int get_number_segments(void);
	//Get the index of the point with the nearest point to the given on
	int get_nearest_point_index(Geo_Point *point);
	///Get the total distance of the polysegment (start to end)
	double get_total_distance(void);
	///Get the total horizontal distance (start to end)
	double get_horizontal_distance(void);
	///Get the total vertical distance (start to end)
	double get_vertical_distance(void);
	///Get the distance along a polysegment from the beginning to a given point
	double get_distance_along_polysegment(Geo_Point * check_point);
	///Get a pointer to the last segment
	Geo_Segment *get_last_segment(void);
	///Get a pointer to the last point
	Geo_Point *get_last_point(void);
	///Get a pointer to the first segment
	Geo_Segment *get_first_segment(void);
	///Get a pointer to the first point
	Geo_Point *get_first_point(void);
	///Get a pointer to a segment by a given index
	Geo_Segment *get_segment(const int index);

	///Get the maximal y-value of the cubature
	double get_max_y(void);

	///Calculate the first _geo_interception_point with a Geo_Straight_Line
	_geo_interception_point calc_first_interception(Geo_Straight_Line *my_line);
	///Calculate the last _geo_interception_point with a Geo_Straight_Line
	_geo_interception_point calc_last_interception(Geo_Straight_Line *my_line);

	///Calculate the first _geo_interception_point with a Geo_Segment
	_geo_interception_point calc_first_interception(Geo_Segment *my_line);
	///Calculate the last _geo_interception_point with a Geo_Segment
	_geo_interception_point calc_last_interception(Geo_Segment *my_line);

	///Calculate the interception points with a Geo_Segement and fill the Geo_Interception_Point_List 
    void calculate_segment_interception(Geo_Interception_Point_List *intercept_list, Geo_Segment *check_segment);
	
	///Output the members to display/console
	void output_members(ostringstream *cout);
	
	///Set if the line is closed (true) or not (false)
	void set_is_closed(const bool flag);
	///Get if the line is closed (true) or not (false)
	bool get_is_closed(void);

	///Copy polysegment
	void copy_polysegment(Geo_Polysegment *seg);
	///Get a string for inserting a polysegment to a sql-database 
	string get_polysegment2sql_string(void);
	///Get a string for inserting a line to a sql-database (static)
	static string get_line2sql_string(const double x_coor1, const double y_coor1, const double x_coor2, const double y_coor2);

protected:
	//members
	///Number of points
	int number_points;
	///Number of segments (number of points-1)
	int number_segments;
	///This flag specifies if the line is closed (true) or not (false)
	bool is_closed_flag;

private:

	///Total length of the polysegment
	double total_length;
	///Total horizontal distance of the polysegment
	double total_horizontal;
	///Total vertical distance of the polysegment
	double total_vertical;

	//methods
	///Calculate the distances: total length, vertical and horizontal distances
	void calc_total_length(void);

	///Check the points for generating the poly segment
	void check_points(Geo_Point *points);
	///Check if the polysegemnt is a simple one=> no interception of the segments
	void check_simple_polysegment(void);

	///Allocate the segments
	void allocate_segment(void);
	///Delete the segments
	void delete_segment(void);

	///Set error(s)
	Error set_error(const int err_type);

};
#endif