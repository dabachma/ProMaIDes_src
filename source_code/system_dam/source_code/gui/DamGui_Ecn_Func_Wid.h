#pragma once
/**\class DamGui_Ecn_Func_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef DAMGUI_ECN_FUNC_WID_H
#define DAMGUI_ECN_FUNC_WID_H

//forms
#include "ui_DamGui_Ecn_Func_Wid.h"

//system sys
#include "Common_Const.h"
#include "_Sys_Abstract_Base_Wid.h"
#include "Sys_Memory_Count.h"
#include "Sys_Base_Edit_Dia.h"

#include "Dam_Ecn_Damage_Function.h"

///Widget to show or edit economic damage function data \ingroup dam
/**

\see Dam_Ecn_Damage_Function
*/
class DamGui_Ecn_Func_Wid : public _Sys_Abstract_Base_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT 

public:

	///Default constructor
	DamGui_Ecn_Func_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~DamGui_Ecn_Func_Wid(void);

	//methods
	void set_editable(const bool state);
	///Set all members of the widget, similar to a copy constructor
	void set_member(_Sys_Abstract_Base_Wid *ptr);
	///Set default values
	void set_default_values(void);

	///Set the member of the widget per database
	void set_member(QSqlDatabase *ptr_database, const int func_no);


public slots:
	
	///Open the dialog to edit values in the widget
	void show_as_dialog(void);

private:
	//Qt-form member
	Ui::DamGui_Ecn_Func ui;

	//methods
	///Set the damage function points to a plot widget
	void set_function2plot(Dam_Ecn_Damage_Function *func);
	///Set error(s)
	Error set_error(const int err_type);
};

#endif //DAMGUI_ECN_FUNC_WID_H
