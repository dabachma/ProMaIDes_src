#pragma once
/**\class DamGui_Eco_Btype_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef DAMGUI_ECO_BTYPE_WID_H
#define DAMGUI_ECO_BTYPE_WID_H

//forms
#include "ui_DamGui_Eco_Btype_Wid.h"

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "_Sys_Abstract_Base_Wid.h"
#include "Sys_Base_Edit_Dia.h"

//necessary classes
#include "DamGui_Eco_Btype_Func_Wid.h"
#include"Dam_Eco_Btype.h"

///Widget to show or edit the ecologic biotope type data \ingroup dam
/**
\see Dam_Eco_Btype
*/
class DamGui_Eco_Btype_Wid : public _Sys_Abstract_Base_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT 

public:

	///Default constructor
	DamGui_Eco_Btype_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~DamGui_Eco_Btype_Wid(void);

	//member
	///Impact widgets
	DamGui_Eco_Btype_Func_Wid **impact_widgets;
	//methods
	///Method to set the whole widget editable
	void set_editable(const bool state);
	///Set the number of impact widgets
	void set_number_of_impact_widgets(int number);
	///Common getter the number of impactWidgets
	int get_number_of_impact_widgets(void);
	///Set impact widget properties
	void set_impact_widget_properties(int index, int impact_type_index, double weight, bool stepwise_flag);
	///Set all members of the widget, similar to a copy constructor
	void set_member(_Sys_Abstract_Base_Wid *ptr);
	///Set default values
	void set_default_values(void);

	///Set the member of the widget per database
	void set_member(QSqlDatabase *ptr_database, const int btype_no);

public slots:
	
	///Open the dialog to edit values in the widget
	void show_as_dialog(void);

private:

	//member
	///Qt-form member
	Ui::DamGui_Eco_Btype ui;
	///Count the number of added impact widgets
	int number_of_impact_widgets;
	///DataRole
	DataRole role;

	//methods

	///Allocate impact widgets
	void allocate_impact_widgets(void);
	///Delete impact widgets
	void delete_impact_widgets(void);

	///Set the damage function(s) to the plot(s) of the widget
	void set_function2plot(Dam_Eco_Btype *btype);
};

#endif //DAMGUI_ECO_BTYPE_WID_H
