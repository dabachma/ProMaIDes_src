#pragma once
/**\class Sys_Sql_Help_Condition_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/
#ifndef SYS_SQL_HELP_CONDITION_WID_H
#define SYS_SQL_HELP_CONDITION_WID_H

//Qt-libs
#include <QtGui>
#include <QWidget>
//forms
#include "ui_Sys_Sql_Help_Condition_Wid.h"


///Widget-class for the assistance in the creation of an sql-query. Here the where-condition is build up. \ingroup sys
/**
*/
class Sys_Sql_Help_Condition_Wid : public QWidget, public Ui::Sys_Sql_Help_Condition_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:


	///Default constructor
	Sys_Sql_Help_Condition_Wid(QWidget *parent = NULL);
	///Default destructor
	~Sys_Sql_Help_Condition_Wid(void);

	//method
	///Set the list widget with the column names
	void set_list_column_name(QStringList *list);

public slots:
	///Set a text to the display, which is send by the Sys_Letter_Button
	void set_text2display(QString txt);
	///Set a text to the display, which is send by the list widget
	void set_itemtext2display(QListWidgetItem *item);

	///Clear the display
	void clear_display(void);
	///Close the window
	void close_window(void);

	///A slot to send the display text
	void slot2send_txt(void);

signals:
	///Send the display text
	void send_display_txt(const QString button_txt);


};
#endif
