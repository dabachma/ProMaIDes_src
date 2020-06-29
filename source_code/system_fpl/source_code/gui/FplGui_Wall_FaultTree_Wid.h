#pragma once
/**\class FplGui_Wall_FaultTree_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPLGUI_WALL_FAULTTREE_WID_H
#define FPLGUI_WALL_FAULTTREE_WID_H
//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "Sys_Base_Edit_Dia.h"
#include "FplGui_FaultTree_BaseWid.h"
///Widget to show or edit 'Wall FallTree' data
/**

*/
class FplGui_Wall_FaultTree_Wid : public FplGui_FaultTree_BaseWid
{
	Q_OBJECT // Qt-macro

public:
	///Default constructor
	FplGui_Wall_FaultTree_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~FplGui_Wall_FaultTree_Wid(void);

	//methods
	///
	virtual void reset();
	///Method to set the whole widget editable
	virtual void set_editable(const bool state);
	///Set all members of the widget, similar to a copy constructor
	virtual void set_member(_Sys_Abstract_Base_Wid *ptr);
	///Set default values
	virtual void set_default_values(void);

public slots:
	///Open the dialog to edit values in the widget
	virtual void show_as_dialog(void);
	///
	virtual void set_item_position_to_defaults();

private:
	///
	virtual void setup_items();
	///
	virtual void set_items_checkable(const bool checkable);
};

#endif // FPLGUI_WALL_FAULTTREE_WID_H

