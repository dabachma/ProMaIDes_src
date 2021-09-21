#pragma once
/**\class Geo_Point
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef GEO_POINT_H
#define GEO_POINT_H
//libs

//system class
#include "_Geo_Geometrie.h"

///Geometric class for a point \ingroup sys
/**
A point consists of a x- and y-coordinate. A name can also be set.
It is the base-class for all other geomertrical classes, e.g. line, ray, segment, polygon etc.
*/
class Geo_Point : public _Geo_Geometrie
{
public:
	///Default constructor
	Geo_Point(void);
	///Copy constructor
	Geo_Point(const Geo_Point& object);
	///Constructor where all information are given
	Geo_Point(const double x_coor, const double y_coor, const string name);
	///Constructor where just the pointname is given
	Geo_Point(const string name);
	///Default destructor
	~Geo_Point(void);

	//members
	
	//methods
	///Set the name of the point
	void set_point_name(const string name);
	///Get the point name
	string get_point_name(void);
	///Get the pointer to the point name
	string* get_ptr_point_name(void);
	///Set the coordinates of the point
	void set_point_coordinate(const double x_coor, const double y_coor);
	///Set the x-coordinate of the point
	void set_point_x_coordinate(const double x_coor);
	///Set the y-coordinate of the point
	void set_point_y_coordinate(const double y_coor);
	///Set the coordinates and the name of the point
	void set_point(const double x_coor, const double y_coor, const string name);
	///Set the point by a given point
	void set_point(Geo_Point *point);
	///set the infos of the point
	void set_point(Geo_Point point);
	///Get the x-coordinate
	double get_xcoordinate(void);
	///Get the y-coordinate
	double get_ycoordinate(void);
	///Get the point number
	int get_number(void);
	///Set the point number
	void set_number(const int id);
	///Add values to the point coordinates
	void add_values2coordinate(const double x, const double y);
	
	///Calculate the horizontal distance between two points (a pointer is given)
	double horizontal_distance(Geo_Point *point);
	///Calculate the horizontal distance between two points
	double horizontal_distance(Geo_Point point);
	///Calculate the vertical distance between two points (a pointer is given)
	double vertical_distance(Geo_Point *point);
	///Calculate the vertical distance between two points
	double vertical_distance(Geo_Point point);
	///Calculate the direct distance between two points
	double distance(Geo_Point *point);
	///Get the nearest point of two given points
	int get_nearest_point(Geo_Point *point_1, Geo_Point *point_2);
	///Calculate the angle to y-axis, if the two points are at one staight line
	double direction_yaxsis(Geo_Point *point);
	///Calculate the angle to x-axis, if the two points are at one staight line
	double direction_xaxsis(Geo_Point *point);



	///Transform a point with a given angle of the coordinate system (counterclockwise is positive angle)
	Geo_Point transform_coordinate_system(const double angle);
	///Transform a point with a given angle, x and y origin of the coordinate system (counterclockwise is positive angle)
	Geo_Point transform_coordinate_system(const double angle, const double x_origin, const double y_origin);

	///Check if this point is left of the given one
	bool check_left_of(Geo_Point *point);
	///Check if this point is right of the given one
	bool check_right_of(Geo_Point *point);
	///Check if this point is below the given one
	bool check_below_of(Geo_Point *point);
	///Check if this point is above the given one
	bool check_above_of(Geo_Point *point);

	///Move the point for a given distance in x-direction (positiv distance is to the right)
	void move_to_x_direction(const double distance);
	///Move the point for a given distance in y-direction (positiv distance is upwards)
	void move_to_y_direction(const double distance);
	
	///Output the members to display/console
	virtual void output_members(ostringstream *cout);
	//Output the coordinate members to display/console
	void output_coor_members(ostringstream *cout);


	///Get a string for inserting a point to a sql-database
	static string get_point2sql_string(const double x_coor, const double y_coor);

	///Check same point
	bool check_same_point(Geo_Point *point);

	///Copy operator
	Geo_Point& operator=(const Geo_Point& object);
	///Compare operator ==
	bool operator==(const Geo_Point& object);
	///Compare operator !=
	bool operator!=(const Geo_Point& object);


protected:
	//members (add members: think to the =operator!)
	///x-coordinate of the point
	double x_coordinate;
	///y-coordinate of the point
	double y_coordinate;
	///Point name
	string name;
	///Point number
	int number;

	///buffer sum
	double summe;
	
private:



	//method
	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif