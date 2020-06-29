#pragma once
/**\class FplGui_HighGround_FaultTree_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef FPLGUI_HIGHGROUND_FAULTTREE_WID_H
#define FPLGUI_HIGHGROUND_FAULTTREE_WID_H

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "Sys_Base_Edit_Dia.h"
#include "FplGui_FaultTree_BaseWid.h"

///Widget to show or edit 'HighGround FallTree' data
/**

*/
class FplGui_HighGround_FaultTree_Wid : public FplGui_FaultTree_BaseWid
{
	Q_OBJECT //Qt-macro

public:
	///Default constructor
	FplGui_HighGround_FaultTree_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~FplGui_HighGround_FaultTree_Wid(void);

	//methods
	///
	void set_overflow_active(const bool active);
	///
	bool is_overflow_active() const;
	///
	virtual void reset();
	///Method to set the whole widget editable
	virtual void set_editable(const bool state);
	///Set all members of the widget, similar to a copy constructor
	virtual void set_member(_Sys_Abstract_Base_Wid *ptr);
	///Set default values
	virtual void set_default_values(void);

public slots:
	//methods
	///Open the dialog to edit values in the widget
	virtual void show_as_dialog(void);
	///
	virtual void set_item_position_to_defaults();

private:
	//members
	///
	Sys_Elastic_GraphicsJunction *failureItem;
	///
	Sys_Elastic_GraphicsPixmapItem *overflowItem;
	///
	Sys_Elastic_GraphicsEdge *failureOverflowEdge;
	//methods
	///
	virtual void setup_items();
	///
	virtual void set_items_checkable(const bool checkable);
};

#endif // FPLGUI_HIGHGROUND_FAULTTREE_WID_H
