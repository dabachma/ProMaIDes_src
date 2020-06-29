#pragma once
/**\class Risk_Fpl_Stress
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/

#ifndef RISK_FPL_STRESS_H
#define RISK_FPL_STRESS_H

//system sys
#include "_Sys_Common_System.h"
#include "Common_Const.h"

//system risk



///Structure for gathering the relevant information about the applied hydraulic stress to a specific FPL-section
struct _risk_section_stress{

	///Identifer of the fpl-section
	int id_section;
	///Used index of the fpl-section
	int used_index;
	///Waterlevel (applied stress) applied to a section
	double waterlevel;
	///Time when the waterlevel (applied stress) occurs
	double time;
	///Randomly changed time, when the waterlevel occurs
	double rand_time;
};

///Class for gathering and managing the data of the hydraulic stress to the FPL-section(s)  \ingroup risk
/**
This class is used in the catchment area risk approach for updating the probabilities of
the break scenario(s) with new hydraulic information.
\see Risk_Catchment_Scenario_Tree

*/
class Risk_Fpl_Stress
{
public:
	///Default constructor
	Risk_Fpl_Stress(void);
	///Default destructor
	~Risk_Fpl_Stress(void);

	//method
	///Get the identifier string of the break scenario
	string get_id_string_break_sc(void);
	///Set the identifier string of the break scenario
	void set_id_string_break_sc(string id_break_sc);
	///Append a string to the identifier string of the break scenario
	void append2id_string_break_sc(string append_str);

	///Get number of relevant FPL-section(s)
	int get_number_fpl_section(void);
	///Set number of relevant FPL-section(s) and allocate the _risk_section_stress data structure
	void set_number_fpl_section(const int number_fpl_sec);

	///Get the pointer to the individuel section stress
	_risk_section_stress *get_ptr_section_stress(void);

	///Get the a pointer to the individuel section stress for a specific section
	_risk_section_stress *get_section_stress(const int index);

	///Set the information of the individuel section stress for a specific section
	void set_section_stress(const int index, _risk_section_stress *stress);

	///Copy the object information from a given source object
	void copy_infos(Risk_Fpl_Stress *src);

	///Set if the hydraulic stress are calculated (:=true) or predicted one (:=false)
	void set_hyd_is_predicted(const bool flag);
	///Get if the hydraulic stress are calculated (:=true) or predicted one (:=false)
	bool get_hyd_is_predicted(void);

	///Predict new hydraulic stress out of different stress for this object
	void predict_values(Risk_Fpl_Stress *base, const int break_id, Risk_Fpl_Stress *part_new_info);

	///Get the maximum waterlevel for a given section id of the hydraulic information
	static double get_maximum_waterlevel(Risk_Fpl_Stress *results, const int section_id);

private:

	//members

	///Identifer string of the break scenario
	string id_break_sc;
	///Number of relevant FPL-section(s)
	int number_fpl_sec;

	///Pointer to the individuel section stress
	_risk_section_stress *section_stress;

	///Flag if the hydraulic stress are calculated (:=true) or predicted one (:=false)
	bool calculated_info;


	//methods

	///Allocate the data structure for each individuel FPL-section stress (_risk_section_stress)
	void allocate_section_stress(void);
	///Delete the data structure for each individuel FPL-section stress (_risk_section_stress)
	void delete_section_stress(void);


	///Set error(s)
	Error set_error(const int err_type);


};
#endif
