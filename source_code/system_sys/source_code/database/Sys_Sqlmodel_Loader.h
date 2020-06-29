#pragma once
/**\class Sys_Sqlmodel_Loader
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_SQLMODEL_LOADER_H
#define SYS_SQLMODEL_LOADER_H

//qt libs
#include <QObject>
#include <QSqlQueryModel>

#include <QThread>
#include <string>

using namespace std;

///Thread-class for loading sql table models with an extra thread \ingroup sys
/**

*/
class Sys_Sqlmodel_Loader : public QThread 
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT 

public:
	///Default constructor
	Sys_Sqlmodel_Loader(void);
	///Default destructor
	~Sys_Sqlmodel_Loader(void);

	//members
	///Pointer to the query model
	QSqlQueryModel *ptr_model;

	///Copy of the database
	QSqlDatabase qsqldatabase;

	//methods
	///Run the thread; the pointer to the query-model (ptr_model) has to be set before running the thread
	void run(void);
	///Set a pointer to the database; the copy of the database is made in this method
	void set_ptr2database(QSqlDatabase *qsqldatabase);
	///Set the sql-query string
	void set_query_string(const string query);


private:
	//members
	///Sql-Query string
    string query_string;

	
	//methods
	///Allocate the query model
	void allocate_model(void);
	///Delete the query model
	void delete_model(void);

	

};
#endif
