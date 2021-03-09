#pragma once
/**\class Fpl_Sec_Type_Dike
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_SEC_TYPE_DIKE_H
#define FPL_SEC_TYPE_DIKE_H

//system fpl
#include "_Fpl_Section_Type.h"


#include "Fpl_Cub_Dike_Waterside.h"
#include "Fpl_Cub_Dike_Landside.h"
#include "Fpl_Cub_Dike_Crest.h"
#include "Fpl_Dike_Geo_Foreland.h"
#include "Fpl_Dike_Geo_Hinterland.h"

#include "Fpl_Mech_Slope_Stability_Dike.h"
#include "Fpl_Mech_Wave_Generation.h"
#include "Fpl_Mech_Waverunup_Dike.h"
#include "Fpl_Mech_Overflow_Dike.h"
#include "Fpl_Mech_Bursting_Din_Dike.h"
#include "Fpl_Mech_Piping_Sell_Dike.h"
#include "Fpl_Mech_Piping_Lane_Dike.h"
#include "Fpl_Mech_Piping_Schmertmann_Dike.h"
#include "Fpl_Mech_Landside_Erosion.h"
#include "Fpl_Mech_Wave_Impact_Waterside_Dike.h"
#include "Fpl_Mech_Waterside_Erosion.h"
#include "Fpl_Mech_Waterside_Erosion_Start.h"
#include "Fpl_Cub_Waterside_Materialzone.h"



#include "Fpl_Seepage_Line_Point_List.h"

#include "Fpl_Seepage_Calculator_Dike.h"


///Class for the section type of the flood-protection line as dike \ingroup fpl
/**
*/
class Fpl_Sec_Type_Dike : public _Fpl_Section_Type
{
public:
	///Default constructor
	Fpl_Sec_Type_Dike(void);
	///Default destructor
	~Fpl_Sec_Type_Dike(void);

	//members
	///Pointer to the table for storing the results of the mechanism of the dike section 
	static Tables *result_table;

	//method
	///Set the input of the dune section (geometrie, variables, faulttree) per database
	void set_input(const int section_id,  const bool frc_sim, QSqlDatabase *ptr_database);
	///Read in the fpl-section type from file
	void read_section_type_per_file(QFile *ifile, int *line_counter, const bool frc_sim);
	///Read the mechanisms of the fault-tree per file
	void read_mechanism_faulttree_perfile(QFile *ifile, int *line_counter);
	///Check which mechanisms of the fault-tree are required per file
	void check_mechanism_faulttree_perfile(QFile *ifile, int *line_counter);
	///Read the waterside geometry of the section per file
	void read_waterside_geometry(QFile *ifile, int *line_counter);
	///Read the landside geometry of the section per file
	void read_landside_geometry(QFile *ifile, int *line_counter);
	///Read the geometrical material zone(s) of the section per file
	void read_geo_matzones(QFile *ifile, int *line_counter);
	///Read the material variable zone(s) of the section per file
	void read_var_matzones(QFile *ifile, int *line_counter);

	///Read the waterside material variable zone(s) of the section per file
	void read_var_waterside_matzones(QFile *ifile, int *line_counter);

	///Transfer the section type data to database, e.g. geometry, random variables etc.
	void transfer_sectiontype2database(const int section_id, QSqlDatabase *ptr_database);

	///Calculate the fault tree with random distributed variables (true) or with deterministic variables (false)
	int make_faulttree(const bool random_calculation=false);

	///Check the statistics of the random variables used in the section type
	void check_statistic(void);
	///Output the statistics of the random variables used in the section type to display/console
	void output_statistic(void);
	///Reset the statistic important values
	void reset_statistics(void);

	///Output the section members to display/console
	void output_member(void);
	///Output the reliability of the fault tree mechanisms for a deterministic calculation to display/console
	void output_reliability(string output_folder, const int sec_id, const string sec_name);
	///Output the geometry to tecplot
	void output_geometry2tecplot(ofstream *output);
	///Output the geometry to paraview
	void output_geometry2paraview(ofstream *output);
	///Output the geometry to excel
	void output_geometry2excel(ofstream *output);
	///Output result members of the mechanisms to database table
	void output_result2table(QSqlDatabase *ptr_database, _fpl_simulation_type simulation_type, _sys_system_id id, const int section_id, const int counter_mc_sim);

	///Output the deterministic results to tecplot
	void output_determ_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id);
	///Output the deterministic results to Paraview
	void output_determ_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id);
	///Output the deterministic results to Excel
	void output_determ_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id);

	///Output the MC results to tecplot
	void output_mc_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id);
	///Output the MC results to Paraview
	void output_mc_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id);
	///Output the MC results to Excel
	void output_mc_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id);

	///Output the FRC results to tecplot
	void output_frc_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id);
	///Output the FRC results to Paraview
	void output_frc_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id);
	///Output the FRC results to Excel
	void output_frc_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id);

	///Create the database table for the results of the mechanism of the dike section 
	static void create_result_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the results of the mechanism of the dike section  
	static void delete_data_in_result_table(QSqlDatabase *ptr_database);
	///Delete a data set in the database table for the results of the mechanism of the dike section for a specific system state
	static void delete_data_in_result_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete a data set in the database table for the results of the mechanism of the dike section 
	static void delete_data_in_result_table(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const _fpl_simulation_type sim_type);
	///Set the database table for the results of the mechanism of the dike section : it sets the table name and the name of the columns and allocate them
	static void set_result_table(QSqlDatabase *ptr_database, const bool not_close=false);
    ///Select results of given fpl section in a database table
    static int select_results_in_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id, const _fpl_simulation_type sim_type);

	///Close and delete the database table for the results of the mechanism of the dike section 
	static void close_result_table(void);

	///Switch the applied-flag for the results of the mechanism of the dike section in the database table for a defined system state
	static void switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the results of the mechanism of the dike section in the database table for a defined system state and id
	static void switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id);

	///Check and set the section geometry out of the read in geometry members
	void check_set_geometry(void);

private:
	//members
	///Pointer to wave generation mechanism induced by wind for a dike section
	Fpl_Mech_Wave_Generation *wave_wind_event;
	///Pointer to wave forced runup by wind for a dike section after Eurotop-manual (2007)
	Fpl_Mech_Wave_Runup *wave_runup_event;
	///Pointer to an overflow event for a dike section after modified Poleni-formula
	Fpl_Mech_Overflow_Dike *overflow_event;
	///Pointer to an bursting event for a dike section after DIN19712
	Fpl_Mech_Bursting_Din_Dike *bursting_din_event;
	///Pointer to an piping event for a dike section after Sellmeijer
	Fpl_Mech_Piping_Sell_Dike *piping_sell_event;
	///Pointer to an piping event for a dike section after Lane
	Fpl_Mech_Piping_Lane_Dike *piping_lane_event;
	///Pointer to an piping event for a dike section after Schmertmann
	Fpl_Mech_Piping_Schmertmann_Dike *piping_schmertmann_event;
	///Pointer to an slope stability event for landside of a dike section after Krey
	Fpl_Mech_Slope_Stability_Dike *slope_stability_landside_event;
	///Pointer to an slope stability event for waterside of a dike section after Krey
	Fpl_Mech_Slope_Stability_Dike *slope_stability_waterside_event;

	///Pointer to an erosion event for landside of a dike section
	Fpl_Mech_Landside_Erosion *erosion_landside_event;
	///Pointer to an erosion event for waterside of a dike section
	Fpl_Mech_Waterside_Erosion *erosion_waterside_event;
	///Pointer to an start erosion event for waterside of a dike section
	Fpl_Mech_Waterside_Erosion_Start *erosion_waterside_start_event;
	///Pointer to an wave pressure impact event for the waterside of a dike section
	Fpl_Mech_Wave_Impact_Waterside_Dike *wave_impact_event;

	
	///Base point of the landside of the dike section
	Fpl_Section_Points base_land;
	///Crest point of the landside of the dike section
	Fpl_Section_Points crest_land;
	///The cubature of the waterside of the dike section
	Fpl_Cub_Dike_Waterside waterside_cubature;
	///The cubature of the crest of the dike section
	Fpl_Cub_Dike_Crest crest_cubature;
	///The cubature of the landside of the dike
	Fpl_Cub_Dike_Landside landside_cubature;

	///The foreland geometrie of the dike
	Fpl_Dike_Geo_Foreland foreland;
	///The hinterland geometrie of the dike
	Fpl_Dike_Geo_Hinterland hinterland;

	///Number of geometrical material zone of the dike 
	int number_material_zones;
	///Geometrical material zones of the dike
	Fpl_Dike_Geo_Materialzone *material_zones;

	///Number of material variable zone of the dike 
	int number_material_variable_zones;
	///Material variable zones of the dike
	Fpl_Dike_Var_Materialzone *material_variable_zones;

	///Number of material waterside zone
	int number_mat_waterside_zones;
	///Waterside material zone
	Fpl_Cub_Waterside_Materialzone *material_waterside_zone;

	///Polygon of the outer boundary of the dike (cubature, foreland, hinterland)
	Geo_Simple_Polygon outer_polygon;
	///Polysegment of the outer boundary of the dike (cubature, foreland, hinterland)
	Geo_Polysegment outer_polysegment;

	///Seepage point list for the dike body ascending water
	Fpl_Seepage_Line_Point_List seepage_body_ascending;
	///Seepage point list for the dike body descending water
	Fpl_Seepage_Line_Point_List seepage_body_descending;

	///Calculator class for the seepage claculation
	Fpl_Seepage_Calculator_Dike seepage_calculator;

	///Area of the section cuabture
	double area_cubature;


	///Random variable of the water density
	Fpl_Random_Variables water_density;

	///The current waterlevel for calculation
	double current_waterlevel;


	///Index of the waterlevel at the impermeable layer begin
	int index_h_imperm_layer;

	
	//methods
	///Allocate the mechanism for wave generation induced by wind 
	void allocate_wind_wave_mechanism(void);
	///Delete the mechanism for wave generation induced by wind 
	void delete_wind_wave_mechanism(void);
	///Allocate the mechanism for wave forced runup after Eurotop-manual (2007)
	void allocate_wave_runup_mechanism(void);
	///Delete the mechanism for wave forced runup after Eurotop-manual (2007)
	void delete_wave_runup_mechanism(void);
	///Allocate the mechanism for an overflow after modified Poleni-formula
	void allocate_overflow_mechanism(void);
	///Delete the mechanism for an overflow after modified Poleni-formula
	void delete_overflow_mechanism(void);

	///Allocate the mechanism for bursting after DIN19712 
	void allocate_bursting_din_mechanism(void);
	///Delete the mechanism for bursting after DIN19712 
	void delete_bursting_din_mechanism(void);
	///Allocate the mechanism for piping after Sellmeijer 
	void allocate_piping_sell_mechanism(void);
	///Delete the mechanism for piping after Sellmeijer 
	void delete_piping_sell_mechanism(void);
	///Allocate the mechanism for piping after Lane 
	void allocate_piping_lane_mechanism(void);
	///Delete the mechanism for piping after Lane 
	void delete_piping_lane_mechanism(void);
	///Allocate the mechanism for piping after Schmertmann 
	void allocate_piping_schmertmann_mechanism(void);
	///Delete the mechanism for piping after Schmertmann 
	void delete_piping_schmertmann_mechanism(void);

	///Allocate the mechanism for slope stability landside of the section after Krey 
	void allocate_slope_stability_landside_mechanism(void);
	///Delete the mechanism for slope stability landside of the section after Krey 
	void delete_slope_stability_landside_mechanism(void);
	///Allocate the mechanism for slope stability waterside of the section after Krey 
	void allocate_slope_stability_waterside_mechanism(void);
	///Delete the mechanism for slope stability waterside of the section after Krey 
	void delete_slope_stability_waterside_mechanism(void);

	///Allocate the mechanism for erosion landside of the section
	void allocate_erosion_landside_mechanism(void);
	///Delete the mechanism for erosion landside of the section 
	void delete_erosion_landside_mechanism(void);

	///Allocate the mechanism for erosion waterside of the section
	void allocate_erosion_waterside_mechanism(void);
	///Delete the mechanism for erosion waterside of the section 
	void delete_erosion_waterside_mechanism(void);

	///Allocate the mechanism for start the erosion waterside of the section
	void allocate_start_erosion_waterside_mechanism(void);
	///Delete the mechanism for start the erosion waterside of the section 
	void delete_start_erosion_waterside_mechanism(void);


	///Allocate the mechanism of wave pressure impact on the waterside of the section
	void allocate_wave_impact_mechanism(void);
	///Delete the mechanism of wave pressure impact on the waterside of the section 
	void delete_wave_impact_mechanism(void);

	///Allocate the geometrical material zones of the dike
	void allocate_material_zones(void);
	///Delete the geometrical material zones of the dike
	void delete_material_zones(void);

	///Allocate the material variable zone
	void allocate_material_variable_zone(void);
	///Delete the material variable zone
	void delete_material_variable_zone(void);

	///Allocate the waterside material variable zone
	void allocate_waterside_variable_zone(void);
	///Delete the waterside material variable zone
	void delete_waterside_variable_zone(void);


	///Set the general parameters of the section to the mechanisms
	void set_general(void);
	///Set the random variables of this section and the relevant mechanisms from database table
	void set_variables(const bool frc_sim, QSqlDatabase *ptr_database, const int section_id);
	///Set the geometry of the section and their surrounding via a database table
	void set_geometrie(QSqlDatabase *ptr_database, const int section_id);
	///Set control parameters of the mechanisms
	void set_control_parameter_mechanism(QSqlDatabase *ptr_database, const int section_id, const bool frc_sim);
	///Initialize the mechanisms
	void init_mechanism(const int section_id);
	///Set the fault tree for this section type via a database table
	void init_fault_tree(QSqlDatabase *ptr_database);
	///Decide and allocate the mechanism, which should by applied during calculation
	void decide_mechanism(const string mechanism_name, const bool application_flag, const bool applied_in_tree);
	///Set the dike geometry via database and set it to the relevant mechanisms
	void read_cubature(QSqlDatabase *ptr_database, const int section_id);
	///Set the dike material zones via database 
	void read_matzones(QSqlDatabase *ptr_database, const int section_id);
	///Set the dike material variable zones via database 
	void read_mat_variable_zones(QSqlDatabase *ptr_database, const int section_id);
	///Connect the material zones to the material variable zone
	void connect_material_zones(void);

	///Set the dike material variable zones of the waterside via database 
	void read_mat_variable_zones_waterside(QSqlDatabase *ptr_database, const int section_id);
	///Connect the material zones to the material variable zone of the waterside
	void connect_material_zones_waterside(void);
	
	///Create the geometrical outer boundary of the dike as polysegment
	void make_geometrical_outer_dike_polysegment(void);
	///Create the geometrical outer boundary of the dike as polygon
	void make_geometrical_outer_dike_polygon(void);
	///Calcualte the area of the cubature
	void calc_area_cubature(void);

	///Check the initialized fault-tree
	void check_fault_tree(void);

	///Check the gradient of a dike for their physical sense
	void check_dike_gradients(const double grad, const bool waterside);


	///Reset the flags for the random generation
	void reset_random_flag(void);

	///Calculate new waterlevels of the seepage line
	void calculate_waterlevel_seepage_line(const double water_level, Fpl_Seepage_Line_Point_List *ascending, Fpl_Seepage_Line_Point_List *descending);

	///Output the seepage line to tecplot
	void output_seepage2tecplot(const string seepage_file);
	///Output the seepage line to paraview
	void output_seepage2paraview(const string seepage_file);
	///Output the seepage line to excel
	void output_seepage2excel(const string seepage_file);

	///Get a pointer to a material variable zone
	Fpl_Dike_Var_Materialzone* get_ptr_specified_material_variable_zone(_fpl_zone_type specifier);

	///Count the number of grass cover zones and artificial revetments of the waterside
	void count_waterside_zones(int *no_artif, int *no_grass);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif
