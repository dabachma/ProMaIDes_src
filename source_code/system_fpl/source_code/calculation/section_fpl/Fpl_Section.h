#pragma once
/**\class Fpl_Section
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_SECTION_H
#define FPL_SECTION_H
//qt-libs
#include <QStringList>
//system fpl
#include "_Fpl_Section_Type.h"
#include "Fpl_Sec_Type_Highground.h"
#include "Fpl_Sec_Type_Dike.h"
#include "Fpl_Sec_Type_Dune.h"
#include "Fpl_Sec_Type_Gate.h"
//system sys
#include "Common_Const.h"
//system hyd
#include "_Hyd_River_Profile.h"
#include "Hyd_Floodplain_Dikeline_Point.h"
#include "FplGui_Frc_Values_Dia.h"
#include "Hyd_Hydraulic_System.h"
#include "Fpl_Frc_Curve.h"

//forward
struct _fpl_frc_result;


///Class representing one section of the flood-protection line (fpl) \ingroup fpl
/**
This section consists of an section type, where the probability calculations are performed. 
This section also interacts with the hydraulic module (\ref hyd), to simulate a failure of
the fpl-section in the hydraulic component.

\see _Fpl_Section_Type
*/
class Fpl_Section : public _Sys_Common_System
{

public:
	///Default constructor
	Fpl_Section(void);
	///Default destructor
	~Fpl_Section(void);

	//members
	//Pointer to the section type, e.g. dike, highground, wall, mobile wall etc.
	_Fpl_Section_Type *type_of_section;
	///Pointer to the table for storing the data of the the section in a database
	static Tables *table;

	//methods

	///Read in the fpl-scetion from file
	void read_section_per_file(const string file_section, const bool flag_frc_sim, QSqlDatabase *ptr_database);
	///Transfer the members read in per file to database
	void transfer_member2database(QSqlDatabase *ptr_database);

	///Create the database table for the section data
	static void create_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the section data
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Delete data in the database table for the section data for a specific section
	static void delete_data_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id);
	///Set the database table for the section data: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the section data
	static void close_table(void);
	///Check if all section are combined to the hydraulic system
	static bool check_all_combined2hyd_system(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Reset the combining to the hydraulic system 
	static void reset_combined2hyd_system(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Check if there are fpl-section set in database
	static bool check_some_section_set_in_database(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Select and count the number of relevant fpl section in a database table
	static int select_relevant_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output = true);
	///Select one specific relevant fpl section in a database table
	static int select_relevant_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id,  const bool with_output = true);
	///Select and count the number of fpl section for the given measure state in a database table
	static int select_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id);
	///Select and count the number of fpl section with a specific section type for the given measure state in a database table 
	static int select_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const _fpl_section_types type);

	///Get a list of all available section ids 
	static QList<int> get_list_relevant_section_database(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Get a list of last x section ids 
	static QList<int> get_list_last_section_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int last);

	///Select one specific fpl section in a database table
	static int select_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id);
	///Select one specific fpl section in a database table
	static int select_section_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int sec_id);

	///Switch the applied-flag for the section in the database table for a defined system state
	static void switch_applied_flag_section_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the section in the database table for a defined system state and id
	static void switch_applied_flag_section_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id);

	///Copy the fpl-section parameters of a given system id to another one in database tables
	static void copy_fpl_section(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id);


	///Check if all section results (frc/mc), where the results are set by hand, are set
	static bool check_set_by_hand_section_result(QSqlDatabase *ptr_database, const _sys_system_id id, string *txt);

	///Delete the detailed result values of the section types for a given section
	void delete_detailed_results_database(QSqlDatabase *ptr_database, const _fpl_simulation_type sim_type);

	///Calculate the fault tree with random distributed variables (true) or with deterministic variables (false)
	int make_faulttree(const bool random_calculation);

	///Input the fpl-section data form a query model per database
	void input_section_perdatabase(const QSqlQueryModel *results, const int glob_index, QSqlDatabase *ptr_database, const bool applied_important=true);
	///Input the fpl-section and the appending data form a query model per database
	void input_section_total_perdatabase(const QSqlQueryModel *results, const int glob_index, QSqlDatabase *ptr_database, const bool flag_frc_sim, const bool for_output = false);

	///Output the section members to display/console
	void output_members(void);

	///Output the deterministic reliability of the section
	void output_reliability(void);

	///Output the section geometry to a tecplot file
	void output_geometry2tecplot(void);
	///Output the section geometry to a paraview file
	void output_geometry2paraview(void);
	///Output the section geometry to a excel file
	void output_geometry2excel(void);

	///Output the deterministic results to a tecplot file
	void output_determ_results2tecplot(QSqlDatabase *ptr_database);
	///Output the deterministic results to a paraview file
	void output_determ_results2paraview(QSqlDatabase *ptr_database);
	///Output the deterministic results to a excel file
	void output_determ_results2excel(QSqlDatabase *ptr_database);

	///Output the MC results to a tecplot file
	void output_mc_results2tecplot(QSqlDatabase *ptr_database);
	///Output the MC results to a paraview file
	void output_mc_results2paraview(QSqlDatabase *ptr_database);
	///Output the MC results to a excel file
	void output_mc_results2excel(QSqlDatabase *ptr_database);


	///Output the FRC results to a tecplot file
	void output_frc_results2tecplot(QSqlDatabase *ptr_database);
	///Output the FRC results to a paraview file
	void output_frc_results2paraview(QSqlDatabase *ptr_database);
	///Output the FRC results to a excel file
	void output_frc_results2excel(QSqlDatabase *ptr_database);


	///Check and generate folder
	static string check_generate_folder(const string folder_name, string *path);

	///Output result members to database table
	void output_result2table(QSqlDatabase *ptr_database, _fpl_simulation_type simulation_type, const int counter_mc_sim);

	///Set probabilistic results of a section per dialog
	void set_probabilistic_result_per_dialog(QSqlDatabase *ptr_database);

	///Check the statistics of the random variables used in the section types
	void check_statistic(void);
	///Reset the statistic important values
	void reset_statistics(void);
	///Output the statistics of the random variables used in the section types to display/console
	void output_statistic(void);
	
	///Get the section id
	int get_id_section(void);
	///Set the section id
	void set_section_id(QSqlDatabase *ptr_database);
	///Get the name of the section
	string get_name_section(void);
	///Get the type of the section
	_fpl_section_types get_type_section(void);

	///Set the flag for a fragility curve (frc)-calculation
	void set_frc_flag(const bool frc_flag);

	///Get a string for displaying the section for a selection
	string get_string_selection(void);	

	///Check the file for the fpl-section if the !$BEGIN... always are close with !$END...
	void check_input_file_section(const string file_name);

	///Output section statistics to display/console
	void output_section_statistics(void);

	///Set the number of exceptions (error/warning) before an action
	void set_start_exception_number(void);
	///Set the exception number (error/warning), which occured during an action
	void set_exception_number(void);

	///Combine the section to the hydraulic river profiles or the coast dikeline points
	void combine_section2hydraulic(Hyd_Hydraulic_System *hyd_system, QSqlDatabase *ptr_database);

	///Idealise the hydraulic system by the fpl-section
	void idealise_hyd_by_section(Hyd_Hydraulic_System *hyd_system);
	///Idealise the hydraulic river model by the fpl-section
	void idealise_hyd_by_section(Hyd_Model_River *hyd_river);

	///Load the list of involved river profile/dikeline points for the fpl-section
	void load_list_involved_profiles_points(Hyd_Hydraulic_System *hyd_system);



	///Check the hydraulic connection parameters
	void check_connection_params(void);

	///Get the breach starting parameter structure
	_hyd_breach_start_parameters get_breach_start_parameters(void);
	///Get the left bank flag
	bool get_left_bank_flag(void);
	///Set the starting breach height
	void set_starting_breach_height(const double breach_height_start);
	
	///Get the number of involved river profiles
	int get_number_involved_profiles(void);
	///Get the list of the involved river profiles
	_Hyd_River_Profile** get_list_involved_river_profiles(void);


	///Get the number of involved coast dikeline points
	int get_number_involved_points(void);
	///Get the list of the involved coast dikeline points
	Hyd_Floodplain_Dikeline_Point** get_list_involved_coast_dikeline_points(void);




	///Calculate the probability per fragility curve with the information of the waterlevel of the involved river sections
	_fpl_frc_result calculate_probability_per_frc(QSqlDatabase *ptr_database, const int id_hyd_sc, const string break_id, const bool break_flag);


	///Load and initialize the frc-curve and their dividing intervals 
	void init_intervals_frc_curve(QSqlDatabase *ptr_database, const double max_waterlevel);
	///Generate the interval and the break height via Monte Carlo analysis and get the identifier
	string get_relevant_interval_frc_curve(_Fpl_Random_Generator *ran_gen);

	///Transfer the interval data to database
	void transfer_interval_data2database(QSqlDatabase *ptr_database, const int sz_bound_id);


	///Get the pointer to the frc-data
	Fpl_Frc_Curve* get_ptr_frc_data(void);

	///Evaluate the maximal waterlevel in the involved river section for a given scenario
	double evaluate_max_waterlevel_involved_section(QSqlDatabase *ptr_database, const int id_hyd_sc, const string break_id);

	///Overwrite the main section parameters with the given one
	void overwrite_main_section_parameter(Fpl_Section *given_sec);

	///Compare the geometrical settings of a section 
	void compare_geometrical_settings(Fpl_Section *section);

	///Set the name of the output folder
	void set_output_folder_name(const string name);

	///Evaluate the next section id from the database table
	static int evaluate_next_section_database(QSqlDatabase *ptr_database);


private:
	//members
	///Global index of the section the database table
	int glob_id;
	///Identifier of the section
	int id_section;
	///Type of the section
	_fpl_section_types type;
	///Name of the section
	string section_name;
	///The orthogonal direction of the section to the water side (north ^ is zero)
	double direction;
	///Flag on which bank of the river the section is stationed (in flow direction)
	/**This variable is just used by a connection of the fpl-module with the hydraulic-module.
	*/
	bool left_flag;
	///Length of the section
	double section_length;
	///Riverstation, where the section starts
	/**This variable is just used by a connection of the fpl-module with the hydraulic-module.
	*/
	double station_start;
	///Riverstation, where the section ends
	/**This variable is just used by a connection of the fpl-module with the hydraulic-module.
	*/
	double station_end;
	///Break parameters of this fpl-section
	/**The break parameters specifies, if the section fails abprupt, like a wall of continuous, like a dike.
	The type of breaching is evaluated after the fpl-section type. 
	This variable is just used by a coupling of the fpl-module with the hydraulic-module.
	\see _Hyd_break_parameters
	*/
	_hyd_break_parameters break_params;
	///Parameters for starting a breach of this fpl-section
	_hyd_breach_start_parameters breach_start;

	///Maximum breach width of this fpl-section
	/**It specifies the maximal width of a brach occuring in this fpl-section. For an highground section
	type it will be set to 0.0 m.
	This variable is just used by a coupling of the fpl-module with the hydraulic-module.
	*/
	//double max_breach_width;

	///Flag if a frc-simulation should be performed
	bool flag_frc_sim;

	///The crest point of the structure on the waterside
	Fpl_Section_Points crest_water;

	///Height of the structure
	double structure_height;

	///Polysegment of the fpl-section
	Geo_Polysegment polysegment;



	///Number of keyvalues, which must be found for a file input
	int must_found;

	///Number of error at the begin of an action
	int number_error_begin;
	///Number of warning at the begin of an action
	int number_warning_begin;
	///Number of error at the end of an action
	int number_error;
	///Number of warning at the end of an action
	int number_warning;

	///List of pointer to the river-profiles, which are part of the fpl-section; it is allocated during the idealisation process of the hydraulic system
	_Hyd_River_Profile **list_involved_profiles;
	///Number of involved river profiles
	int number_involved_profiles;
	

	///List of pointer to the coast dikeline-points which are part of the fpl-section; it is allocated during the idealisation process of the hydraulic system
	Hyd_Floodplain_Dikeline_Point **list_involved_points;
	///Number of involved coast dikelinie-points
	int number_involved_points;

	///Pointer to the frc-curve data
	Fpl_Frc_Curve *frc_data;

	///Similarity length
	double similarity_lenght;
	///Similarity factor
	double similarity_factor;

	///Name of the output folder
	string output_folder;

	///Flag if it is an file input for a river section
	bool inp_river_sec;

	//methods
	///Allocate the section type
	void allocate_section_type(void);
	///Delete the section type
	void delete_section_type(void);

	///Allocate the list of involved river profiles
	void allocate_list_involved_profiles(void);
	///Delete the list of involved river profiles
	void delete_list_involved_profiles(void);

	///Allocate the list of involved coast dikeline-points
	void allocate_list_involved_points(void);
	///Delete the list of involved coast dikeline-points
	void delete_list_involved_points(void);



	///Allocate the frc-curve data
	bool allocate_frc_data(void);
	///Delete the frc-curve data
	void delete_frc_data(void);

	///Set the waterside crest point per database table
	void set_crest_water_database(QSqlDatabase *ptr_database, const bool applied_important=true);
	///Transfer crest point to database
	void transfer_crest_water2database(QSqlDatabase *ptr_database);

	///Read general section data from file
	void read_general_section_data_per_file(QFile *ifile, int *line_counter);
	///Read hydraulic connection data of the section from file
	void read_hyd_connection_data_per_file(QFile *ifile, int *line_counter);

	///Find the key-values for the file input of the general section data
	void find_key_values_general_file(const string myline, int *must_found_counter);
	///Find the key-values for the file input of the hydraulic connection data of the section
	void find_key_hyd_connection_file(const string myline, int *must_found_counter);

	///Transfer section members to database
	void transfer_section2database(QSqlDatabase *ptr_database);

	///Set the polyline-string and the section length due to combining with the river profiles of the section
	void set_length_polyline(Hyd_Model_River *river, QSqlDatabase *ptr_database);
	///Set the polyline-string and the section length due to combining with the coast dikeline 
	void set_length_polyline(Hyd_Floodplain_Polysegment *coast_line, QSqlDatabase *ptr_database);

	///Check if a fpl-section is concerning the hydraulic parameters (structure height, start-, end-point etc.) similar to the given section
	bool check_fpl_section_similar_hyd_param(Fpl_Section *section2check);

	///Copy the main section parameters
	void copy_main_section_parameter(Fpl_Section *copy);

	


	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif