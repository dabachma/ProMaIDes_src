#pragma once
/**\class HydGui_Rv_Profile_Break_Params_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef HYDGUI_RV_PROFILE_BREAK_PARAMS_WID_H
#define HYDGUI_RV_PROFILE_BREAK_PARAMS_WID_H

//forms
#include "ui_HydGui_Rv_Profile_Break_Params_Wid.h"

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "_Sys_Abstract_Base_Wid.h"

struct _hyd_break_parameters;

///Widget to show or edit the break parameter of a river profile in the general profile widget \ingroup hyd 
/**
It is part of the general profile widget. The data for displaying and editing 
are read and transfered to the current database.

\see _Hyd_River_Profile, HydGui_Rv_Profile_General_Wid
*/
class HydGui_Rv_Profile_Break_Params_Wid : public QWidget, public Ui::break_params
{
	 //Macro for using signals and slots (Qt)in this class
	Q_OBJECT 

public:

	///Default constructor
	HydGui_Rv_Profile_Break_Params_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~HydGui_Rv_Profile_Break_Params_Wid(void);

	//methods

	///Method to set the whole widget editable
	void set_editable(const bool state);
	///Set default values
	void set_default_values(void);
	///Set the left break parameters enabled
	void set_left_break(const bool state);
	///Set the right break parameters enabled
	void set_right_break(const bool state);
	///Set wall or dike as boundary building
	void set_abrupt_breach_left(const bool state);
	///Set wall or dike as boundary building
	void set_abrupt_breach_right(const bool state);
	///Get state of abrupt breach left
	bool get_abrupt_breach_left(void);
	///Get state of abrupt breach right
	bool get_abrupt_breach_right(void);

	///Set the left break parameters
	void set_left_break_params(const bool left_break, _hyd_break_parameters *params, const int index_base);
	///Set the right break parameters
	void set_right_break_params(const bool right_break, _hyd_break_parameters *params, const int index_base);


private slots:

	///Updates the widget to activate/deactivate the left break parameter
	void break_left_check_clicked(const bool state);
	///Updates the widget to activate/deactivate the right break parameter
	void break_right_check_clicked(const bool state);
	///Updates the widget to activate/deactivate the left break parameter for wall-break (discontinuous)
	void wall_left_radio_clicked(const bool state);
	///Updates the widget to activate/deactivate the left break parameter for dike-break (continuous)
	void dike_left_radio_clicked(const bool state);
	///Updates the widget to activate/deactivate the right break parameter for wall-break (discontinuous)
	void wall_right_radio_clicked(const bool state);
	///Updates the widget to activate/deactivate the right break parameter for dike-break (continuous)
	void dike_right_radio_clicked(const bool state);

private:
	
	//member
	///Flag for the left wall-break parameters
	bool left_wall;
	///Flag for the left dike-break parameters
	bool left_dike;
	///Flag for the right wall-break parameters
	bool right_wall;
	///Flag for the right dike-break parameters
	bool right_dike;
};

#endif //HYDGUI_RV_PROFILE_BREAK_PARAMS_WID_H
