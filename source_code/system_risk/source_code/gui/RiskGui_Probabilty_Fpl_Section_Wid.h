#pragma once
/**\class RiskGui_Detailed_Risk_Summary_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/
#ifndef RISKGUI_PROBABILTY_FPL_SECTION_WID_H
#define RISKGUI_PROBABILTY_FPL_SECTION_WID_H


//Qt-libs
#include <QtGui>

//system sys
#include "_Sys_Result_Summary_Wid.h"
//system risk
#include "Risk_System.h"


///Widget-class for displaying the probability of each FPL-section in a table widget as a summary \ingroup risk
/**
Here the probability of each FPL-section, that means the sum of the break
scenarios, where the section is involved, for each bouandary scenario is displayed 
in a table as summary.

*/
class RiskGui_Probabilty_Fpl_Section_Wid : public _Sys_Result_Summary_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	RiskGui_Probabilty_Fpl_Section_Wid(QWidget *parent = 0);
	///Default destructor
	~RiskGui_Probabilty_Fpl_Section_Wid(void);

	//method
	///Set-up the widget
	void set_up_widget(QSqlDatabase *ptr_database, const _sys_system_id id);

private:

	//method
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
