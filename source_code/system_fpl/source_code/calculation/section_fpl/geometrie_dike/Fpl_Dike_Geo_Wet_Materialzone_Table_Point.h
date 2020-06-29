#pragma once
/**\class Fpl_Dike_Geo_Wet_Materialzone_Table_Point
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/

#ifndef FPL_DIKE_GEO_WET_MATERIALZONE_TABLE_POINT_H
#define FPL_DIKE_GEO_WET_MATERIALZONE_TABLE_POINT_H

//system sys

//system fpl
#include "Fpl_Dike_Geo_Materialzone_Connector.h"


///Points of the table, where the wetted areas of a slice of a slope circle are stored in advance \ingroup fpl
/**

\see Fpl_Dike_Geo_Wet_Materialzone_Table, Fpl_Dike_Geo_Materialzone_Connector
*/
class Fpl_Dike_Geo_Wet_Materialzone_Table_Point
{
public:
	///Default constructor
	Fpl_Dike_Geo_Wet_Materialzone_Table_Point(void);
	///Default destructor
	~Fpl_Dike_Geo_Wet_Materialzone_Table_Point(void);

	//method
	///Set the waterlevel
	void set_waterlevel(const double h);
	///Get the waterlevel
	double get_waterlevel(void);

	///Set the percentage of the total area of the slice
	void set_perc_area(const double percentage_area);
	///Get the percentage of the total area of the slice
	double get_perc_area(void);

	///Set and allocate the connected material zones
	void set_allocate_mat_zones(const int number);
	///Get the number of connected material zones
	int get_number_material_zones(void);

	///Set the connected material zones
	void set_connected_material_zones(Fpl_Dike_Geo_Materialzone_Connector* zone_connector);

	///Copy the table point
	void copy_table_point(Fpl_Dike_Geo_Wet_Materialzone_Table_Point *src);

	///Get a pointer to the material zones
	Fpl_Dike_Geo_Materialzone_Connector* get_ptr_connected_mat_zones(const int index);

private:

	//member
	///Waterlevel
	double waterlevel;
	///Percentage of the total area of the slice
	double area_perc;
	///Number of material zones
	int number_zones;
	///Pointer to the material zone connector
	Fpl_Dike_Geo_Materialzone_Connector *ptr_zone_connector;

	//method
	///Allocate the material zone connector
	void allocate_zone_connector(void);
	///Delete the the material zone connector
	void delete_zone_connector(void);

	///Set the error(s)
	Error set_error(const int err_type);


};
#endif