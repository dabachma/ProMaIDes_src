#pragma once
/**\class Hyd_Boundary_Szenario
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/

#ifndef HYD_BOUNDARY_SZENARIO_H
#define HYD_BOUNDARY_SZENARIO_H

//sys-class
#include "_Sys_Common_System.h"


///Class for handling of boundary scenarios for one hydraulic system \ingroup hyd
/**
Each boundary szenario represents the discharges to an hydraulic system for one flood-/drought event.
For one hydraulic system different hydraulic boundary scenarios can be defined. Thus, the parts of the system, e.g.
river models, floodplain models etc., are equal but have different boundary condition at each boundary szenario.
Moreover, the szenarios creates the interface to an risk-calculation by defining probailites of ouccurence to each szenario.
There are two probilities to define:
	- annuality [a], which implicates a reoccurence probaility of the flood event by the reciprocal value [1/a]
	- general reoccurence probability of this event; this probility defines events, which have the same peak, but different
	different hydrographs
	- the total occurence probability is the product of the afore mentioned probabilities

The boundary scenarios are managed in Hyd_Boundary_Szenario_Management or in Hyd_Boundary_Szenario_List. Classes, which are directly depending to the szenarios,
like the hydraulic system, the models and the parts (elements/profiles) are child classes of this class.

\see Hyd_Hydraulic_System

*/
class Hyd_Boundary_Szenario : public _Sys_Common_System
{
	///Managment class for objects of this class
	friend class Hyd_Boundary_Szenario_Management;
	///Display and gui-edit class for objects of this class
	friend class HydGui_Bound_Scenario_Table_Wid;

public:
	///Default constructor
	Hyd_Boundary_Szenario(void);
	///Copy constructor
	Hyd_Boundary_Szenario(Hyd_Boundary_Szenario &copy);
	///Default destructor
	~Hyd_Boundary_Szenario(void);

	//methods

	///Set the id, name and the probabilities
	void set_members(const int id, const double annuality, const double prob_occur, const string name = label::not_set);

	///Set the members per pointer to this class
	void set_members(Hyd_Boundary_Szenario *ptr);
	///Set the id of the hydraulic boundary scenario
	void set_id(const int id);

	/////Set the flag, if the szenario is claculated (true) or not (false)
	//void set_is_calculated(const bool is_calculated);

	/////Get the flag if the szenario is claculated
	//bool get_is_calculated(void);
	///Get the id of the szenario
	int get_id(void);
	///Get probability per year
	double get_prob_year(void);
	///Set probability per year
	void set_prob_year(const double prob);
	///Get total occurence probability
	double get_prob_total(void);
	///Get the occurence probability
	double get_prob_occurence(void);
	///Get the annuality
	int get_annuality(void);
	///Get the name
	string get_name(void);
	///Get the description
	string get_description(void);

	///Get if the scnearios is selected for handling
	bool get_is_selected(void);
	///Set the flag for handling this scenario
	void set_is_selected(const bool flag);

	///Get a text for a item of a list widget
	string get_sz_text(void);

	///Set new name
	void set_name(const string name);
	///Set new description
	void set_description(const string descript);

	///Update the probability of the max-event in database table
	void update_max_event_prob_database(QSqlDatabase *ptr_database);

	///Get a pointer to the damage was output flag
	bool* get_ptr_dam_was_output(void);

	///Copy operator
	Hyd_Boundary_Szenario& operator= (const Hyd_Boundary_Szenario& copy);

private:
	//members

	///Identifier of the szenario
	int id_sz;
	///Name of the szenario
	string name;
	///Description of the scenario
	string description;
	///Annuality of the szenario [a]
	int annuality;
	///Reoccurence probability per year [1/a]
	double prob_annuality;
	///Reoccurence probability of the event
	double prob_event;
	///Flag if the sceneraio is selected by the user for handling it (e.g. calculation, checking etc.)
	bool is_selected;

	///Flag if the damage was output
	bool dam_was_output;

	//method

	///Set error(s)
	Error set_error(const int err_type);
};
#endif
