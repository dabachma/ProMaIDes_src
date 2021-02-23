#pragma once
/**\class Fpl_Mech_Erosion_Dune_van_Gent
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2012
*/
#ifndef FPL_MECH_EROSION_DUNE_VAN_GENT_H
#define FPL_MECH_EROSION_DUNE_VAN_GENT_H

//system fpl
#include "_Fpl_Mechanism.h"


///Mechanism-class for the water side dune erosion after van Gent 2008 \ingroup fpl
/**
Further information are provided in "Large-scale dune erosion tests to study the influence of wave periods" (2008).
The calculation of the settling velocity of a dune particle, one inpuit parameter of the dune erosion calculation
is provided in Madsen (2003).

*/
class Fpl_Mech_Erosion_Dune_van_Gent : public _Fpl_Mechanism
{
public:
	///Default constructor
	Fpl_Mech_Erosion_Dune_van_Gent(void);
	///Default destructor
	~Fpl_Mech_Erosion_Dune_van_Gent(void);

	//members
	///Result value as random variable: resistance
	Fpl_Random_Variables resistance;
	///Result value as random variable: stress
	Fpl_Random_Variables stress;

	///Randomvariable of the critical crest width
	Fpl_Random_Variables critical_crest_width;

	///Randomvariable of the mean diameter of the sand particle
	Fpl_Random_Variables mean_diameter;
	///Randomvariable of the roundness factor of the sand particle
	Fpl_Random_Variables roundness_fac;
	///Randomvariable of the shape factor (csf) of the sand particle
	Fpl_Random_Variables csf;
	///Randomvariable of the of the grain density of the sand material
	Fpl_Random_Variables grain_density;
	///Randomvariable of the of the settling velocity of the sand material
	Fpl_Random_Variables settling_velocity;



	//method
	///Set the random variables of the result members
	void set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random input variables per database table
	void set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random input variables per file
	void set_variables(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start);

	///Transfer the bursting-mechanism (DIN 19712) data to database, e.g. geometry, random variables etc.
	void transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database);


	///Output members of the mechanism to display/console
	void output_members(void);
	
	///Check the statistic of the random variables of the mechanism
	void check_statistic(void);
	///Reset the statistic important values
	void reset_statistics(void);
	///Output the statistics of the random variables of the mechanism to display/console
	void output_statistic(void);
	///Output the results of a deterministic calculation to display/console
	void output_determ_results(void);
	///Output the new profile shape to tecplot
	void output_results2tecplot(const string new_profile);
	///Output the new profile shape to paraview
	void output_results2paraview(const string new_profile);
	///Output the new profile shape to excel
	void output_results2excel(const string new_profile);

	///Calculate the mechanism
	bool calculate_mechanism(const bool random_calculation, const double waterlevel, double wave_height, double wave_period, const double water_density, const double section_direction, const double wave_direction);

	///Set the pointer to the area of the outer dune polygon and the pointer to the outer polygon itself
	void set_ptr_area_poly_outer(double *area, Geo_Simple_Polygon *poly);
	///Set the pointer to the crest points
	void set_ptr_crest_points(Geo_Point *land_crest, Geo_Point *water_crest);
	///Set pointer to the foreland, the waterside cubature and the crest cubature
	void set_ptr_cubature(Fpl_Dike_Geo_Foreland *foreland, Fpl_Cub_Dike_Waterside *waterside, Fpl_Cub_Dike_Crest *crest, double *area);

	///Initialize the polygon for the new shape of the dune after wave attack (use it after the geometric pointers are set)
	void init_new_shape_poly(void);



private:
	
	//members
	///Matrix to interpolate the A-value from the particle shape factor csf and the particle roundness (after Madsen 2003)
	double a_matrix[3][6];
	///Matrix to interpolate the B-value from the particle shape factor csf and the particle roundness (after Madsen 2003)
	double b_matrix[3][6];

	///Values of the matrix columns (roundness)
	double matrix_col[3];
	///Values of the matrix rows (csf)
	double matrix_row[6];

	///Vertical line
	Geo_Straight_Line vertical_line;
	///Line with an 1:1 gradient
	Geo_Straight_Line one2one_line;
	///Horizontal line of the waterlevel
	Geo_Straight_Line waterlevel_line;
	///Line with an 1:12.5 gradient
	Geo_Segment one2twelve_line;

	///New shape of the dune after wave attack
	Geo_Simple_Polygon new_shape_poly;

	///Number of points representing the new shape part
	int number_new_points;

	///Delta x for the discretization of the new shape
	const double delta_x;



	///Buffer variable for the resistance
	double buff_res;
	///Buffer variable for the stress
	double buff_stress;
	///Integer for a failure (0:=no failure; 1:= failure) 
	bool failure;

	///Pointer to the outer polygon area
	double *ptr_area_out;
	///Pointer to the cubature area
	double *ptr_area_cubature;
	///Pointer to the outer polygon
	Geo_Simple_Polygon *ptr_out_poly;
	///Pointer to the landside crest point of the dune
	Geo_Point *ptr_land_crest;
	///Pointer to the waterside crest point of the dune
	Geo_Point *ptr_water_crest;
	///Pointer to the foreland cubature
	Fpl_Dike_Geo_Foreland *ptr_foreland;
	///Pointer to the waterside cubature
	Fpl_Cub_Dike_Waterside *ptr_waterside;
	///Pointer to the crest cubature
	Fpl_Cub_Dike_Crest *ptr_crest;

	///Point inside the old profile
	int no_point_inside;
	///Foreland to short
	int no_foreland_short;

	///List for the area calculation (performance reason)
	Geo_Polygon_Interception_Point_List poly1;
	///List for the area calculation (performance reason)
	Geo_Polygon_Interception_Point_List poly2;
	///List for the area calculation (performance reason)
	Geo_Point_List point_list;
	///List for the area calculation (performance reason)
	Geo_Interception_Point_List inter_point_list;
	///List for the area calculation (performance reason)
	Geo_Polygon_Interception_Point inter_point;



	//method
	///Reset the random generated flags of the random variables
	void reset_random_flag(void);

	///Calculate the settling veleocity of the sand material
	void calculate_settling_velocity(const bool random_calculation, const double water_density);

	///Interpolate value
	double interpolate_matrix_value(const double csf, const double roundness, double mat[3][6]);

	///Initiate the A-matrix and B-matrix
	void init_matrix(void);

	///Calculate new dune profile
	void calculate_new_dune_profile(const bool random_calculation,  const double wave_height, const double wave_period);
	
	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);

};

#endif