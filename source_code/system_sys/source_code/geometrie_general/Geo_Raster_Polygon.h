#pragma once
/**\class Geo_Raster_Polygon
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef GEO_RASTER_POLYGON_H
#define GEO_RASTER_POLYGON_H

//system sys-geo libs
#include "_Geo_Polygon.h"
#include "Geo_Raster_Segment.h"

///Geometrie class for a general polygon (rectangle) of a geometrical raster \ingroup sys
/**

*/
class Geo_Raster_Polygon : public _Geo_Polygon
{
public:
	///Default constructor
	Geo_Raster_Polygon(void);
	///Default destructor
	virtual ~Geo_Raster_Polygon(void);

	//methods
	///Get a segments of the polygon with the given index (index from 0-3)
	Geo_Segment* get_segment(const int index);
	///Get a point of the polygon with the given index (index from 0-3)
	Geo_Point* get_point(const int index);
	///Set the points with a given mid_point, x-width, y-width and angle of the coordinate system
	void set_points(Geo_Point *mid_point, const double x_width, const double y_width, const double angle, const double x_origin=0.0, const double y_origin=0.0);
	///Set the polygon's segments
	void set_polygon_segments(Geo_Segment *segment_y, Geo_Segment *segment_x, Geo_Segment *segment_minus_y, Geo_Segment *segment_minus_x);
	///Set the polygon's segments
	void set_polygon_segments(Geo_Raster_Segment *segment_y, Geo_Raster_Segment *segment_x, Geo_Raster_Segment *segment_minus_y, Geo_Raster_Segment *segment_minus_x);

	///Get the midpoint of the element
	Geo_Point get_mid_point(void);


private:

	///Sort the points in one direction
	void sort_points(void);
	///Set error(s)
	Error set_error(const int err_type);


};

#endif