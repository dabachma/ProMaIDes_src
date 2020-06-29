#pragma once
/**\class Sys_Sql_Table_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_SQL_TABLE_WID_H
#define SYS_SQL_TABLE_WID_H
//std-libs
#include <fstream>

//qt-libs
#include <QTableView>
#include <QMenu>
#include <QContextMenuEvent>
#include <QString>
#include <QStringList>
#include <QStandardItemModel>
#include <QModelIndex>

//system libs
#include "Sys_Export_Dia.h"
//system sys
#include "Common_Const.h"
#include <error.h>


///Tableview-class for displaing the database table \ingroup sys
/**
	It is like the QTableView from Qt, there is just a context menu added for an 
	export of the data selected data in the table view to tecplot or ascii.

*/
class Sys_Sql_Table_Wid : public QTableView
{

//Macro for using signals and slots (Qt)in this class	
Q_OBJECT

public:
	///Default constructor
	Sys_Sql_Table_Wid(QWidget *parent = NULL);
	///Default destructor
	~Sys_Sql_Table_Wid(void);

	//methods

	///Set the title of the table, which is displayed
	void set_table_title(const string title);

    ///Set Query model
    void set_ptr_model(QSqlQueryModel *ptr);


private slots:
	///Open the export dialog
	void export_selected_data(void);
	///Handle a context menu request (Right Mouse Click)
	void context_menu_request(QPoint point);

    ///Set filter per column
    void set_column_filer(const QModelIndex & index);

private:
	//member
	///Type of data export
	_sys_export_type export_type;
	///Filename for the data export
	string filename;
	///Action for the export of selected data to a file
	QAction *export_action;
	///Context menu
	QMenu context_menu;
	///Private variable my_dia, which we will use to refer to an instance of Sys_Export_Dia
	Sys_Export_Dia my_dia;
	///Title of the table, which is displayed
	string table_title;


    ///Wrapper model for sorting and filtering
    QSortFilterProxyModel *proxyModel;

    ///QSqlqueryModel
    QSqlQueryModel *ptr_sqlModel;

	//methods
	///Export selected data to file in ascii-format separated with tabs
	void export_selected_data2ascii_tab(void);
	///Export selected data to file in ascii-format separated with comma
	void export_selected_data2ascii_comma(void);
	///Export selected data to file in tecplot-format
	void export_selected_data2tecplot(void);

	///Set error(s)
	Error set_error(const int err_type);

};
#endif
