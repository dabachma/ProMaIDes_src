#pragma once
/**\class Fpl_Cub_Waterside_Materialzone
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2012 
*/
#ifndef FPL_CUB_WATERSIDE_MATERIALZONE_H
#define FPL_CUB_WATERSIDE_MATERIALZONE_H

///Structure to summarize the random variables for artificial revetments
struct _fpl_rand_artificial_revetment{
	///Random variables of the bulk density of the cover material (artificial revetment) for wave impact and erosion 
	Fpl_Random_Variables bulk_density;
	///Random variables of the porosity of the cover material (artificial revetment) for wave impact and erosion
	Fpl_Random_Variables porosity;
};
///Structure to summarize the random variables of the wave impact for artificial revetments (Pilarczyek 1998)
struct _fpl_rand_artificial_revetment_wave_impact{
	///Random variables of the stability factor f for the revetment (artificial revetment) for wave impact
	Fpl_Random_Variables stability_f_variables;
	///Random variables of the exponent x for the revetment (artificial revetment) for wave impact
	Fpl_Random_Variables exponent_x_variables;
};

///Structure to summarize the random variables of the erosion for artificial revetments
struct  _fpl_rand_artificial_revetment_erosion{
	///Random variables of the stability, where the flow regime, the stability and the shields parameter is included (Pilarczyek 1998) (erosion start mechanism)
	Fpl_Random_Variables stability_total_revetment;
	///Random variables of the stability for the friction angle (Pilarczyek 1998) (erosion start mechanism)
	Fpl_Random_Variables friction_angle;
	///Random variables of the stability for the stability of the underlaying cover (erosion mechanism)
	Fpl_Random_Variables stability_under_layer;
	///Random variables of the stability for thickness of the underlaying cover (erosion mechanism)
	Fpl_Random_Variables thickness_under_layer;

};
///Structure to summarize the random variables for grass cover
struct _fpl_rand_grass_cover{
	///Random variables of the root zone thickness (grass revetment) for wave impact and erosion
	Fpl_Random_Variables root_zone_thickness_variables;
	///Random variables of the crack thickness (grass revetment) for wave impact and erosion
	Fpl_Random_Variables crack_thickness_variables;
};
///Structure to summarize the random variables of erosion for grass cover
struct _fpl_rand_grass_cover_erosion{
	///Random variables of the grass cover quality (grass revetment) (erosion start mechanism / erosion mechanism)
	Fpl_Random_Variables grass_quality;
	///Random variables of the stability for the stability of the cover under the grass root zone (erosion mechanism)
	Fpl_Random_Variables stability_under_layer;
};
///Structure to summarize the random variables of the wave impact for grass cover
struct _fpl_rand_grass_cover_wave_impact{
	///Random variables of the undrained cohesion (grass revetment) for wave impact
	Fpl_Random_Variables undrained_cohesion_variables;
	///Random variables of the root cohesion (grass revetment) for wave impact
	Fpl_Random_Variables root_cohesion_variables;

};

//system fpl
#include "Fpl_Random_Variables.h"

///Class for relevant material variables of the waterside \ingroup fpl
/**

*/
class Fpl_Cub_Waterside_Materialzone
{
public:
	///Default constructor
	Fpl_Cub_Waterside_Materialzone(void);
	///Default destructor
	~Fpl_Cub_Waterside_Materialzone(void);

	//members
	///Random variable of the segment roughness (wave runup)
	Fpl_Random_Variables roughness;
	///Random variables of the cover thickness (artificial revetment and gross cover) for wave impact and erosion
	Fpl_Random_Variables cover_thickness_variables;
	
	
	///Random variables for the artificial revetments 
	_fpl_rand_artificial_revetment *rand_arti_revet;
	///Random variables for the artificial revetments for wave impact
	_fpl_rand_artificial_revetment_wave_impact *rand_arti_revet_imp;
	///Random variables for the artificial revetments for erosion
	_fpl_rand_artificial_revetment_erosion *rand_arti_revet_ero;

	///Random variables for grass cover 
	_fpl_rand_grass_cover *rand_grass;
	///Random variables for grass cover for wave impact
	_fpl_rand_grass_cover_wave_impact *rand_grass_cover_imp;
	///Random variables for grass cover for erosion
	_fpl_rand_grass_cover_erosion *rand_grass_cover_ero;


	//method

	///Set the variable of the material variable zone via database
	void set_input(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database, const int zone_id, const bool runup, const bool wave_impact, const bool erosion);
	///Transfer the slope stability-mechanism random variables data to database
	void transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database);
	///Read the material variable zone per file
	void read_matzone_per_file(QFile *ifile, int *line_counter, const bool runup, const bool wave_impact, const bool erosion);

	///Get the zone identifier
	int get_zone_id(void);

	

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

	///Get the grass cover flag
	bool get_grass_cover_flag(void);



private:
	//member
	///Zone identifier
	int zone_id;

	
	///Flag if the variables for the wave runup are required
	bool runup_requiered;
	///Flag if the variables for the wave impact are required
	bool wave_impact_required;
	///Flag if the variables for the erosion are required
	bool erosion_required;
	///Flag if it is a artificial revetment or a grass cover
	bool grass_cover;



	//method
	///Find the start of an input block of the fpl-section in file
	qint64 find_start_block_file(QFile *ifile, int *line_counter, const string begin, const string end, const bool must_found);
	///Find the key-values for the file input of the material variable zone data
	void find_key_values_file(const string myline, int *must_found_counter);

	///Allocate the random variables of the wave impact
	void allocate_zone_material_wave_impact(void);
	///Delete the random variables of the wave impact
	void delete_zone_material_wave_impact(void);

	///Allocate the random variables of the erosion
	void allocate_zone_material_erosion(void);
	///Delete the random variables of the erosion
	void delete_zone_material_erosion(void);


	///Set the error(s)
	Error set_error(const int err_type);
};
#endif

