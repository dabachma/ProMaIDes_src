#pragma once
/**\class DamGui_Result_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef DAMGUI_RESULT_WID_H
#define DAMGUI_RESULT_WID_H


//Qt-libs
#include <QtGui>


//forms
#include "ui_DamGui_Result_Wid.h"

//system sys
#include "_Sys_Data_Wid.h"
//system risk
#include "Dam_Damage_System.h"




///Widget-class for displaying the damage results for each generated scenario \ingroup dam
/**

*/
class DamGui_Result_Wid : public _Sys_Data_Wid, public Ui::DamGui_Result_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT
public:

	///Default constructor
	DamGui_Result_Wid(QWidget *parent = 0);
	///Default destructor
	~DamGui_Result_Wid(void);


	//method
	///Set tooltip of the header
	void set_header_tooltip(const string tooltip, QTreeWidgetItem *item);
	///Set-up the widget
	void set_up_widget(QSqlDatabase *ptr_database, const int glob_id);

private:

};
#endif
