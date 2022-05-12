#pragma once
/**\class Dam_Sc_Point_Manager
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/
#ifndef DAM_SC_POINT_MANAGER_H
#define DAM_SC_POINT_MANAGER_H

//dam-libs

#include "Dam_Sc_Point.h"

///Class for the managing the points of the simple counting damages \ingroup dam
/**

*/
class Dam_Sc_Point_Manager
{
public:
	///Default constructor
	Dam_Sc_Point_Manager(void);
	///Default destructor
	~Dam_Sc_Point_Manager(void);

	//methods
	///Read in the simple counting point data from file
	void read_points_per_file(string fname);

	///Transfer the simple counting point data to a database: the point information
	void transfer_input_members2database(QSqlDatabase *ptr_database);

	///Input the simple counting point data per database: point information
	void input_perdatabase_point_data(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Output the members of simple counting point data to display/console
	void output_member(void);
	///Output the sum of the damage results to display/console
	void output_result_damage(void);

	///Sum up the total damage results from database
	void sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);

	///Connect the simple-counting subcategory to the point and calculate the damage per subcategory
	double connect_sc_subcategory2points(const int number_categories, Dam_Sc_Subcategory *sc_subcategory);

	///Set intercepted point to the indizes of the hydraulic floodplain
	void set_intercepted_point2floodplain_id(Hyd_Model_Floodplain *fp_model);
	///Transfer point data evaluated by an interception to database: identifier of the floodplain, -floodplain element
	void transfer_intercepted_point_data2database(QSqlDatabase *ptr_database);

	///Check the connection to the hydraulic of the points
	static bool check_connection2hydraulic(QSqlDatabase *ptr_database,  const _sys_system_id id);

	///Get the number of points
	int get_number_points(void);

	///Calculate the damages
	void calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number);

	///Output the simple counting damages to database
	void output_results2database(QSqlDatabase *ptr_database,const int bound_sz, const string break_sz, bool *was_output, const bool must_output);

	///Check if the points are connected to the hydraulic
	int check_points_connected2hyd(void);

	///Get the affected score for category public buildings
	double get_score_pub_build(void);
	///Get the affected score for category ecologic perilous sites or buildings
	double get_score_eco_build(void);
	///Get the affected score for category cultural heritage
	double get_score_cult_build(void);
	///Get the affected score for category buildings with highly vulnerable person
	double get_score_person_build(void);


	///Reset the flag, that the hydraulic connection is set 
	static void reset_hydraulic_connection_flag(QSqlDatabase *ptr_database);

private:

	//members

	///Number of points
	int number_points;
	///Pointer to the simple-counting information of each raster element
	Dam_Sc_Point *point;

	///Affected score for category 1 public buildings
	double affected_score_pup_build;
	///Affected score for category 2 ecologic perilous sites or buildings
	double affected_score_eco_build;
	///Affected score for category 3 cultural heritage
	double affected_score_cult_build;
	///Affected score for category 4 buildings with highly vulnerable person
	double affected_score_person_build;

	//methods

	///Reset the result value
	void reset_result_value(void);

	///Allocate the simple-counting points
	void allocate_points(void);
	///Delete the simple-counting points
	void delete_points(void);
	///Set the system-id to thepoint information
	void set_sys_system_id(const _sys_system_id id);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif