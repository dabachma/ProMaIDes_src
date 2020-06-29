#pragma once
/**\class Sys_Project_Copier
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/
#ifndef SYS_PROJECT_COPIER_H
#define SYS_PROJECT_COPIER_H

//qt libs
#include <QObject>
#include <QSqlQueryModel>
#include <QThread>
#include <Data_Base.h>

///Thread-class for copying projects \ingroup sys
/**

*/
class Sys_Project_Copier: public QThread 
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT 
public:
	///Default constructor
	Sys_Project_Copier(void);
	///Default destructor
	~Sys_Project_Copier(void);

	//method
	///Run the thread; 
	void run(void);

	///Set the members for copying
	void set_copy_member(Data_Base *ptr_database, const string new_project, const string old_project);


private:

	//members
	///Pointer to the database
	Data_Base *ptr_database;
	///New project name
	string new_project_name;
	///Old project name
	string old_project_name;

	//method
	///Copy relevant database tables
	void copy_relevant_database_tables(Data_Base *ptr_database, string new_project_name);
	///Copy one database table
	void copy_one_database_table(const string src, const string dest, QSqlQuery *model, Data_Base *ptr_database);

	///Set error(s)
	Error set_error(const int err_type);
};

#endif
