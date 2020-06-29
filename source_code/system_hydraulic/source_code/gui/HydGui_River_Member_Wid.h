#pragma once
/**\class HydGui_River_Member_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef HYDGUI_RIVER_MEMBER_WID_H
#define HYDGUI_RIVER_MEMBER_WID_H

//forms
#include "ui_HydGui_River_Member_Wid.h"

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "_Sys_Abstract_Base_Wid.h"
#include "Sys_Base_Edit_Dia.h"
#include "Error.h"

///Widget for displaying and editing the general river-model members \ingroup hyd
/**
The data for displaying and editing are read and transfered to the current database.

\see Hyd_Model_River
*/
class HydGui_River_Member_Wid : public _Sys_Abstract_Base_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT 

public:
	///Default Constructor
	HydGui_River_Member_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default Destructor
	~HydGui_River_Member_Wid(void);

	//methods
	///Method to set the whole widget editable
	void set_editable(const bool state);
	///Set all members of the widget, similar to a copy constructor
	void set_member(_Sys_Abstract_Base_Wid *ptr);
	///Set default values
	void set_default_values(void);
	///Set the member of the widget per database
	void set_member(QSqlDatabase *ptr_database, const int rv_no);

public slots:
	///Open the dialog to edit values in the widget
	void show_as_dialog(void);

private:
	///Qt-form member
	Ui::HydGui_River_Member ui;

	//method
	///Transfer members to database
	void transfer_members2database(HydGui_River_Member_Wid *dialog);
	///Set the error(s)
	Error set_error(const int err_type);
};

#endif // HYDGUI_RIVER_MEMBER_WID_H
