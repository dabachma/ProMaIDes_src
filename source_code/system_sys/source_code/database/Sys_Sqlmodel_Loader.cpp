#include "Sys_Headers_Precompiled.h"
//#include "Sys_Sqlmodel_Loader.h"

//Default constructor
Sys_Sqlmodel_Loader::Sys_Sqlmodel_Loader(void){
	this->ptr_model=NULL;
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Sqlmodel_Loader), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Sqlmodel_Loader::~Sys_Sqlmodel_Loader(void){
	this->delete_model();
	if(QSqlDatabase::contains(sys_label::str_sys_data_tables.c_str())==true){
		if(QSqlDatabase::database(sys_label::str_sys_data_tables.c_str(),false).isOpen()==true){
			QSqlDatabase::database(sys_label::str_sys_data_tables.c_str(),false).close();
		}
		QSqlDatabase::removeDatabase(sys_label::str_sys_data_tables.c_str());
	}
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Sqlmodel_Loader), _sys_system_modules::SYS_SYS);
}
//_______________
//public
//Run the thread; the pointer to the query-model (ptr_model) has to be set before running the thread
void Sys_Sqlmodel_Loader::run(void){
/**
Including the database, the table name and the sql-query text has to be set before running the thread.
*/
	this->delete_model();
	this->allocate_model();
	this->ptr_model->setQuery(QSqlQuery(this->query_string.c_str(), this->qsqldatabase));
}
//Set a pointer to the database; the copy of the database is made in this method
void Sys_Sqlmodel_Loader::set_ptr2database(QSqlDatabase *qsqldatabase){
	if(QSqlDatabase::contains(sys_label::str_sys_data_tables.c_str())==false){
		this->qsqldatabase=QSqlDatabase::cloneDatabase(*qsqldatabase, sys_label::str_sys_data_tables.c_str());
		this->qsqldatabase.open();
	}
}
//Set the sql-query string
void Sys_Sqlmodel_Loader::set_query_string(const string query){
	this->query_string=query;
}
//________________
//private
//Allocate the query model
void Sys_Sqlmodel_Loader::allocate_model(void){
	if(this->ptr_model==NULL){
		this->ptr_model=new QSqlQueryModel();
	}
}
//Delete the query model
void Sys_Sqlmodel_Loader::delete_model(void){
	if(this->ptr_model!=NULL){
		delete this->ptr_model;
		this->ptr_model=NULL;
	}
}