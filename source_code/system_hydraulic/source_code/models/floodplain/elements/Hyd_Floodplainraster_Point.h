#pragma once
/**\class Hyd_Floodplainraster_Point
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_FLOODPLAINRASTER_POINT_H
#define HYD_FLOODPLAINRASTER_POINT_H

//sys system
#include "Geo_Point.h"

///Geometrical point of a floodplain raster \ingroup hyd
/**

\see Hyd_Floodplain_Raster, Hyd_Floodplainraster_Segment, Hyd_Floodplainraster_Polygon
*/
class Hyd_Floodplainraster_Point : public Geo_Point
{
public:
	///Default constructor
	Hyd_Floodplainraster_Point(void);
	///Copy constructor
	Hyd_Floodplainraster_Point(const Hyd_Floodplainraster_Point& object);
	///Default destructor
	~Hyd_Floodplainraster_Point(void);

	///Set the point index
	void set_point_index(const int index);
	///Get the point index
	int get_point_index(void);
	///Output members
	void output_members(ostringstream *cout);
	///Output header for members output
	void output_header(ostringstream *cout);

	///Copy operator
	Hyd_Floodplainraster_Point& operator=(const Hyd_Floodplainraster_Point& object);
	


private:
	///Index of the point
	int point_index;
};
#endif
