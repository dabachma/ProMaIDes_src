#pragma once
/**\class Dam_Ecn_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/
#ifndef DAM_ECN_SYSTEM_H
#define DAM_ECN_SYSTEM_H

//dam libs
#include "_Dam_System.h"
#include "Dam_Ecn_Damage_Function.h"
#include "Dam_Ecn_Raster.h"

///Class for the calculation of the economical damage, via damage functions, landuse categories and the impact value based on an geometrical raster \ingroup dam
/**

*/
class Dam_Ecn_System : public _Dam_System
{
public:
	///Default constructor
	Dam_Ecn_System(void);
	///Default destructor
	~Dam_Ecn_System(void);

	//members

	///The damage function of the system
	Dam_Ecn_Damage_Function *dam_function;
	///Pointer to the damage raster, where the geometrical as well as the economic damage information are stored
	Dam_Ecn_Raster *damage_raster;

	///methods

	///Get the number of raster of the ecnomic damage system
	int get_number_raster(void);
	///Import the damage function from a file to the database
	void import_damage_func_file2database(QSqlDatabase *ptr_database);
	///Add the damage function from a file to the database (the existing one are not deleted)
	void add_damage_func_file2database(QSqlDatabase *ptr_database);
	///Add a economical damage raster from file to database
	void add_damage_raster_file2database(QSqlDatabase *ptr_database, const bool internaly_used=false);
	///Add a economical damage raster from file to database for multiple raster input
	void add_damage_raster_file2database_multi(QSqlDatabase *ptr_database);
	///Delete all economical data in the database: raster, raster elements, damage function, results
	void del_damage_data_database(QSqlDatabase *ptr_database);

	///Ask for the file of the damage function per dialog
	bool ask_file_damage_function(QWidget *parent=NULL);
	///Ask for the file(s) of the damage raster(s) per dialog
	bool ask_file_damage_raster(QWidget *parent=NULL);
	///Ask for the file(s) of the damage raster(s) per dialog for multiple raster input
	bool ask_file_damage_raster_multi(void);

	///Ask for raster to handled in a dialog
	bool ask_raster2handled_dialog(QSqlDatabase *ptr_database, QWidget *parent=NULL);

	///Ask for the flag, what should be deleted (raster, damage functions, results)
	bool ask_deleting_flag(QWidget *parent=NULL);

	///Get a text for deleting economic information in database
	string get_deleting_text(void);

	///Read in the economical damage system per database
	void read_damage_system_per_database(QSqlDatabase *ptr_database);

	///Read in the economic damage functions from file
	void read_damage_function_per_file(const string file_damfunc);
	///Read in the economic damage functions from database
	void read_damage_function_per_database(QSqlDatabase *ptr_database);

	///Read in the economic raster from file
	void read_damage_raster_per_file(const int no, string *fname_mob, string *fname_immob, string *fname_mob_stock, string *fname_immob_stock);
	///Read in the general information of the economic raster from database
	void read_damage_raster_per_database_general(QSqlDatabase *ptr_database, const bool with_output=true);

	///Calculate the damages
	void calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number, const int dam_raster_id);
	///Delete the result members for a given system-id and a scenario (boundary-, break-)
	void delete_result_members_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);
	///Output the result members to a database table
	void output_result_member2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int dam_raster_id, bool *was_output);

	///Output the members
	void output_members(void);
	///Output statistic of economical damage system
	void output_statistic(void);
	///Output the sum over the raster of the damage results to display/console
	void output_result_damage(void);

	///Get the results of the immobile damages
	_dam_ecn_result get_immob_results(void);
	///Get the results of the mobile damages
	_dam_ecn_result get_mob_results(void);

	///Initialize the damage raster(s)
	void init_damage_rasters(const int index, QSqlDatabase *ptr_database, const _sys_system_id id);

	///Reduce area of intercepted elements
	void reduce_area_intercepted_elems(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Intercept damage elements with the hydraulic elements
	void intercept_hydraulic2damage(Hyd_Model_Floodplain *fp_models, const int number_fp, QSqlDatabase *ptr_database, const _sys_system_id id);

	///Transfer data evaluated by an interception to database: identifier of the floodplain, -floodplain element and the reduced area
	void transfer_intercepted_data2database(QSqlDatabase *ptr_database);

	///Sum up the total damage results for a given system-id and scenario (boundary-, break-) from the database
	void sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);

	///Check the economic damage system
	void check_system(void);

	///Intercept the economical damage raster with the given polygons and change the landuse-id
	void intercept_polygons2landuse_change(QSqlDatabase *ptr_database, const _sys_system_id id, const int number_polys, Dam_Polygon *polys, QList <int> current_id, QList <int> new_id);

	///Get the flag if the raster are to delete
	bool get_raster2delete(void);

	///Calculate the damage per landuse id by using the calculated results in database
	void calculate_damage_landuse_id(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);

private:

	//members
	///Flag if the raster for individual stockvalues is applied
	bool stockvalue_raster_flag;

	///Number of damage function in the system
	int no_dam_func;

	///Number of damage raster
	int no_dam_raster;

	///Current filename of the damage function file
	/** It is set via dialog. \see ask_file_damage_function(QWidget *parent=NULL)*/
	string file_dam_func;
	///Current filename of the raster for immobile damages land-use categories
	/** It is set via dialog. \see ask_file_damage_raster(QWidget *parent=NULL) */
	string file_raster_immob;
	///Current filename of the raster for mobile damages land-use categories
	/** It is set via dialog. \see ask_file_damage_raster(QWidget *parent=NULL) */
	string file_raster_mob;
	///Current filename of the raster for immobile individual stock-values
	/** It is set via dialog. \see ask_file_damage_raster(QWidget *parent=NULL) */
	string file_raster_immob_stock;
	///Current filename of the raster for mobile individual stock-values
	/** It is set via dialog. \see ask_file_damage_raster(QWidget *parent=NULL) */
	string file_raster_mob_stock;

	///Flag for deleting the raster data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_raster_flag;
	///Flag for deleting the damage function data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_func_flag;
	///Flag for deleting the damage result data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_res_flag;

	///Sum over the raster of the immobile damage results
	_dam_ecn_result result_immob_dam;
	///Sum over the raster of the mobile damage results
	_dam_ecn_result result_mob_dam;
	///Sum over the raster of the total damage results
	_dam_ecn_result result_total_dam;

	//methods
	///Reset the result members
	void reset_result_members(void);
	///Connect the element to their functions
	void connect_elems2functions(const int index);
	///Set intercepted elements to no-info
	void set_intercept_elems2no_info(void);

	///Allocate the damage functions
	void allocate_damage_function(void);
	///Delete the damage functions
	void delete_damage_function(void);

	///Allocate the damage raster
	void allocate_damage_raster(void);
	///Delete the damage raster
	void delete_damage_raster(void);

	///Check the if there are same raster
	void check_raster(void);
	///Check the damage function
	void check_damage_function(void);

	///Output the damage function to display/console
	void output_dam_func(void);
	///Output the rasters to display/console
	void output_raster(void);

	///Output the error_statistic of the system
	void output_error_statistic(void);

	///Delete selected raster in database
	void delete_selected_raster_database(QSqlDatabase *ptr_database);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif