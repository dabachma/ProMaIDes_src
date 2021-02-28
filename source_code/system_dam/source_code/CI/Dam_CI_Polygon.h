#pragma once
/**\class _Dam_System
	\author produced by AG FRM HS-M
	\version 0.0.1
	\date 2021
*/

#ifndef DAM_CI_POLYGON
#define DAM_CI_POLYGON



//sys libs
#include "_Sys_Common_System.h"
#include "Sys_Multi_Filechooser_Dia.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Diverse_Text_Dia.h"
#include "Geo_Simple_Polygon.h"

//dam-libs
#include "_Dam_CI_Element.h"


///Class for the polygons as CI elements points which are used in the CI-damage calculation  \ingroup dam
/**
The CI-elements, points and polygons, are connected by lists (\ref Dam_CI_Element_List) with each other. In- and outcomming list are available.
*/
class Dam_CI_Polygon : public _Dam_CI_Element, public Geo_Simple_Polygon
{
public:
	///Default constructor
	Dam_CI_Polygon(void);
	///Default destructor
	~Dam_CI_Polygon(void);

	//members
	///Pointer to the table for the CI polygon information in a database
	static Tables *polygon_table;
	///Pointer to the table for the results of the damage calculation for the CI polygon in a database
	static Tables *polygon_erg_table;
	///Pointer to the table for the instationary results of the damage calculation for the CI polygon in a database
	static Tables *polygon_instat_erg_table;



	//method
	///Create the database table for the CI polygons
	static void create_polygon_table(QSqlDatabase *ptr_database);
	///Set the database table for the CI polygon: it sets the table name and the name of the columns and allocate them
	static void set_polygon_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the CI polygon
	static void delete_data_in_polygon_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the CI polygon of the given system state
	static void delete_data_in_polygon_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Close and delete the database table for the CI polygon
	static void close_polygon_table(void);
	///Get the maximum value of the global index of the CI polygon database table
	static int get_max_glob_id_polygon_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the CI polygon data to database table
	static string get_insert_header_polygon_table(QSqlDatabase *ptr_database);

private:

	//members
	///Pointer to the table for the polygon point data in a database
	static Tables *polygon_point_table;


	//methods
	///Create the database table for the polygon point data in a database
	static void create_point_table(QSqlDatabase *ptr_database);
	///Set the database table for the polygon point data: it sets the table name and the name of the columns and allocate them
	static void set_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the polygon point data
	static void delete_point_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the polygon point data
	static void close_point_table(void);
	///Transfer the polygon point data of a file into a database table
	void transfer_polygon_point2database_table(QSqlDatabase *ptr_database, const int poly_id);
	///Select and count the number of relevant polygon points in a database table
	static int select_relevant_points_database(QSqlTableModel *results, const int global_polygon_id);



	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
