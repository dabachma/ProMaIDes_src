#pragma once
/**\class _Fpl_Section_Type
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _FPL_SECTION_TYPE_H
#define _FPL_SECTION_TYPE_H
//libs

//system fpl
#include "Fpl_Random_Variables.h" 
#include "Fpl_Section_Points.h"
//system sys
#include "Common_Const.h"
#include "_Sys_Common_System.h"

//forward declaration
///Enumerator for the simulation type \ingroup fpl
enum _fpl_simulation_type{
    ///Frgaility-curve (frc) calculation
    sim_frc,
    ///Monte-Carlo Simulation
    sim_mc,
    ///Deterministic simulation
    sim_determ,
};

///Enumerator for specifiying the type of the section \ingroup fpl
enum _fpl_section_types{
	///Dike section
	dike_sec,
	///Dune section
	dune_sec,
	///Gate section
	gate_sec,
	///High-ground section
	high_ground_sec,
	///Wall section
	wall_sec,
	///Mobile wall section
	mob_wall_sec,
	///Section result data is set by hand
	by_hand_sec,
	///All section type
	all_sec,
	///Unknown section type
	unknown_sec,
};

///Enumerator for specifiying the mechanism of a specific section type \ingroup fpl
enum _fpl_mechanism_types{
	///Mechanism of overflow (failure mechanism)
	overflow,
	///Wind-wave generation; the wave parameters are calculated (mechanism)
	wind_wave,
	///Mechanism of wave runup (failure mechanism);
	wave_runup,
	///Mechanism of bursting (failure mechanism in combination with piping)
	bursting,
	///Mechanism of piping after Sellmeijer (failure mechanism in combination with bursting)
	piping_selli,
	///Mechanism of piping after Lane (failure mechanism in combination with bursting)
	piping_lane,
	///Mechanism of piping after Schmertmann (failure mechanism in combination with bursting)
	piping_schmert,
	///Mechanism of the landside stability with slip circles (failure mechanism)
	landside_stability,
	///Mechanism of the waterside stability with slip circles (failure mechanism)
	waterside_stability,
	///Mechanism of the landside erosion (failure mechanism)
	landside_erosion,
	///Mechanism of the waterside erosion of dunes(failure mechanism)
	waterside_erosion_gent,
	///Mechanism of wave impact
	wave_impact,
	///Mechanism of waterside erosion
	waterside_erosion,
	///Mechanism of start waterside erosion
	start_waterside_erosion
};

///struct for the output control parameters
struct output_control {
	///Tecplot output
	bool tec_output;
	///Paraview output
	bool para_output;
	///Excel output
	bool excel_output;

};

///Base-class for the section type of the flood-protection line  \ingroup fpl
/**
It is represented by a profile of the structure, e.g. dike, wall or mobile wall.
*/
class _Fpl_Section_Type : public _Sys_Common_System
{

public:
	///Default constructor
	_Fpl_Section_Type(void);
	///Default destructor
	virtual ~_Fpl_Section_Type(void);

	//members
	///Pointer to the table for storing, which mechanism are applied of a failure calculation
	static Tables *table_mechanism;
	///The variable of the waterlevel as random variable
	Fpl_Random_Variables waterlevel;
	
	//methods

	///Read in the fpl-section type from file
	virtual void read_section_type_per_file(QFile *ifile, int *line_counter, const bool frc_sim)=0;
	///Read the mechanisms of the fault-tree per file
	virtual void read_mechanism_faulttree_perfile(QFile *ifile, int *line_counter);
	///Check which mechanisms of the fault-tree are required per file
	virtual void check_mechanism_faulttree_perfile(QFile *ifile, int *line_counter);
	///Read the waterside geometry of the section per file
	virtual void read_waterside_geometry(QFile *ifile, int *line_counter);
	///Read the landside geometry of the section per file
	virtual void read_landside_geometry(QFile *, int *);
	///Read the geometrical material zone(s) of the section per file
	virtual void read_geo_matzones(QFile *, int *);
	///Read the material variable zone(s) of the section per file
	virtual void read_var_matzones(QFile *, int *);

	///Read the waterside material variable zone(s) of the section per file
	virtual void read_var_waterside_matzones(QFile *ifile, int *line_counter);

	///Check and set the section geometry out of the read in geometry members
	virtual void check_set_geometry(void);


	///Transfer the section type data to database, e.g. geometry, random variables etc.
	virtual void transfer_sectiontype2database(const int section_id, QSqlDatabase *ptr_database)=0;

	///Set the database table for the applied mechanism: it sets the table name and the name of the columns and allocate them
	static void set_table_mechanism(QSqlDatabase *ptr_database);
	///Create the database table for the applied mechanisms
	static bool create_table_mechanism(QSqlDatabase *ptr_database);
	///Reset all data in the database table for the applied mechanism to the default values
	static void reset_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the applied mechanism
	static void close_table(void);
	///Set the predefined data to the mechanism database table; it is called after the creation of this table
	static void set_predefined_data2mechanism_table(QSqlDatabase *ptr_database);

	///Select the relevant mechanisms for a specific section type and return the number of found mechanism(s)
	static int select_relevant_mechanism(QSqlQueryModel *results, QSqlDatabase *ptr_database, _fpl_section_types sec_type);
	///Select the mechanisms for a specific section type and return the number of found mechanism(s)
	static int select_mechanism(QSqlQueryModel *results, QSqlDatabase *ptr_database, _fpl_section_types sec_type);



	///Set the input (geometrie, variables, faulttree) via database
	virtual void set_input(const int section_id,  const bool frc_sim, QSqlDatabase *ptr_database)=0;
	
	///Set the waterlevel in case of an frc-sim
	void set_frc_waterlevel(const double frc_waterlevel);
	///Set the direction of the section
	void set_section_direction(const double direction);

	///Set the pointer to the water side crest point
	void set_ptr_waterside_crest(Fpl_Section_Points *crest_water);

	///Calculate the fault tree with random distributed variables (true) or with deterministic variables (false)
	virtual int make_faulttree(const bool random_calculation=false)=0;

	///Check the statistics of the random variables used in the section type
	virtual void check_statistic(void)=0;
	///Reset the statistic important values
	virtual void reset_statistics(void)=0;
	///Output the statistics of the random variables used in the section type to display/console
	virtual void output_statistic(void)=0;
	///Output the section members to display/console
	virtual void output_member(void)=0;
	///Output the reliability of the fault tree mechanisms for a deterministic calculation to display/console
	virtual void output_reliability(string output_folder, const int sec_id, const string sec_name)=0;

	///Output the geometry to tecplot
	virtual void output_geometry2tecplot(ofstream *output)=0;

	///Output the geometry to paraview
	virtual void output_geometry2paraview(ofstream *output) = 0;
	///Output the geometry to excel
	virtual void output_geometry2excel(ofstream *output) = 0;

	///Output the deterministic results to tecplot
	virtual void output_determ_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) = 0;
	///Output the deterministic results to Paraview
	virtual void output_determ_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) = 0;
	///Output the deterministic results to excel
	virtual void output_determ_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) = 0;

	///Output the MC results to tecplot
	virtual void output_mc_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) = 0;
	///Output the MC results to Paraview
	virtual void output_mc_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) = 0;
	///Output the MC results to excel
	virtual void output_mc_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) = 0;

	///Output the FRC results to tecplot
	virtual void output_frc_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) = 0;
	///Output the FRC results to Paraview
	virtual void output_frc_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) = 0;
	///Output the FRC results to excel
	virtual void output_frc_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) = 0;



	///Get output control flags per table
	output_control get_output_control_flags(void);

	///Set output control flags
	void set_output_control_flags(QSqlDatabase *ptr_database);

	///Output result members of the mechanisms to database table
	virtual void output_result2table(QSqlDatabase *ptr_database, _fpl_simulation_type simulation_type, _sys_system_id id, const int section_id, const int counter_mc_sim)=0;

	///Convert a string into a section type (_fpl_section_types)
	static _fpl_section_types convert_txt2section_type(const string txt);
	///Convert a section type (_fpl_section_types) into a string 
	static string convert_section_type2txt(const _fpl_section_types type);

	///Convert a string into a mechanism type (_fpl_mechanism_types)
	static _fpl_mechanism_types convert_txt2mechanism_type(const string txt);
	///Convert a mechanism type (_fpl_mechanism_types) into a string 
	static string convert_mechanism_type2txt(const _fpl_mechanism_types type);

protected:
	//members
	///Flags to control the section output
	output_control output_flags;

	///The base point of the structure on the waterside
	Fpl_Section_Points base_water;
	///Pointer to the crest point of the structure on the waterside
	Fpl_Section_Points *crest_water;
	///The orthogonal direction of the section structure to the water side (north ^ is zero)
	double section_direction;
	///Number of applied mechanisms
	int number_mech;

	///Flag if the waterlevel is to low
	bool low_waterlevel_flag;


	//methods
	///Find the start of an input block of the fpl-section in file
	qint64 find_start_block_file(QFile *ifile, int *line_counter, const string begin, const string end, const bool must_found);
	///Reset the flags for the random generation
	virtual void reset_random_flag(void);
	///Set the section type specific parameter via a database table
	virtual void set_variables(const bool frc_sim, QSqlDatabase *ptr_database, const int section_id)=0;
	///Set the section type specific geometrie via a database table
	virtual void set_geometrie(QSqlDatabase *ptr_database, const int section_id)=0;
	///Set the applied mechanism of the fault tree via a database table 
	virtual void init_fault_tree(QSqlDatabase *ptr_database)=0;
	///Decide if a mechanism is apllied given by the fault tree and allocate the classes for this mechanism
	virtual void decide_mechanism(const string mechanism_name, const bool application_flag, const bool applied_in_tree)=0;




private:
	//members
	
	//methods
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
