#pragma once
/**\class Fpl_Mech_Wave_Generation
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_MECH_WAVE_GENERATION_H
#define FPL_MECH_WAVE_GENERATION_H

//fpl-libs
#include "_Fpl_Mechanism.h"
#include "Fpl_Wind_Fetch.h"
#include "Fpl_Waveparam_Calculation.h"

///Mechanism-class for the wave generation induced by wind forces \ingroup fpl
/**
This mechansim is independent from the section type. It can be used for all section types. 
One algorithm for calculate the wave generation is after KrylowII (Fpl_Waveparam_Calculation).
Other algorithm can be implemented.
*/
class Fpl_Mech_Wave_Generation : public _Fpl_Mechanism
{
public:
	///Default constructor
	Fpl_Mech_Wave_Generation(void);
	///Constructor, where the section direction is given
	Fpl_Mech_Wave_Generation(const double direction);
	///Default destructor
	~Fpl_Mech_Wave_Generation(void);

	//member
	///Wave parameter calculator
	Fpl_Waveparam_Calculation wave_calculator;

	//method
	///Set the fetches (the geometric parameters) via database table 
	void new_set_fetch(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random variable of the wind-speed-direction via database table
	void set_windspeed_direction(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random variables for the results via database table
	void set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random input variables per database table
	void set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random input variables as well as the geometrical data per file
	void set_input(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start);


	///Set the section direction
	void set_section_direction(const double direction);

	///Transfer the wind-wave generation data to database, e.g. geometry, random variables etc.
	void transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database);


	///Output members to display/console
	void output_members(void);

	///Calculate the relevant wave parameters caused by a given wind speed and direction
	_fpl_waveparam_result calculate_wind_wave_generation(const bool random_calculation, const double waterlevel);

	
	///Check the statistic of the random variables
	void check_statistic(void);
	///Reset the statistic important values
	void reset_statistics(void);
	///Output the statistics of the random variables to display/console
	void output_statistic(void);

	///Output the deterministic results to display/console in case of an determistic calculation
	void output_determ_results(void);

	///Set the pointer to the foreland
	void set_ptr_foreland(Fpl_Dike_Geo_Foreland *foreland);

private:
	//members
	///Geometrical wind fetches
	Fpl_Wind_Fetch my_fetch;
	///Random variable of the wind-speed-direction
	Fpl_Random_Variables wind_speed_direction;

	///Orthogonal direction of the water side (north is zero)
	double section_direction;
	///Section direction minus 90°
	double anticlockwise_water_bank;
	///Section Direction plus 90°
	double clockwise_water_bank;
	///Result members for a KrylowII calculation
	_fpl_waveparam_result determ_result;

	///Flag if the wave parameters are generated or user defined
	bool user_defined;

	///Pointer to the foreland
	Fpl_Dike_Geo_Foreland *ptr_foreland;

	///Corrected wind/wave direction
	double *corrected_w_dir;
	///Number of corrected wind/wave direction
	int number_corr_w_dir;

	//methods
	///Calculate user difined wave parameter
	_fpl_waveparam_result calculate_user_defined(const bool random_calculation, const double waterlevel);
	///Calculate the mechanism; here the method after KrylowII is applied 
	_fpl_waveparam_result calculate_krylowII(const bool random_calculation, const double waterlevel, _fpl_result *wind);
	//...implement further algorithm


	///Allocate the corrected wind/wave direction
	void alloc_corr_w_dir(void);
	///Delete the corrected wind/wave direction
	void delete_corr_w_dir(void);

	///Check the wind direction
	double check_wind_direction(const bool random, const double direction);

	///Reset the random generated flags of the random variables
	void reset_random_flag(void);

	///Set error(s)
	Error set_error(const int err_type);
};
#endif