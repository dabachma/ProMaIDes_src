#pragma once
/**\class _Dam_System
	\author produced by AG FRM HS-M
	\version 0.0.1
	\date 2021
*/

#ifndef _DAM_CI_ELEMENT
#define _DAM_CI_ELEMENT



//sys libs
#include "_Sys_Common_System.h"
#include "Sys_Multi_Filechooser_Dia.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Diverse_Text_Dia.h"

//dam-libs
//#include "Dam_CI_Element_List.h"
//forward declaration
class Dam_CI_Element_List;


///Enumerator for specifiying the sectors of the CI \ingroup dam
enum _dam_ci_sector {
	///Electicity
	electricity = 1,
	///Information technology
	information_technology = 2,
	///Water supply
	water_supply = 3,
	///Water treatment
	water_treatment = 4,


	///Emergency service
	emergency_service = 10,
	///Health
	health = 11,
	///Logistic goods
	logistic_goods = 12,
	///Logistic goods
	logistic_personal = 13,
	///Govermental
	goverment = 14,
	///Hazardous sites
	haz_material = 15,
	///Industry
	industry= 16,
	///Cultural sites
	cultur = 17,
	///Education
	education = 18,
	///Jail
	jail= 19,

	undefined = 0
};

///Enumerator for specifiying the failure (failure type) of a CI-element \ingroup dam
enum _dam_ci_failure_type {
	///No failure
	no_failure=0,
	///direct failure by water
	direct = 1,
	///Sectoral
	sectoral= 2,
	///Transsectoral
	transsectoral = 3,
	///Direct but activated; just for emergency CI-elements
	direct_activ = 4,

	undefined_failure = 5
};



//Base class for the CI elements points and polygons which are used in the CI-damage calculation  \ingroup dam
/**

*/
class _Dam_CI_Element : public _Sys_Common_System
{


public:
	///Default constructor
	_Dam_CI_Element(void);
	///Copy constructor
	_Dam_CI_Element(const _Dam_CI_Element& object);
	///Default destructor
	virtual ~_Dam_CI_Element(void);


	//method
	///Get element active flag
	bool get_element_active(void);
	///Set element active flag
	void set_element_active(const bool active);
	///Get the sector id
	int get_sector_id(void);
	///Get the sector name
	string get_sector_name(void);

	///Get the failure type enum
	_dam_ci_failure_type get_failure_type(void);
	///Set the failure type enum
	void set_failure_type(const _dam_ci_failure_type type);
	///Set the active flag
	void set_active_flag(const bool flag);
	///Set the activation time and flag
	void set_active_time_flag(const bool flag, const double time);
	///Set the was-affected flag
	void set_was_affected_flag(const bool flag);

	///Set the index of the connected FP-model
	void set_index_floodplain(const int index);
	///Set the index of the connected FP-element
	void set_index_floodplain_element(const int index);

	///Get the index of the connected FP-model
	int get_index_floodplain(void);
	///Get the index of the connected FP-element
	int get_index_floodplain_element(void);

	///Get connected flag
	bool get_connected_flag(void);

	///Decide according to the sector-id, if the element gets an endflag
	static bool sector_id2endflag(const int sec_id);

	///Reset result values
	void reset_result_values(void);
	///Add pointer of incoming CI-elements to list
	void add_incomings(_Dam_CI_Element* add);
	///Add pointer of outgoing CI-elements to list
	void add_outgoing(_Dam_CI_Element* add);

	///Get the number of outgoing CI-elements
	int get_number_outgoing(void);
	///Get the pointer to the outgoing CI-elements
	_Dam_CI_Element **get_outgoing_elements(void);

	///Init the sectors list of incoming and outgoing CI-elements
	void init_sec_list(void);

	///Get final level flag
	bool get_end_level_flag(void);
	///Get failure duration
	double get_failure_duration(void);

	///Get recovery time
	double get_recovery_time(void);
	///Get activation time
	double get_activation_time(void);
	///Get regular flag
	bool get_regular_flag(void);

	///Calculate indirect damages
	void calculate_indirect_damages(void);
	///Calculate indirect damages instationary
	void calculate_indirect_damages_instationary(const double time);

	///Convert string to failure type (_dam_ci_failure_type)
	static _dam_ci_failure_type convert_txt2failuretype(const string txt);
	///Convert category (_dam_sc_category) to string
	static string convert_sector_id2txt(const _dam_ci_sector sec);
	///Transfer the sector id in the enum _dam_sc_category
	static _dam_ci_sector convert_id2enum(const int id);

	///Check the connection of the CI-elements
	void check_connections(void);

	///Get the pointer to the Geo_Point
	Geo_Point *get_ptr_point(void);

	///Get the int if the element is a point (=0) or a polygon (=1)
	int get_is_point_id(void);

	///Copy operator
	_Dam_CI_Element& operator=(const _Dam_CI_Element& object);

protected:
	//members
	///Boundary value (waterlevel), when the CI element fails
	double boundary_value;

	///Time to recover after failure [d]
	double recovery_time;
	///Time to activate [d]
	double activation_time;

	///Time to recover after failure (original) [d]
	double orig_recovery_time;
	///Time to activate (original) [d]
	double orig_activation_time;

	///Regular flag: is the structure regular := true or emergency :=false
	bool regular_flag;




	///Index of the floodplain, to which the CI element is connected
	int index_fp;
	///Index of the floodplain element, to which the CI element is connected
	int index_fp_elem;
	///Flag if CI element is connected to the hydraulic
	bool is_connected;
	///Global index of CI element in the database
	int global_index;

	///Incomings CI elements
	_Dam_CI_Element **incomings;
	///Outgoings CI elements
	_Dam_CI_Element **outgoing;
	///Number of incoming CI elements
	int no_incoming;
	///Number of outgoing CI elements
	int no_outgoing;

	///List with summarized sectors of the incomings; just regular are counted
	QList<QList<int>> list_sec_incoming;

	///List with summarized sectors of the emergency CI-elements
	QList<QList<int>> list_sec_emergency;


	///Id of the sector
	int sector_id;
	///Name of the sector (resulting from id)
	string sector_name;

	///Flag if the element is active
	bool active_flag;
	///Flag if the element was affected
	bool was_affected;
	///Failure type
	string failure_type;
	///Failure type as enum
	_dam_ci_failure_type failure_type_enum;
	//Failure duration
	double failure_duration;
	///Flag if the object is a final position in the net
	bool final_flag;

	///Int for deciding if it is a point (=0) or a polygon (=1)
	int is_point_id;

	///Pointer to the point of the structure
	Geo_Point *ptr_point;


	///Convert failure type (_dam_ci_failure_type) to string
	static string convert_failuretype2txt(const _dam_ci_failure_type type);

	///Transfer the failure type in the enum _dam_ci_failure_type
	_dam_ci_failure_type convert_failuretype2enum(const int id);


private:


	//members
	///Number of list-elements in a block for allocation
	const int block_elems;

	///Allocate the incoming CI-elements
	void allocate_incomings(void);
	///Delete the incoming CI-elements
	void delete_incomings(void);

	///Allocate the outgoing CI-elements
	void allocate_outgoing(void);
	///Delete the outgoing CI-elements
	void delete_outgoing(void);

	///Set error(s)
	Error set_error(const int err_type);


};
#endif
