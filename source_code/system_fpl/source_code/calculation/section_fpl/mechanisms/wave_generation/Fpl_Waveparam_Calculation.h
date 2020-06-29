#pragma once
/**\class Fpl_Waveparam_Calculation
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_WAVEPARAM_CALCULATION_H
#define FPL_WAVEPARAM_CALCULATION_H

//fpl-libs
#include "Fpl_Local_Wind_Fetch.h"
//system sys libs
#include "Common_Const.h"

//struct
struct _fpl_waveparam_result{
	double wave_height;
	double wave_period;
	double wave_length;
	double mean_waterdepth;
	double wave_setup;
	double wind_direction;
	double wave_height_original;
};

///Class for calculate the wind induced wave generation with the KrylowII algorithm \ingroup fpl
/**
Detailed information about the KrylowII algoortihm can be find in DVWK246 /1997
*/
class Fpl_Waveparam_Calculation
{
public:
	///Default constructor
	Fpl_Waveparam_Calculation(void);
	///Default destructor
	~Fpl_Waveparam_Calculation(void);


	//members
	///Wave height parameter as random variable
	Fpl_Random_Variables wave_height;
	///Wave length parameter as random variable
	Fpl_Random_Variables wave_lenght;
	///Wave period parameter as random variable
	Fpl_Random_Variables wave_period;
	///Mean depth parameter as random variable
	Fpl_Random_Variables mean_depth;
	///Static wave setup parameter as random variable
	Fpl_Random_Variables wave_setup;

	///m-factor for the gradient of the correlation line between wave height and wave period (wp=m*(wh)^e+b) in case of user defined parameters
	Fpl_Random_Variables m_corr_fac;
	///e-factor for the exponent of the correlation line between wave height and wave period (wp=m*(wh)^e+b) in case of user defined parameters
	Fpl_Random_Variables e_corr_fac;
	///b-factor for the y-interception of the correlation line between wave height and wave period (wp=m*(wh)^e+b) in case of user defined parameters
	Fpl_Random_Variables b_corr_fac;
	///Goda factor for the probabilistic modelling of the goda-factor for wave reduction in shallow waters
	Fpl_Random_Variables goda_fac;


	//method
	///Set the random variables for the results via database table
	void set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, const bool user_defined);
	///Set the random input variables per database table
	void set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random input variables per file
	void set_variables(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start, const bool user_defined);

	///Calculate the required wave parameter with user defined values
	_fpl_waveparam_result calculate_user_defined_params(const bool random_calculation, const double waterlevel, Fpl_Dike_Geo_Foreland *foreland);
	///Calculate the required wave parameter with the KrylowII algorithm
	_fpl_waveparam_result calculate_krylow_params(const bool random_calculation, const double wind_direction,  const double section_direction, const double windspeed, Fpl_Wind_Fetch *global_fetches, const double waterlevel);

	///Transfer the wind-wave result data (KrylowII) to database, e.g. geometry, random variables etc.
	void transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database);

	///Output members to display/console
	void output_member(void);

	///Check the statistic of the random variables
	void check_statistic(void);
	///Reset the statistic important values
	void reset_statistics(void);
	///Output the statistics of the random variables to display/console
	void output_statistic(void);

	///Output the found local fetches (part of the given fetches), which are used for the simulation run
	void output_found_localfetches(void);

	///Reset the random generated flags of the random variables
	void reset_random_flag(void);
	

private:
	//members
	///Pointer to the local fetch set corresponding to a wind direction
	Fpl_Local_Wind_Fetch *local_fetches;
	///Number of local fetch set allocated 
	int number_local_direction;
	
	///Flag if the calculation is performed by user defined parameters
	bool user_defined;

	///Mean depth in the foreland
	double mean_depth_foreland;
	///Mean foreland slope
	double mean_slope_foreland;


	//methods
	///Search for an already allocated local fetch set to the corresponding winddirection and return the index
	int find_local_fetches(const double wind_direction, const double section_direction, Fpl_Wind_Fetch *global_fetches);
	///Generate a new local fetch set, if no local fetches for the corresponding winddirection is found
	void renew_local_fetches(void);

	///Calculate the mean depth in the foreland and the mean foreland slope
	void calc_mean_foreland_depth(Fpl_Dike_Geo_Foreland *foreland, const double wave_length);

	///Calculate the wave length iterativly (implicit formula)
	double iterate_wavelength(const double wave_period, const double mean_waterdepth);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif