#pragma once
/**\class HydGui_Rv_Profile_General_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef HYDGUI_RV_PROFILE_GENERAL_WID_H
#define HYDGUI_RV_PROFILE_GENERAL_WID_H

//forms
#include "ui_HydGui_Rv_Profile_General_Wid.h"

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "HydGui_River_Geometrics_Table.h"
#include "_Sys_Abstract_Base_Wid.h"
#include "Sys_Base_Edit_Dia.h"
#include "Sys_Base_Expand_Wid.h"
#include "HydGui_Rv_Profile_Boundary_Lateral_Params_Wid.h"
#include "HydGui_Rv_Profile_Boundary_Point_Params_Wid.h"
#include "HydGui_Rv_Profile_Break_Params_Wid.h"
#include "HydGui_Rv_Profile_Overflow_Params_Wid.h"
#include "Hyd_River_Profile_Connection_Standard.h"

///Widget for displaying the river profile parameters \ingroup hyd
/**
Here general parameters as well as break-, overflow-, boundary-parameters are displayed. There is also a point-table and
a graphic of the river profile. In the context menu the edit-mode can be activated.
The data for displaying and editing are read and transfered to the current database.

\see _Hyd_River_Profile
*/
class HydGui_Rv_Profile_General_Wid : public _Sys_Abstract_Base_Wid, public Ui::HydGui_Rv_Profile_General
{
	 //Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:

	///Default Constructor
	HydGui_Rv_Profile_General_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default Destructor
	~HydGui_Rv_Profile_General_Wid(void);

	//member

	///Array of pointers to the boundary lateral parameters
	HydGui_Rv_Profile_Boundary_Lateral_Params_Wid **boundary_lateral_wids;
	///Array of pointers to the boundary point parameters
	HydGui_Rv_Profile_Boundary_Point_Params_Wid **boundary_point_wids;
	///Break parameters widget
	HydGui_Rv_Profile_Break_Params_Wid *break_params;
	///Overflow parameters widget
	HydGui_Rv_Profile_Overflow_Params_Wid *overflow_params;

	//methods
	///Method to set the whole widget editable
	void set_editable(const bool state);
	///Set the number of boundary parameters and expander widgets
	void set_number_of_boundary_param_wids(const int number);
	///Get the number of boundary parameters and expander widgets
	int get_number_of_boundary_param_wids(void);
	///Set main properties of the boundary parameters
	void set_boundary_params_properties(const int index, const string title, const int sc_id = -1);
	///Set all members of the widget, similar to a copy constructor
	void set_member(_Sys_Abstract_Base_Wid *ptr);
	///Set default values
	void set_default_values(void);
	///Set the member of the widget per database
	void set_member(QSqlDatabase *ptr_database, const int rv_no, const int glob_prof_no, const _sys_system_id id);

public slots:

	///Open the dialog to edit values in the widget
	void show_as_dialog(void);

	///Open all expander widgets
	void open_expander_widgets(void);
	///Close all expander widgets
	void close_expander_widgets(void);

private:
	//member
	///Qt-form member
	Ui::HydGui_Rv_Profile_General ui;
	///Array of pointers to expander widgets used in the boundary parameters groupbox
	Sys_Base_Expand_Wid **expander_wids;
	///DataRole of this widget
	DataRole role;
	///Number of boundary parameters and expander widgets
	int number_of_boundary_params;

	///Action for closing all expander widgets
	QAction *close_expand_action;
	///Icon for closing all expander widgets
	QIcon close_expand_icon;
	///Action for open all expander widgets
	QAction *open_expand_action;
	///Icon for open all expander widgets
	QIcon open_expand_icon;

	//method
	///Allocate boundary lateral widgets, boundary point widgets and expander wids
	void allocate_boundary_params(void);
	///Delete boundary lateral widgets, boundary point widgets and expander wids
	void delete_boundary_params(void);

	///Set the general members of a river profile to the display widget
	void set_profile_members2widget(_Hyd_River_Profile *prof);
	///Set profile boundary condition members to the widget
	void set_profile_boundary2widget(Hyd_River_Profile_Connection_Standard *prof, const _sys_system_id id, const int glob_prof_id);

	///Transfer members to database
	void transfer_members2database(HydGui_Rv_Profile_General_Wid *dialog);
	///Set the error(s)
	Error set_error(const int err_type);
};

#endif //HYDGUI_RV_PROFILE_GENERAL_WID_H
