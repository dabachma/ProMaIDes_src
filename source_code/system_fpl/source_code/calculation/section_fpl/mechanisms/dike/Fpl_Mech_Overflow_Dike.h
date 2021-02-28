#pragma once
/**\class Fpl_Mech_Overflow_Dike
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_MECH_OVERFLOW_DIKE_H
#define FPL_MECH_OVERFLOW_DIKE_H

//fpl-libs
#include "_Fpl_Mechanism.h"
#include "Fpl_Random_Variables.h"
#include "Fpl_Cub_Dike_Waterside.h"
#include "Fpl_Cub_Dike_Crest.h"

///Mechanism-class for the overflow over a dike section type due to a waterlevel higher than the dike crest \ingroup fpl
/**
The base formula for the overflow calculation is the Poleni-formula. To descide different types of weir type
and include factors for a dike overflow slightly change formulation are used. This formulation are given in:
	-	"Ueberfaelle und Wehre" (Guenther; vieweg 2005)
	-	"Combined Wave Overtopping and Overflow of Dikes and Seawalls" (Bleck, Schüttrumpf; 2000) [poster].
*/
class Fpl_Mech_Overflow_Dike : public _Fpl_Mechanism
{
public:
	///Default constructor
	Fpl_Mech_Overflow_Dike(void);
	///Default destructor
	~Fpl_Mech_Overflow_Dike(void);

	//members
	///Random variable of the Poleni-factor
	Fpl_Random_Variables mue_factor;
	///Random variable of the factor for the crest width
	Fpl_Random_Variables factor_crestwidth;
	///Random variable of the factor for the flow contraction
	Fpl_Random_Variables factor_flowcontraction;
	///Random variable of the factor for the waterside slope
	Fpl_Random_Variables factor_watersideslope;
	///Random variable of the factor for the landside slope
	Fpl_Random_Variables factor_landsideslope;


	///Random variable result the overflow discharge
	Fpl_Random_Variables q_overflow;

	//methods
	///Set the pointer to the waterside cubature, which is allocated in the dike-section class
	void set_waterside_cubature(Fpl_Cub_Dike_Waterside *waterside_cubature);
	///Set the pointer to the landside cubature, which is allocated in the dike-section class
	void set_landside_cubature(Fpl_Cub_Dike_Landside *landside_cubature);
	///Set the pointer to the crest cubature, which is allocated in the dike-section class
	void set_crest_cubature(Fpl_Cub_Dike_Crest *crest_cubature);
	//Set the pointer to the crest cubature, which is allocated in the dike-section class
	//void set_landside_cubature(Fpl_Cub_Dike_Landsie *landside_cubature);
	///Transfer the overflow-mechanism data to database, e.g. geometry, random variables etc.
	void transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database);

	///Set the random variables of the results for the overflow mechanism per database table
	void set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random input variables per database table
	void set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random input variables per file
	void set_variables(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start);


	///Output members to display/console
	void output_members(void);

	///Calculate the mechanism; main input parameter is the waterlevel before the dike section
	double calculate_mechanism(const bool random_calculation, const double waterlevel);
	
	///Check the statistic of the random variables
	void check_statistic(void);
	///Reset the statistic important values
	void reset_statistics(void);
	///Output the statistics of the random variables to display/console
	void output_statistic(void);
	///Output the deterministic results to display/console in case of an determistic calculation
	void output_determ_results(void);

private:
	//members
	///Pointer to the geometrical waterside cubature of the dike; it is managed in the dike section class.
	Fpl_Cub_Dike_Waterside *waterside_cubature;
	
	///Pointer to the geometrical landside cubature of the dike; it is managed in the dike section class.
	Fpl_Cub_Dike_Landside *landside_cubature;

	///Pointer to the geometrical crest cubature of the dike; it is managed in the dike section class.
	Fpl_Cub_Dike_Crest *crest_cubature;
	///Horizontal line representing the relevant waterlevel (waterlevel-2.0*delt_h)
	Geo_Straight_Line crest_minus_2delta_h;
	///Store the number of the segment, which was intercepted an random simulation run before, for the sake of time-saving by an new interception search
	int no_segment_interception;

	///Delta h, waterlevel-crest_height
	double delta_h;
	///Overflow discharge buffer
	double q_overflow_buff;
	///Poleni factor (mue) buffer
	double mue_buff;
	///Buffer for the crest width factor
	double factor_crestwidth_buff;
	///Buffer for the flow contraction factor
	double factor_flowcontraction_buff;
	///Buffer for the waterside solpe factor
	double factor_watersideslope_buff;
	///Buffer for the landsideslope factor
	double factor_landsideslope_buff;
	///Buffer for the contraction height
	double contraction_height;
	///Waterside slope
	double slope_water_side;
	//Landside slope
	double slope_land_side;
	///Delta h to width factor
	double delta_h2width;


	//method
	///Reset the random generated flags of the random variables
	void reset_random_flag(void);
	///Reset the buffer variables
	void reset_buffer_variables(void);
	///Generate the line of the relevant waterlevel (dike_crest-2*delta_h)
	void make_relevant_interception_line(void);
	///Decide the poleni-factor (mue) calculation for the ccorrespodnig weir type
	void decide_mue_calculation(const bool random_calculation);
	///Calculate the poleni-factor (mue) for a sharp-crested weir
	void calculate_sharp_weir_mue(void);
	///Calculate the poleni-factor (mue) for a short-crested weir
	double calculate_short_weir_mue(void);
	///Calculate the poleni-factor (mue) for a broad-crested weir
	double calculate_broad_weir_mue(void);
	///Calculate additional factors for a broad weir formula 
	void calculate_broad_weir_factors(const bool random_calculation);
	///Interpolate the poleni-factor (mue) between the weir types (short and broad crested weir)
	void interpolate_weir_types_mue(void);
	///Calculate the overflow discharge
	void calculate_q_overflow(void);
};
#endif