#pragma once
/**\class Fpl_Cub_Dike_Waterside
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/
#ifndef FPL_CUB_DIKE_WATERSIDE_H
#define FPL_CUB_DIKE_WATERSIDE_H

//fpl-libs
#include "Fpl_Cub_Dike_Segment_Waterside.h"
#include "Fpl_Section_Points.h"

///Data structure for a the contraction height for the dike overflow mechanism in case of waterside berms \ingroup fpl
struct _fpl_contraction_height{
	///Height between crest point and horizontal berm
	double height;
	///Interpolated effective height
	double interpolated_height;
	///Effective berm width
	double effective_width;
	///Influence area mets the ground
	bool ground_flag;
};

///Class for managing the geomtric parameters of the waterside slope of a dike section  \ingroup fpl
/**

*/
class Fpl_Cub_Dike_Waterside
{
public:
	///Default constructor
	Fpl_Cub_Dike_Waterside(void);
	///Default destructor
	~Fpl_Cub_Dike_Waterside(void);

	//members

	///Random variable for the reduction factor due to berms (gamma berm) for the mechanism wave runup
	Fpl_Random_Variables gamma_berm_total;

	//method
	///Set the geometrie of the waterside cubature per database table
	void set_segments(Fpl_Section_Points *base_point, Fpl_Section_Points *crest_point, QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the geometrie of the waterside cubature per file
	void set_segments(Fpl_Section_Points *base_point, Fpl_Section_Points *crest_point, QFile *ifile, int *line_counter);

	///Set the random variables of reduction factors of the wave runup mechanism (gamma berm) per database
	void set_gamma_berm(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random variables of reduction factors of the wave runup mechanism (gamma berm) per file
	void set_gamma_berm(QFile *ifile, int *line_counter);

	///Transfer the geomtrical data of the waterside dike cubature to database
	void transfer_geometry2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database);

	///Transfer the random variables of the waterside dike cubature to database
	void transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database);

	///Output the geometrie of the waterside to display/console
	void output_geometrie(void);
	///Output the members to display/console
	void output_members(void);

	///Get the total distance of the waterside cubature
	double get_total_distance(void);
	///Get the horizontal distance of the waterside cubature
	double get_horizontal_distance(void);
	///Get the vertical distance of the waterside cubature
	double get_vertical_distance(void);
	///Get the dike height
	double get_dike_height(void);
	///Get if there are berms at the waterside
	bool get_with_berm_flag(void);
	///Get the total berm reduction factor of the wav runup mechanism (gamma berm) randomly distributed
	double get_total_gamma_berm(const bool random_calculation);

	///Get the number of segments
	int get_number_segments(void);
	///Get a pointer to a segments specified with an index
	Fpl_Cub_Dike_Segment_Waterside *get_segment(const int index);

	///Get a pointer to the last point (:= crest waterside)
	Geo_Point *get_ptr_last_point(void);

	///Reset the flags if the variables where randomly generated
	void reset_random_flag(void);

	///Calculate the interception of a given line (e.g. waterlevel) and the cubature to get the relevant cubature; the search is upwards
	void calculate_intercept_cubature_firstup(Geo_Straight_Line *ptr_relv_waterlevel, int *no_segment_intercept);
	///Calculate the interception of a given line (e.g. waterlevel) and the cubature to get the relevant cubature; the search is downwards
	void calculate_intercept_idealcubature_firstdown(Geo_Straight_Line *ptr_relv_waterlevel, int *no_segment_intercept);

	///Calculate gamma material factor at the real cubature for the mechanism of wave runup
	double calculate_gamma_material(const double random_calculation, const double wave_height, const int no_segment_intercept);
	///Calculate the middle of the gradient at the real cubature for the mechanism of wave runup
	double calculate_mid_relevant_gradient(const int no_segment_intercept);
	///Calculate the middle of the gradient at the waterside cubature
	double calculate_mid_gradient(void);
	///Calculate the gamma berm and the waveheight reduction factor for a berm or a foreshore at the idealisized cubature for the mechanism of wave runup
	void calculate_gammaberm_waveheightreduction(const double wave_length, double *wave_height, const double waterlevel);

	///Calculate the contration height for the overflow mechanism
	void calculate_contraction_height(const double delta_h, double *contraction_height);
	///Calculate the relevant slope for the overflow mechanism at the ideal cubature
	void calculate_weir_slope(double *weir_slope, const int no_segment_intercept);

	///Check the statistic of the random generation process
	void check_statistic(void);
	///Reset the statistic important values
	void reset_statistics(void);
	///Output the statistics of the random generation process to display/console
	void output_statistic(void);
	///Output the deterministic results to display/console
	void output_determ_results(void);

	

private:
	//member
	///Pointer to the waterside base point
	Fpl_Section_Points *ptr_base_point;
	///Pointer to the waterside crest point
	Fpl_Section_Points *ptr_crest_point;
	///Number of points representing the waterside cubature (number_waterside_points + crest point + base point)
	int number_points;
	///Flag if the cubature has berms
	bool with_berm_flag;
	///Segments of the waterside cubature (real cubature)
	Fpl_Cub_Dike_Segment_Waterside *ptr_segments;
	///Number of segments of the real waterside cubature
	int number_segments;
	///Segments of the waterside cubature (ideal cubature: berms are combined and turned to horizontal)
	Fpl_Cub_Dike_Segment_Waterside *ptr_ideal_segments;
	///Number of segments of the ideal waterside cubature
	int number_ideal_segments;

	///Line for the calculation of the influence length of a berm of the mechanism wave runup
	Geo_Straight_Line *line_influence_berm;

	///The new shorten segment of the cubature, where the cubature is intercepted by the waterlevel (relevant cubature)
	Fpl_Cub_Dike_Segment_Waterside first_segment;
	///Interception point between waterlevel and cubature
	_geo_interception_point interception;

	///Reduction factor (gamma material) for the mechanism wave runup
	double mid_gamma_material;

	///Value of the reduction factor due to berms (gamma berm) for the mechanism wave runup
	double gamma_berm;
	///Number horizontal berms
	int number_horizontal_berms;

	///Contraction height for the mechanim overflow
	_fpl_contraction_height *part_contraction;

	///Total length of the waterside cubature
	double total_length;
	///Horizontal length of the waterside cubature
	double total_horizontal;
	///Vertical length of the waterside cubature
	double total_vertical;




	//methods
	///Allocate the segments of the real cubature
	void allocate_segment_real(void);
	///Delete the segments of the real cubature
	void delete_segment_real(void);
	///Allocate the segments of the ideal cubature
	void allocate_segment_ideal(void);
	///Delete the segments of the ideal cubature
	void delete_segment_ideal(void);

	///Allocate the line for calculation of the berm influence
	void allocate_berm_influence_line(void);
	///Delete the line for calculation of the berm influence
	void delete_berm_influence_line(void);
	///Allocate the line for calculation of the contraction factor
	void allocate_contraction_factor(void);
	///Delete the line for calculation of the contraction factor
	void delete_contraction_factor(void);

	///Check the cubature segments
	void check_cubature(void);
	///Idealizied the cubature (ideal cubature: berms are combined and turned to horizontal)
	void idealizise_cubature(void);
	///Calculate the total length as well as the vertical and horizontal distance
	void calc_total_length(void);
	///Set the pointer to the base point and the crest point
	void set_base_crest_point(Fpl_Section_Points *base_point, Fpl_Section_Points *crest_point);
	///Calculate the influence length of a berm for the mechanism wave-runup
	void calculate_influence_length_berm(const double waveheight, const int index_berm);
	///Prepare the calculation of a partly contraction height for the overflow mechanism
	void prepare_contraction_heights(void);
	///Find the key-values for the file input of the geometrical points and the material zone variables
	void find_key_values_file(const string myline);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif