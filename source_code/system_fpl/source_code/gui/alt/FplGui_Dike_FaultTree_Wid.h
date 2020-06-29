#pragma once
/**\class FplGui_Dike_FaultTree_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPLGUI_DIKE_FAULTTREE_WID_H
#define FPLGUI_DIKE_FAULTTREE_WID_H

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "Sys_Base_Edit_Dia.h"
#include "FplGui_FaultTree_BaseWid.h"

///Widget to show or edit 'Dike FallTree' data
/**
 
*/
class FplGui_Dike_FaultTree_Wid : public FplGui_FaultTree_BaseWid
{
	Q_OBJECT // qt-macro

public:
	///Default constructor
	FplGui_Dike_FaultTree_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~FplGui_Dike_FaultTree_Wid(void);
	///Set the overflow item state.
	void set_overflow_active(const bool active);
	///Get the overflow item state.
	bool is_overflow_active(void) const;
	///Set the wave overtopping item state.
	void set_waveOvertopping_active(const bool active);
	///Get the wave overtopping item state.
	bool is_waveOvertopping_active(void) const;
	///Set the bursting item state.
	void set_bursting_active(const bool active);
	///Get the bursting item state.
	bool is_bursting_active(void) const;
	///Set the piping item state.
	void set_piping_sellmeijer_active(const bool active);
	///Get the piping item state.
	bool is_piping_sellmeijer_active(void);
	///Set the piping item state.
	void set_piping_lane_active(const bool active);
	///Get the piping item state.
	bool is_piping_lane_active(void);
	///Set the piping item state.
	void set_piping_rename_active(const bool active);
	///Get the piping item state.
	bool is_piping_rename_active(void);
	///Set the slope landside item state.
	void set_slope_landside_active(const bool active);
	///Get the slope landside item state.
	bool is_slope_landside_active(void) const;
	///Set the slope waterside item state.
	void set_slope_waterside_active(const bool active);
	///Get the slope waterside item state.
	bool is_slope_waterside_active(void) const;

	//methods
	///
	virtual void reset(void);
	///Method to set the whole widget editable
	virtual void set_editable(const bool editable);
	///Set all embers via database table
	virtual void set_member(QSqlDatabase *ptr_database);
	///Set all members of the widget, similar to a copy constructor
	virtual void set_member(_Sys_Abstract_Base_Wid *ptr);
	///Set default values
	virtual void set_default_values(void);

public slots:
	///Open the dialog to edit values in the widget
	virtual void show_as_dialog(void);
	///Rearrange the position of the items to standard positioning
	virtual void set_item_position_to_defaults(void);

private:
	//member
	///Pointer to the junctions widgets (and or or)
	Sys_Elastic_GraphicsJunction *failureItem;
	Sys_Elastic_GraphicsJunction *forkItemMid;
	Sys_Elastic_GraphicsJunction *forkItemLeft;
	///Pointer to the widgets of the failure mechanisms
	Sys_Elastic_GraphicsPixmapItem *overflowItem;
	Sys_Elastic_GraphicsPixmapItem *waveOvertoppingItem;
	Sys_Elastic_GraphicsJunction *forkItemRight;
	Sys_Elastic_GraphicsJunction *forkItemRightLeft;
	Sys_Elastic_GraphicsPixmapItem *burstingItem;
	Sys_Elastic_GraphicsPixmapItem *pipingItem;
	Sys_Elastic_GraphicsJunction *forkItemRightRight;
	Sys_Elastic_GraphicsPixmapItem *slopeLandsideItem;
	Sys_Elastic_GraphicsPixmapItem *slopeWatersideItem;

	///Pointer to the connection lines
	Sys_Elastic_GraphicsEdge *failureFork1Edge;
	Sys_Elastic_GraphicsEdge *forkMidForkLeftEdge;
	Sys_Elastic_GraphicsEdge *forkLeftOverflowEdge;
	Sys_Elastic_GraphicsEdge *forkLeftWaveOvertoppingEdge;
	Sys_Elastic_GraphicsEdge *forkMidforkRightEdge;
	Sys_Elastic_GraphicsEdge *forkRightForkRightLeftEdge;
	Sys_Elastic_GraphicsEdge *forkRightLeftBurstingEdge;
	Sys_Elastic_GraphicsEdge *forkRightLeftPipingEdge;
	Sys_Elastic_GraphicsEdge *forkRightSlopeLandsideEdge;
	Sys_Elastic_GraphicsEdge *forkRightSlopeSeesideEdge;
	Sys_Elastic_GraphicsEdge *forkRightforkRightRightEdge;

	//methods
	///Transfer members to database
	void transfer_members2database(FplGui_Dike_FaultTree_Wid *dialog);
	///Setup all hosted items.
	virtual void setup_items(void);
	///Make all hosted items editable.
	virtual void set_items_checkable(const bool checkable);
	///Set the error(s)
	Error set_error(const int err_type);
};
#endif // FPLGUI_DIKE_FAULTTREE_WID_H

