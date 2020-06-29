#pragma once
/**\class Fpl_Mech_Piping_Sell_Dike
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_MECH_PIPING_SELL_DIKE_H
#define FPL_MECH_PIPING_SELL_DIKE_H

//system fpl
#include "_Fpl_Mechanism.h"
#include "Fpl_Dike_Var_Materialzone.h"

///Mechanism-class for a piping mechanism in the dike underground calculated via the Sellmeijer formula \ingroup fpl
/**


*/
class Fpl_Mech_Piping_Sell_Dike : public _Fpl_Mechanism
{
public:
	///Default constructor
	Fpl_Mech_Piping_Sell_Dike(void);
	///Default destructor
	~Fpl_Mech_Piping_Sell_Dike(void);

	//members
	///Result value as random variable: resistance
	Fpl_Random_Variables resistance;
	///Size of the permeable layer in the underground of the dike section [m]
	Fpl_Random_Variables size_perm_layer;
	///Graindiameter 70 % of the grain distribution (d_70) [m] 
	Fpl_Random_Variables d_70;

	//method
	///Set the random variables of the result members
	void set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random input variables per database table
	void set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, Fpl_Dike_Var_Materialzone *zone);
	///Set the random input variables per file
	void set_variables(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start, Fpl_Dike_Var_Materialzone *zone);

	///Transfer the piping-mechanism (Sellmeijer) data to database, e.g. geometry, random variables etc.
	void transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database, Fpl_Dike_Var_Materialzone *zone);


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
	int calculate_mechanism(const bool random_calculation, const double waterlevel, const double water_density, const double leak_length);


	///Set the pointer to the permeable layer material variable zone
	void set_ptr_permeable_layer(Fpl_Dike_Var_Materialzone *zone);

private:

	//method
	///Reset the random generated flags of the random variables
	void reset_random_flag(void);

	///Buffer variable for the resistance
	double buff_res;
	///Buffer variable for the stress
	double buff_stress;
	///Integer for a failure (0:=no failure; 1:= failure) 
	bool failure;

	///Buffer for the intrinsinc permeability
	double buff_k;
	///Buffer for the c-value 
	double buff_c;
	///Buffer for the alpha value
	double buff_alpha;

	///Pointer to the material variable zone for the permeable layer
	Fpl_Dike_Var_Materialzone *permeable_zone;

	//members
	///Density of the material of the permeable layer in the underground of the dike section [kg/m³]
	Fpl_Random_Variables dens_perm_layer;
	///Rolling resistance angle [°]
	Fpl_Random_Variables rolling_resistance_angle;
	///Dragforce factor [-]
	Fpl_Random_Variables drag_force_factor;
	///Permeability (k_f) permeable layer in the underground of the dike section [m/s]
	Fpl_Random_Variables k_f_perm_layer;

	//method
	///Set warning(s)
	Warning set_warning(const int warn_type);


};
#endif
