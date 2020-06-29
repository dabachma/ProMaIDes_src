#pragma once
/**\class RiskGui_Result_Scenario_Widd
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef RISKGUI_RESULT_SCENARIO_WID_H
#define RISKGUI_RESULT_SCENARIO_WID_H


//Qt-libs
#include <QtGui>

//system sys
#include "_Sys_Result_Summary_Wid.h"
//system risk
#include "Risk_System.h"
#include "Risk_Catchment_Break_Scenario_List.h"


///Widget-class for displaying the generate scenarios of the catchment risk approach in a table widget \ingroup risk
/**

*/
class RiskGui_Result_Scenario_Wid : public _Sys_Result_Summary_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT
public:
	///Default constructor
	RiskGui_Result_Scenario_Wid(QWidget *parent = 0);
	///Default destructor
	~RiskGui_Result_Scenario_Wid(void);

	///Set-up the widget
	void set_up_widget(QSqlDatabase *ptr_database, const _sys_system_id id);

};
#endif
