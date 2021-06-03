#pragma once
/**\class Fpl_Slip_Circle
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/
#ifndef FPL_SLIP_CIRCLE_H
#define FPL_SLIP_CIRCLE_H

//system sys
#include "Geo_Circle.h"

//system fpl
#include "Fpl_Slope_Slices.h"
#include "Fpl_Slope_External_Force.h"


///Geometrical class of one slip circle with slices \ingroup fpl
/**


*/
class Fpl_Slip_Circle : public Geo_Circle
{
public:
	///Default constructor
	Fpl_Slip_Circle(void);
	///Default destructor
	~Fpl_Slip_Circle(void);

	//methods
	///Get number of interceptions with the structure cubatur
	int get_number_interception(void);
	///Get the number of slices
	int get_number_slices(void);
	///Set the slices width
	void set_slices_width(const double width_slices);

	///Get the flag, if it is a relevant slip circle, who intercepts the cubature
	bool get_relevant_circle(void);
	///Set the flag, if it is a relevant slip circle
	void set_relevant_circle(const bool relevant);

	///Check the interception with the cubature
	void check_interception2structure(Geo_Polysegment *cubature, Geo_Point *crest_mid, int *number_no, int *number_one, int *number_more_two, int *number_y_coor, Geo_Simple_Polygon *outer_poly, int *number_outer);

	///Initialize the slices of each slip circle
	bool init_slices(Geo_Simple_Polygon *cubature, const double y_max, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones, Fpl_Dike_Geo_Materialzone_Connector *buffer_all_zone, const double delta_h, int *number_slice_reduced, const double y_min, int *number_slice2small);
	///Set the relevant points of the seepage line to the given list
	void add_seepage_line_points(Fpl_Seepage_Line_Point_List *seepage_list, Geo_Polysegment *cubature);

	///Output the members to display/console
	void output_members(const int no);

	///Calculate the stability of the slip circle
	void calculate_stability(Fpl_Seepage_Line_Point_List *seepage_list, double *macro, double *micro, const double water_density);

	///Set if it is a landside or a waterside slip circle
	void set_waterside_slip_circle(const bool flag);
	///Get if it is a landside slip circle
	bool get_landside_slip_circle(void);

	///Get if it is a relevant slip circle for the macro stability
	bool get_relevant_macro_circle(void);
	
	///Get if the seepage line is intercepting the slip circle
	bool get_seepage_inside_circle(void);

	///Check the area of the section which is intercept by thze slip circle
	void check_area_intercept(const double area_cubature, int *number_small_area);

	///Output the slices to tecplot
	void output_slices2tecplot(ofstream *output, const bool landside);
	///Output the slices to paraview
	void output_slices2paraview(ofstream *output, const bool landside, const int before, const int after);
	///Output the slices to excel
	void output_slices2excel(ofstream *output, const bool landside, const int before, const int after);

	///Set the relevant segments of the cubature, which are involved in the slip cricle
	void set_involved_cubature_segment(Geo_Polysegment *cubature);

private:

	//members
	
	///Width of the slices
	double slices_width;
	///Width of the last slice
	double last_slice_width;

	///Minimum of slices
	const int minimum_number_slices;

	///Number of slices
	int number_slices;
	///Pointer to the slices
	Fpl_Slope_Slices *slices;

	///Points where the cubature is intercepted by the slip circle
	Geo_Point_List points_cubature;

	///Flag if it is a relevant slip circle, who intercept the cubature
	bool relevant_circle;

	///Flag if it is a relevant slip circlefor the macro stability
	bool relevant_macro_circle;
	///Flag if the slip circle is intercepting the seepage line
	bool seepage_inside;

	///Flag if it is waterside or a landside slipe circle
	bool landside_flag;

	///Intercepted area of the slip circle
	double intercept_area;

	///Segments of the cubature, which are involved in the slip circle
	Fpl_Slope_External_Force *cub_segments;


	//method
	///Allocate the slices
	void allocate_slices(void);
	///Delete the slices
	void delete_slices(void);

	///Allocate the segments of the cubature, which are involved in the slip circle
	void allocate_cub_segments(void);
	///Delete the segments of the cubature, which are involved in the slip circle
	void delete_cub_segments(void);

	///Intercept each slices and calculate the relevant area
	bool make_geometrical_slice_interception(Geo_Simple_Polygon *cubature, const double y_max, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones, Fpl_Dike_Geo_Materialzone_Connector *buffer_all_zone, const double delta_h,  const double y_min);

	///Calculate additional moment of the waterbody
	double calc_additional_moment_waterbody(const double waterlevel, const double water_density);


	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);


};
#endif

