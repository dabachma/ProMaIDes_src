#pragma once
/**\class Fpl_Mech_Bursting_Din_Dike
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_MECH_BURSTING_DIN_DIKE_H
#define FPL_MECH_BURSTING_DIN_DIKE_H

//system fpl
#include "_Fpl_Mechanism.h"
#include "Fpl_Dike_Var_Materialzone.h"
#include "Fpl_Bursting_Slice.h"

///Mechanism-class for a bursting mechanism at the landside base point of a dike section after the DIN 19712\ingroup fpl
/**
Further information are provided in DIN 19712 "Flussdeiche".

*/
class Fpl_Mech_Bursting_Din_Dike : public _Fpl_Mechanism
{
public:
	///Default constructor
	Fpl_Mech_Bursting_Din_Dike(void);
	///Default destructor
	~Fpl_Mech_Bursting_Din_Dike(void);

	//members
	///Result value as random variable: resistance
	Fpl_Random_Variables resistance;
	///Result value as random variable: stress
	Fpl_Random_Variables stress;


	//method
	///Set the random variables of the result members
	void set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id);
	///Set the random input variables per database table
	void set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, Fpl_Dike_Var_Materialzone *zone);
	///Set the random input variables per file
	void set_variables(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start, Fpl_Dike_Var_Materialzone *zone);

	///Transfer the bursting-mechanism (DIN 19712) data to database, e.g. geometry, random variables etc.
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
	bool calculate_mechanism(const bool random_calculation, const double waterlevel, const double water_density, Fpl_Seepage_Line_Point_List *seepage_list);

	///Set the fixed leakage length
	void set_fixed_leak_length(const double length);
	///Get total leakage length
	double get_total_leak_length(void);

	///Set the pointer to the impermeable layer material variable zone
	void set_ptr_impermeable_layer(Fpl_Dike_Var_Materialzone *zone);

	///Set the pointer and the numbner of geometrical material zone(s) of the section
	void set_ptr_no_material_zone(const int number, Fpl_Dike_Geo_Materialzone *ptr_mat_zone);

	///Calculate geometrical values and sclice für the bursting calculation in case of available material zone(s)
	double calculate_geometrical_values(Fpl_Section_Points *ptr_base_land, Fpl_Section_Points *ptr_base_water, Fpl_Section_Points *ptr_crest_land, Fpl_Dike_Geo_Hinterland *ptr_hinterland, Fpl_Seepage_Line_Point_List *seepage_list, Geo_Polysegment *outer_polysegment);

private:
	//members
	///Fixed leakage length (dike waterside base to dike landside base)
	double fixed_leak_length;
	///Randomvariable of the variable part of the leakage length
	Fpl_Random_Variables variable_leak_length;
	///Total leakage length (fixed + variable)
	double total_leak_length;

	///Pointer to the material variable zone for the impermeable layer
	Fpl_Dike_Var_Materialzone *impermeable_zone;

	///Gradient for calculation of the loss of the hydraulic head [m/m]
	Fpl_Random_Variables hydraulic_head_loss;

	///Size of the impermeable layer [m]
	Fpl_Random_Variables size_imperm_layer;
	///Density of the material of the impermeable layer [kg/m³]
	Fpl_Random_Variables dens_imperm_layer;

	///Buffer variable for the resistance
	double buff_res;
	///Buffer variable for the stress
	double buff_stress;
	///Flag for a failure 
	bool failure;

	///Pointer to the geometrical zone of the section
	Fpl_Dike_Geo_Materialzone *ptr_mat_zone;
	///Number of geometrical zone(s)
	int number_mat_zone;

	///Slices for the bursting calculation
	Fpl_Bursting_Slice *slices;
	///Number of bursting slices
	int number_slices;


	//method
	///Reset the random generated flags of the random variables
	void reset_random_flag(void);

	///Allocate the slices of bursting
	void allocate_bursting_slices(void);
	///Delete the slices of bursting
	void delete_bursting_slices(void);

	///Calculate the mechanism with slices
	bool calculate_mecha_with_slice(const bool random_calculation, const double waterlevel, const double water_density, Fpl_Seepage_Line_Point_List *seepage_list);

	///Calculate the mechanism without slices
	bool calculate_mecha_without_slice(const bool random_calculation, const double waterlevel, const double water_density);


	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
	


};
#endif
