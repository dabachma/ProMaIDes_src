#pragma once
/**\class Fpl_Dike_Geo_Materialzone_Connector
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/

#ifndef FPL_DIKE_GEO_MATERIALZONE_CONNECTOR_H
#define FPL_DIKE_GEO_MATERIALZONE_CONNECTOR_H

//system sys

//system fpl
#include "Fpl_Dike_Geo_Materialzone.h"



///Coonection class to the material zones of a dike \ingroup fpl
/**

\see Fpl_Dike_Geo_Materialzone
*/
class Fpl_Dike_Geo_Materialzone_Connector
{
public:
	///Default constructor
	Fpl_Dike_Geo_Materialzone_Connector(void);
	///Default destructor
	~Fpl_Dike_Geo_Materialzone_Connector(void);

	//method
	///Get the percentage of the material zone
	double get_percentage_mat_zone(void);
	///Get the secondary information
	double get_secondary_info(void);

	///Get the pointer to the material zone
	Fpl_Dike_Geo_Materialzone *get_ptr_mat_zone(void);

	///Set the percentage of the material zone
	void set_percentage_mat_zone(const double percentage);
	///Set the secondary information
	void set_secondary_info(const double second_info);

	///Set the pointer to the material zone
	void set_ptr_mat_zone(Fpl_Dike_Geo_Materialzone *zone);

	///Reset the zone connector
	void reset_connector(void);

private:

	//member
	///Percentage of the material zone
	double perc;
	///Secondary information
	double second_info;

	///Pointer to the material zone
	Fpl_Dike_Geo_Materialzone *ptr_mat_zone;
};
#endif
