#pragma once
/**\class Fpl_Calculation
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_BURSTING_SLICE_H
#define FPL_BURSTING_SLICE_H


//fpl-system
#include "Fpl_Dike_Geo_Materialzone_Connector.h"


///Geometrical class of one slice for the bursting calculation \ingroup fpl
/**


*/
class Fpl_Bursting_Slice
{
public:
	///Default constructor
	Fpl_Bursting_Slice(void);
	///Default destructor
	~Fpl_Bursting_Slice(void);

	//method
	///Inititalize the material zones
	void init_zone_connector(const double x_coor, const int number, Fpl_Dike_Geo_Materialzone *ptr_mat_zone, Fpl_Dike_Geo_Materialzone_Connector *connect_buffer);

	///Set the fix leakage length
	void set_fixed_leak_length(const double length);
	///Get the fixed leakage length
	double get_fixed_leak_length(void);

	///Get the size of the impermeable layer
	double get_size_impermeable_layer(void);

	///Get the flag if the impermeable zone is intercepted
	bool get_impermeable_zone_intercepted(void);

	///Get the extra load on the impermeable layer
	double get_extra_load(Fpl_Seepage_Line_Point_List *seepage_list, const double density_water);

	///Add the relevant points of the seepage line to the given list
	void add_seepage_line_point(Fpl_Seepage_Line_Point_List *seepage_list, Geo_Polysegment *outer_polysegment);



private:

	///x-coordinate of the slice
	double x_coor;

	///Fixed leakage length
	double fixed_leak_length;

	///Size of the impermeable layer
	double size_imperm;

	///Number of intercepted zone
	int number_intercept_zone;

	///Intercepted zone
	Fpl_Dike_Geo_Materialzone_Connector *intercept_zone;

	///Flag if the impermeable zone is intercepted
	bool imperm_intercepted;

	///Index of the seepage line
	int index_seepage;

	//method
	///Allocate the zone connector
	void allocate_zone_connector(void);
	///Delete the zone connector
	void delete_zone_connector(void);

	///Set the error(s)
	Error set_error(const int err_type);



};
#endif
