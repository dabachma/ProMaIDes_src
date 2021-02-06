#pragma once
/**\class Sys_Database_Project_Tree_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_DATABASE_PROJECT_TREE_WID_H
#define SYS_DATABASE_PROJECT_TREE_WID_H

//qt-libs
#include <QWidget>
#include  <QTreeWidget>
#include  <QTreeWidgetItem>
#include  <QSqlDatabase>
#include  <QMenu>
#include <string>

using namespace std;

///Widget class as tree-widget for displaying the existing project tables in the database \ingroup sys
/**

*/
class Sys_Database_Project_Tree_Wid : public QWidget
{
//Macro for using signals and slots (Qt)in this class
	 Q_OBJECT

public:
	///Default constructor
	Sys_Database_Project_Tree_Wid(QWidget *parent = 0);
	///Default destructor
	~Sys_Database_Project_Tree_Wid(void);

	//methods
	///Clear the tree-view
	void clear_project_tree(void);

signals:
	///Send a table is activated
    void tableActivated(const QString &table);
	///Send a table is clicked
    void tableClicked(const QString &table);
	///Send a metadata request
    void metaDataRequested(const QString &tableName);
	///Send a text to the status bar
	void send_txt2statusbar(QString txt, int time);

public slots:
	///Refresh the view
    void refresh(void);
	///Show the metadata to the view
	void show_metadata(void);
	///Recieve, that a tree item is activated
    void on_tree_itemActivated(QTreeWidgetItem *item, int column);
	///Recieve, that a tree item is clicked
    void on_tree_itemClicked(QTreeWidgetItem *item, int column);
	///Recieve, that activated tree item is changed to another one
    void on_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
	//members
	///Previous actviated/clicked item
	QTreeWidgetItem *previous_activated;

    //methods
	///Set a item as the active/non-active item of the tree
	void setActive(QTreeWidgetItem *item);
	///Set the active item with bold font or reset it to normal font
	void qSetBold(QTreeWidgetItem *item, bool bold);

	///Create folders for the modules
	void create_folder_modules(QTreeWidgetItem *root_item);
	///Create tables in folders
	void create_tables_folder(QStringList *table_list);

	///Sort tables to folder
	void sort_tables2folder(string table_name);

	///Get complete table name for the sql-query
	string get_table_name_sql(QTreeWidgetItem *item);
	///Get complete table name for the sql-query without schema name
	string get_table_name_no_schema(QTreeWidgetItem *item);

	//members
	///Tree-widget to displaying the tables and the module folders
    QTreeWidget *tree;
	///Action for showing the meta data of a database table; it is used in a context menu
    QAction *metaDataAction;
	///Name of the active database
    QString activeDb;


};
#endif
