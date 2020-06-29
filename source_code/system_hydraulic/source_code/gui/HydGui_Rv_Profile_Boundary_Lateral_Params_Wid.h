#pragma once
/**\class HydGui_Rv_Profile_Boundary_Lateral_Params_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef HYDGUI_RV_PROFILE_BOUNDARY_LATERAL_PARAMS_WID_H
#define HYDGUI_RV_PROFILE_BOUNDARY_LATERAL_PARAMS_WID_H

//forms
#include "ui_HydGui_Rv_Profile_Boundary_Lateral_Params_Wid.h"

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "_Sys_Abstract_Base_Wid.h"

///Widget to show or edit the lateral boundary conditions [m³/s/m] of a river profile \ingroup hyd
/**

\see _Hyd_River_Profile
*/
class HydGui_Rv_Profile_Boundary_Lateral_Params_Wid : public QWidget, public Ui::boundary_lateral_params
{
	Q_OBJECT //Qt-macro

public:

	///Default construcor
	HydGui_Rv_Profile_Boundary_Lateral_Params_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~HydGui_Rv_Profile_Boundary_Lateral_Params_Wid(void);

	//methods
	
	///Method to set the whole widget editable
	void set_editable(const bool state);
	///Set the scenario id
	void set_sc_id(const int id);
	///Get the scenario id
	int get_sc_id(void);
	///Set the widgets DataRole
	void set_data_role(DataRole role);
	///Set the stationary and the value properties
	void set_values(const bool stationary, const double value);
	///Set default values
	void set_default_values(void);

public slots:

	///Set condition, configures the GUI 
	void set_condition(const bool state);
	///Set stationary, configures the GUI
	void set_stationary(const bool state);

private:
	
	//member 

	///Scenario id
	int sc_id;
	///The widgets DataRole
	DataRole role;
	///The precision before it has been changed threw the precision dialog
	int previous_precision;
};

#endif //HYDGUI_RV_PROFILE_BOUNDARY_LATERAL_PARAMS_WID_H
