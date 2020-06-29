#pragma once
/**\class HydGui_Coupling_Structure_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef HYDGUI_COUPLING_STRUCTURE_WID
#define HYDGUI_COUPLING_STRUCTURE_WID

//forms
#include "ui_HydGui_Coupling_Structure_Wid.h"

//system sys
#include "Common_Const.h"
#include "_Sys_Abstract_Base_Wid.h"
#include "Sys_Memory_Count.h"
#include "Sys_Base_Edit_Dia.h"

///Widget to show or edit '' data
/**

*/
class HydGui_Coupling_Structure_Wid : public _Sys_Abstract_Base_Wid
{
	Q_OBJECT //Qt-macro

public:
	///Default constructor
	HydGui_Coupling_Structure_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~HydGui_Coupling_Structure_Wid(void);

	//methods
	///Method to set the whole widget editable
	void set_editable(const bool state);
	///Set all members of the widget, similar to a copy constructor
	void set_member(_Sys_Abstract_Base_Wid *ptr);
	///Set default values
	void set_default_values(void);

public slots:
	///Open the dialog to edit values in the widget
	void show_as_dialog(void);

private:
	//member
	///
	Ui::HydGui_Coupling_Structure ui;
	///'previous_precision' Saves the precision of the uncontrollable_param widget`s spinbox
	double prev_prec;

private slots:
	
	///Change the label text and tooltip for controllable- and uncontrollable param
	void type_changed(QString item);
	///Save the precision of the uncontrollable_param widget`s spinbox
	void precision_changed(const int prec);
};

#endif //HYDGUI_COUPLING_STRUCTURE_WID
