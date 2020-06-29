#pragma once
/**\class Fpl_Dike_Var_Materialzone
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/
#ifndef FPL_DIKE_VAR_MATERIAL_ZONE_H
#define FPL_DIKE_VAR_MATERIAL_ZONE_H





//system fpl
#include "Fpl_Random_Variables.h"

///Class for relevant material variables of the slope mechanism representing the dike zones\ingroup fpl
/**
\see Fpl_Mech_Slope_Stability_Dike
*/
class Fpl_Dike_Var_Materialzone
{
public:
	///Default constructor
	Fpl_Dike_Var_Materialzone(void);
	///Default destructor
	~Fpl_Dike_Var_Materialzone(void);


	//members
	///Random variable of the grain density
	Fpl_Random_Variables grain_density;
	///Random variable of the porosity
	Fpl_Random_Variables porosity;
	///Random variable of the water content
	Fpl_Random_Variables water_content;

	///Random variable of the angle of friction
	Fpl_Random_Variables angle_friction;
	///Random variable of the cohesion
	Fpl_Random_Variables cohesion;

	///Random variable of the grain size of 10 percent
	Fpl_Random_Variables d_10;
	///Random variable of the grain size of 60 percent
	Fpl_Random_Variables d_60;
	///Random variable of the grain size of 70 percent
	Fpl_Random_Variables d_70;


	//method

	///Set the variable of the material variable zone via database
	void set_input(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database, const int zone_id, const bool slope_required, const bool grain_size_required);
	///Transfer the slope stability-mechanism random variables data to database
	void transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database);
	///Read the material variable zone per file
	void read_matzone_per_file(QFile *ifile, int *line_counter, const bool slope_required, const int index_perm_layer);

	///Get the zone identifier
	int get_zone_id(void);

	///Calculate the densities
	void calc_densities(const double water_density);

	///Get the uplift density of the soil under water
	double get_uplift_density(void);
	///Get the wet density of the soil
	double get_wet_density(void);


	///Reset the statistics of the random variables of the zone variables to display/console
	void reset_statistic(void);
	///Check the statistics of the random variables of the zone variables to display/console
	void check_statistic(void);
	///Output the statistics of the random variables of the zone variables to display/console
	void output_statistic(void);

	///Output the results of a deterministic calculation to display/console
	void output_determ_results(void);

	///Reset the random generated flag
	void reset_random_flag(void);

	///Reset the random generated flag
	void generate_random_variables(const bool random_calculation);

	///Output the material variable zone to display/console
	void output_member(void);

	///Calculculate the tangens of the friction angle
	void calc_tan_friction(void);

	///Get the tangens friction angle
	double get_tan_friction_angle(void);


private:
	//member
	///Zone identifier
	int zone_id;

	///Uplift density of the soil under water
	double uplift_density;
	///Wet density of the soil
	double wet_density;

	///Tangens of the friction angle
	double tan_friction;

	///Flag if the variables for the slope mechanism are required
	bool slope_required;
	///Flag if the variables for the grain size distribution are required
	bool grain_size_required;

	//method
	///Find the start of an input block of the fpl-section in file
	qint64 find_start_block_file(QFile *ifile, int *line_counter, const string begin, const string end, const bool must_found);
	///Find the key-values for the file input of the material variable zone data
	void find_key_values_file(const string myline, int *must_found_counter);

	///Check the grain size distribution if no permeable layer is set
	void check_grain_size_distribution(void);

	///Set the error(s)
	Error set_error(const int err_type);


};
#endif
