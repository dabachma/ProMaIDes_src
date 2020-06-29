#pragma once
/**\class _Geo_Geometrie
	\autho_Geo_r Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _GEO_GEOMETRIE_H
#define _GEO_GEOMETRIE_H

//system classes
#include "Common_Const.h"
#include "Error.h"
#include "Warning.h"
#include "Sys_Memory_Count.h"
#include "Sys_Common_Output.h"
#include "Tables.h"

///Geometric base class \ingroup sys
/**
Here different gemetric operations are implemented, which can be useful for the
geometric child-classes.
*/
class _Geo_Geometrie
{
public:
	///Default constructor
	_Geo_Geometrie(void);
	///Default destructor
	virtual ~_Geo_Geometrie(void);

	//members

	///Convert grad to rad
	static double grad_to_rad(const double grad);
	///Convert rad to grad
	static double rad_to_grad(const double rad);
	///Convert any given angle to 0 to 360°
	static double angle_to_360(const double angle);

	

};
#endif