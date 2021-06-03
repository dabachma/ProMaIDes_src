#pragma once
/**\class Fpl_Mech_Slope_Stability_Dike
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2010
*/
#ifndef FPL_MECH_SLOPE_STABILITY_H
#define FPL_MECH_SLOPE_STABILITY_H

//system fpl
#include "_Fpl_Mechanism.h"

#include "Fpl_Slip_Circle.h"
#include "Fpl_Dike_Var_Materialzone.h"

///Namespace for the keystring in a database table of the control parameters and the default paramaters for a slope stability calculation \ingroup fpl
namespace fpl_slope_control_param{
	///Default stepsize for the search raster in x-direction
	const double def_delta_x=0.5;
	///Default stepsize for the search raster in y-direction
	const double def_delta_y=0.5;
	///Default number of search points in x-direction
	const int def_number_x=20;
	///Default number of search points in y-direction
	const int def_number_y=20;
	///Default stepsize for the radius
	const double def_delta_radius=0.5;

	///Default stepsize for the table storing of different waterlevels in a slice
	const double def_delta_h=0.1;

	///Default slice width
	const double def_slice_width=0.2;
};

///Mechanism-class for a slope stability mechanism at a dike via the Krey-formula \ingroup fpl
/**
The relevant slip circle is determined via a search raster of points. The formula for the slope stability
is the Krey Formula.

*/
class Fpl_Mech_Slope_Stability_Dike : public _Fpl_Mechanism
{
public:
	///Default constructor
	Fpl_Mech_Slope_Stability_Dike(void);
	///Default destructor
	~Fpl_Mech_Slope_Stability_Dike(void);

	//members
	///Result value as random variable: stability=force/resistance; the half crest is intercepted by the slip circle
	Fpl_Random_Variables stability_macro;
	///Result value as random variable: stability=force/resistance; the waterlvel is intercepted by the slip circle
	Fpl_Random_Variables stability_micro;
	///Pointer to the table for storing the control parameters of the slope mechanism
	static Tables *control_table;

	//methods

	///Create the database table for the control parameters of the slope mechanism
	static void create_control_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the control parameters of the slope mechanism
	static void delete_data_in_control_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the control parameters of the slope mechanism for a specific fpl-section
	static void delete_data_in_control_table(QSqlDatabase *ptr_database, const int sec_id);
	///Set the database table for the control parameters of the slope mechanism: it sets the table name and the name of the columns and allocate them
	static void set_control_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the control parameters of the slope mechanism
	static void close_control_table(void);

	///Select the control parameters of a fpl-section in a database table
	static int select_control_parameter_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int sec_id, const bool landside_flag);

	///Set the random variables of the result members per database table
	void set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the control parameter per database table
	bool set_control_parameter(QSqlDatabase *ptr_database, const int section_id);
	///Set the random input variables per file
	void set_input(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start);

	///Transfer the slope stability-mechanism data to database, e.g. control parameter, geometry, random variables etc.
	void transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database);

	///Initiate the slope stability calculation
	void init_calculation(Geo_Simple_Polygon *outer_polygon, Geo_Polysegment *outer_polysegment, Geo_Point *base_land, Geo_Point *crest_mid, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones, const double area_cubature);

	///Add the relevant points of the seepage line to the given list
	void add_seepage_line_point(Fpl_Seepage_Line_Point_List *seepage_list, Geo_Polysegment *outer_polysegment);

	///Output members of the mechanism to display/console
	void output_members(void);

	///Output the header to tecplot
	static void output_header2tecplot(const string slope_file);
	///Output the search raster and the critical slipe circle to tecplot
	void output_results2tecplot(const string slope_file, int *zone_counter);
	///Output the search raster and the critical slipe circle to paraview
	void output_results2paraview(const string slope_file, int *zone_counter);
	///Output the search raster and the critical slipe circle to excel
	void output_results2excel(const string slope_file, int *zone_counter);

	///Check the statistic of the random variables of the mechanism
	void check_statistic(void);
	///Reset the statistic important values
	void reset_statistics(void);
	///Output the statistics of the random variables of the mechanism to display/console
	void output_statistic(void);
	///Output the results of a deterministic calculation to display/console
	void output_determ_results(void);

	///Calculate the mechanism; main input parameter is the waterlevel before the dike section
	void calculate_mechanism(const bool random_calculation, Fpl_Seepage_Line_Point_List *seepage_list, const double water_density, bool *macro, bool *micro);

	///Set if it is a landside or a waterside slope mechanism
	void set_waterside_slope_mechanism(const bool flag);

	///Sum up the counter of the single occurence probability by 1 of the micro instability
	void sum_up_counter_single_prob_micro(void);
	///Sum up the counter of the fault-tree occurence probability by 1 of the micro instability
	void sum_up_counter_tree_prob_micro(void);
	///Get the counter of the single occurence probability of the micro instability
	double get_counter_single_prob_micro(void);
	///Get the counter of the fault-tree occurence probability of the micro instability
	double get_counter_tree_prob_micro(void);
	///Reset the counter of the single occurence probability of the micro instability
	void reset_counter_single_prob_micro(void);
	///Reset the counter of the fault-tree occurence probability of the micro instability
	void reset_counter_tree_prob_micro(void);
	///Reset the counter of the fault-tree occurence probability and of the single occurence probability of the micro instability
	void reset_counter_prob_micro(void);

private:

	//members

	///x-coordinate of the lowest, most left point of the search raster related to the waterside dike toe
	double x_origin;
	///y-coordinate of the lowest, most left point of the search raster related to the waterside dike toe
	double y_origin;
	///Stepsize for the search raster in x-direction
	double delta_x;
	///Stepsize for the search raster in y-direction
	double delta_y;
	///Number of search points in x-direction
	int number_x;
	///Number of search points in y-direction
	int number_y;
	///Stepsize for the radius
	double delta_radius;

	///Stepsize for storing the table for different waterlevels in the slices
	double delta_h;

	///The width of the slices
	double slice_width;

	///Maximum of the radius
	double maximum_radius;
	///Number of different radius
	int number_radius;

	///Flag if it is waterside or a landside slope mechanism
	bool landside_flag;

	///Number of slip circle
	int number_slip_circle;
	///Slip circle
	Fpl_Slip_Circle *slip_circle;
	///Number of relevant slip circle
	int number_relevant_slip_circle;

	///Number of slip circles with one intercpetion
	int number_circle_one;
	///Number of slip circles with no intercpetion
	int number_circle_no;
	///Number of slip circle with more than two interception
	int number_circle_more_two;
	///Number of slip circle where radius is changed
	int number_circle_radius_change;
	///Number of slip circle where the area is to small
	int number_circle_area_small;
	///Number of slip circle where the second y-coordinate is smaller/equal the first one
	int number_circle_y_small;
	///Number of slip circle where the geometrical interception was not correct
	int number_circle_geo_error;
	///Number of slip circle where the slice width is reduced
	int number_circle_reduced_slice_width;
	///Number of slip circle where the slice is to small
	int number_slice2small;
	///Number of slip circle midpoints inside the section polygon
	int number_circle_mid_in_polygon;
	///Number of slip cricle beyond the mid of the crest (landside:=left of crest mid; waterside:= right of crest mid)
	int number_beyond_crest_mid;
	///Number of slip circle, which intercepts the outer boundary
	int number_circle_outer_bound;

	///Number of geometrical material zones
	int number_geo_material_zones;
	///Pointer to the geometrical material zones
	Fpl_Dike_Geo_Materialzone *ptr_geo_material_zones;

	///Pointer to the relevant slip circle for macro instability
	Fpl_Slip_Circle *relevant_macro_circle;
	///Pointer to the relevant slip circle for micro instability
	Fpl_Slip_Circle *relevant_micro_circle;

	///Counter for the single occurence probability of the micro instability (the counter of the macro instability is defined in the parent class)
	int counter_single_prob_micro;
	///Counter for the fault-tree occurence probabillty of the micro instability (the counter of the macro instability is defined in the parent class)
	int counter_tree_prob_micro;

	//method
	///Reset the random generated flag
	void reset_random_flag(void);

	///Allocate the slip circle
	void allocate_slip_circle(void);
	///Delete the slip circle
	void delete_slip_circle(void);

	///Transfer the control parameters to database table
	void transfer_controlparam2database(const int section_id, QSqlDatabase *ptr_database);
	///Set the control parameters per file
	void set_controlparam_per_file(QFile *ifile, int *line_counter);
	///Find the key-values for the file input of the slope control parameters
	void find_key_values_slope_control(const string myline, int *must_found_counter);

	///Check the control parameters
	void check_control_parameters(void);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif