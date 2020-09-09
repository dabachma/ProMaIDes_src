#pragma once
/**\class Tables
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef TABLES_H
#define TABLES_H

//libs

//qtclasses
#include <QMutex>
#include <QtSql> 
#include <QTableView>

//system sys class
#include "Error.h"
#include "Warning.h"
#include "Sys_Memory_Count.h"
#include "Sys_Common_Output.h"
#include "Data_Base.h"
#include "Common_Const.h"

//structures
///Structure to combine a id-name of the table/column of table (this name is used in the programm) and a user defined name, which is used in the database \ingroup sys
struct _sys_id_name{
		///Id-name, which is used in the programm, it is defined in the table-file
		string id;
		///User defined name, which is used in the database; it is also given in the table-file
		string name;
		///Flag if the defined user name is found
		bool found_flag;
};
///Structure for creating the columns of a table \ingroup sys
struct _Sys_data_tab_column{
	///Name of the column
	string name;
	///String for the datatype of the column
	string type;
	///Flag if the column gets a key
	bool key_flag;
	///Flag if the coulumn gets a primary key
	bool primary_key_flag;
	///String for the default value
	string default_value;
	///Flag if the column is a unsigned value
	bool unsigned_flag;
};
//enums
///Enumerator for different table type for each module \ingroup sys
enum _sys_table_type{
		///Table type for tables of the \ref fpl 
		fpl,
		///Table type for tables of the \ref hyd
		hyd,
		///Table type for tables of the \ref madm
		madm,
		///Table type for tables of the \ref dam
		dam,
		///Table type for tables of the \ref risk
		risk,
		///Table type for tables of the \ref cost
		cost,
		///Table type for tables of the \ref alt
		alt,
};

///Class for defining a table of a database \ingroup sys
/**
Here the user defined table name/column names are combined with the id-names. These id-names are used in the programm to 
access the database. The combination of the used names and the id-names are specified in the table-file by the user.
It will be also checked, if the defined table and their columns exist in the database.
*/
class Tables
{
public:
	///Default constructor
	Tables(void);
	///Constructor with a given id-name for the table and the id-names for the column; also the number of table columns is given; the table is opened and checked
	Tables(const string id_name, const string id_column[], const int column_number);
	///Default destructor
	~Tables(void);

	//members

	///Combination of the id-name and the used name in the database of the table name
	_sys_id_name table_name;

	//methods

	///Set the file name, where the tables are specified (id-name to used name), per console (table-file)
	static void set_file_name(void);
	///Set the file name, where the tables are specified (id-name to used name), directly (table-file)
	static void set_file_name(const string name_table_file);
	///Get the table-file name; here the table names are specified (id-name to used name)
	static string get_table_file_name(void);

	///Set the id_name for the name of the database table
	void set_id_name_table(const string id_name);
	///Set the id_name for the clomun-names of the database table and the number of columns
	void set_id_name_columns(const string id_column[], const int column_number);

	///Set and combine the used name of the table and the columns with the id-names from the table-file
	void set_name(QSqlDatabase *ptr_database, _sys_table_type actuel_type);

	///Create a table in the database, if it does not exists (return value true)
	bool create_non_existing_tables(const string name, _Sys_data_tab_column columns[], const int column_number, QSqlDatabase *ptr_database, _sys_table_type actuel_type);



	///Get the used name of the table; needed for a database query
	string get_table_name(void);
	///Get the used name of a column with a given id-name; needed for a database query
	string get_column_name(const string id);
	///Get the used name of a column with a given id-name; needed for a database query; table name is added
	string get_column_name_table(const string id);

	///Delete the whole data in the database table
	void delete_data_in_table(QSqlDatabase *ptr_database);

	///Get the maximum value as integer of a given column; used for evaluation the global identifier of data records
	int maximum_int_of_column(const string column_name, QSqlDatabase *ptr_database);
	///Create an index to a given column in table (return value true)
	void create_index2column(QSqlDatabase *ptr_database, const string column_name);
	///Create a spatial index to a given column in table (return value true)
	void create_spatial_index2column(QSqlDatabase *ptr_database, const string column_name);
	
	///Output the table name and column names: id-names as well as used names
	void output_tab_col(void);

	///Set the projetc prefix
	static void set_project_prefix(const string project_name);

	///Add columns to a database table
	void add_columns(QSqlDatabase *ptr_database, const string name, const string new_column_name, const string type_column, const bool unsigened, const string default_value, _sys_table_type actuel_type);

	///Add columns to the project file
	void add_columns_file(const string filename, const string name, const string new_column_name);



	///Get the number of columns
	int get_number_col(void);
	///Get the pointer to the columns
	_sys_id_name* get_ptr_col(void);


private:
	//members

	///Pointer to the combination of the id-name and the used name in the database of the column names
	_sys_id_name *column;
	///Number of columns in the table
	int col_num;
	///Name of the table-file
	static string table_file_name;
	///Type of the table; it specifices the belonging of the table to a module
	_sys_table_type tabletype;
	///Project type for the memory counter
	_sys_system_modules modul_type;
	///Project name as prefix
	static string project_prefix;

	///Tables suffix for identifiying PromaIdes-tables
	const string table_suffix;

	///Locker object for a thread safe access to the table
	QMutex my_locker;

	//methods

	///Allocate the column _sys_id_name structure for the columns of the database table
	void allocate_columns(void);
	///Return the user specifed name following the keyword (expression), which is found in the table-file after the id-name
	string found_string(const string expression, const string search_string);
	///Search for a keyword (expression) in the table-file (here the id-names); return true, when the id-name is founded in the table-file
	bool found_expression(const string expression, const string search_string);

	///Check, if the table, specified by the table name, exists in the database 
	bool check_names(QSqlDatabase *ptr_database);
	///Check, if the columns of the table, specified by the column names, exist in the database 
	bool check_column_names(const QSqlRecord column_name);
	///Convert the enumerator _sys_table_type to a string
	string convert_table_type2string(const _sys_table_type type);
	///Get the total table name completly with project-prefix and type-prefix; the name is in the lower case
	string get_total_table_name(void);

	///Set the prefix for the output
	void set_output_prefix(void);
	///Table type to system type
	void table_type2system_type(void);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
	
};
#endif 