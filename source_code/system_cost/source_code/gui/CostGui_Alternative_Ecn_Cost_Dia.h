#pragma once
/**\class CostGui_Alternative_Ecn_Cost_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef COSTGUI_ALTERNATIVE_ECN_COST_DIA_H
#define COSTGUI_ALTERNATIVE_ECN_COST_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>

//sys-libs
#include "Common_Const.h"

//forms
#include "ui_CostGui_Alternative_Ecn_Cost_Dia.h"





///Dialog-class for setting economical cost data (user-defined) for a specific alternative \ingroup cost
/**
*/
class CostGui_Alternative_Ecn_Cost_Dia : public QDialog, public Ui::CostGui_Alternative_Ecn_Cost_Dia
{

	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	CostGui_Alternative_Ecn_Cost_Dia(QWidget *parent = 0);
	///Default destructor
	~CostGui_Alternative_Ecn_Cost_Dia(void);

	//member
	///Cost data, which is set by the dialog
	_cost_ecn_cost_data cost_data;

	//methods
	///Set the costs to the dialog and the alternative data to the dialog
	void set_dialog(const string alt_name, const string alt_descript, const _cost_ecn_cost_data cost_data);

	///Start the dialog it returns true by acception, false by rejection
	bool start_dialog(void);

	///Get the cost data, which are set
	_cost_ecn_cost_data get_cost_data(void);

private slots:

	///Check planning cost settings
	void check_planning_cost(bool flag);

};
#endif
