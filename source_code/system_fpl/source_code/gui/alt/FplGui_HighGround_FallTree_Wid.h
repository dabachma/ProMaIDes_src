#pragma once
/**\class FplGui_HighGround_FallTree_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef FPLGUI_HIGHGROUND_FALLTREE_WID_H
#define FPLGUI_HIGHGROUND_FALLTREE_WID_H

#define DEBUG

//forms
#include "ui_FplGui_HighGround_FallTree_Wid.h"

//Qt
#include <QLinearGradient>
#include <QMouseEvent>

//system sys
#include "Common_Const.h"
#include "_Sys_Abstract_Base_Wid.h"
#include "Memory_Count.h"
#include "Sys_Base_Edit_Dia.h"
#include "Sys_FallTree_BaseWid.h"
#include "FplGui_FaultTree_GraphicsScene.h"

///Widget to show or edit 'HighGround FallTree' data
/**

*/
class FplGui_HighGround_FallTree_Wid : public _Sys_Abstract_Base_Wid
{
	Q_OBJECT //Qt-macro

public:

	///Default constructor
	FplGui_HighGround_FallTree_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~FplGui_HighGround_FallTree_Wid(void);

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

#ifdef DEBUG
	///
	void onMousePositionChanged(QPointF pos);
#endif //DEBUG
	///
	void on_item_state_changed(const bool state);

private:
	///
	Ui::FplGui_HighGround_FallTree ui;
	///
	FplGui_FaultTree_GraphicsScene *scene;
	///
	Sys_FallTree_BaseWid *overflowItem;
};

#endif //FPLGUI_HIGHGROUND_FALLTREE_WID_H
