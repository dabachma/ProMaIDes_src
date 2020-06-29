#pragma once
/**\class RiskGui_Detailed_Result_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef RISKGUI_DETAILED_RESULT_WID_H
#define RISKGUI_DETAILED_RESULT_WID_H


//Qt-libs
#include <QtGui>

//forms
#include "ui_RiskGui_Detailed_Result_Wid.h"
//hyd system
#include "Hyd_Boundary_Szenario_Management.h"
//risk system
#include "Risk_Catchment_Break_Scenario_List.h"
//system sys
#include "_Sys_Data_Wid.h"


///Widget-class for displaying the detailed results of the risk calculation \ingroup risk
/**
\see Risk_System
*/

class RiskGui_Detailed_Result_Wid : public _Sys_Data_Wid, public Ui::RiskGui_Detailed_Result_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	RiskGui_Detailed_Result_Wid(QWidget *parent = 0);
	///Default destructor
	~RiskGui_Detailed_Result_Wid(void);

	//methods
	///Set tooltip of the header
	void set_header_tooltip(const string tooltip, QTreeWidgetItem *item);
	///Set-up the widget
	void set_up_widget(QSqlDatabase *ptr_database, const int glob_id, const _sys_system_id id, const _risk_type type);
};
#endif
