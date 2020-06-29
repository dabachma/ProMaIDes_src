#pragma once
/**\class Fpl_Dike_Segment
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/
#ifndef FPL_DIKE_SEGMENTH
#define FPL_DIKE_SEGMENT_H

//sys_libs
#include "Geo_Segment.h"


///Geometric class for a segment of the hinterland and the foreland of a dike section  \ingroup fpl
/**
 
*/
class Fpl_Dike_Segment : public Geo_Segment
{
public:
	///Default constructor
	Fpl_Dike_Segment(void);
	///Default destructor
	~Fpl_Dike_Segment(void);
};
#endif
