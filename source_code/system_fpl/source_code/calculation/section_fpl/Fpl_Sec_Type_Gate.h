#pragma once
/**\class Fpl_Sec_Type_Gate
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/
#ifndef FLP_SEC_TYPE_GATE_H
#define FLP_SEC_TYPE_GATE_H

#include "_Fpl_Section_Type.h"


///Class for the section type of the flood-protection line as gate  \ingroup fpl
/**
*/
class Fpl_Sec_Type_Gate : public _Fpl_Section_Type
{
public:
	///Default constructor
	Fpl_Sec_Type_Gate(void);
	///Default destructor
	~Fpl_Sec_Type_Gate(void);

	//members
	///Pointer to the table for storing the results of the mechanism of the dike section 
	static Tables *result_table;

	//method
	///Set the input of the dike section (geometrie, variables, faulttree) per database
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
	static void set_result_table(QSqlDatabase *ptr_database);
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
	///Pointer to wave generation mechanism induced by wind for a dike section after KrylowII
	//Fpl_Mech_Wave_Generation *wave_wind_event;
	///Pointer to erosion event after van Gent et al.
	//Fpl_Mech_Wave_Generation *wave_wind_event;

	
	///Base point of the landside of the dike section
	Fpl_Section_Points base_land;
	///Crest point of the landside of the dike section
	Fpl_Section_Points crest_land;
	/////The cubature of the waterside of the dike section
	//Fpl_Cub_Dike_Waterside waterside_cubature;
	/////The cubature of the crest of the dike section
	//Fpl_Cub_Dike_Crest crest_cubature;
	/////The cubature of the landside of the dike
	//Fpl_Cub_Dike_Landside landside_cubature;

	/////The foreland geometrie of the dike
	//Fpl_Dike_Geo_Foreland foreland;
	/////The hinterland geometrie of the dike
	//Fpl_Dike_Geo_Hinterland hinterland;



	///Random variable of the water density
	Fpl_Random_Variables water_density;

	///The current waterlevel for calculation
	double current_waterlevel;

	
	//methods
	///Allocate the mechanism for wave generation induced by wind after KrylowII
	//void allocate_wind_wave_mechanism(void);
	/////Delete the mechanism for wave generation induced by wind after KrylowII
	//void delete_wind_wave_mechanism(void);



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

	///Check the initialized fault-tree
	void check_fault_tree(void);


	///Reset the flags for the random generation
	void reset_random_flag(void);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};

#endif