#pragma once
/**\class Geo_Polygon_Interception_Point
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010
*/
#ifndef GEO_POLYGON_INTERCEPTION_POINT_H
#define GEO_POLYGON_INTERCEPTION_POINT_H

//geo libs
#include "Geo_Point.h"



///Geometric point class for special points by a polygon-polygon interception.  \ingroup sys
/**
This class is needed by implementing the Weiler-Atherton algorithm for determine the
intersection og two polygons. Two lists are generated for each polygon, which
includes the polygon points theirself and the interception points of the two polygons.

\see Geo_Point, Geo_Polygon_Interception_Point_List
*/

class Geo_Polygon_Interception_Point : public Geo_Point
{
public:
	///Default constructor
	Geo_Polygon_Interception_Point(void);
	///Copy constructor
	Geo_Polygon_Interception_Point(const Geo_Polygon_Interception_Point& object);
	///destructor
	~Geo_Polygon_Interception_Point(void);

	//methods

	///Get if the point is a polygon point
	bool get_polygon_point_flag(void);
	///Set, that the point is a polygon point
	void set_polygon_point_flag(const bool flag);
	///Get if the point is a interception point 
	bool get_interception_point_flag(void);
	///Set, that the point is a interception point
	void set_interception_point_flag(const bool flag);

	///Get if the point lies inside for a pure polygon point or the segment goes inside for a interception point
	bool get_inside_flag(void);
	///Set that the point lies inside for a pure polygon point or the segment goes inside for a interception point
	void set_inside_flag(const bool flag);

	///Get the index to the correspondig point of the second list
	int get_index_corresponding_point(void);
	///Set the index to the correspondig point of the second list
	void set_index_corresponding_point(const int index);

	///Set the geometrical point data
	void set_geometrical_point_data(Geo_Point *point);
	///Reset the flags
	void reset_flags(void);

	///Get is used flag
	bool get_is_used_flag(void);
	///Set is used flag
	void set_is_used_flag(const bool is_used);

	///Copy operator
	Geo_Polygon_Interception_Point& operator=(const Geo_Polygon_Interception_Point& object);

private:

	//members
	///Flag, if the point is a polygon point
	bool polygon_point;
	///Flag if the point is a interception point
	bool interception_point;
	///Flag if the point lies inside for a pure polygon point or the segment goes inside for a interception point
	bool inside_flag;


	///Flag if the point is already used for a polygon
	bool is_used;

	///Index to the correspondig point of the second list
	int index_other_list;


};
#endif
