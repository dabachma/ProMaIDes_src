#pragma once
/**\class Geo_Convex_Polygon
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010
*/
#ifndef GEO_CONVEX_POLYGON_H
#define GEO_CONVEX_POLYGON_H

//geo libs
#include "Geo_Simple_Polygon.h"



///Geometric class for a convex polygon.  \ingroup sys
/**
A convex polygon is a simple polygon, whose interior angles are all smaller than 180°. It consits of segments.
\see Geo_Point, Geo_Segment, Geo_Simple_Polygon
*/

class Geo_Convex_Polygon : public Geo_Simple_Polygon
{
public:
	///Default constructor
	Geo_Convex_Polygon(void);
	///Default destructor
	virtual ~Geo_Convex_Polygon(void);

	///Check if a given convex polygon is completly inside this convex polygon
	bool check_polygon_inside(_Geo_Polygon *check_polygon);
};
#endif
