#pragma once
/**\class Fpl_Mech_Wave_Runup
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_MECH_WAVE_RUNUP_H
#define FPL_MECH_WAVE_RUNUP_H

//libs
#include "_Fpl_Mechanism.h"
#include "Fpl_Cub_Dike_Waterside.h"

//struct
///Dat structure for the wave runup results \ingroup fpl
struct _fpl_wave_runup_result{
	///Wave runup height [m]
	double wave_runup;
	///Wave runup discharge [m³/s per m]
	double wave_overtopping;
};
///Mechanism-class for the wave runup induced by waves for a dike section type \ingroup fpl
/**
The base formula of this mechanism are described in detail in "Technical report Wave Run-up and Wave 
overtopping at dikes" (TAW, the Netherlands, 2002). This mechanism is applicable to a dike section. 
The geometrical data important for the calculation of the wave runup are stored in the pointer Fpl_Cub_Dike_Waterside. 
The waterside cubature is managed in Fpl_Sec_Type_Dike.
The wind iduced wave parameters are resulting parameters of the wind wave-generation mechanism (Fpl_Mech_Wave_Generation) and 
also main input varaibles of this mechanism.
*/
class Fpl_Mech_Wave_Runup : public _Fpl_Mechanism
{
public:
	///Default constructor
	Fpl_Mech_Wave_Runup(void);
	///Default destructor
	~Fpl_Mech_Wave_Runup(void);

	//members
	///Random variable of the reduction factor due to the wave direction
	Fpl_Random_Variables gamma_wavedirection;
	///Random variable of the mid gradient of the waterside slope
	Fpl_Random_Variables mid_gradient_waterside;
	///Random variable of the results variable, the wave runup height
	Fpl_Random_Variables height_runup;
	///Random variable of the results variable, the wave runup overtopping discharge
	Fpl_Random_Variables discharge_overtopping;
	
	///Horizontal line representing the relevant waterlevel (waterlevel-1.5*hm)
	Geo_Straight_Line waterline_minus_hm;

	///Pointer to the geometrical waterside cubature of the dike; it is managed in the dike section class.
	Fpl_Cub_Dike_Waterside *waterside_cubature;
	

	//methods
	///Set the pointer to the waterside cubature, which is allocated in the dike-section class
	void set_waterside_cubature(Fpl_Cub_Dike_Waterside *waterside_cubature);
	///Set the random variables of the results for the wave runup mechanism per database table
	virtual void set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random input variables per database table
	virtual void set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random input variables as well as the geometrical data per file
	void set_input(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start);

	///Transfer the wave-runup generation data to database, e.g. geometry, random variables etc.
	virtual void transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database);

	///Output members to display/console
	void output_members(void);
	
	///Calculate the mechanism; main input parameter are the wave condition due to the wind machanism as well as the geometrical waterside cubature
	_fpl_wave_runup_result calculate_mechanism(const bool random_calculation, const double wind_direction, const double section_direction, double wave_height, double wave_period, const double wave_length, const double waterlevel);

	///Check the statistic of the random variables
	void check_statistic(void);
	///Reset the statistic important values
	void reset_statistics(void);
	///Output the statistics of the random variables to display/console
	void output_statistic(void);
	///Output the deterministic results to display/console in case of an determistic calculation
	void output_determ_results(void);

	///Get a pointer to the mid breaker factor
	double *get_ptr_mid_breaker_fac(void);

protected:
	//members


	///Store the number of the segment, which was intercepted an random simulation run before, for the sake of time-saving by an new interception search
	int no_segment_intercept;
	///The local wave direction
	double local_wave_direction;
	///The free-board height
	double free_board;
	///The real freeboard height
	double real_free_board;
	///The wave steepness (s0)
	double wave_steepness;
	///The breaker factor (xsi0)
	double breaker_factor;

	///The reduction factor for the berms (gamma berm)
	double gamma_berms;

	//method
	///Reset the random generated flags of the random variables
	void reset_random_flag(void);
	///Calculate the angle of wave attack, from which the reduction factor for the wave direction is determined
	double calculate_gamma_wavedirection(const double wind_direction, const double section_direction, double *wave_height, double *wave_period);
	///Generate the line of the relevant waterlevel (waterlevel-1.5*hm)
	void make_line_relevant_waterlevel(const double waterlevel, const double wave_height);
	///Calculate the breaker factor and the wave steepness
	void calculate_breaker_factor(const double wave_height, const double wave_period);
	///Calculate the wave runup height
	double calculate_runup(const double wave_height, const double roughness_gamma);
	///Calculate the overtopping discharge
	virtual double calculate_overtopping_discharge(const double wave_height, const double roughness_gamma);
	///Interpolate the mid gradient in case of freeboard < 0 (waterlevel over crest) for a smooth transition
	void interpolate_mid_gradient_overcrest(double *mid_gradient, const double wave_height);

};
#endif