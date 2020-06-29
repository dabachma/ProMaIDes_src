#pragma once
/**\class Fpl_Seepage_Line_Point
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/
#ifndef FPL_SEEPAGE_LINE_POINT_H
#define FPL_SEEPAGE_LINE_POINT_H

//system sys

//system fpl





///Class for a relevant point of the seepage line \ingroup fpl
/**
This points are caracterized by a x-coordinate relative to the waterside toe of the structure and
a y-coordinate as a waterlevel of the seepage line. This waterlevel is also relative to
the waterside toe of the structure. 

*/
class Fpl_Seepage_Line_Point
{
public:
	///Default constructor
	Fpl_Seepage_Line_Point(void);
	///Copy constructor
	Fpl_Seepage_Line_Point(const Fpl_Seepage_Line_Point& object);
	///Default destructor
	~Fpl_Seepage_Line_Point(void);

	///Set the x-coordinate
	void set_x_coordinate(const double x);
	///Set the waterlevel 
	void set_waterlevel(const double h);
	///Get the x_coordinate
	double get_x_coordinate(void);
	///Get the waterlevel
	double get_waterlevel(void);

	///Set the flag, if the point is inside the dike body
	void set_inside_dike_body_flag(const bool flag);
	///Get the flag, if the point is inside the dike body
	bool get_inside_dike_body_flag(void);

	///Get the y-coordinate of the cubature
	double get_y_coordinate_cubature(void);
	///Set the y-coordinate of the cubature
	void set_y_coordinate_cubature(Geo_Polysegment *cubature);

	///Copy operator
	Fpl_Seepage_Line_Point& operator=(const Fpl_Seepage_Line_Point& object);

private:
	//members
	///x-coordinate of the point relative to the waterside toe of the structure (0/0)
	double x_coordinate;
	///Waterlevel of the seepage line relative to the waterside toe of the structure (0/0)
	double waterlevel;

	///Set y-coordinate of the cubature
	double y_cubature;

	///Flag if the point is inside the dike body
	bool inside_dike_body;

	///Set warning(s)
	Warning set_warning(const int warn_type);

};

#endif