#pragma once
/**\class FplGui_FaultTree_BaseWid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef FPLGUI_FAULTTREE_BASEWID_H
#define FPLGUI_FAULTTREE_BASEWID_H

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "_Sys_Abstract_Base_Wid.h"
#include "Sys_Elastic_GraphicsView.h"
#include "Sys_Elastic_GraphicsPixmapItem.h"
#include "Sys_Elastic_GraphicsJunction.h"

///Widget to show or edit 'Fall Tree' data
/**

*/
class FplGui_FaultTree_BaseWid : public _Sys_Abstract_Base_Wid
{
	Q_OBJECT // Qt-macro

public:
	///Default constructor
	FplGui_FaultTree_BaseWid(QWidget *parent = 0);
	///Default destructor
	~FplGui_FaultTree_BaseWid(void);
	///Get the items taht are used in that fault tree widget
	QList<Sys_Elastic_GraphicsPixmapItem *> get_items() const;
	///Set all items to default values, for example 'inactive'
	virtual void reset() = 0;
	///Set default values
	virtual void set_default_values(void) = 0;
	///Changes the editable state for all child widgets in the formular/dialog
	virtual void set_editable(const bool state) = 0;
	///Set class member
	virtual void set_member(_Sys_Abstract_Base_Wid *ptr) = 0;

public slots:
	///Arrange the fault tree items to a default structure
	virtual void set_item_position_to_defaults(void) = 0;
	///Open the dialog to edit values in the widget
	virtual void show_as_dialog(void) = 0;

protected:
	///
	QList<Sys_Elastic_GraphicsPixmapItem *> items;
	///
	Sys_Elastic_GraphicsView *view;
	///
	Sys_Elastic_GraphicsScene *scene;
	///
	QAction *arrangeAction;

	///Make all hosted items editable.
	virtual void set_items_checkable(const bool checkable) = 0;
	///Setup all hosted items.
	virtual void setup_items(void) = 0;
};

#endif // FPLGUI_FAULTTREE_BASEWID_H
