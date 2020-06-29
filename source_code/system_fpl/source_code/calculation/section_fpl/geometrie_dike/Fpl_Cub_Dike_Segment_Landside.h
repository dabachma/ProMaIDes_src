#pragma once
/**\class Fpl_Cub_Dike_Segment_Landside
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/
#ifndef FPL_CUB_DIKE_SEGMENT_LANDSIDE_H
#define FPL_CUB_DIKE_SEGMENT_LANDSIDE_H

//sys_libs
#include "Geo_Segment.h"
//fpl-libs
#include "Fpl_Random_Variables.h"

///Geometric class for a segment of the landside cubature of a dike section  \ingroup fpl
/**
 
*/
class Fpl_Cub_Dike_Segment_Landside: public Geo_Segment
{
public:
	///Default constructor
	Fpl_Cub_Dike_Segment_Landside(void);
	///Default destructor
	~Fpl_Cub_Dike_Segment_Landside(void);
};
#endif
