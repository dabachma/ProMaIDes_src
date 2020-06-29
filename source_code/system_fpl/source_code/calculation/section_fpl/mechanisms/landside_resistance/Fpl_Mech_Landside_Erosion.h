#pragma once
/**\class Fpl_Mech_Landside_Erosion
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2012 
*/
#ifndef FPL_MECH_LANDSIDE_EROSION_H
#define FPL_MECH_LANDSIDE_EROSION_H

//system fpl
#include "_Fpl_Mechanism.h"


///Mechanism-class for the landside resistance\ingroup fpl
/**


*/


class Fpl_Mech_Landside_Erosion : public _Fpl_Mechanism
{
public:
	///Default constructor
	Fpl_Mech_Landside_Erosion(void);
	///Default destructor
	~Fpl_Mech_Landside_Erosion(void);


	//members
	///Result value as random variable: resistance
	Fpl_Random_Variables resistance;
	///Result value as random variable: stress
	Fpl_Random_Variables stress;

	///Randomvariable of the critical discharge
	Fpl_Random_Variables critical_discharge;


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

	///Calculate the mechanism; main input parameter is discharge over the landside slope
	bool calculate_mechanism(const bool random_calculation, const double q_current);

private:
	
	//members

	///Buffer variable for the resistance
	double buff_res;
	///Buffer variable for the stress
	double buff_stress;
	///Flag for a failure 
	bool failure;


	//method
	///Reset the random generated flags of the random variables
	void reset_random_flag(void);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);


};
#endif

