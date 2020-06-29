#pragma once
/**\class RiskGui_System_Member_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/

#ifndef RISKGUI_SYSTEM_MEMBER_WID_H
#define RISKGUI_SYSTEM_MEMBER_WID_H

//forms
#include "ui_RiskGui_System_Member_Wid.h"

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "_Sys_Abstract_Base_Wid.h"
#include "Sys_Base_Edit_Dia.h"

//system risk
#include "Risk_System.h"

///Widget for displaying the general parameters for the risk calculation based on the catchment risk approach \ingroup risk
/**

*/
class RiskGui_System_Member_Wid : public _Sys_Abstract_Base_Wid
{
	 //Macro for using signals and slots (Qt)in this class
	Q_OBJECT 

public:

	///Default constructor
	RiskGui_System_Member_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~RiskGui_System_Member_Wid(void);

	//methods
	///Method to set the whole widget editable
	void set_editable(const bool state);
	///Set all members of the widget, similar to a copy constructor
	void set_member(_Sys_Abstract_Base_Wid *ptr);
	///Set the member of the widget
	void set_member(QSqlDatabase *ptr_database);
	///Set default values
	void set_default_values(void);

public slots:
	
	///Open the dialog to edit values in the widget
	void show_as_dialog(void);

private:
	//member
	///Qt-form member
	Ui::RiskGui_System_Member_Wid ui;

	//method
	///Transfer members to database
	void transfer_members2database(RiskGui_System_Member_Wid *dialog);

	///Set the error(s)
	Error set_error(const int err_type);
};

#endif //RISKGUI_SYSTEM_MEMBER_WID_H
