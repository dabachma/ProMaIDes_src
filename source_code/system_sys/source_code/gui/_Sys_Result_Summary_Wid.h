#pragma once
/**\class _Sys_Result_Summary_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _SYS_RESULT_SUMMARY_WID_H
#define _SYS_RESULT_SUMMARY_WID_H


//Qt-libs
#include <QtGui>

//system sys
#include "Sys_Export_Dia.h"
#include "_Sys_Data_Wid.h"
#include <error.h>

//forms
#include "ui_Sys_Result_Summary_Wid.h"




///Base-widget class for displaying summaries of the results in a table widegt \ingroup sys
/**

*/
class _Sys_Result_Summary_Wid : public _Sys_Data_Wid, public Ui::Sys_Result_Summary_Wid
{

	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	_Sys_Result_Summary_Wid(QWidget *parent = 0);
	///Default destructor
	virtual ~_Sys_Result_Summary_Wid(void);

	//methods
	///Set tooltip of the header
	void set_header_tooltip(const string tooltip);
	///Set-up the widget
	virtual void set_up_widget(QSqlDatabase *ptr_database, const _sys_system_id id)=0;

protected slots:
	///Sort widget by marked column, ascending order
	void sort_widget_by_column_up(void);
	///Sort widget by marked column, descending order
	void sort_widget_by_column_down(void);

	///Open the export dialog
	void export_selected_data(void);

protected:
	//members
	///Type of data export
	_sys_export_type export_type;
	///Filename for the data export
	string filename;
	///Action for the export of selected data to a file
	QAction *export_action;
	///Action for sorting
	QAction *sort_action_up;
	///Action for sorting
	QAction *sort_action_down;

	///Private variable my_dia, which we will use to refer to an instance of Sys_Export_Dia
	Sys_Export_Dia my_dia;

	///System-id as information for an output
	_sys_system_id sys_id;

private:



	//methods
	///Export selected data to file in ascii-format separated with tabs
	void export_selected_data2ascii_tab(void);
	///Export selected data to file in ascii-format separated with comma
	void export_selected_data2ascii_comma(void);

	///Set error(s)
	Error set_error(const int err_type);




};
#endif
