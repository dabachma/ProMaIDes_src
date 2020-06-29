#pragma once
/**\class Geo_Straight_Line
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef GEO_STRAIGHT_LINE_H
#define GEO_STRAIGHT_LINE_H

//system libs
#include "_Geo_Geometrie.h"
#include "Geo_Point.h"
#include "Geo_Interception_Point_List.h"

//forward declaration
class Geo_Point_List;

///Geometric class for a line \ingroup sys
/**
A line consists of two points and is in each direction infinite.

\see Geo_Point
*/
class Geo_Straight_Line: public _Geo_Geometrie
{

public:
	///Default constructor
	Geo_Straight_Line(void);
	///Constructor, where the two line points are given
	Geo_Straight_Line(Geo_Point *point1, Geo_Point *point2);
	///Constructor, where one line point is given; the line is horizontal
	Geo_Straight_Line(Geo_Point *point1);
	///Default destructor
	virtual ~Geo_Straight_Line(void);

	//members
	///First point of the line
	Geo_Point point1;
	///Second point of the line
	Geo_Point point2;

	//methods
	///Set the points of a line
	void set_points(Geo_Point *point1, Geo_Point *point2);
	///Set the coordinates of a line
	void set_coordinates(const double x1, const double y1, const double x2, const double y2);
	///Set a point for a horizontal line
	void set_points(Geo_Point *point1);
	///Set coordinates for a horizontal line
	void set_coordinates(const double x, const double y);
	///Set coordinates for a vertical line
	void set_coordinates(const double x);

	///Set coordinates of one point and the gradient
	void set_coor_grad(const double x, const double y, const double grad);

	///Get the gradient
	double get_gradient(void);
	///Get the y-axis interception value
	double get_y_interception(void);
	///Get the name of the line 
	string get_name(void);
	///Get the pointer to name of the line 
	string* get_ptr_name(void);
	///Get flag if the line is vertical
	bool get_vertical_flag(void);

	///Check if the given point is on the line
	bool calc_point_on_line(Geo_Point *check_point);

	///Calculate and retrieve an interception point with the given line
	void calc_interception(Geo_Straight_Line *interception_line,_geo_interception_point *my_interception);

	///Calculate the interception with a square-root function (c1-(x-c3)*c2)^0.5+c4
	void calc_interception_square_root(Geo_Point_List *list, const double c1, const double c2, const double c3, const double c4);

	///Move the line for a given distance in x-direction (positiv distance is to the right)
	void move_to_x_direction(const double distance);
	///Move the line for a given distance in y-direction (positiv distance is upwards)
	void move_to_y_direction(const double distance);

	///Output the members to display/console
	void output_members(ostringstream *cout);

	///Copy operator 
	Geo_Straight_Line& operator=(const Geo_Straight_Line& object);

protected:
	//members
	
	///Line gradient
	double gradient;
	///y-axis interception value
	double y_interception;
	///Flag if the line is vertical
	bool vertical_flag;
	///Name of the line
	string name;

	//methods
	///Calculate the gradient of the line
	void calc_gradient(void);
	///Calculate the y-axis interception value
	void calc_y_interception(void);
};
#endif