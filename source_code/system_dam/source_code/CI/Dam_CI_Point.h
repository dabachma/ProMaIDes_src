#pragma once
/**\class _Dam_System
	\author produced by AG FRM HS-M
	\version 0.0.1
	\date 2021
*/

#ifndef DAM_CI_POINT
#define DAM_CI_POINT



//sys libs
#include "_Sys_Common_System.h"
#include "Sys_Multi_Filechooser_Dia.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Diverse_Text_Dia.h"
#include "Geo_Point.h"

//dam-libs
#include "_Dam_CI_Element.h"

///Class for the points as CI (Critical Infrastructure) elements points which are used in the CI-damage calculation  \ingroup dam
/**
The CI-elements, points and polygons, are connected by lists (\ref Dam_CI_Element_List) with each other. In- and outcomming list are available.
*/


class Dam_CI_Point : public _Dam_CI_Element , public Geo_Point
{
public:
	///Default constructor
	Dam_CI_Point(void);
	///Default destructor
	~Dam_CI_Point(void);

	//members
	///Pointer to the table for the CI point information in a database
	static Tables *point_table;
	///Pointer to the table for the results of the damage calculation for the CI points in a database
	static Tables *point_erg_table;
	///Pointer to the table for the instationary results of the damage calculation for the CI point in a database
	static Tables *point_instat_erg_table;


	//methods
		///Input the CI point data per database
	void input_point_perdatabase(const QSqlQueryModel *results, const int glob_index);

	///Get a string to set the CI point data from the raster interception to the database table: identifier of the floodplain, -floodplain element
	bool get_string_interception_point_data2database(ostringstream *text);

	///Create the database table for the CI points
	static void create_point_table(QSqlDatabase *ptr_database);
	///Set the database table for the CI points: it sets the table name and the name of the columns and allocate them
	static void set_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the CI points
	static void delete_data_in_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the CI points of the given system state
	static void delete_data_in_point_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Close and delete the database table for the CI points
	static void close_point_table(void);
	///Get the maximum value of the global index of the CI point database table
	static int get_max_glob_id_point_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the CI point data to database table
	static string get_insert_header_point_table(QSqlDatabase *ptr_database);

private:
	
	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};

#endif

