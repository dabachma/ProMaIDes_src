#pragma once
/**\class FplGui_Faulttree_Dike_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2012
*/

#ifndef FPLGUI_FAULTTREE_DIKE_WID_H
#define FPLGUI_FAULTTREE_DIKE_WID_H

//forms
#include "ui_FplGui_Faulttree_Dike_Wid.h"

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "_Sys_Abstract_Base_Wid.h"
#include "Sys_Base_Edit_Dia.h"

//system fpl
#include "_Fpl_Section_Type.h"


///Widget for displaying the faulttree of the dike section type \ingroup fpl
/**

*/
class FplGui_Faulttree_Dike_Wid : public _Sys_Abstract_Base_Wid
{

	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT 


public:
	///Default constructor
	FplGui_Faulttree_Dike_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~FplGui_Faulttree_Dike_Wid(void);

	//methods
	///Set the member, which are displayed in the widget
	void set_member(_Sys_Abstract_Base_Wid *ptr) ;
	///Changes the editable state for all child widgets in the formular/dialog
	void set_editable(const bool state);

	///Set default values
	void set_default_values(void);

	///Set the member of the widget
	void set_member(QSqlDatabase *ptr_database);


public slots:
	///Open the dialog to edit values in the widget
	void show_as_dialog(void);
	///Check checkbox wind wave mechanism
	void check_wind_wave(bool clicked);
	///Check checkbox start erosion waterside mechanism
	void check_ero_waterside_start(bool clicked);
	///Check checkbox heave mechanism
	void check_heave(bool clicked);
	///Check checkbox piping Sellmeijer mechanism
	void check_pip_sell(bool clicked);
	///Check checkbox piping Lane mechanism
	void check_pip_lane(bool clicked);
	///Check checkbox piping Schmertmann mechanism
	void check_pip_schmert(bool clicked);

private:
	//member
	///Qt-form member
	Ui::FplGui_Faulttree_Dike_Wid ui;

	//method
	///Transfer members to database
	void transfer_members2database(FplGui_Faulttree_Dike_Wid *dialog);

	///Set the error(s)
	Error set_error(const int err_type);
};
#endif

