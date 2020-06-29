#pragma once
/**\class Fpl_Mech_Piping_Lane_Dike
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_MECH_PIPING_LANE_DIKE_H
#define FPL_MECH_PIPING_LANE_DIKE_H

//system fpl
#include "_Fpl_Mechanism.h"

///Mechanism-class for a piping mechanism in the dike underground calculated via the Lane formula \ingroup fpl
/**


*/
class Fpl_Mech_Piping_Lane_Dike : public _Fpl_Mechanism
{
public:
	///Default constructor
	Fpl_Mech_Piping_Lane_Dike(void);
	///Default destructor
	~Fpl_Mech_Piping_Lane_Dike(void);

	//members
	///Result value as random variable: resistance
	Fpl_Random_Variables resistance;

	//method
	///Set the random variables of the result members
	void set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random input variables per database table
	void set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random input variables per file
	void set_variables(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start);

	///Transfer the piping-mechanism (lane) data to database, e.g. geometry, random variables etc.
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

	///Calculate the mechanism; main input parameter is the waterlevel before the dike section
	int calculate_mechanism(const bool random_calculation, const double waterlevel, const double leak_length);

private:

	//members
	///Random variable of the Lane-factor [-]
	Fpl_Random_Variables lane_factor;



	///Buffer variable for the resistance
	double buff_res;
	///Buffer variable for the stress
	double buff_stress;
	///Integer for a failure (0:=no failure; 1:= failure) 
	bool failure;


	//method
	///Reset the random generated flags of the random variables
	void reset_random_flag(void);
};
#endif
