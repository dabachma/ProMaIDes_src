#pragma once
/**\class Sys_Database_Project_Tree_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_DATABASE_PROJECTBROWSER_WID_H
#define SYS_DATABASE_PROJECTBROWSER_WID_H

//qt-libs
#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QTextEdit>
#include <QSqlError>

//froms
#include "ui_Sys_Database_Projectbrowser_Wid.h"

//system sys libs
#include "Sys_Database_Project_Tree_Wid.h"
#include "Sys_Sqlmodel_Loader.h"
#include "Sys_Sql_Table_Wid.h"
#include "Sys_Sql_Help_Condition_Wid.h"
#include "Sys_Sql_Help_Row_Wid.h"


///Widget class of a simple sql database browser for browsing the project tables in the database \ingroup sys
/**

*/
class Sys_Database_Projectbrowser_Wid : public QWidget, private Ui::Sys_Database_Projectbrowser_Wid
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	Sys_Database_Projectbrowser_Wid(QWidget *parent = 0);
	///Default destructor
	~Sys_Database_Projectbrowser_Wid(void);

	//members
	///Widget for helping to set-up a sql query; here the where-condition is set
	Sys_Sql_Help_Condition_Wid sql_help_cond_wid;
	///Widget for helping to set-up a sql query; here the rowa are set
	Sys_Sql_Help_Row_Wid sql_help_row_wid;

	//method
	///Set a database connection and refresh the table-project tree view (Sys_Database_Project_Tree_Wid)
	void set_db_connection(QSqlDatabase *ptr_database);

	///Disconnect the browser from the database and clear all views
	void disconnect_db(void);

	///Refresh the project-tree view
	void refresh_project_tree(void);

	///Set the pointer to the table model
	void set_ptr_table_model(Sys_Sql_Table_Wid *ptr_table_view);
	///Set the pointer to the status message line
	void set_ptr_status_msg(QTextEdit *ptr_status_msg);

	///Get the pointer to the table loader thread
	QThread* get_ptr_table_loader_thread(void);

	///Get the pointer to the tree widget
	Sys_Database_Project_Tree_Wid * get_ptr_tree_wid(void);


	////Set a pointer to the system id 
	void set_ptr_sys_system_id(_sys_system_id *id);

public slots:
	///Set and show the sql-query to the database
	void exec(void);
	///Show the database table
	void showTable(const QString &table_str);
	///Show the meta data of the database table
    void showMetaData(const QString &table_str);
	///Recieve, if a table is activated in the table-project tree view (Sys_Database_Project_Tree_Wid)
	void on_projecttreewid_tableActivated(const QString &table);
	///Recieve, if a metadata request is made in the table-project tree view (Sys_Database_Project_Tree_Wid)
	void on_projecttreewid_metaDataRequested(const QString &table);
	///Recieve the submit-command from the submit-button
	void on_submitButton_clicked(void);
	///Recieve the stop-command from the stop-button
	void on_stopButton_clicked(void);
	///Recieve the clear-command from the clear-button
    void on_clearButton_clicked(void);
	///Recieve a satus message and display it to the status text-line
	void display_status_msg(const QString message);
	///Recieve a marked table name and set it to the table-text label
	void set_table_txt_label(const QString &table);

	///Set text to the row display
	void set_text2row_display(QString txt);
	///Set text to the condition display
	void set_text2cond_display(QString txt);

	///Set table row names to helper widgets
	void set_row_name2helper(QString table_name);

signals:
	///Send a status message
    void statusMessage(const QString message);
	///Send a clear-command to the database table view
	void send_db_clear(void);


private:

	//member
	///Pointer to the current database
	QSqlDatabase *ptr_database;

	///Pointer to the table view for displaying the data
	Sys_Sql_Table_Wid *ptr_table_view;
	///Pointer to the status message line to displaying the status of the request
	QTextEdit *ptr_status_msg;
	
	///Pointer to the table loader thread
	Sys_Sqlmodel_Loader *loader_thread;

	///Model for clearing the table view, to free the memory
	QStandardItemModel clearer;

	///Time classes for the real time calculation (start time point)
	time_t start_time;
	///Time classes for the real time calculation (actual time point)
	time_t actual_time;

	///Identifier of the object, namely id-areastate and id-measure number
	_sys_system_id *system_id;

	//Flag if stop-button was clicked
	bool stopbutton_clicked;


	//method
	///Get select statement
	string get_select_statement(void);
	///Allocate the table loader thread
	void allocate_table_loader(void);
	///Delete the table loader thread
	void delete_table_loader(void);

private slots:
	///Recieve the signal from the loader thread, that loading the data is finished
	void loading_is_finished(void);

	///Show the sql-row helper widget
	void show_sql_row_helper(void);
	///Show the sql-condition helper widget
	void show_sql_condition_helper(void);


};
#endif

