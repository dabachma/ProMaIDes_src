#pragma once
/**\class Fpl_Slope_Slices
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/
#ifndef FPL_SLOPE_SLICES_H
#define FPL_SLOPE_SLICES_H

//system sys

//system fpl
#include "Fpl_Dike_Geo_Materialzone_Connector.h"
#include "Fpl_Dike_Geo_Wet_Materialzone_Table.h"
#include "Fpl_Seepage_Line_Point_List.h"

//forward declaration
class Fpl_Slip_Circle;

///Geometrical class of one slice of a slip circle \ingroup fpl
/**


*/
class Fpl_Slope_Slices
{
public:
	///Default constructor
	Fpl_Slope_Slices(void);
	///Default destructor
	~Fpl_Slope_Slices(void);


	//methods
	///Initialize the slice of the slope circle 
	bool init_slice(Geo_Simple_Polygon *poly_cub, Geo_Simple_Polygon *poly_slice, Geo_Simple_Polygon *poly_wet_slice, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones, Fpl_Dike_Geo_Materialzone_Connector *buffer_all_zone, Fpl_Slip_Circle *ptr_circle, const double delta_h, const double h_start);

	///Get the total slice area
	double get_total_slice_area(void);
	///Get the total slip lenght
	double get_total_slip_length(void);
	///Get the mid point of the slice at the slip surface
	Geo_Point* get_mid_point(void);

	///Get the mid weight of the slice
	double get_mid_weight(Fpl_Seepage_Line_Point_List *seepage_list, bool *wet_flag, double *water_area, int *number_comp_wet);
	///Get the mid tangent of the angle of friction
	double get_mid_tan_friction(void);
	///Get the mit of cohesion
	double get_mid_cohesion(void);

	///Get the number of the connected material zones of the slice area
	int get_number_connect_zone_area(void);
	///Get the pointer of the connected material zones of the slice area
	Fpl_Dike_Geo_Materialzone_Connector *get_ptr_connect_zone_area(void);

	///Get the number of the connected material zones of the slip surface
	int get_number_connect_zone_slip_surface(void);
	///Get the pointer of the connected material zones of the slip surface
	Fpl_Dike_Geo_Materialzone_Connector *get_ptr_connect_zone_slip_surface(void);

	///Output the members to display/console
	void output_members(const int no);

	///Get the angle of the slice (degree)
	double get_slice_angle(void);
	///Get the slice angle (rad)
	double get_slice_angle_rad(void);

	///Get the sinus of the slices angle
	double get_sin_angle_slice(void);
	///Get the cosinus of the slice angle
	double get_cos_angle_slice(void);

	///Set the index of the point of the seepage line
	void set_index_point_seepage_line(const int index);
	///Get the index of the point of the seepage line
	int get_index_point_seepage_line(void);

	///Get the slice width
	double get_slice_width(void);
	///Set the slice width
	void set_slice_width(const double width);


private:
	//members

	///Total slice area
	double total_area;
	///Total slip length
	double total_slip_length;
	///Slice width
	double slice_width;
	///The mid point of the slice at the slip surface
	Geo_Point mid_point;

	///Angle of slice angle (rad)
	double angle_of_slice;

	///Sinus of slice angle
	double sin_angle_slice;
	///Cosinus of slice angle
	double cos_angle_slice;

	///Index of the connected point of the seepage line
	int index_seepage_point;



	///Number of the connected material zones of the slice area
	int number_connect_zone_area;
	///Pointer to the connected material zones of the slice area
	Fpl_Dike_Geo_Materialzone_Connector *connect_zone_area;

	///Number of the connected material zones of the slip surface
	int number_connect_zone_slip_surface;
	///Pointer to the connected material zones of the slip surface
	Fpl_Dike_Geo_Materialzone_Connector *connect_zone_slip_surface;

	///Table where the information about the wetted areas of the slices are stored
	Fpl_Dike_Geo_Wet_Materialzone_Table wet_area_table;


	//method
	///Allocate the connected material zones for the area in the slice
	void allocate_area_material_zone(void);
	///Delete the connected material zones for the area in the slice
	void delete_area_material_zone(void);

	///Allocate the connected material zones for the slip surface
	void allocate_slip_surface_material_zone(void);
	///Delete the connected material zones for the slip surface
	void delete_slip_surface_material_zone(void);

	///Calculate and set the slice angle
	void set_slice_angle(Fpl_Slip_Circle *ptr_circle);
	///Make the geometrical interception with material zones for the dry area calculation
	bool mat_zone_interception_dry_area(Geo_Simple_Polygon *poly_slice, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones, Fpl_Dike_Geo_Materialzone_Connector *buffer_all_zone);
	///Make the geometrical interception with material zones for the slip surface calculation
	bool mat_zone_interception_slip_surface(Geo_Simple_Polygon *poly_slice, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones, Fpl_Dike_Geo_Materialzone_Connector *buffer_all_zone);
	///Make the geometrical interception with material zones for the wet area calculation
	bool mat_zone_interception_wet_area(Geo_Simple_Polygon *poly_slice, Geo_Simple_Polygon *poly_wet_slice, const double delta_h, Fpl_Dike_Geo_Materialzone_Connector *buffer_all_zone, Geo_Simple_Polygon *poly_cub, const double h_start);


	///Set the error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);




};
#endif
