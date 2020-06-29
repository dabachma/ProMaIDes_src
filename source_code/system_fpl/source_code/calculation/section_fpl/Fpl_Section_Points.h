#pragma once
/**\class Fpl_Section_Points
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef FPL_SECTION_POINTS_H
#define FPL_SECTION_POINTS_H
//libs

//system class
#include "Geo_Point.h"

///Geometric points  of a fpl-section \ingroup fpl
/**

*/
class Fpl_Section_Points : public Geo_Point
{
public:
	///Default constructor
	Fpl_Section_Points(void);
	///Copy constructor
	Fpl_Section_Points(const Fpl_Section_Points& object);
	///Constructor with a given point name 
	Fpl_Section_Points(const string name);
	///Default destructor
	~Fpl_Section_Points(void);

	///Get the point number
	int get_number(void);
	///Get the zonenumber
	int get_zone_number(void);
	///Set the zone number
	void set_zone_number(const int zone);

	///Pointer to the table for storing the information about the geometric points of the fpl-sections
	static Tables *point_table;

	//methods
	///Set the database table for the geometric points of the fpl-sections: it sets the table name and the name of the columns and allocate them
	static void set_point_table(QSqlDatabase *ptr_database);
	///Create the database table for the geometric points of the fpl-sections
	static void create_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for geometric points of the fpl-sections
	static void delete_data_in_point_table(QSqlDatabase *ptr_database);
	///Delete the data in the database table for geometric points of a specific fpl-sections
	static void delete_data_in_point_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const int sec_id);
	///Close and delete the database table for geometric points of the fpl-sections
	static void close_point_table(void);

	///Switch the applied-flag  of the geometrical points in the database table for a defined system state
	static void switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag  of the geometrical points in the database table for a defined system state and section id
	static void switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id);

	///Copy the geometrical points of a fpl-section of a given system id to another one in database tables
	static void copy_fpl_geo_points(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id);


	///Read in a geometric point from the database table and retrieve it 
	void set_point_per_database(const _sys_system_id id_sys, const int section_id, const string point_name, QSqlDatabase *ptr_database, const string region, const string section_type, const int zone_number, const bool with_output=false, const bool applied_important=true); 
	///Read in a geometric point from the database table and retrieve it 
	void set_point_per_database(const _sys_system_id id_sys, const int section_id, const string point_name, QSqlDatabase *ptr_database, const string region, const string section_type,  const bool with_output=false, const bool applied_important=true); 

	///Read in a geometric point from model which was filled with data from a database table and retrieve it 
	void set_point_per_database(QSqlQueryModel *results, const int index); 
	///Set the input for the gemetrical points of a fpl-section per file
	void set_point_per_file(QFile *ifile, int *line_counter, const string name, const int zone_number, const string end_block_str);

	///Transfer the point data to database table
	void transfer_point2database(const _sys_system_id id_sys, const int section_id, QSqlDatabase *ptr_database, const string region, const string section_type);

	///Select points of multiple zone into a QSqlQueryModel
	static void select_different_zone_points(QSqlQueryModel *results, const _sys_system_id id_sys, const int section_id, const string point_name, QSqlDatabase *ptr_database, const string region, const string section_type, const bool with_output=false); 

	///Output the members to display/console
	void output_members(ostringstream *cout);

	


	///Copy operator
	Fpl_Section_Points& operator=(const Fpl_Section_Points& object);

private:
	//members
	///Zonenumber, which the point represented
	int zone_number;

	//methods
	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif
