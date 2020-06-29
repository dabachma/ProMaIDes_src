#pragma once
/**\class DamGui_Sc_Point_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2012 
*/
#ifndef DAMGUI_SC_POINT_WID_H
#define DAMGUI_SC_POINT_WID_H


//Qt-libs
#include <QtGui>


//forms
#include "ui_DamGui_Sc_Point_Wid.h"

//system sys
#include "_Sys_Data_Wid.h"
//system risk
#include "Dam_Damage_System.h"




///Widget-class for displaying the simple counting damage points \ingroup dam
/**

*/
class DamGui_Sc_Point_Wid: public  _Sys_Data_Wid, public Ui::DamGui_Sc_Point_Wid
{

	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:

	///Default constructor
	DamGui_Sc_Point_Wid(QWidget *parent = 0);
	///Default destructor
	~DamGui_Sc_Point_Wid(void);


	//method
	///Set-up the widget
	void set_up_widget(QSqlDatabase *ptr_database, const _sys_system_id id);
};
#endif
