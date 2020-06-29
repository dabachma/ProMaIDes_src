#pragma once
/**\class DamGui_Eco_Btype_Func_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef DAMGUI_ECO_BTYPE_FUNC_WID_H
#define DAMGUI_ECO_BTYPE_FUNC_WID_H

//forms
#include "ui_DamGui_Eco_Btype_Func_Wid.h"

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "_Sys_Abstract_Base_Wid.h"

#include "Dam_Eco_Btype_Damage_Function.h"

///Widget to show or edit damge function of the ecological biotope type \ingroup dam
/**

\see Dam_Eco_Btype_Damage_Function, DamGui_Eco_Btype_Wid
*/
class DamGui_Eco_Btype_Func_Wid : public QWidget, public Ui::DamGui_Eco_Btype_Func
{
	Q_OBJECT //Qt-macro

public:

	///Default constructor
	DamGui_Eco_Btype_Func_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~DamGui_Eco_Btype_Func_Wid(void);

	//methods

	///Method to set the title of the main groupbox
	void set_groupBox_title(string text);
	///Method to set the whole widget editable
	void set_editable(const bool state);
	///Common getter for private member 'editable'
	bool is_editable(void);
	///Set default values
	void set_default_values(void);
	///Set members to the widget
	void set_member2widget(Dam_Eco_Btype_Damage_Function *func);

private:

	//member

	///Save editable status, true if the widget is editable
	bool editable;
};

#endif //DAMGUI_ECO_BTYPE_FUNC_WID_H
