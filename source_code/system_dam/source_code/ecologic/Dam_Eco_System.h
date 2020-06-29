#pragma once
/**\class Dam_Eco_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/
#ifndef DAM_ECO_SYSTEM_H
#define DAM_ECO_SYSTEM_H

#include <qsplitter.h>
//dam-libs
#include "_Dam_System.h"
#include "Dam_Eco_Btype.h"
#include "Dam_Eco_Btype_Raster.h"
#include "Dam_Eco_Soil_Raster.h"
#include "Dam_Eco_Soil_Erosion_Function.h"
#include "Dam_Eco_Soil_Cost_Function.h"

///Class for the calculation of the ecological damage \ingroup dam
/**

*/
class Dam_Eco_System : public _Dam_System
{
public:
	///Default constructor
	Dam_Eco_System(void);
	///Default destructor
	~Dam_Eco_System(void);

	//member
	///Pointer to the biotope-types
	Dam_Eco_Btype *b_types;
	///Pointer to the raster of the biotope-types
	Dam_Eco_Btype_Raster *raster_btypes;

	///Pointer to the raster of the soil-erosion
	Dam_Eco_Soil_Raster *soil_raster;
	///Pointer to the soil-erosion functions
	Dam_Eco_Soil_Erosion_Function *soil_ero_function;
	///Pointer to the soil-cost functions
	Dam_Eco_Soil_Cost_Function *soil_cost_function;

	//methods
	///Get the number of biotope-type raster
	int get_number_biotope_raster(void);
	///Get the number of soil-type raster
	int get_number_soil_raster(void);
	///Import the biotope types from a file to the database
	void import_biotope_types_file2database(QSqlDatabase *ptr_database);
	///Add the biotope types from a file to the database (the existing one are not deleted)
	void add_biotope_types_file2database(QSqlDatabase *ptr_database);
	///Add a biotope type raster from file to database
	void add_biotope_raster_file2database(QSqlDatabase *ptr_database, const bool internaly_used=false);
	///Add a biotope type raster from file to database for multiple raster input
	void add_biotope_raster_file2database_multi(QSqlDatabase *ptr_database);

	///Delete all biotope-type data in the database: raster, raster elements, biotope-types, results
	void del_biotope_data_database(QSqlDatabase *ptr_database);

	///Ask for the file of the biotope types per dialog
	bool ask_file_biotope_types(QWidget *parent=NULL);
	///Ask for the file of the biotope raster per dialog
	bool ask_file_biotope_raster(QWidget *parent=NULL);
	///Ask for the file(s) of the biotope raster(s) per dialog for multiple raster input
	bool ask_file_biotope_raster_multi(void);
	///Ask for the flag, what should be deleted (raster, biotope-type, results)
	bool ask_deleting_biotope_flag(QWidget *parent=NULL);

	///Ask for biotope raster to handled in a dialog
	bool ask_biotope_raster2handled_dialog(QSqlDatabase *ptr_database, QWidget *parent=NULL);
	///Get a text for deleting biotope information in database
	string get_biotope_deleting_text(void);

	///Read in the ecologic biotope-types from file
	void read_biotope_types_per_file(const string file_biotype);
	///Read in the ecologic biotope-types from database
	void read_biotope_types_per_database(QSqlDatabase *ptr_database);

	///Read in the ecologic biotope raster from file
	void read_biotope_raster_per_file(const int no, string *fname);
	///Read in the general information of the ecologic biotope raster from database
	void read_biotope_raster_per_database_general(QSqlDatabase *ptr_database, const bool with_output=true);

	///Get the flag if the biotope raster are to delete
	bool get_biotope_raster2delete(void);

	///Import the ecological soil types and their cost function from a file to the database
	void import_soil_type_cost_function_file2database(QSqlDatabase *ptr_database);
	///Add the ecological soil types and their cost function from a file to the database (the existing one are not deleted)
	void add_soil_type_cost_function_file2database(QSqlDatabase *ptr_database);
	///Import ecological soil erosion types and their function from a file to the database
	void import_soil_type_erosion_function_file2database(QSqlDatabase *ptr_database);
	///Add a ecological soil erosion raster from file to database
	void add_soil_type_erosion_raster_file2database(QSqlDatabase *ptr_database, const bool internaly_used=false);
	///Add a ecological soil erosion raster from file to database for multiple raster input
	void add_soil_type_erosion_raster_file2database_multi(QSqlDatabase *ptr_database);

	///Delete all soil-erosion data in the database: raster, raster elements, cost-function, erosion-function, results
	void del_soil_erosion_data_database(QSqlDatabase *ptr_database);

	///Ask for the file of the ecological soil types and their cost function per dialog
	bool ask_file_soil_type_cost_function(QWidget *parent=NULL);
	///Ask for the file of the ecological soil erosion types and their function per dialog
	bool ask_file_soil_type_erosion_function(QWidget *parent=NULL);
	///Ask for the file of the ecological soil erosion raster per dialog
	bool ask_file_soil_type_erosion_raster(QWidget *parent=NULL);
	///Ask for the file(s) of the ecological soil erosion raster(s) per dialog for multiple raster input
	bool ask_file_soil_type_erosion_raster_multi(void);
	///Ask for the flag, what should be deleted (raster, cost-function, erosion function, results)
	bool ask_deleting_soil_erosion_flag(QWidget *parent=NULL);

	///Ask for soil erosion raster to handled in a dialog
	bool ask_soil_erosion_raster2handled_dialog(QSqlDatabase *ptr_database, QWidget *parent=NULL);
	///Get a text for deleting soil erosion information in database
	string get_soil_erosion_deleting_text(void);

	///Read in the ecological soil-cost types and their cost function from file
	void read_soil_type_cost_function(const string file);
	///Read in the ecological soil-cost types and their cost function from database
	void read_soil_type_cost_function_per_database(QSqlDatabase *ptr_database);

	///Read in the ecological soil-erosion types and their function from file
	void read_soil_type_erosion_function(const string file);
	///Read in the ecological soil-erosion types and their function from database
	void read_soil_type_erosion_function_per_database(QSqlDatabase *ptr_database);

	///Read in the ecological soil-erosion raster from file
	void read_soil_type_erosion_rasters(const int no, string *fname_type, string *fname_erosion);
	///Read in the general information of th ecological soil-erosion raster from database
	void read_soil_type_erosion_raster_per_database_general(QSqlDatabase *ptr_database, const bool with_output=true);

	///Read inf the ecological system per database
	void read_system_per_database(QSqlDatabase *ptr_database);

	///Calculate the damages for the soil-erosion types
	void calculate_soil_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number, const int raster_id);
	///Calculate the damages for the biotope types
	void calculate_biotope_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number, const int raster_id);
	///Delete the soil-erosion result members for a given system-id and a scenario (boundary-, break-)
	void delete_soil_result_members_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);
	///Delete the biotope result members for a given system-id and a scenario (boundary-, break-)
	void delete_biotope_result_members_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);
	///Output the soil-erosion result members to a database table
	void output_soil_result_member2database(QSqlDatabase *ptr_database,  const int bound_sz, const string break_sz, const int raster_id, bool *was_output);
	///Output the biotope result members to a database table
	void output_biotope_result_member2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int raster_id, bool *was_output);

	///Get the flag if the soil erosion raster are to delete
	bool get_soil_erosion_raster2delete(void);

	///Output the members
	void output_members(void);
	///Output statistic of ecological damage system
	void output_statistic(void);

	///Output the sum over the raster of the damage results to display/console
	void output_result_damage(void);

	///Get the results of the biotope-tape damages
	double get_biotope_type_damages(void);
	///Get the results of the biotope-tape damages
	double get_soil_erosion_damages(void);

	///Initialize the soil damage raster
	void init_soil_rasters(const int index, QSqlDatabase *ptr_database, const _sys_system_id id);
	///Initialize the biotope damage raster
	void init_biotope_rasters(const int index, QSqlDatabase *ptr_database, const _sys_system_id id);

	///Reduce area of intercepted elements
	void reduce_area_intercepted_elems(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Intercept damage elements with the hydraulic elements
	void intercept_hydraulic2damage(Hyd_Model_Floodplain *fp_models, const int number_fp, QSqlDatabase *ptr_database, const _sys_system_id id);

	///Transfer data evaluated by an interception to database: identifier of the floodplain, -floodplain element and the reduced area
	void transfer_intercepted_data2database(QSqlDatabase *ptr_database);

	///Sum up the total damage results for a given system-id and scenario (boundary-, break-) from the database
	void sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);

	///Check the ecologic damage system
	void check_system(void);

	///Intercept the biotope-type damage raster with the given polygons and change the biotope-type-id
	void intercept_polygons2biotope_type_change(QSqlDatabase *ptr_database, const _sys_system_id id, const int number_polys, Dam_Polygon *polys, QList <int> current_id, QList <int> new_id);
	///Intercept the soil-erosion damage raster with the given polygons and change the soil-type-id
	void intercept_polygons2soil_type_change(QSqlDatabase *ptr_database, const _sys_system_id id, const int number_polys, Dam_Polygon *polys, QList <int> current_id, QList <int> new_id);

private:

	//member
	///Number of biotope-types
	int number_btype;
	///Number of raster for the biotope-types
	int number_raster_btype;

	///Number soil-erosion raster
	int number_soil_ero_raster;
	///Number soil-erosion functions
	int number_soil_ero_function;
	///Number soil-cost functions
	int number_soil_cost_function;

	///Current filename of the ecologic biotope-type file
	/** It is set via dialog. \see ask_file_biotope_types(QWidget *parent=NULL)*/
	string file_b_type;
	///Current filename of the raster for the biotope-types
	/** It is set via dialog. \see ask_file_biotope_raster(QWidget *parent=NULL) */
	string file_raster_b_type;
	///Current filename of the ecological soil types and their cost function
	/** It is set via dialog. \see ask_file_soil_type_cost_function(QWidget *parent=NULL) */
	string file_soil_cost;
	///Current filename of the ecological soil erosion types and their function
	/** It is set via dialog. \see ask_file_soil_type_erosion_function(QWidget *parent=NULL) */
	string file_soil_erosion;
	///Current filename of the raster for soil-cost types
	/** It is set via dialog. \see ask_file_soil_type_erosion_raster(QWidget *parent=NULL) */
	string file_raster_soil_cost;
	///Current filename of the raster for soil-erosion types
	/** It is set via dialog. \see ask_file_soil_type_erosion_raster(QWidget *parent=NULL) */
	string file_raster_soil_erosion;

	///Flag for deleting the raster data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_raster_flag;
	///Flag for deleting the biotope-type data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_btype_flag;
	///Flag for deleting the damage result data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_res_flag;
	///Flag for deleting the soil-erosion function data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_soil_ero_flag;
	///Flag for deleting the soil-cost function data in the database
	/** It is set via dialog. \see ask_deleting_flag(QWidget *parent=NULL) */
	bool del_soil_cost_flag;

	///Sum over the raster of the biotope-type damages
	double result_biotope_dam;
	///Sum over the raster of the soil-erosion damages
	double result_soil_ero_dam;

	//methods
	///Connect the soil-erosion elements to the functions
	void connect_soil_elems2functions(const int index);
	///Connect the biotope elements to the functions
	void connect_biotope_elems2functions(const int index);
	///Set intercepted elements to no-info
	void set_intercept_elems2no_info(void);

	///Allocate the biotope-types
	void allocate_btypes(void);
	///Delete the biotope-types
	void delete_btypes(void);
	///Allocate the raster of the biotope-types
	void allocate_btypes_raster(void);
	///Delete the raster of the biotope-types
	void delete_btypes_raster(void);

	///Allocate the raster of the soil-erosion
	void allocate_soil_ero_raster(void);
	///Delete the raster of the soil-erosion
	void delete_soil_ero_raster(void);
	///Allocate the functions of the soil-erosion
	void allocate_soil_ero_function(void);
	///Delete the functions of the soil-erosion
	void delete_soil_ero_function(void);
	///Allocate the functions of the soil-cost
	void allocate_soil_cost_function(void);
	///Delete the functions of the soil-cost
	void delete_soil_cost_function(void);

	///Check the if there are same soil-erosion raster
	void check_soil_erosion_raster(void);
	///Check the soil-erosion functions
	void check_soil_erosion_function(void);
	///Check the soil-cost functions
	void check_soil_cost_function(void);

	///Check the if there are same biotope-type raster
	void check_b_type_raster(void);
	///Check the biotope-types
	void check_biotope_type(void);

	///Output the error-statistic of the system
	void output_error_statistic(void);

	///Delete selected biotope raster in database
	void delete_selected_biotope_raster_database(QSqlDatabase *ptr_database);
	///Delete selected soil erosion raster in database
	void delete_selected_soil_erosion_raster_database(QSqlDatabase *ptr_database);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
