#pragma once
/**\class DamGui_Result_Summary_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef DAMGUI_RESULT_SUMMARY_WID_H
#define DAMGUI_RESULT_SUMMARY_WID_H


//Qt-libs
#include <QtGui>

//system sys
#include "_Sys_Result_Summary_Wid.h"
//system risk
#include "Dam_Damage_System.h"


///Widget-class for displaying the damage results in a table widget as summary \ingroup dam
/**

*/
class DamGui_Result_Summary_Wid : public _Sys_Result_Summary_Wid
{

	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	DamGui_Result_Summary_Wid(QWidget *parent = 0);
	///Default destructor
	~DamGui_Result_Summary_Wid(void);


	//method
	///Set-up the widget
	void set_up_widget(QSqlDatabase *ptr_database, const _sys_system_id id);

	///Set the risk approach identifier
	void set_risk_approach_id(const string id_str);

private:

	//members
	///Risk approach identifier
	string risk_id;

};
#endif
