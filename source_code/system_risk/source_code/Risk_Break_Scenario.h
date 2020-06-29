#pragma once
/**\class Risk_Break_Scenario
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef RISK_BREAK_SCENARIO_H
#define RISK_BREAK_SCENARIO_H


//system sys
#include "_Sys_Common_System.h"
#include "Common_Const.h"
//system fpl
#include "Fpl_Calculation.h"
//system hyd
#include "Hyd_Multiple_Hydraulic_Systems.h"
#include "Hyd_Boundary_Szenario.h"
//system dam
#include "Dam_Damage_System.h"

///Enumerator to distinguish the type of the risk calculation, e.g. peforming a nobreak-risk analysis or a risk analysis for a catchment area \ingroup risk
enum _risk_type{
	///A risk analysis without any break of a fpl-section. Risk is defined as: annuality of flood event times damage
	nobreak_risk,
	///A risk analysis with one break in a fpl-section (scenario based). Hydraulic- and fpl-calculation are independent. Risk is defined as: annuality of sectionfailure times damage
	scenario_risk,
	///A risk analysis with one/multiple break(s) in a  different fpl-section (catchment area based). Hydraulic- and fpl-calculation are connected via the fragility curve.
	catchment_risk
};

///Enumerator for specifying the break scenario source \ingroup risk
enum _risk_break_sc_source{
	///Break scenario is user defined
	sc_user_defined,
	///Break scenario is a fixed and must be calculated
	sc_fixed,
	///Break scenario is generated via monte carlo analysis
	sc_generated
};

//forward declaration
struct _risk_predicted_risk;


///Class for defining one break scenario \ingroup risk
/**

*/
class Risk_Break_Scenario : public _Sys_Common_System
{
public:
	///Default constructor
	Risk_Break_Scenario(void);
	///Copy constructor
    Risk_Break_Scenario(const Risk_Break_Scenario& copy);

	///Default destructor
	virtual ~Risk_Break_Scenario(void);

	//method
	///Get the break scenario string
	string get_break_scenario_string(void);
	///Get the hydraulic boundary scenario 
    Hyd_Boundary_Szenario& get_hydraulic_boundary_scenario(void);
	///Set the hydraulic boundary scenario 
	void set_hydraulic_boundary_scenario(Hyd_Boundary_Szenario sc);
	///Get the number of involved fpl-sections
	int get_number_involved_sections(void);
	///Get if a given section id is involved in this break scenario
	bool section_is_involved(const int section_id);
	///Check if a given section is involved in this break scenario
	bool section_is_involved(const int section_id, const string interval_id);

	///Get a pointer to the identifier of the involved fpl-sections
	_fpl_break_info *get_ptr_involved_sections(void);
	///Get the id of the involved fpl-section specified by the index
	_fpl_break_info get_involved_section(const int index);
	///Get the string id for a involved fpl-section specified by the index
	string get_string_id_involved_section(const int index);

	

	///Set a break scenario with the given fpl-section
	void set_break_scenario(const int number_section, _fpl_break_info *section_ids, Hyd_Boundary_Szenario *hydraulic_bound_sc);
	///Set a break scenario string and translate it to the involved section
    void set_break_scenario_str(const Hyd_Boundary_Szenario hydraulic_bound_sc, const string break_sc);

	///Compare two break scenarios, if the given one includes information for this scenario
	bool compare_scenario_information(Risk_Break_Scenario *given);

	///Get the risk type
	_risk_type get_risk_type(void);
	///Set the risk type
	void set_risk_type(_risk_type type);


	///Output the members to display/console
	void output_members(void);

	///Convert a string into a risk type (_risk_type)
	static _risk_type convert_txt2risk_type(const string txt);
	///Convert a risk type (_risk_type) into a string 
	static string convert_risk_type2txt(const _risk_type type);

	///Convert a string into a source type (_risk_break_sc_source)
	static _risk_break_sc_source convert_txt2scource(const string txt);
	///Convert a source type (_risk_break_sc_source) into a string 
	static string convert_source2txt(const _risk_break_sc_source src);

	///Get the total probability of the scenario 
	double get_prob_total(void);
	///Get the probability of the scenario without the hydraulic boundary probability
	double get_prob_scenario(void);

	///Calculate the probability
	void calculate_probability(QSqlDatabase *ptr_database);

	///Set the probability of the scenario
	void set_probability_sc(const double prb);

	///Set the frc-interval break height by database table
	void set_breakheight_database(QSqlDatabase *ptr_database);

	///Get a description string of the break scenario with the name f the hydraulic boundary scenario
	string get_description_string_with_name(void);
	///Get a description string of the break scenario just with the identifier
	string get_description_string(void);

	///Get if the scenario is already calculated
	bool get_is_calculated(void);
	///Check if the sceneraio is already calculated
	void check_is_calculated(QSqlDatabase *ptr_database);
	
	///Get if the scenario is selected
	bool get_is_selected(void);
	///Set if the scenario is slected
	void set_is_selected(const bool flag);

	///Set the flag if the break scenario is already hydaulically calculated
	void set_is_hydraulic_calculated(const bool flag);
	///Get the flag if the break scenario is already hydaulically calculated
	bool get_is_hydraulic_calculated(void);

	///Set the source of the scenario
	void set_source_scenario(const _risk_break_sc_source src);
	///Get the source of the scenario
	_risk_break_sc_source get_source_scenario(void);

	///Reduce the involved section of a break scenario with a given one
	void reduce_involved_section(Risk_Break_Scenario *given);
	///Reduce the involved section of a break scenario by the given section id
	void reduce_involved_section(const int given);

	///Get the number of the maximal interval number
	int get_max_interval(void);
	///Reduce all interval by the given integer; smallest value is 0
	void reduce_interval(const int reduce);

	///Set the predicted risk data from database for the break scenario
	void set_predicted_risk_data(QSqlDatabase *ptr_database);

	///Add predicted risk data to a given one
	void add_predicted_risk_data(_risk_predicted_risk *addto);

	///Calculate the percentage of the predicted risk data
	void calculated_percentage_predicted_risk_data(_risk_predicted_risk *total);

	///Get the pointer to the predicted risk values, if available
	_risk_predicted_risk* get_ptr_predicted_risk_values(void);

	///Decide if a fpl-section is involved (as break location) in this break scenario
	bool decide_fpl_section_involved(const int sec_id);


	///Copy operator
    Risk_Break_Scenario operator= (const Risk_Break_Scenario& copy);


private:

	//members
	///String which identifies the break scenario
	string break_sc_id;
	///Identifier of the hydraulic boundary scenario
	Hyd_Boundary_Szenario id_hydraulic_bound;

	///Type of risk calculation for the break scenario
	_risk_type risk_type;

	///Number of fpl-section involved
	int number_section;
	///Array of the fpl-section identifier
	_fpl_break_info *fpl_section_ids;

	///Probability of the scenario without the probability of the hydraulic boundary scenario
	double prob_without_hyd_sc;

	///Flag if the scenario is already calculated 
	bool is_calculated;
	///Flag if the scenario is selected 
	bool is_selected;

	///Flag if the break-scenario is hydraulically calculated
	bool hyd_is_calculated;

	///Source of the break scenario \see _risk_break_sc_source
	_risk_break_sc_source source;

	///Pointer to the data structure for storing the predicted risk results
	_risk_predicted_risk *ptr_predicted_risk;

	

	//method
	///Generate identifier for the break scenario
	void generate_break_scenario_string(void);
	///Translate the identifer of the break scenarios to the involved fpl-sections
	void translate_string2involved_section(void);
	///Find the risk type out of the id-string
	void find_risk_type_idstring(string *buffer);

	///Allocate array for the involved fpl-sections
	void allocate_array_section(void);
	///Delete array for the involved fpl-sections
	void delete_array_section(void);


	///Sort involved section after the section-id
	void sort_involved_section_id(void);

	///Allocate the predicted risk result structure
	void allocate_predicted_risk_result(void);

	///Delete the predicted risk result structure
	void delete_predicted_risk_result(void);


	///Set error(s)
	Error set_error(const int err_type);



};
#endif
