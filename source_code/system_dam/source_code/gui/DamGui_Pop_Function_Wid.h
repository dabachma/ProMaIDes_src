#pragma once
/**\class DamGui_Pop_Function_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef DAMGUI_POP_FUNCTION_WID
#define DAMGUI_POP_FUNCTION_WID

//forms
#include "ui_DamGui_Pop_Function_Wid.h"

//system sys
#include "Common_Const.h"
#include "_Sys_Abstract_Base_Wid.h"
#include "Sys_Memory_Count.h"
#include "Sys_Base_Edit_Dia.h"

///Widget to show or edit the people2risk damage category \ingroup dam
/**

\see Dam_People_Damage_Function
*/
class DamGui_Pop_Function_Wid : public _Sys_Abstract_Base_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT 

public:

	///Default constructor
	DamGui_Pop_Function_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~DamGui_Pop_Function_Wid(void);

	//methods
	///Method to set the whole widget editable
	void set_editable(const bool state);
	///Set all members of the widget, similar to a copy constructor
	void set_member(_Sys_Abstract_Base_Wid *ptr);
	///Set default values
	void set_default_values(void);
	///Set the title of the groupbox_1
	void set_groupBox_1_title(const string title);

	///Set the member of the widget per database
	void set_member(QSqlDatabase *ptr_database, const int cat_no);

public slots:
	
	///Open the dialog to edit values in the widget
	void show_as_dialog(void);

private:
	///
	Ui::DamGui_Pop_Function ui;
};

#endif //DAMGUI_POP_FUNCTION_WID
