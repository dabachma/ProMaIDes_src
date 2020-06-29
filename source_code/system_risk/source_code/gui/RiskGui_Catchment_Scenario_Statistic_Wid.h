#pragma once
/**\class RiskGui_Catchment_Scenario_Statistic_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef RISKGUI_CATCHMENT_SCENARIO_STATISTIC_WID_H
#define RISKGUI_CATCHMENT_SCENARIO_STATISTIC_WID_H


//Qt-libs
#include <QtGui>

//system sys
#include "_Sys_Result_Summary_Wid.h"

//risk system
#include "Risk_Catchment_Break_Scenario_List.h"


///Widget-class for displaying the statistics of the catchment risk states \ingroup risk
/**
\see Risk_Break_Scenario_Management
*/


class RiskGui_Catchment_Scenario_Statistic_Wid : public _Sys_Result_Summary_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	RiskGui_Catchment_Scenario_Statistic_Wid(QWidget *parent = 0);
	///Default destructor
	~RiskGui_Catchment_Scenario_Statistic_Wid(void);

	//methods
	///Set-up the widget
	void set_up_widget(QSqlDatabase *ptr_database, const _sys_system_id id);
};
#endif
