#pragma once
/**\class DamGui_Ecn_Func_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef SYS_BASE_EDIT_DIA_H
#define SYS_BASE_EDIT_DIA_H

//forms
#include "ui_Sys_Base_Edit_Dia.h"

//system sys
#include "_Sys_Abstract_Base_Wid.h"
#include "Common_Const.h"
#include "Sys_Memory_Count.h"

///Basic dialog to show and edit several data formulars
/**

*/
class Sys_Base_Edit_Dia : public QDialog, public Ui::Sys_Base_Edit
{
	Q_OBJECT //Macro for using signals and slots (Qt)in this class

public:

	///Default constructor
	Sys_Base_Edit_Dia(_Sys_Abstract_Base_Wid *child = 0, QWidget *parent = 0);
	///Default destructor
	~Sys_Base_Edit_Dia(void);

	//method
	///Start the dialog
	bool start_dialog(void);

public slots:

	///Show the given Base_Wid as Dialog to make editing data possible
	void add_child(_Sys_Abstract_Base_Wid *wid);
	/*///Close the dialog when the cancel button has been pressed
	void cancel_pressed(void);*/



private:

	//member

	///Custom context menu for the dialog
	QMenu *dial_context_menu;
	///The widget thats called the dialog
	_Sys_Abstract_Base_Wid *child;

	//method

	/////Listen for events that make the apply button enabled
	//void set_applyButton_enable(QEvent *key_event);



//private slots:
//
//	///Close the dialog and assume the changed parameters
//	void ok_pressed(void);
//
//signals:
//
//	///
//	void dialog_applied(_Sys_Abstract_Base_Wid *child);
};

#endif //SYS_BASE_EDIT_DIA_H
