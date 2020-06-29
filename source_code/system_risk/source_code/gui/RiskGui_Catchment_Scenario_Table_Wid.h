#pragma once
/**\class RiskGui_Catchment_Scenario_Table_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef RISKGUI_CATCHMENT_SCENARIO_TABLE_WID_H
#define RISKGUI_CATCHMENT_SCENARIO_TABLE_WID_H


//Qt-libs
#include <QtGui>

//forms
#include "ui_RiskGui_Catchment_Scenario_Table_Wid.h"



//risk system
#include "Risk_Catchment_Break_Scenario_List.h"


///Widget-class for displaying catchment risk states \ingroup risk
/**
\see Risk_Break_Scenario_Management
*/
class RiskGui_Catchment_Scenario_Table_Wid : public QWidget, public Ui::RiskGui_Catchment_Scenario_Table_Wid
{

	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	RiskGui_Catchment_Scenario_Table_Wid(QWidget *parent = 0);
	///Default destructor
	~RiskGui_Catchment_Scenario_Table_Wid(void);

	//methods
	///Set-up the widget
	void set_up_widget(Risk_Catchment_Break_Scenario_List *list);


	///Get a pointer to the table widget
	QTableWidget* get_ptr_table_widget(void);

private slots:

	///Sort after probability
	void sort_prob(bool flag);
	///Sort after probability with the probability of the hydraulic boundary scenario (with HYD)
	void sort_prob_with_hyd(bool flag);
	///Sort after the predicted risk percentage
	void sort_predicted_risk(bool flag);

	///Combo box text has changed
	void combo_box_changed(int buffer);

};
#endif
