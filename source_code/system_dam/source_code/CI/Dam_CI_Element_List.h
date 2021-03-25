#pragma once
/**\class _Dam_System
	\author produced by AG FRM HS-M
	\version 0.0.1
	\date 2021
*/

#ifndef DAM_CI_ELEMENT_LIST
#define DAM_CI_ELEMENT_LIST



//sys libs
#include "_Sys_Common_System.h"
#include "Sys_Multi_Filechooser_Dia.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Diverse_Text_Dia.h"
#include "Geo_Polysegment.h"


//dam-libs
#include "Dam_CI_Point.h"
#include "Dam_CI_Polygon.h"

///Class for the points as CI elements points which are used in the CI-damage calculation  \ingroup dam
/**
The list connects the _Dam_CI_Elements with each other. In- and outcomming list are available.

*/
class Dam_CI_Element_List : public _Dam_CI_Element
{
public:
	///Default constructor
	Dam_CI_Element_List(void);
	///Default destructor
	~Dam_CI_Element_List(void);

	//member
	///List to store the connected element: id_in, point_flag, id out, point_flag
	QList<QList<int>> elem_list;

	
	///Pointer to the table for the CI connection information in a database
	static Tables *connection_table;

	///Pointer to the table for the CI connection results in a database
	static Tables *connection_erg_table;
	///Pointer to the table for the CI connection instationary results in a database
	static Tables *connection_instat_erg_table;

	//method
	///Input CI connection data with database
	void input_connection_perdatabase(const QSqlQueryModel *results, const int glob_index);

	///Create the database table for the CI connections
	static void create_connection_table(QSqlDatabase *ptr_database);
	///Set the database table for the CI connection: it sets the table name and the name of the columns and allocate them
	static void set_connection_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Delete all data in the database table for the CI connection
	static void delete_data_in_connection_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the CI connection of the given system state
	static void delete_data_in_connection_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Close and delete the database table for the CI connection
	static void close_connection_table(void);
	///Get the maximum value of the global index of the CI connection database table
	static int get_max_glob_id_connection_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the CI connection data to database table
	static string get_insert_header_connection_table(QSqlDatabase *ptr_database);
	///Get the data-string to complete a insert-string for inserting the data of the connection to database
	string get_datastring_members2database(const int global_id, const int index_con, Geo_Point *in, Geo_Point *out, const int sec_id, const string sec_name, const bool transsec_flag);

	///Select and count the number of relevant CI-connections in a database table
	static int select_relevant_connection_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output = true);
	///Select and count the number of relevant CI connection in a database table (just part of it)
	static int select_relevant_connection_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int offset, const int number_rows, const bool with_output = true);
	///Count the number of relevant CI connection in a database table
	static int count_relevant_connection_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output = true);



	///Create the database table for the results of the damage calculation per CI-connection
	static void create_erg_table(QSqlDatabase *ptr_database);
	///Set the database table for the results of the damage calculation per CI-connection: it sets the table name and the name of the columns and allocate them
	static void set_erg_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Delete all data in the database table for the results of the damage calculation per CI-connection
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database);
	///Delete the data in the database table for the results of the damage calculation specified by the system state
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete the data in the database table for the results of the damage calculation specified parameters
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag = false);
	///Delete the data in the database table for the results of the damage calculation specified by the system state and the scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);
	///Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario-ids
	static void delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz);

	///Close and delete the database table for the results of the damage calculation per CI-connection
	static void close_erg_table(void);
	///Get the maximum value of the global index of the result CI-connection database table
	static int get_max_glob_id_connection_erg_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the CI-connection result data to database table
	static string get_insert_header_erg_table(QSqlDatabase *ptr_database);


	///Switch the applied-flag for the CI-connections damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag for the CI-connections damage results in the database table for a defined system state
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);

	///Copy the results of a given system id to another one in database tables
	static void copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);

	///Create the database table for the instationary results of the damage calculation per CI-connection
	static void create_instat_erg_table(QSqlDatabase *ptr_database);
	///Set the database table for the instationary results of the damage calculation per CI-connection: it sets the table name and the name of the columns and allocate them
	static void set_instat_erg_table(QSqlDatabase *ptr_database, const bool not_close = false);
	///Delete all data in the database table for the instationary results of the damage calculation per CI-connection
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database);
	///Delete the data in the database table for the instationary results of the damage calculation specified by the system state
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete the data in the database table for the instationary results of the damage calculation specified parameters
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag = false);
	///Delete the data in the database table for the instationary results of the damage calculation specified by the system state and the scenario-ids
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz);
	///Delete the data in the database table for the instationary results of the damage calculation specified by the hydraulic scenario-ids
	static void delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const int bound_sz);

	///Close and delete the database table for the instationary results of the damage calculation per CI-connection
	static void close_instat_erg_table(void);
	///Get the maximum value of the global index of the instationary result CI-connection database table
	static int get_max_glob_id_connection_instat_erg_table(QSqlDatabase *ptr_database);
	///Get the header for inserting the CI-connection instationary result data to database table
	static string get_insert_header_instat_erg_table(QSqlDatabase *ptr_database);

	///Copy the instationaryresults of a given system id to another one in database tables per CI-connection
	static void copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);






	///Get number of connections
	int get_number_connection(void);
	///Get output the connections
	void output_connection(ostringstream *cout);

	///Check the last set connections
	void check_members(void);

private:

	///Set error(s)
	Error set_error(const int err_type);
};
#endif

