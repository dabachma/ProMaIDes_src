#pragma once
/**\class HydGui_Rv_Profile_Overflow_Params_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef HYDGUI_RV_PROFILE_OVERFLOW_PARAMS_WID_H
#define HYDGUI_RV_PROFILE_OVERFLOW_PARAMS_WID_H

//forms
#include "ui_HydGui_Rv_Profile_Overflow_Params_Wid.h"

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "_Sys_Abstract_Base_Wid.h"

///Widget to show or edit the overflow parameter of a river profile in the general profile widget \ingroup hyd 
/**
It is part of the general profile widget. The data for displaying and editing 
are read and transfered to the current database.

\see _Hyd_River_Profile, HydGui_Rv_Profile_General_Wid
*/
class HydGui_Rv_Profile_Overflow_Params_Wid : public QWidget, public Ui::overflow_params
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:

	///Default constructor
	HydGui_Rv_Profile_Overflow_Params_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~HydGui_Rv_Profile_Overflow_Params_Wid(void);

	//methods

	///Set the widget editable
	void set_editable(const bool state);
	///Set default values
	void set_default_values(void);

	///Set values of the display widget
	void set_values(const bool r_ov, const double pol_r, const bool l_ov, const double pol_l);

public slots:

	///Updates the widget to activate/deactivate the left overflow parameter
	void set_overflow_coupling_left(const bool state);
	///Updates the widget to activate/deactivate the right overflow parameter
	void set_overflow_coupling_right(const bool state);
};

#endif //HYDGUI_RV_PROFILE_OVERFLOW_PARAMS_WID_H
