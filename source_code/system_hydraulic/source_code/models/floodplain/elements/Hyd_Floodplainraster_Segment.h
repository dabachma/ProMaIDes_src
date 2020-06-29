#pragma once
/**\class Hyd_Floodplainraster_Segment
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_FLOODPLAINRASTER_SEGMENT_H
#define HYD_FLOODPLAINRASTER_SEGMENT_H

//system classes
#include "Geo_Raster_Segment.h"
//hyd-system
#include "Hyd_Element_Floodplain.h"

///Enumerator for the floodplain segment raster type \ingroup hyd
enum _hyd_floodplain_raster_segment_type{
	///Floodplain raster segment is at the border of the floodplain raster
	BORDER,
	///Floodplain raster segment is in x-flow direction (positiv x)
	XFLOW,
	///Floodplain raster segment is in y-flow direction (positiv y)
	YFLOW
};

///Geometrical segment of a floodplain raster \ingroup hyd
/**
The segments are generated with the help of the raster points. With the help of the
segment each polygon of the floodplain raster can be generated.

\see Hyd_Floodplain_Raster, Hyd_Floodplainraster_Point, Hyd_Floodplainraster_Polygon
*/
class Hyd_Floodplainraster_Segment : public Geo_Raster_Segment
{
public:
	///Default constructor
	Hyd_Floodplainraster_Segment(void);
	///Default destructor
	~Hyd_Floodplainraster_Segment(void);


	//methods

	///Set the type of the segment
	void set_segmenttype(_hyd_floodplain_raster_segment_type type);
	///Set a pointer to Hyd_Element_Floodplain 
	void set_element_ptr(Hyd_Element_Floodplain *element);

	///Set the index in the raster of the segment
	void set_segment_index(const int index);
	///Get the index in the raster of the segment
	int get_segment_index(void);

	///Set the index in the raster of the first point
	void set_firstpoint_index(const int index);
	///Get the index in the raster of the first point
	int get_firstpoint_index(void);

	///Set the index in the raster of the second point
	void set_secondpoint_index(const int index);
	///Get the index in the raster of the second point
	int get_secondpoint_index(void);

	///Output the header for the members
	void output_header(ostringstream *cout);
	///Output members
	void output_members(ostringstream *cout);

	///Assign a border height and a border flag to the Hyd_Element_Flodplain pointer
	void assign_value_flag2floodplain_element(const double value_h, const double value_pol, const bool flag);
	
	///Switch the points
	void switch_points(void);

	///Copy operator
	Hyd_Floodplainraster_Segment& operator=(const Hyd_Floodplainraster_Segment& object);



private:

	//members
	///Index in the raster of the segment
	int segment_index;
	///Index in the raster of the first point
	int index_point_1;
	///Index in the raster of the second point
	int index_point_2;

	///Type of the segment
	_hyd_floodplain_raster_segment_type type;

	///Pointer to the floodplain element
	Hyd_Element_Floodplain *ptr_floodplain_elem;

	//methods

	///Conversion function for output (Enumerator _hyd_floodplain_raster_segment_type to text)
	string convert_segment_type2txt(_hyd_floodplain_raster_segment_type type);

};
#endif
