#pragma once
/**\class Fpl_Dike_Geo_Wet_Materialzone_Table
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/

#ifndef FPL_DIKE_GEO_WET_MATERIALZONE_TABLE_H
#define FPL_DIKE_GEO_WET_MATERIALZONE_TABLE_H

//system sys

//system fpl
#include "Fpl_Dike_Geo_Wet_Materialzone_Table_Point.h"


///Table class to store the wetted areas of a slice of a slope circle in advance \ingroup fpl
/**

\see Fpl_Dike_Geo_Wet_Materialzone_Table_Point, Fpl_Dike_Geo_Materialzone_Connector

*/
class Fpl_Dike_Geo_Wet_Materialzone_Table
{
public:
	///Default constructor
	Fpl_Dike_Geo_Wet_Materialzone_Table(void);
	///Default destructor
	~Fpl_Dike_Geo_Wet_Materialzone_Table(void);

	//method
	///Get the number of table points
	int get_number_table_points(void);
	///Set the number of the maximum of connected material zones
	void set_number_connected_mat_zones(const int max_number_connected);
	///Set a new table point
	void add_table_point(const double waterlevel, const double perc_area, Fpl_Dike_Geo_Materialzone_Connector* zone_connector);
	///Get a pointer to table point
	Fpl_Dike_Geo_Wet_Materialzone_Table_Point *get_ptr_table_point(const int index);

	///Interpolate a given waterlevel
	Fpl_Dike_Geo_Wet_Materialzone_Table_Point *interpolate_table_values(const double water_h);

	///Sort the table after a ascending waterlevel level
	void sort_table_waterlevel(void);


private:
	//members
	///Number of table points
	int number_points;
	///Maximum number of material zones in the slice
	int max_number_mat_zone;
	///Last index of the last interpolated point
	int last_index;

	///Pointer to the table points
	Fpl_Dike_Geo_Wet_Materialzone_Table_Point *table_points;

	///Number of list-elements in a block for allocation
	const int block_elems;

	///Point with interpolated values
	Fpl_Dike_Geo_Wet_Materialzone_Table_Point interpol_point;

	//methods
	///Delete the table points
	void delete_table_points(void);

	///Find the value to interpolate via a downwards search
	Fpl_Dike_Geo_Wet_Materialzone_Table_Point *interpolatedownwards_value(const double water_h);
	///Find the value to interpolate via a upwards search
	Fpl_Dike_Geo_Wet_Materialzone_Table_Point *interpolateupwards_value(const double water_h);

	///Set the error(s)
	Error set_error(const int err_type);

};
#endif
