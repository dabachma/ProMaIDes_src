#pragma once
/**\class Data_Base
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef DATA_BASE_H
#define DATA_BASE_H

//libs
 
//qt libs
//from the Qt open sorce version;
#include <QtSql>
//to protect the request to the database
#include <QMutex>
//to protect the request to the database
#include <QTimer>

//system sys class
#include "Error.h"
#include "Warning.h"
#include "Sys_Memory_Count.h"
#include "Tables.h"
#include "Common_Const.h"
#include "Sys_Common_Output.h"
#include "Sys_Database_Dia.h"
#include "Sys_Database_File_Dia.h"
#include "Sys_AES_Algo.h"

///Enumerator for different driver-types, which are implemented in QSql \ingroup sys
enum _sys_driver_type{
	///Driver for a connection to a mysql-database
	MYSQL,
	///Driver for a connection to a postgresql-database
	POSTGRESQL,
	///Unknwon driver type
	UNKNOWN_DR,
};

///Data structure to merge the database connection parameters \ingroup sys
struct _sys_database_params{
	///Hostname of the database for the database connection
	string host_name;
	///Database name for the database connection
	string database_name;
	///User name for the database connection
	string user_name;
	///Password for the database connection
	string password;
	///Driver for the connection
	string driver_name;
	///Driver type of the database connection
	_sys_driver_type driver_type;
	///Flag if the password is saved in file
	bool password_saved;
};



///Wrapper-class around the class QSqlDatabase from Qt for the managing of a database connection \ingroup sys
/**
The database connection is managed with this class. Two drivers are implemented:
	- QPostgreSql
	- QMySql (it has no connection to QGis yet)

*/
class Data_Base
{

public:
	///Default constructor
	Data_Base(void);
	///Constructor with a given filename, where the database connection parameters are set
	Data_Base(const string con_filename);
	///Constructor with a given database connection parameters
	Data_Base(const string hostname, const string databasename, const string username, const string pass_word, const string driver);
	///Constructor with a given database connection parameters as structure
	Data_Base(const _sys_database_params param);
	///Default destructor
	~Data_Base(void);

	//members

	//methods

	///Decide, which database action should be done (console dialog)
	void action(void);
	///Decide how the database connection parameters are set (console dialog)
	void set_input(void);

	///Get the driver text of the database
	static _sys_driver_type get_driver_type(void);

	///Output the database connection parameters
	void output_members(void);
	
	///Output the database connection status (true:= database is connected; false:=database is not connected)
	void output_con_status(void);
	///Check and return database connection status (true:= database is connected; false:=database is not connected)
	bool check_con_status(void);

	///Get a pointer to the QSqlDatabase
	QSqlDatabase* get_database(void);
	///Close the database connection
	void close_database(void);
	///Refresh the database connection
	void refresh_connection(void);

	///Set the database connection parameters via console
	void set_input_byhand(void);
	///Set the database connection parameters via dialog
	void set_input_dia_all(void);
	///Set the database connection parameters via a file; the filename/path is asked via console
	void set_input_byfile(void);
	///Set the database connection parameters via a file; the filename/path is asked via dialog
	void set_input_file_dialog(void);
	///Set the database connection parameters via a file; the filename/path is implemented (it is just for development)
	void set_input_development(void);
	///Set the database connection parameters via the given structure
	void set_input_given(_sys_database_params given);
	///Set the database connection parameters via the given structure, additional the table file name is given
	void set_input_given(_sys_database_params given, const string file_name);
	///Set the database connection parameters via a file, which is given
	void set_input_byfile(const string file_name);


	///Set a query to the database via QSqlTableModel (thread safe, due to lockinq with QMutex)
	static void database_request(QSqlTableModel *table_model);
	///Set a query to the database via QSqlTableModel and a qiven query string (thread safe, due to lockinq with QMutex)
	static void database_request(QSqlQuery *query, string querystring, QSqlDatabase *ptr_database);
	///Set a query to the database via QSqlTableModel, a qiven query string and a pointer to the database (thread safe, due to lockinq with QMutex)
	static void database_request(QSqlQueryModel *query_model, string querystring, QSqlDatabase *ptr_database);
	///Check the database connection in case of an error
	static void database_check_connection(QSqlDatabase *ptr_database);
	///Submit data to the database via QSqlTableModel (thread safe, due to lockinq with QMutex)
	static void database_submit(QSqlTableModel *table_model);
	///Get a list of existing tables in the database (thread safe, due to lockinq with QMutex)
	static void database_tables(QStringList *table_list, QSqlDatabase *ptr_database);
	///Get a list of existing columns of a database table in the database (thread safe, due to lockinq with QMutex); problems with newer postgre database version (5.2.2021); columns is always zero
	static void database_table_columns(QSqlRecord *columns, QString table_name, QSqlDatabase *ptr_database);

	///Get a list of existing columns of a database table in the database by an query (thread safe, due to lockinq with QMutex); please use this function instead of database_table_columns(...) 
	static void database_table_columns_query(QSqlRecord *columns, string schema_name,  string table_name, QSqlDatabase *ptr_database);

	///Convert the driver name into an enumerator of the driver types
	static _sys_driver_type convert_txt2drivertype(const string txt);
	

	///Connect and open the database
	void open_database(void);

	///Output the database connection parameter with their keywors to a file
	void output_database_connection2file(QFile *output_file);
	///Output the database connection parameter with their keywors to a file
	void output_database_connection2file(ofstream *output_file);

	///Get the database connection parameter 
	_sys_database_params get_database_conn_param(void);

	///Set the flag, for stopping the reconnection tries
	static void set_reconnection_flag(const bool flag);


private:
	//members

	///Hostname of the database for the database connection
	string host_name;
	///Database name for the database connection
	string database_name;
	///User name for the database connection
	string user_name;
	///Password for the database connection
	string password;
	///Flag if the password is saved in file
	bool password_save_flag;
	///Driver for the connection
	string driver_name;

	///Driver type of the database connection
	static _sys_driver_type driver_type;

	///Locker class for different database actions, which are used from different threads
	static QMutex my_locker;
	
	///Database, which is connected
	QSqlDatabase database;

	///Filename, where the database connection parameter are stored
	string file_name_connection;
	///Flag for the connection status of the database (true:= database is connected; false:=database is not connected)
	bool connection_ok;


	//Flag, for stopping the reconnection tries
	static bool reconnect_flag;

	//methods
	///Set the database connection parameters per file
	void read_input(void);
	
	///Find the following string to an keyword (expression) out of the search_string
	string found_string(string expression, string search_string);

	///Set the prefix for an output
	void set_output_prefix(void);

	///Set error(s)
	Error set_error(const int err_type);
};
#endif