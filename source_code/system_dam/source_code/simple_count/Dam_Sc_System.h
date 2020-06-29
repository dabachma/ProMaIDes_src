#pragma once
/**\class Dam_Sc_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2012
*/
#ifndef DAM_SC_SYSTEM_H
#define DAM_SC_SYSTEM_H

//dam-libs
#include "_Dam_System.h"
#include "Dam_Sc_Subcategory.h"
#include "Dam_Sc_Point_Manager.h"

///Class for the managing of the simple counting categories \ingroup dam
/**

*/
class Dam_Sc_System : public _Dam_System
{
public:
	///Default constructor
	Dam_Sc_System(void);
	///Default destructor
	~Dam_Sc_System(void);

	//members
	///Manager of the point data
	Dam_Sc_Point_Manager point_manager;

	//methods
	///Import the simple counting subcategories from a file to the database
	void sc_subcategory_file2database(QSqlDatabase *ptr_database);
	///Read in the simple counting subcategories from database
	void read_sc_subcategory_per_database(QSqlDatabase *ptr_database);

	///Add a simple counting point data from file to database
	void add_damage_points_file2database(QSqlDatabase *ptr_database);
	///Read in the simple counting points from database
	void read_sc_points_per_database(QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output=true);

	///Read in the simple counting damage system per database
	void read_system_per_database(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Delete all simple counting data in the database: points, subcategories, results etc.
	void del_damage_data_database(QSqlDatabase *ptr_database);

	///Ask for the file of the simple counting subcategories per dialog
	bool ask_file_subcategories(QWidget *parent=NULL);
	///Ask for the file of the simple counting damage points per dialog
	bool ask_file_damage_points(QWidget *parent=NULL);

	///Ask for the flag, what should be deleted (points, subcategories)
	bool ask_deleting_flag(QWidget *parent);

	///Output the members
	void output_members(void);
	///Output statistic of simple counting system
	void output_statistic(void);
	///Output the damage results to display/console
	void output_result_damage(void);

	///Intercept simple counting points with the hydraulic elements
	void intercept_hydraulic2damage(Hyd_Model_Floodplain *fp_models, const int number_fp, QSqlDatabase *ptr_database, const _sys_system_id id);
	///Transfer data evaluated by an interception to database: identifier of the floodplain, -floodplain element
	void transfer_intercepted_data2database(QSqlDatabase *ptr_database);

	///Calculate the damages
	void calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number);
	///Delete the result members for a given system-id and a scenario (boundary-, break-)
	void delete_result_members_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);
	///Output the result members to a database table
	void output_result_member2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output);

	///Sum up the total damage results for a given system-id and scenario (boundary-, break-) from the database
	void sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);

	///Check the damage system
	void check_system(void);

	///Get a text for deleting simple counting information in database
	string get_deleting_text(void);

private:

	//members
	///Number of subcategories
	int number_subcategories;
	///Pointer to the subcategories
	Dam_Sc_Subcategory *subcategories;

	///Current filename of the subcategories
	/** It is set via dialog. \see ask_file_subcategories(QWidget *parent=NULL)*/
	string file_subcat;
	///Current filename of the points
	/** It is set via dialog. \see ask_file_points(QWidget *parent=NULL)*/
	string file_points;

	///Flag for deleting the subcategories in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_subcat_flag;
	///Flag for deleting the points data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_points_flag;
	///Flag for deleting the damage result data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_res_flag;

	///Score not connected to subcategory
	double score_not_con2subcat;

	//methods
	///Read in the simple counting subcategories from file
	void read_sc_subcategory_per_file(const string file);
	///Transfer the data of the simple counting subcategories to database
	void transfer_subcat2database(QSqlDatabase *ptr_database);

	///Allocate the simple counting categories
	void allocate_subcategories(void);
	///Delete the simple counting categories
	void delete_subcategories(void);

	///Output the error_statistic of the system
	void output_error_statistic(void);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif