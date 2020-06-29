#pragma once
/**\class Fpl_Dike_Geo_Materialzone
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/

#ifndef FPL_DIKE_GEO_MATERIALZONE_H
#define FPL_DIKE_GEO_MATERIALZONE_H

///Enumerator for specifiying the material variable zone \ingroup fpl
enum _fpl_zone_type{
	///Zone as impermeable layer
	imperm_layer,
	///Zone as permeable layer
	perm_layer,
	///Not further specified
	not_set_zone,
};

//system sys
#include "Geo_Simple_Polygon.h"
#include "Fpl_Dike_Var_Materialzone.h"

//system fpl



///Geometrical class of the materialzones of a dike body and his underground \ingroup fpl
/**


*/

class Fpl_Dike_Geo_Materialzone: public Geo_Simple_Polygon, public _Sys_Common_System
{
public:
	///Default constructor
	Fpl_Dike_Geo_Materialzone(void);
	///Default destructor
	~Fpl_Dike_Geo_Materialzone(void);


	//members
	///Pointer to the table for storing the material zones 
	static Tables *matzone_table;
	///Pointer to the table for storing the material zone points
	static Tables *matzone_point_table;

	//methods
	///Read in the material zone from file
	void read_matzone_per_file(QFile *ifile, int *line_counter);
	///Transfer the members read in per file to database
	void transfer_matzone2database(QSqlDatabase *ptr_database, const int section_id);

	///Set the input of the material zones per database
	void set_input(QSqlQueryModel *results, const int index, const int section_id,  QSqlDatabase *ptr_database);

	///Create the database table for the material zones
	static void create_matzone_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the material zones
	static void delete_data_in_matzone_table(QSqlDatabase *ptr_database);
	///Delete the data in the database table for the material zones of a specific fpl-sections
	static void delete_data_in_matzone_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const int sec_id);

	///Set the database table for the material zones: it sets the table name and the name of the columns and allocate them
	static void set_matzone_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the material zones
	static void close_matzone_table(void);

	///Switch the applied-flag material zones in the database table for a defined system state
	static void switch_applied_flag_matzone_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag material zones in the database table for a defined system state and section id
	static void switch_applied_flag_matzone_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id);

	///Copy the material zones of a fpl-section of a given system id to another one in database tables
	static void copy_fpl_matzone(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id);


	///Select and count the number of relevant material zones in a database table
	static int select_relevant_matzone_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, const bool with_output = true);

	///Create the database table for the material zone points
	static void create_matzone_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the material zone points
	static void delete_data_in_matzone_point_table(QSqlDatabase *ptr_database);
	///Set the database table for the material zone points: it sets the table name and the name of the columns and allocate them
	static void set_matzone_point_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the material zone points
	static void close_matzone_point_table(void);

	///Select and count the number of relevant material zone points in a database table
	static int select_relevant_matzone_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id,const int section_id, const int zone_id);

	///Get the zone name
	string get_zone_name(void);
	///Get the zone number
	int get_zone_number(void);
	///Set the zone number
	void set_zone_number(const int number);
	///Get the material variable zone number
	int get_variable_zone_number(void);

	///Set the pointer to the relevant variables
	void set_ptr_relevant_variables(Fpl_Dike_Var_Materialzone *variables);
	///Get the pointer to the relevant variables
	Fpl_Dike_Var_Materialzone* get_ptr_relevant_variables(void);

	///Output the material zone to display/console
	void output_member(void);

	///Output zone to tecplot
	void output2tecplot(ofstream *output);

	///Get the zone type
	_fpl_zone_type get_zone_type(void);

	///Convert a string into the zone type (_fpl_zone_type)
	static _fpl_zone_type convert_txt2zone_type(const string txt);
	///Convert a zone type (_fpl_zone_type) into a string 
	static string convert_zone_type2txt(const _fpl_zone_type type);

	///Get the minimal x-coordiante 
	double get_min_x_coordinate(void);


private:

	//members
	///Number of the material zone
	int zone_number;
	///Number of the material variable zone
	int variable_zone_number;
	///Name of the material zone
	string zone_name;

	///Specifies the zone type
	_fpl_zone_type zone_type;

	///Pointer to the slope relevant variables of the material zone
	Fpl_Dike_Var_Materialzone *variables;

	//method

	///Delete the data in the database table for the material zone points of a specific fpl-sections
	static void delete_data_in_matzone_point_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const int sec_id);
	///Switch the applied-flag material zone points in the database table for a defined system state
	static void switch_applied_flag_matzone_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag material zone points in the database table for a defined system state and section id
	static void switch_applied_flag_matzone_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id);
	///Copy the material zone points of a fpl-section of a given system id to another one in database tables
	static void copy_fpl_matzone_point(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id);



	///Transfer the point data to database table
	void transfer_point_data2database(QSqlDatabase *ptr_database, const int section_id);
	///Find the start of an input block of the material zone
	qint64 find_start_block_file(QFile *ifile, int *line_counter, const string begin, const string end, const bool must_found);

	///Find the key-values for the file input of the material zone data
	void find_key_values_file(const string myline, int *must_found_counter);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};

#endif