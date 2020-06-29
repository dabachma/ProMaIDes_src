#pragma once
/**\class Fpl_Local_Wind_Fetch
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_LOCAL_FETCH_KRYLOW_H
#define FPL_LOCAL_FETCH_KRYLOW_H

//libs 
#include "Fpl_Wind_Fetch.h"

///Data structure storing the parameters of one fetch \ingroup fpl
struct _fpl_local_wind_fetch_param{
	///Spectral factor
	double a_angle;
	///Orthogonal length of the fetch
	double ortho_length;
	///Identifier of the global fetch zone 
	int index_variable;
	///Flag if the fetch is dry
	bool dry_flag;
};

///Class representing a set of fetches for the wind-wave generation mechanism \ingroup fpl
/**
This set of fetches is a selction of the user defined fetches of the fpl-section. Each set of fetches
has a corresponding wind direction. With the help of this fetches the mechanism calculates
the wave-generation, e.g. wave-height, -length etc. Different approaches can be used, like the KrolowII mechanism.
They are dynamically produced during the calculation process depending to randomly distributed the wind-direction. 
Further information are given in e.g. DVWK 246 (1997).
\see Fpl_Waveparam_Calculation
*/
class Fpl_Local_Wind_Fetch
{
///Class which manages the local fecthes for the wind-wave calculation
friend class Fpl_Waveparam_Calculation;

public:
	///Default constructor
	Fpl_Local_Wind_Fetch(void);
	///Copy constructor
	Fpl_Local_Wind_Fetch(const Fpl_Local_Wind_Fetch& object);
	///Default destructor
	~Fpl_Local_Wind_Fetch(void);

	//method
	///Get the wind direction as identifier
	double give_wind_direction(void);
	///Give the number of fetches in the set
	int give_number_fetches(void);
	///Get the number of relevant fetches in the set
	int get_number_relevant_fetches(void);
	///Set a new local fetch set
	void set_fetches(const double wind_direction, const double section_direction, Fpl_Wind_Fetch *global_fetches);

	///Output the members to display/console
	void output_members(void);

	///Copy operator (required for the dynamic re-allocation of the local fetches)
	Fpl_Local_Wind_Fetch& operator=(const Fpl_Local_Wind_Fetch& object);

private:
	//members 
	///Pointer to the fetches in the local fetch set
	_fpl_local_wind_fetch_param *fetch_param;
	///Winddirection of this local fetch set
	double wind_direction;
	///Anticlockwise fetchend (local 0°) calculated with winddirection-90°
	double anticlockwise_fetchend_global; 
	///Clockwise fetchend (local 180°) calculated with winddirection+90°
	double clockwise_fetchend_global;
	///Number of fetches in the set
	const int number_of_fetches;
	///Number of relevant fetches
	int number_rel_fetches;
	///Angle of the section bank (anticlockwise)
	double anticlockwise_bank_local;
	///Angle of the section bank (clockwise)
	double clockwise_bank_local;

	//method
	///Set the fetches of one set from the given global (user-defined) fecthes
	void set_fetch_param(Fpl_Wind_Fetch *global_fetches);

	///Set the error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif