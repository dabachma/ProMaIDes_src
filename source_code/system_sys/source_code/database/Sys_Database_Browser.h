#pragma once
/**\class Sys_Database_Browser
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_DATABASE_BROWSER_H
#define SYS_DATABASE_BROWSER_H
//libs
//classes QT
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QtSql>
#include <QWidget>
//system sys class
#include "Error.h"
#include "Sys_Memory_Count.h"
#include "Data_Base.h"
#include "Common_Const.h"
#include "Geo_Simple_Polygon.h"


///Database class for launching a mysql request to a database via different filter strings; the request is stored in a QSqlQueryModel  \ingroup sys
/**

*/
class Sys_Database_Browser{

public:
	///Default constructor
	Sys_Database_Browser(void);
	///Default destructor
	virtual ~Sys_Database_Browser(void);

	//members
	
	//method
	///Set the database tablename for the request
	void set_tablename(const string tablename);
	///Set a "where"-filter for selecting required data records of a table
	void set_where_filter(const string filter);
	///Set a filter for selecting required columns of a table (select)
	void set_column_filter(const string filter);
	///Set a filter for sorting the selecting (order by)
	void set_sort_filter(const string column_name);
	///Set a filter for joining two tables (left join)
	void set_join_filter(const string filter, const string join_tablename);
	///Allocate and launch the request and get a pointer to the selected results (QSqlQueryModel)
	QSqlQueryModel* table_to_model(QSqlDatabase *ptr_database);
	///Delete the selected rows of the table (use it after launching the request with table_to_model(QSqlDatabase *ptr_database))
	void delete_rows(QSqlDatabase *ptr_database);
	///Get a string for inserting a polygon-datatype to a database table field (update); "where"-filter and tablename have to be set before
	string get_inserting_polygon_filter(const string column_name, _Geo_Polygon *polygon);
	///Get a string for inserting a polyline-datatype to a database table field (update); "where"-filter and tablename have to be set before
	string get_inserting_polyline_filter(const string column_name, Geo_Polysegment *polyline);
	///Get a string for inserting a polyline-datatype to a database table field (update); "where"-filter and tablename have to be set before
	string get_inserting_polyline_filter(const string column_name, Geo_Point *points, const int number);


	
protected:
	//members
	///The query-model for selecting data-records of a database table
	QSqlQueryModel *querymodel;
	//Filter for selecting required columns of a table (select)
	ostringstream column_filter;
	///Filter for selecting required data records of a table (where)
	ostringstream where_filter;
	///Filter for the table name (from)
	ostringstream table_filter;
	///Filter for sorting the selecting (order by) 
	ostringstream sort_filter;
	///Filter for joining two tables (left join) 
	ostringstream join_filter;
	///Name of the database table, for which the request should be launched
	string my_tablename;
	
private:
	//methods
	///Set error(s)
	Error set_error(const int err_type);
};
#endif 