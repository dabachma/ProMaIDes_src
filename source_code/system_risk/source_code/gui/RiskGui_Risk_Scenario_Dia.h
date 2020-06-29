#pragma once
/**\class RiskGui_Risk_Scenario_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef RISKGUI_RISK_SCENARIO_DIA_H
#define RISKGUI_RISK_SCENARIO_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>
//forms
#include "ui_RiskGui_Risk_Scenario_Dia.h"

//system sys
#include "Sys_Diverse_Text_Dia.h"

//fpl-system
#include "Fpl_Section.h"

//risk
#include "Risk_Break_Scenario.h"

///Dialog-class for combining a fpl-section with one hydraulic boundary scenario to a risk break scenario \ingroup risk
/**
This dialog is used for the scenario risk approach, where the probability resulting of a
monte-carlo analysis of one fpl-section is combined with one hydraulic boundary scenario. The breaching is
located in the selected fpl-section.

\see Risk_System, _risk_type

*/
class RiskGui_Risk_Scenario_Dia : public QDialog, public Ui::RiskGui_Risk_Scenario_Dia
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT 

public:

	///Default constructor
	RiskGui_Risk_Scenario_Dia(QWidget *parent = NULL);
	///Default destructor
	~RiskGui_Risk_Scenario_Dia(void);


	//members
	///Set the dialog, the list widgets etc.
	void set_dialog(QSqlDatabase *ptr_database, _sys_system_id sys_id, const _risk_type type);
	///Start the dialog; it returns true by acception, false by rejection
	bool start_dialog(void);

	///Get the data of the break scenario, which was choosen
	_fpl_break_info get_break_scenario_data(void);
	///Get the data of the hydraulic boundary scenario, which was choosen
	Hyd_Boundary_Szenario* get_hyd_boundary_scenario_data(void);


private slots:
	///Check if the selction are correctly set
	bool check_selections_correct(void);

private:

	///Selected break scenario data
	_fpl_break_info break_scenario;
	///Manager for the hydarulic boundary scenarios
	Hyd_Boundary_Szenario_Management hyd_bound_sc;

	///Risk type for which the dialog is launched
	_risk_type type;

	///Identifier of the hydraulic boundary scenario
	int id_hyd_bound_sc;

	//methods

	///Set error(s)
	Error set_error(const int err_type);

};
#endif
