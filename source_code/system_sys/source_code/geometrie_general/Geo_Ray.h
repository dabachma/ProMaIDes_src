#pragma once
/**\class Geo_Ray
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef GEO_RAY_H
#define GEO_RAY_H

//system libs
#include "Geo_Straight_Line.h" 
#include "Geo_Segment.h"

///Geometric class for a ray \ingroup sys
/**
A ray consists of two points and is in one direction infinite.

\see Geo_Point
*/
class Geo_Ray : public Geo_Straight_Line
{
public:
	///Default constructor
	Geo_Ray(void);
	///Default destructor
	~Geo_Ray(void);


	//methods
	///Calculate if a given interception point is on the ray
	bool calc_interception_point_on_ray(Geo_Point *check_point);
	///Calculate if a given point is on the ray
	bool calc_point_on_ray(Geo_Point *check_point);
	///Get and calculate an interceptionpoint with a given segment
	void calc_interception(Geo_Segment *interception_segment,  _geo_interception_point *my_interception);
};
#endif