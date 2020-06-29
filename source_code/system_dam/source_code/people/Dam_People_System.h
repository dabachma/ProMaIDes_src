#pragma once
/**\class Dam_People_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/
#ifndef DAM_PEOPLE_SYSTEM_H
#define DAM_PEOPLE_SYSTEM_H

//dam-libs
#include "_Dam_System.h"
#include "Dam_People_Raster.h"
#include "Dam_People_Damage_Function.h"

///Class for the calculation of the damage to people \ingroup dam
/**

*/
class Dam_People_System : public _Dam_System
{
public:
	///Default constructor
	Dam_People_System(void);
	///Default destructor
	~Dam_People_System(void);

	//member
	///Pointer to the damage function
	Dam_People_Damage_Function *dam_function;
	///Pointer to the people2risk raster
	Dam_People_Raster *raster;

	//method
	///Get the number of raster of the people2risk damage system
	int get_number_raster(void);
	///Import the damage function for the people2risk calculation from a file to the database
	void import_damage_func_file2database(QSqlDatabase *ptr_database);
	///Add the damage function for the people2risk calculation from a file to the database (the existing one are not deleted)
	void add_damage_func_file2database(QSqlDatabase *ptr_database);
	///Add a people2risk raster from file to database
	void add_people2risk_raster_file2database(QSqlDatabase *ptr_database, const bool internaly_used=false);
	///Add a people2risk damage raster from file to database for multiple raster input
	void add_people2risk_raster_file2database_multi(QSqlDatabase *ptr_database);
	///Delete all people2risk data in the database: raster, raster elements, damage function, results
	void del_damage_data_database(QSqlDatabase *ptr_database);

	///Ask for the file of the damage function for the people2risk calculation per dialog
	bool ask_file_damage_function(QWidget *parent=NULL);
	///Ask for the file(s) of the people2risk raster(s) per dialog
	bool ask_file_people2risk_raster(QWidget *parent=NULL);
	///Ask for the file(s) of the damage raster(s) per dialog for multiple raster input
	bool ask_file_people2risk_raster_multi(void);
	///Ask for the flag, what should be deleted (raster, damage functions, results)
	bool ask_deleting_flag(QWidget *parent=NULL);

	///Read in the people2risk damage system per database
	void read_people2risk_system_per_database(QSqlDatabase *ptr_database);

	///Calculate the damages
	void calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number, const int raster_id);
	///Delete the result members for a given system-id and a scenario (boundary-, break-)
	void delete_result_members_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);
	///Output the result members to a database table
	void output_result_member2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int raster_id, bool *was_output);

	///Read in the damage function for the people2risk calculation per file
	void read_damage_function_per_file(const string file_dam_func);
	///Read in the people2risk damage functions from database
	void read_damage_function_per_database(QSqlDatabase *ptr_database);

	///Read a people2risk raster from file
	void read_people2risk_raster_per_file(const int no, string *fname_type, string *fname_people);
	///Read in the general people2risk raster from database
	void read_people2risk_raster_per_database_general(QSqlDatabase *ptr_database, const bool with_output=true);

	///Ask for raster to handled in a dialog
	bool ask_raster2handled_dialog(QSqlDatabase *ptr_database, QWidget *parent=NULL);
	///Get a text for deleting people2risk information in database
	string get_deleting_text(void);

	///Output the members
	void output_members(void);
	///Output statistic of people2risk damage system
	void output_statistic(void);
	///Output the sum over the raster of the damage results to display/console
	void output_result_damage(void);

	///Get the results of the affected people
	double get_affected_pop_results(void);
	///Get the results of the endangered people
	double get_endangered_pop_results(void);

	///Initialize the people2risk raster(s)
	void init_damage_rasters(const int index, QSqlDatabase *ptr_database, const _sys_system_id id);

	///Reduce area of intercepted elements of a people2risk raster
	void reduce_area_intercepted_elems(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Intercept people2risk damage elements with the hydraulic elements
	void intercept_hydraulic2damage(Hyd_Model_Floodplain *fp_models, const int number_fp, QSqlDatabase *ptr_database, const _sys_system_id id);

	///Transfer data evaluated by an interception to database: identifier of the floodplain, -floodplain element and the reduced area
	void transfer_intercepted_data2database(QSqlDatabase *ptr_database);

	///Sum up the total damage results for a given system-id and scenario (boundary-, break-) from the database
	void sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);

	///Check the people2risk damage system
	void check_system(void);

	///Intercept the people2risk damage raster with the given polygons and change the people2risk category
	void intercept_polygons2category_change(QSqlDatabase *ptr_database, const _sys_system_id id, const int number_polys, Dam_Polygon *polys, QList <int> current_id, QList <int> new_id);

	///Get the flag if the raster are to delete
	bool get_raster2delete(void);

private:

	//member
	///Number of damage function
	int number_dam_function;
	///Number of people2risk-raster
	int number_raster;

	///Current filename of the damage function file for the people2risk calculation
	/** It is set via dialog. \see ask_file_damage_function(QWidget *parent=NULL)*/
	string file_dam_func;
	///Current filename of the raster for people2risk damage type
	/** It is set via dialog. \see ask_file_people2risk_raster(QWidget *parent=NULL) */
	string file_raster_type;
	///Current filename of the raster for the population density
	/** It is set via dialog. \see ask_file_people2risk_raster(QWidget *parent=NULL) */
	string file_raster_pop_dens;

	///Flag for deleting the raster data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_raster_flag;
	///Flag for deleting the damage function data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_func_flag;
	///Flag for deleting the damage result data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_res_flag;

	///Sum over the raster of the people affected results
	double result_affected_pop;
	///Sum over the raster of the people endangered results
	double result_endangered_pop;

	//method
	///Connect the element an their functions
	void connect_elems2functions(const int index);

	///Allocate the people2risk damage function
	void allocate_damage_function(void);
	///Delete the people2risk damage function
	void delete_damage_function(void);
	///Allocate the people2risk raster
	void allocate_raster(void);
	///Delete the people2risk raster
	void delete_raster(void);

	///Check the if there are same raster
	void check_raster(void);
	///Check the damage function
	void check_damage_function(void);

	///Output the error-statistic of the system
	void output_error_statistic(void);

	///Delete selected raster in database
	void delete_selected_raster_database(QSqlDatabase *ptr_database);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif