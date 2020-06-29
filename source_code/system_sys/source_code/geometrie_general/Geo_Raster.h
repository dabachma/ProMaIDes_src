#pragma once
/**\class Geo_Raster
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef GEO_RASTER_H
#define GEO_RASTER_H

//system sys-geo libs
#include "Geo_Raster_Polygon.h"
#include "Geo_Convex_Polygon.h"

///Geometrie class for a geometrical rectangular raster \ingroup sys
/**

*/
class Geo_Raster
{
public:
	///Default constructor
	Geo_Raster(void);
	///Default destructor
	virtual ~Geo_Raster(void);

	//members
	///Geometrical boundary of the raster
	Geo_Convex_Polygon geometrical_bound;
	///Geometrical element as a raster polygon 
	Geo_Raster_Polygon raster_elem;

	//methods
	///Initialize the raster: the points and segments
	virtual void init_raster(void);
	///Delete the raster information like points and segments
	virtual void clear_raster(void);
	///Set a geometrical raster polygon with a given element index
	void set_geometrical_raster_polygon(const int element_index);
	///Get the number of elements
	int get_number_elements(void);

	///Get the area of a raster element
	double get_area(void);

protected:

	//members
	///Number of rasterpolygons = number elements
	int number_polygons;
	///Number of rasterpoints
	int number_points;
	///Number of segments
	int number_segments;

	///Number of elements in x-direction
	int no_elems_x;
	///Number of elements in y-direction
	int no_elems_y;

	///Width of the elements in x-direction
	double width_x;
	///Width of the elements in y-direction
	double width_y;
	///Area of the element
	double area;

	///Coordinate x of the origin
	double x_coor_origin;
	///Coordinate y of the origin
	double y_coor_origin;

	///Pointer to the points of the raster
	Geo_Point *raster_points;
	///Pointer to the segments of the raster
	Geo_Raster_Segment *raster_segments;

	//methods
	///Generate the raster points and -segments
	void generate_points_segments(void);
	///Generate the geometrical boundary
	void generate_boundary(void);

	///Allocate the raster points
	void allocate_raster_points(void);
	///Delete the raster points
	void delete_raster_points(void);

	///Allocate the raster points
	void allocate_raster_segments(void);
	///Delete the raster points
	void delete_raster_segments(void);

	///Check the geometrical information of the raster
	void check_raster_info(void);

private:

	//method
	///Set error(s)
	Error set_error(const int err_type);

};
#endif
