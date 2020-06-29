#pragma once
/**\class _Sys_Data_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _SYS_DATA_WID_H
#define _SYS_DATA_WID_H


//Qt-libs
#include <QtGui>
#include <QMenu>
#include <QObject>
#include <QScrollArea>
#include <QTreeWidgetItem>

//system sys
#include "Sys_Export_Dia.h"





///Base-widget class for displaying data as widgets \ingroup sys
/**

*/
class _Sys_Data_Wid : public QWidget
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	_Sys_Data_Wid(QWidget *parent = 0);
	///Default destructor
	virtual ~_Sys_Data_Wid(void);

	///Set the identifier string to restore the widgets
	void set_identifier(const string id, QTreeWidgetItem *item);
	///Set the QTreeWidgetItem pointer
	void set_treeItem(QTreeWidgetItem *item);
	///Get the pointer of the corresponding tree widget item
	QTreeWidgetItem * get_ptr_treewidget_item(void);


	///Get the get last string after "/" as visible identifier
	static string get_last_string_string_id(const string buffer);

protected:

	//members
	///Action for printing the widget
	QAction *act_print;
	///Action to save the widget as graphic
	QAction *act_save2graphic;
	///Context menu
	QMenu context_menu;
	///Pointer to the scroll-area content of the child-widgets
	QWidget *area2print;

	///Pointer to the scroll-area
	QScrollArea *scroll_area;

	///Pointer to the corresponding item of the tree widget
	QTreeWidgetItem *id_item;





protected slots:

	///Print the document
	void print(void);
	///Save as graphic
	void save_as_graphic(void);

	///Handle a context menu request (Right Mouse Click)
	void context_menu_request(QPoint point);


};
#endif
