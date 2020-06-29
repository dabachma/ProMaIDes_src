#pragma once
/**\class Geo_Complex_Polygon
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/
#ifndef GEO_COMPLEX_POLYGON_H
#define GEO_COMPLEX_POLYGON_H

//geo libs
#include "Geo_Simple_Polygon.h"



///Geometric class for a complex polygon.  \ingroup sys
/**
A simple polygon is a polygon, whose sides can cross each other. It consits of segments.
\see Geo_Simple_Polygon, Geo_Point, Geo_Segment
*/


class Geo_Complex_Polygon : public Geo_Simple_Polygon
{
public:
	///Default constructor
	Geo_Complex_Polygon(void);
	///Default destructor
	~Geo_Complex_Polygon(void);




	//methods
	///Set the points of the polygon
	void set_points(Geo_Point *points);
	///Set the points of the polygon
	void set_points(Geo_Point_List *point_list);




};
#endif
