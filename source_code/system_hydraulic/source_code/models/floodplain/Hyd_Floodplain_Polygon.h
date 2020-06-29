#pragma once
/**\class Hyd_Floodplain_Polygon
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_FLOODPLAIN_POLYGON_H
#define HYD_FLOODPLAIN_POLYGON_H

//sys_libs
#include "Geo_Convex_Polygon.h"
#include "Common_Const.h"

//Hyd libs
//for the input per file
#include "_Hyd_Parse_IO.h"
#include "Hyd_Element_Floodplain.h"

///Geometrical class for the polygons in a floodplain model (Hyd_Model_Floodplain), like noflow polygons \ingroup hyd
/**

*/
class Hyd_Floodplain_Polygon : public Geo_Convex_Polygon
{
public:
	///Default constructor
	Hyd_Floodplain_Polygon(void);
	///Default destructor
	~Hyd_Floodplain_Polygon(void);

	//members


	//methods

	///Input the members per file
	void input_members(const int index, const string filename);

	///Create the database table for the polygon data in a database
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the polygon data: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the polygon data
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the polygon data
	static void close_table(void);
	///Select and count the number of relevant polygons in a database table 
	static int select_relevant_polygons_database(QSqlQueryModel *results, QSqlDatabase *ptr_database,const int model_id, const bool with_output);

	///Transfer the polygon data of a file into a database table
	void transfer_polygons2database_table(QSqlDatabase *ptr_database, const int model_id);
	///Set the polygon data from a database selection 
	void input_members(const QSqlQueryModel *results, QSqlDatabase *ptr_database, const int index);
	
	///Output the members
	void output_members(void);
	//Output the polygon to tecplot
	void output2tecplot(ofstream *output_file);

	///Set the element type (_hyd_elem_type) of the elements which are inside
	void set_elementtype_inside(_hyd_elem_type elementtype);
	///Get the element type (_hyd_elem_type) of the elements which are inside
	_hyd_elem_type get_elementtype_inside(void);

	///Clone the floodplain-polygon
	void clone_polygon(Hyd_Floodplain_Polygon *poly);

private:

	//members

	///Pointer to the table for the polygon data in a database
	static Tables *polygon_table;
	///Pointer to the table for the polygon point data in a database
	static Tables *polygon_point_table;

	///Index of the floodplain polygon
	int index;
	///Type of the elements (_hyd_elem_type) which are in the polygon
	_hyd_elem_type elementtype_inside;

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
	static int select_relevant_points_database(QSqlTableModel *results , const int global_polygon_id);


	///Set error(s)
	Error set_error(const int err_type);

	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif
