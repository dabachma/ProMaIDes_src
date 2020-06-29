#pragma once
/**\class RiskGui_Calculate_All_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef RISKGUI_CALCULATE_ALL_DIA_H
#define RISKGUI_CALCULATE_ALL_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>
//forms
#include "ui_RiskGui_Calculate_All_Dia.h"



//risk


///Dialog-class for ask, which calculation-stpes of the risk calculation should be serial performed  \ingroup risk
/**
This dialog is used for launchn multiple calculation steps of the risk calculation without
a break.

\see Risk_System

*/


class RiskGui_Calculate_All_Dia: public QDialog, public Ui::RiskGui_Calculate_All_Dia
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT 


public:

	///Default constructor
	RiskGui_Calculate_All_Dia(QWidget *parent = NULL);
	///Default destructor
	~RiskGui_Calculate_All_Dia(void);

	///Start the dialog; it returns true by acception, false by rejection
	bool start_dialog(void);

	///Set the flags for calculation
	void set_flags(bool *ca_nobreak, bool *ca_list_gen, bool *ca_onebreak, bool *ca_auto); 

private slots:

	///Check the catchment risk nobreak calculation check box changed the status
	void check_ca_nobreak_change(bool flag);
	///Check the catchment risk scenario list generation check box changed the status
	void check_ca_list_generation_change(bool flag);
	///Check the catchment risk one-break calculation check box changed the status
	void check_ca_onebreak_change(bool flag);
	///Check the catchment risk auto calculation check box changed the status
	void check_ca_auto_change(bool flag);

};
#endif
