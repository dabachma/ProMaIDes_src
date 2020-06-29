#pragma once
/**\class Dam_Pys_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/
#ifndef DAM_PYS_SYSTEM_H
#define DAM_PYS_SYSTEM_H

//dam-libs
#include "_Dam_System.h"
#include "Dam_Pys_Category.h"
#include "Dam_Pys_Density_Function.h"
#include "Dam_Pys_Raster.h"

///Class for the managing of the psycho-social damage calculation \ingroup dam
/**

*/
class Dam_Pys_System : public _Dam_System
{
public:
	///Default constructor
	Dam_Pys_System(void);
	///Default destructor
	~Dam_Pys_System(void);

	///The people-density function of the system
	Dam_Pys_Density_Function density_func;
	///Pointer to the psycho-social categories of scores
	Dam_Pys_Category *category;
	///Pointer to the psycho-social raster
	Dam_Pys_Raster *raster;

	//methods
	///Get the number of raster of the psycho-social system
	int get_number_raster(void);
	///Get the number of categories of the psycho-social system
	int get_number_categories(void);

	///Import the psycho-social density-function from a file to the database
	void import_density_function_file2database(QSqlDatabase *ptr_database);
	///Import the psycho-social categories from a file to the database
	void import_categories_file2database(QSqlDatabase *ptr_database);
	///Add the psycho-social categories from a file to the database (the existing one are not deleted)
	void add_categories_file2database(QSqlDatabase *ptr_database);
	///Add a psycho-social damage raster from file to database
	void add_damage_raster_file2database(QSqlDatabase *ptr_database, const bool internaly_used=false);
	///Add a psycho-social damage raster from file to database for multiple raster input
	void add_damage_raster_file2database_multi(QSqlDatabase *ptr_database);
	///Delete all psycho-social data in the database: raster, raster elements, categories, score functions, results etc.
	void del_damage_data_database(QSqlDatabase *ptr_database);

	///Read in the people-density function from file
	void read_density_function_per_file(const string file_func);
	///Read in the people-density function from database
	void read_density_function_per_database(QSqlDatabase *ptr_database);

	///Read in the raster of the psycho-social system from file
	void read_raster_per_file(const int no, string *fname_pop, string *fname_cat, string *fname_crit_50_59, string *fname_crit_80, string *fname_crit_female, string *fname_crit_owner);
	///Read in the general information of the psycho-social raster from database
	void read_damage_raster_per_database_general(QSqlDatabase *ptr_database, const bool with_output=true);

	///Read in the psycho-social category from file
	void read_category_per_file(const string file_category);
	///Read in the psycho-social category from database
	void read_category_per_database(QSqlDatabase *ptr_database);

	///Read in the psycho-social damage system per database
	void read_system_per_database(QSqlDatabase *ptr_database);

	///Ask for the file of the density function per dialog
	bool ask_file_density_function(QWidget *parent=NULL);
	///Ask for the file of the psycho-social categories per dialog
	bool ask_file_categories(QWidget *parent=NULL);
	///Ask for the file(s) of the damage raster(s) per dialog
	bool ask_file_damage_raster(QWidget *parent=NULL);
	///Ask for the file(s) of the damage raster(s) per dialog for multiple raster input
	bool ask_file_damage_raster_multi(void);
	///Ask for the flag, what should be deleted (raster, density functions, categories, results)
	bool ask_deleting_flag(QWidget *parent=NULL);

	///Ask for raster to handled in a dialog
	bool ask_raster2handled_dialog(QSqlDatabase *ptr_database, QWidget *parent=NULL);
	///Get a text for deleting psycho-social information in database
	string get_deleting_text(void);

	///Output the members
	void output_members(void);
	///Output statistic of psycho-social system
	void output_statistic(void);
	///Output the sum over the raster of the damage results to display/console
	void output_result_damage(void);

	///Get the results for the psycho-social criteria age class 50 to 59
	_dam_pys_result get_result_crit_age50_59(void);
	///Get the results for the psycho-social criteria age class greater 80
	_dam_pys_result get_result_crit_age80(void);
	///Get the results for the psycho-social criteria female gender
	_dam_pys_result get_result_crit_female(void);
	///Get the results for the psycho-social criteria home owner
	_dam_pys_result get_result_crit_owner(void);

	///Initialize the raster of the psycho-social damage system
	void init_damage_raster(const int index, QSqlDatabase *ptr_database, const _sys_system_id id);

	///Reduce area of intercepted elements of a psycho-social raster
	void reduce_area_intercepted_elems(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Intercept psycho-social damage elements with the hydraulic elements
	void intercept_hydraulic2damage(Hyd_Model_Floodplain *fp_models, const int number_fp, QSqlDatabase *ptr_database, const _sys_system_id id);
	///Transfer data evaluated by an interception to database: identifier of the floodplain, -floodplain element and the reduced area
	void transfer_intercepted_data2database(QSqlDatabase *ptr_database);

	///Calculate the damages
	void calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number, const int raster_id);
	///Delete the result members for a given system-id and a scenario (boundary-, break-)
	void delete_result_members_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);
	///Output the result members to a database table
	void output_result_member2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int raster_id, bool *was_output);

	///Sum up the total damage results for a given system-id and scenario (boundary-, break-) from the database
	void sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);

	///Check the psycho-social damage system
	void check_system(void);

	///Intercept the psycho-social damage raster with the given polygons and change the psycho-social category
	void intercept_polygons2category_change(QSqlDatabase *ptr_database, const _sys_system_id id, const int number_polys, Dam_Polygon *polys, QList <int> current_id, QList <int> new_id);

	///Get the flag if the raster are to delete
	bool get_raster2delete(void);

private:

	//members
	///Number of categories
	int no_category;
	///Number of rasters
	int no_rasters;

	///Current filename of the density function file
	/** It is set via dialog. \see ask_file_density_function(QWidget *parent=NULL)*/
	string file_density_func;
	///Current filename of the category file
	/** It is set via dialog. \see ask_file_categories(QWidget *parent=NULL)*/
	string file_category_func;
	///Current filename of the population density raster
	/** It is set via dialog. \see ask_file_damage_raster(QWidget *parent=NULL)*/
	string file_density_raster;
	///Current filename of the criteria 50 to 59 years old
	/** It is set via dialog. \see ask_file_damage_raster(QWidget *parent=NULL)*/
	string file_crit_50_59_raster;
	///Current filename of the criteria greater 80 years old
	/** It is set via dialog. \see ask_file_damage_raster(QWidget *parent=NULL)*/
	string file_crit_80_raster;
	///Current filename of the criteria women
	/** It is set via dialog. \see ask_file_damage_raster(QWidget *parent=NULL)*/
	string file_crit_women_raster;
	///Current filename of the criteria home-owner
	/** It is set via dialog. \see ask_file_damage_raster(QWidget *parent=NULL)*/
	string file_crit_home_owner_raster;
	///Current filename of the categories
	/** It is set via dialog. \see ask_file_damage_raster(QWidget *parent=NULL)*/
	string file_category_raster;

	///Flag for deleting the raster data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_raster_flag;
	///Flag for deleting the density function data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_densfunc_flag;
	///Flag for deleting the damage result data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_res_flag;
	///Flag for deleting the psycho-social category data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_cat_flag;

	///Results for the psycho-social criteria age class 50 to 59
	_dam_pys_result res_crit_age50_59;
	///Results for the psycho-social criteria age class greater 80
	_dam_pys_result res_crit_age80;
	///Results for the psycho-social criteria female gender
	_dam_pys_result res_crit_female;
	///Results for the psycho-social criteria home owner
	_dam_pys_result res_crit_owner;
	///Sum of the results for the psycho-social criteria
	_dam_pys_result res_total;

	//methods
	///Allocate the psycho-social categories
	void allocate_categories(void);
	///Delete the psycho-social categories
	void delete_categories(void);

	///Allocate the score function
	void allocate_rasters(void);
	///Delete the score function
	void delete_rasters(void);
	///Check the if there are same raster
	void check_raster(void);
	///Check the psycho-social categories
	void check_categories(void);

	///Connect the element to their psyco-social score functions
	void connect_elems2categories(const int index);

	///Output the error_statistic of the system
	void output_error_statistic(void);

	///Reset the result value of the system
	void reset_result_value(void);

	///Delete selected raster in database
	void delete_selected_raster_database(QSqlDatabase *ptr_database);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif