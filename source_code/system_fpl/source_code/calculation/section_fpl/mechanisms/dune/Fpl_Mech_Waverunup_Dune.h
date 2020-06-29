#pragma once
/**\class Fpl_Mech_Waverunup_Dune
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2012 
*/
#ifndef FPL_MECH_WAVE_RUNUP_DUNE_H
#define FPL_MECH_WAVE_RUNUP_DUNE_H

//libs
#include "Fpl_Mech_Waverunup_Dike.h"

///Mechanism-class for the wave runup induced by waves for a dune section type \ingroup fpl
/**

*/
class Fpl_Mech_Waverunup_Dune : public Fpl_Mech_Wave_Runup
{
public:
	///Default constructor
	Fpl_Mech_Waverunup_Dune(void);
	///Default destructor
	~Fpl_Mech_Waverunup_Dune(void);

	//methods
	///Set the random variables of the results for the wave runup mechanism per database table
	void set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random input variables per database table
	void set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);

	///Transfer the wave-runup generation data to database, e.g. geometry, random variables etc.
	virtual void transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database);

private:
	//methods
	///Calculate the overtopping discharge
	double calculate_overtopping_discharge(const double wave_height, const double roughness_gamma);

};
#endif

