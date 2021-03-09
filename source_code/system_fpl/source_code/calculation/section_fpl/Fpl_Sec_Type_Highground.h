#pragma once
/**\class Fpl_Sec_Type_Highground
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FLP_SEC_TYPE_HIGHGROUND_H
#define FLP_SEC_TYPE_HIGHGROUND_H

//system fpl
#include "_Fpl_Section_Type.h"
#include "Fpl_Fail_Overflow_HiGround.h"



///Class for the section type of the flood-protection line as highground  \ingroup fpl
/**
*/
class Fpl_Sec_Type_Highground : public _Fpl_Section_Type
{
public:
	///Default constructor
	Fpl_Sec_Type_Highground(void);
	///Default destructor
	~Fpl_Sec_Type_Highground(void);


	//members
	///The geometrical height of the highground
	Fpl_Random_Variables height;

	///Pointer to the table for storing the results of the mechanism of the highground section 
	static Tables *result_table;

	//methods
	///Set the input of the dike section (geometrie, variables, faulttree)
	void set_input(const int section_id,  const bool frc_sim, QSqlDatabase *ptr_database);

	///Transfer the section type data to database, e.g. geometry, random variables etc.
	void transfer_sectiontype2database(const int section_id, QSqlDatabase *ptr_database);
	
	///Read in the fpl-section type from file
	void read_section_type_per_file(QFile *ifile, int *line_counter, const bool frc_sim);

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

	///Create the database table for the results of the mechanism of the highground section 
	static void create_result_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the results of the mechanism of the highground section  
	static void delete_data_in_result_table(QSqlDatabase *ptr_database);
	///Delete a data set in the database table for the results of the mechanism of the highground section for a specific system state
	static void delete_data_in_result_table(QSqlDatabase *ptr_database,const _sys_system_id id);
	///Delete a data set in the database table for the results of the mechanism of the highground section 
	static void delete_data_in_result_table(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const _fpl_simulation_type sim_type);
	///Set the database table for the results of the mechanism of the highground section : it sets the table name and the name of the columns and allocate them
	static void set_result_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the results of the mechanism of the highground section 
	static void close_result_table(void);

	///Switch the applied-flag for the results of the mechanism of the highground section in the database table for a defined system state
	static void switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the results of the mechanism of the highground section in the database table for a defined system state and id
	static void switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id);




private:
	//members

	///Pointer to an overflow mechanism for a highground section
	Fpl_Fail_Overflow_HiGround *overflow_event;

	//methods
	///Allocate the the overflow mechanism event
	void allocate_overflow(void);
	///Delete the overflow mechanism event
	void delete_overflow(void);

	///Set the variable via a database table
	void set_variables(const bool frc_sim, QSqlDatabase *ptr_database, const int section_id);
	///Set the geometry via a database table
	void set_geometrie(QSqlDatabase *ptr_database, const int section_id);
	///Set the fault tree for this section type via a database table
	void init_fault_tree(QSqlDatabase *ptr_database);
	///Decide and allocate the mechanism, which should by applied during calculation
	void decide_mechanism(const string mechanism_name, const bool application_flag, const bool applied_in_tree);

	///Read in the cubature of the section type from database table
	void read_cubature(QSqlDatabase *ptr_database, const int section_id);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif