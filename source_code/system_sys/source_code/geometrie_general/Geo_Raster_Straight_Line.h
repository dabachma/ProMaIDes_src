#pragma once
/**\class Geo_Raster_Straight_Line
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2012
*/
#ifndef GEO_RASTER_STRAIGHT_LINE_H
#define GEO_RASTER_STRAIGHT_LINE_H

//system sys-geo libs
#include "_Geo_Geometrie.h"
#include "Geo_Point.h"

#include "Geo_Interception_Point_List.h"

//forward declaration
class Geo_Point_List;

///Geometrie class for a raster straight line \ingroup sys
/**
It is the same class than the Geo_Straight_Line, just the points are pointers.
*/

class Geo_Raster_Straight_Line : public _Geo_Geometrie
{
public:
	///Default constructor
	Geo_Raster_Straight_Line(void);
	///Constructor, where the two line points are given
	Geo_Raster_Straight_Line(Geo_Point *point1, Geo_Point *point2);
	///Default destructor
	virtual ~Geo_Raster_Straight_Line(void);

	//members
	///First point of the line
	Geo_Point *point1;
	///Second point of the line
	Geo_Point *point2;

	//methods
	///Set the points of a line
	void set_points(Geo_Point *point1, Geo_Point *point2);


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
	void calc_interception(Geo_Raster_Straight_Line *interception_line,_geo_interception_point *my_interception);


	///Output the members to display/console
	void output_members(ostringstream *cout);

	///Copy operator 
	Geo_Raster_Straight_Line& operator=(const Geo_Raster_Straight_Line& object);

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
