#pragma once
/**\class Fpl_Cub_Dike_Crest
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_CUB_DIKE_CREST_H
#define FPL_CUB_DIKE_CREST_H
//system sys
#include "Geo_Segment.h"


///Geometric class representing the dike crest \ingroup fpl
/**
The dike crest belongs to the section type dike. It is part of the dike cubature.
\see Fpl_Sec_Type_Dike
*/
class Fpl_Cub_Dike_Crest
{
public:
	///Default constructor
	Fpl_Cub_Dike_Crest(void);
	///Default destructor
	~Fpl_Cub_Dike_Crest(void);

	//methods

	///Set start- and the end-point of the crest to generate the segment
	void set_segment(Geo_Point *crestwater_point, Geo_Point *crestland_point);

	///Get the crest width
	double get_crest_width(void);
	///Get crest height, the height of the waterside point
	double get_crest_height(void);

	///Get the x coordiante of the crest mid point
	double get_crest_mid_x(void);

	///Output the geometrie of the dike crest to display/console
	void output_geometrie(void);

	///Get the number of segments
	int get_number_segments(void);

	///Get the dike crest segment
	Geo_Segment* get_segment(void);

private:
	//members
	///Segment for the dike crest
	Geo_Segment crest_segment;

	//methods
	///Check the dike crest for gradient (max grad=1.0/19.0)
	void check_crestsegment(void);

	///Set the error(s)
	Error set_error(const int err_type);
};
#endif