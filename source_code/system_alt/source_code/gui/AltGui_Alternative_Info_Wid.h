#pragma once
/**\class AltGui_Alternative_Info_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef ALTGUI_ALTERNATIVE_INFO_WID_H
#define ALTGUI_ALTERNATIVE_INFO_WID_H

//Qt-libs
#include <QtGui>
#include <QWidget>
//forms
#include "ui_AltGui_Alternative_Info_Wid.h"

//system sys
#include "_Sys_Data_Wid.h"
#include "_Sys_Abstract_Base_Wid.h"
#include "Sys_Base_Edit_Dia.h"
#include "Error.h"


///Widget-class for displaying the specific measures information  \ingroup alt
/**

*/
class AltGui_Alternative_Info_Wid : public _Sys_Abstract_Base_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	AltGui_Alternative_Info_Wid(DataRole role = DisplayRole, QWidget *parent = 0);
	///Default destructor
	~AltGui_Alternative_Info_Wid(void);


	//method
	///Set the members via a widget of the same type
	void set_member(_Sys_Abstract_Base_Wid *ptr);
	///Changes the editable state for all child widgets in the formular/dialog
	void set_editable(const bool state);
	///Set the member of the widget
	void set_member(QSqlDatabase *ptr_database, const int id_measure, const int id_areastate);
	///Set default values
	void set_default_values(void);

public slots:
	
	///Open the dialog to edit values in the widget
	void show_as_dialog(void);

private:
	//member
	///Qt-form widget
	Ui::AltGui_Alternative_Info_Wid ui;

	///Current area state
	int area_state;


	//method
	///Transfer members to database
	void transfer_members2database(AltGui_Alternative_Info_Wid *dialog);

	///Set the error(s)
	Error set_error(const int err_type);

};
#endif
