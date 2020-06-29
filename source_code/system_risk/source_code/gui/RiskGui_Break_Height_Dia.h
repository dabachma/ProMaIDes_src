#pragma once
/**\class RiskGui_Break_Height_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef RISKGUI_BREAK_HEIGHT_DIA_H
#define RISKGUI_BREAK_HEIGHT_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>
//forms
#include "ui_RiskGui_Break_Height_Dia.h"

//system sys
#include "_Sys_Common_System.h"
#include "Common_Const.h"

///Dialog-class for asking the required break height of an fpl-section \ingroup risk
/**
This dialog is used for the scenario risk approach. Here the user can define, at which break height the hydraulic 
sections, belonging to the selected fpl-section, will start to break.

\see Risk_System, _risk_type

*/
class RiskGui_Break_Height_Dia : public QDialog, public Ui::RiskGui_Break_Height_Dia
{

//Macro for using signals and slots (Qt)in this class
Q_OBJECT 

public:
	///Default constructor
	RiskGui_Break_Height_Dia(QWidget *parent = NULL);
	///Default destructor
	~RiskGui_Break_Height_Dia(void);


	//methods
	///Set the dialog for a given fpl-section; data are read from database
	void set_section_id(QSqlDatabase *ptr_database, const int sec_id, const _sys_system_id id);
	///Start the dialog; it returns true by acception, false by rejection
	bool start_dialog(void);

	///Get the break height
	double get_break_height(void);

private:

	//methods
	///Set the dialog section label
	void set_dialog_section_label(const string txt);
	///Set the structure height
	void set_structure_height(const double height);
	///set the break height
	void set_break_height(const double break_height);

	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif
