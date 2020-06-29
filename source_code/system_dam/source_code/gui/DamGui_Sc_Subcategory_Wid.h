#pragma once
/**\class DamGui_Sc_Subcategory_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2012 
*/
#ifndef DAMGUI_SC_SUBCATEGORY_WID_H
#define DAMGUI_SC_SUBCATEGORY_WID_H


//Qt-libs
#include <QtGui>


//forms


//system sys
#include "_Sys_Data_Wid.h"
#include "_Sys_Result_Summary_Wid.h"
//system risk
#include "Dam_Damage_System.h"




///Widget-class for displaying the simple counting subcategories \ingroup dam
/**

*/

class DamGui_Sc_Subcategory_Wid  : public _Sys_Result_Summary_Wid
{
public:
	///Default constructor
	DamGui_Sc_Subcategory_Wid(QWidget *parent = 0);
	///Default destructor
	~DamGui_Sc_Subcategory_Wid(void);

	///Set-up the widget
	void set_up_widget(QSqlDatabase *ptr_database,  const _sys_system_id id);
};
#endif

