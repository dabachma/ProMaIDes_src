#pragma once
/**\class Hyd_Floodplainraster_Polygon
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_FLOODPLAINRASTER_POLYGON_H
#define HYD_FLOODPLAINRASTER_POLYGON_H

//hyd-system
#include "Hyd_Element_Floodplain.h"
#include "Hyd_Floodplainraster_Segment.h"
#include "Hyd_Floodplain_Polygon.h"
#include "Hyd_Floodplain_Polysegment.h"

//sys-system 
//(base class)
#include "Geo_Raster_Polygon.h" 

///Geometrical polygon of a floodplain raster \ingroup hyd
/**
This polygons of the raster are always rectangular. They are not stored
in the floodplain raster. They are always generated, than the gemetrical action is performed, e.g. interception
or point-inside check and than again deleted. They are generated with the help of tzhe raster segments.

\see Hyd_Floodplain_Raster, Hyd_Floodplainraster_Segment
*/
class Hyd_Floodplainraster_Polygon: public Geo_Raster_Polygon
{
public:
	///Default constructor
	Hyd_Floodplainraster_Polygon(void);
	///Default destructor
	~Hyd_Floodplainraster_Polygon(void);

	//methods
	///Set the pointer to the floodplain element
	void set_ptr2floodplain_element(Hyd_Element_Floodplain *ptr2elem);

	///Set the polygon's segments
	void set_polygon_segments(Hyd_Floodplainraster_Segment *segment_y, Hyd_Floodplainraster_Segment *segment_x, Hyd_Floodplainraster_Segment *segment_minus_y, Hyd_Floodplainraster_Segment *segment_minus_x);

	///Set the midpoint of the polygon to the given pointer
	void set_mid_point(Geo_Point *mid);

	///Check if this raster polygon is inside the given Hyd_Floodplain_Polygon and set element type of the given polygon
	void is_inside_polygon(Hyd_Floodplain_Polygon *polygon);
	///Check if this raster polygon is intercepted by the given Hyd_Floodplain_Polygon and set the given element type 
	void is_intercepted_polygon(Hyd_Floodplain_Polygon *polygon, _hyd_elem_type type);

	///Calculate the interception points with a Geo_Poly_Segment and fill the Geo_Interception_Point_List 
	void calculate_polysegment_interception(Geo_Interception_Point_List *intercept_list, Geo_Polysegment *check_polysegment);
	
	///Assign dikeline values to the raster
	void assign_values2raster(Hyd_Floodplain_Polysegment *dikeline);

	///Output the members
	void output_members(ostringstream *cout);
	///Output the header for an output (just once)
	void output_header(ostringstream *cout);

	///Get is-outside flag
	bool get_is_outside_flag(void);

	///Set is-outside flag
	void set_is_outside_flag(const bool flag);



private:
	//members
	///Pointer to a flood
	Hyd_Element_Floodplain *ptr2floodplain_elem;
	///Pointer to the segments
	Hyd_Floodplainraster_Segment *segments;

	///Is-outside flag of something, like another polygon. Use it as information-flag
	bool is_ouside;

	//method
	///Sort the points
	void sort_points(void);


};
#endif
