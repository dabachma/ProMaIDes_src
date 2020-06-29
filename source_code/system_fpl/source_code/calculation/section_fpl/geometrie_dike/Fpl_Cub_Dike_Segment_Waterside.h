#pragma once
/**\class Fpl_Cub_Dike_Segment_Waterside
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_CUB_DIKE_SEGMENT_WATERSIDE_H
#define FPL_CUB_DIKE_SEGMENT_WATERSIDE_H


//sys-libs
#include "Geo_Segment.h"

//fpl-libs
#include "Fpl_Cub_Waterside_Materialzone.h"

///Data structure for a horizontal berm \ingroup fpl
struct _fpl_horizontal_berm_data{
	///The real gradient of the berm before it was turned to horizontal
	double real_gradient;
	///The distance between waterlevel and berm
	double distance_water2berm;
	///Influence of the berm position
	double r_db;
	///Influence length of the berm which is relevant
	double influence_lenght_berm;
	///Left influence length
	double x_influence_left;
	///Right influence length
	double x_influence_right;
	///Factor for the influence of the berm width r_b=(berm width)/(influence length of the berm)
	double r_b;
	///Maximal runup
	double max_runup;
	///Flag if the berm should be handled as forshore
	bool foreshore;
	///Flag if the berm is in the transition zone between foreshore and berm
	bool foreshore_berm;
};
///Geometric class for a segment of the waterside slope of a dike section  \ingroup fpl
/**
 
*/
class Fpl_Cub_Dike_Segment_Waterside: public Geo_Segment
{
public:
	///Default constructor
	Fpl_Cub_Dike_Segment_Waterside(void);
	///Copy constructor
	Fpl_Cub_Dike_Segment_Waterside(Fpl_Cub_Dike_Segment_Waterside &object);
	///Default destructor
	~Fpl_Cub_Dike_Segment_Waterside(void);

	//members
	///Pointer to the material variable of the waterside segment
	Fpl_Cub_Waterside_Materialzone *ptr_mat_zone;

	//methods
	///Set the points of the segment
	void set_points(Geo_Point *point1,Geo_Point *point2);
	///Set the zone number of the segment
	void set_zone_number(const int zone_number);
	///Set the gradient reduction before the berm is turned to horizontal
	void set_gradient_reduction(void);
	///Turn the berm segment to horizontal
	void turn_berm_segment(Fpl_Cub_Dike_Segment_Waterside *segment_before, Fpl_Cub_Dike_Segment_Waterside *segment_after);

	///Get the zone number
	int get_zone_number(void);
	///Get the gradient reduction
	double get_gradient_reduction(void);
	///Get the relevant vertical distance before the berms was turned
	double get_relevant_vertical(void);
	///Get the relevant horizontal distance before the berms was turned
	double get_relevant_horizontal(void);
	///Get if this is a berm segment
	bool get_is_berm(void);
	///Get if it is an horizontal berm
	bool get_is_horizontal_berm(void);
	///Get the pointer to the berm data
	_fpl_horizontal_berm_data* get_berm_data(void);
	///Set the foreshore flag
	void set_foreshore_flag(const double wave_length);
	///Set the vertical distance between berm/foreshore and waterlevel
	void set_vertical_water2berm(const double waterlevel);
	///Calculate berm reduction factor (gamma berm)
	void calculate_gamma_berm(const double wave_height, const double crest_height, const double waterlevel);
	///Calculate the wave height reduction due to a foreshore
	void calculate_waveheight_reduction(const double wave_height);
	///Interpolate the berm reduction factor between berm and foreshore
	void interpolate_gamma_berm(const double wave_length);
	///Interpolate the wave height reduction factor between berm and foreshore
	void interpolate_waveheight_reduction(const double wave_length);
	///Get the wave height reduction factor due to a foreshore
	double get_waveheight_reduction(void);
	///Get the berm reduction factor (gamma berm)
	double get_gamma_berm(void);

	///Output the members to display/console
	void output_members(const bool outflag=true);
	///Output the deterministic results
	void output_determ_result(const bool outflag=true);
	///Get the sinus of the slope angle
	double get_sin_alpha(void);
	///Set the erosion start flag
	void set_erosion_start(const bool flag);
	///Get the erosion start flag
	bool get_erosion_start(void);
	///Set the erosion failed flag
	void set_erosion_failed(const bool flag);
	///Get the erosion failed flag
	bool get_erosion_failed(void);
	///Set the erosion failed flag
	void set_wave_impact_failed(const bool flag);
	///Get the erosion failed flag
	bool get_wave_impact_failed(void);


	///Copy operator
	Fpl_Cub_Dike_Segment_Waterside& operator=(const Fpl_Cub_Dike_Segment_Waterside& object);

private:
	//members
	///Flag if the segment is purely a berm (0.0 < gradient <1/15)
	bool berm_flag;
	///Flag if the segment is in the transition zone between berm and slope (1/15 < gradient <1/8)
	bool berm_mid_gradient_flag;
	///Flag if the berm is horizontal (gradient =0.0)
	bool berm_horizontal_flag;
	///Gradient reduction factor
	double gradient_reduction;
	///Wave height reduction factor due to a foreshore
	double waveheight_reduction;
	///Berm reduction factor (gamma berm)
	double gamma_berm;
	///Zone number of the segment
	int zone_number;
	///Pointer to berm data. It is required, if the segment is a berm 
	_fpl_horizontal_berm_data *hori_berm_data;

	///Sinus of the slope angle
	double sin_alpha;

	///Flag if the erosion process started
	bool erosion_start;
	///Flag if erosion failure occurrs
	bool erosion_failed;
	///Flag if a wave impact failure occurrs
	bool wave_impact_failed;
	
	//method
	///Check if the segement have the gradient for a horizontal berm (gradient =0.0), berm (0.0 < gradient <1/15), slope (gradient >1/8), transition zone between slope and berm (1/15 < gradient <1/8)
	void check_segment_for_berm(void);
	///Allocate the berm_data
	void allocate_berm_data(void);
	
	///Set error(s)
	Error set_error(const int err_type);
};
#endif