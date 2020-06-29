#pragma once
/**\class Dam_Polygon
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef DAM_POLYGON_H
#define DAM_POLYGON_H

//sys_libs
#include "Geo_Simple_Polygon.h"
#include "Common_Const.h"

///This class represents a geometrical polygon for an interception and changing of the damage raster properties \ingroup dam
/**

*/
class Dam_Polygon : public Geo_Simple_Polygon
{
public:
	///Default constructor
	Dam_Polygon(void);
	///Default destructor
	~Dam_Polygon(void);

	//method
	//Input the members per file
	void input_members(const int index, const string filename);

private:


	//members
	///Index of the damage polygon
	int index;

	//methods
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
