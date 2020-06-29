#pragma once
/**\class Risk_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef RISKGUI_RESULT_RELIABILITY_DIA_H
#define RISKGUI_RESULT_RELIABILITY_DIA_H

//QT libs

//system sys
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Project.h"
#include "Sys_Diverse_Text_Dia.h"


///Dialog-class for asking if the already stored required results for the risk calculation are reliable \ingroup risk
/**
These results are:
	- results of the hydraulic calculation ,
	- results of the damage calculation,
	- results of the fpl-calculation.

If they are not reliable al results will be newly generated during the risk calculation.

*/
class RiskGui_Result_Reliability_Dia :public Sys_Multi_CheckBox_Dia
{

//Macro for using signals and slots (Qt)in this class
Q_OBJECT 

public:
	///Default constructor
	RiskGui_Result_Reliability_Dia(QWidget *parent = NULL);;
	///Default destructor
	~RiskGui_Result_Reliability_Dia(void);


	//methods
	///Start the dialog
	bool start_dialog(QSqlDatabase *ptr_database, const _sys_system_id id, const bool fpl_important);
	///Get the risk results reliability flags
	_risk_state_flags get_risk_state_flags(void);

private:
	//Generate question text
	string generate_question_txt(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Check if a second warning dialog is required
	bool check_second_dialog_required(QSqlDatabase *ptr_database, const _sys_system_id id);

private slots:
	///Check the settings of the flags
	void check_flag_setting(void);

};
#endif
