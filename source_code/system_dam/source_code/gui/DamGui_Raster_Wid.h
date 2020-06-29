#pragma once
/**\class Sys_Member_Bool_BaseWid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef DAMGUI_RASTER_WID_H
#define DAMGUI_RASTER_WID_H

//forms
#include "ui_DamGui_Raster_Wid.h"

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "_Sys_Abstract_Base_Wid.h"
#include "Sys_Base_Edit_Dia.h"

#include "Dam_Raster.h"

///Widget to show or edit data of damage rasters of all types of damage calculation (ECN, ECO, POP, PYS) \ingroup dam
/**

\see Dam_Raster
*/
class DamGui_Raster_Wid : public _Sys_Abstract_Base_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT 

public:

	///Default constructor
	DamGui_Raster_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~DamGui_Raster_Wid(void);

	//methods
	///Method to set the whole widget editable
	void set_editable(const bool state);
	///Set all members of the widget, similar to a copy constructor
	void set_member(_Sys_Abstract_Base_Wid *ptr);
	///Set default values
	void set_default_values(void);
	
	///Set the member of the widget per database
	void set_member(QSqlDatabase *ptr_database, const int raster_no, const _dam_raster_types type);
	


public slots:
	
	///Open the dialog to edit values in the widget
	void show_as_dialog(void);

private:
	///Qt-form widget
	Ui::DamGui_Raster ui;

	//member
	///Set the raster member 
	void set_raster_member(Dam_Raster *raster);
	
};
#endif //DAMGUI_RASTER_WID_H
