#pragma once
/**\class DamGui_Eco_Soil_Cost_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef DAMGUI_ECO_SOIL_COST_WID_H
#define DAMGUI_ECO_SOIL_COST_WID_H

//forms
#include "ui_DamGui_Eco_Soil_Cost_Wid.h"

//system sys
#include "Common_Const.h"
#include "_Sys_Abstract_Base_Wid.h"
#include "Sys_Memory_Count.h"
#include "Sys_Base_Edit_Dia.h"

///Widget to show or edit the ecological soil-cost function \ingroup dam
/**

\see Dam_Eco_Soil_Cost_Function
*/
class DamGui_Eco_Soil_Cost_Wid : public _Sys_Abstract_Base_Wid
{

	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT 

public:

	///Default constructor
	DamGui_Eco_Soil_Cost_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~DamGui_Eco_Soil_Cost_Wid(void);

	//methods
	///Method to set the whole widget editable
	void set_editable(const bool state);
	///Set all members of the widget, similar to a copy constructor
	void set_member(_Sys_Abstract_Base_Wid *ptr);
	///Set default values
	void set_default_values(void);

	///Set the member of the widget per database
	void set_member(QSqlDatabase *ptr_database, const int func_no);

public slots:
	
	///Open the dialog to edit values in the widget
	void show_as_dialog(void);

private:
	///Qt-form member
	Ui::DamGui_Eco_Soil_Cost ui;
};

#endif //DAMGUI_ECO_SOIL_COST_WID_H
