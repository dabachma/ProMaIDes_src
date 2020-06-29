#pragma once
/**\class RiskGui_Catchment_Scenario_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef RISKGUI_CATCHMENT_SCENARIO_DIA_H
#define RISKGUI_CATCHMENT_SCENARIO_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>

//forms
#include "ui_RiskGui_Catchment_Scenario_Dia.h"


///Dialog-class for selecting catchment risk states for calculation \ingroup risk
/**
*/
class RiskGui_Catchment_Scenario_Dia: public QDialog, public Ui::RiskGui_Catchment_Scenario_Dia
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT 

public:
	///Default constructor
	RiskGui_Catchment_Scenario_Dia(QWidget *parent = 0);
	///Default destructor
	~RiskGui_Catchment_Scenario_Dia(void);

	//methods
	///Set up the dialog
	void set_up_dialog(Risk_Catchment_Break_Scenario_List *list);
	///Start the dialog it returns true by acception, false by rejection
	bool start_dialog(void);

private:
	///List to the available break scenarios for the catchment are risk approach
	Risk_Catchment_Break_Scenario_List *list;
};
#endif
