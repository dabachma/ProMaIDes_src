#pragma once
/**\class DamGui_Pys_Categories_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef DAMGUI_PYS_CATEGORIES_WID
#define DAMGUI_PYS_CATEGORIES_WID

//forms
#include "ui_DamGui_Pys_Categories_Wid.h"

//system sys
#include "Common_Const.h"
#include "_Sys_Abstract_Base_Wid.h"
#include "Sys_Memory_Count.h"
#include "Sys_Base_Edit_Dia.h"

///Widget to show or edit the psychosocial category \ingroup dam
/**

\see Dam_Pys_Category
*/
class DamGui_Pys_Categories_Wid : public _Sys_Abstract_Base_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT 

public:

	///Default constructor
	DamGui_Pys_Categories_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~DamGui_Pys_Categories_Wid(void);

	//methods
	///Method to set the whole widget editable
	void set_editable(const bool state);
	///Set all members of the widget, similar to a copy constructor
	void set_member(_Sys_Abstract_Base_Wid *ptr);
	///Set default values
	void set_default_values(void);
	///Set the member of the widget per database
	void set_member(QSqlDatabase *ptr_database, const int cat_no);

public slots:
	
	///Open the dialog to edit values in the widget
	void show_as_dialog(void);

private:
	///Qt-form member
	Ui::DamGui_Pys_Categories ui;
};

#endif //DAMGUI_PYS_CATEGORIES_WID
