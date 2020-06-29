#pragma once
/**\class AltGui_Id_Replacing_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef ALTGUI_ID_REPLACING_WID_H
#define ALTGUI_ID_REPLACING_WID_H

//Qt-libs
#include <QtGui>
#include <QWidget>
//forms
#include "ui_AltGui_Id_Replacing_Wid.h"

//system alt


///Widget-class for displaying 2 indices in two combo-boxes. One id replaces than the other one in the system as measure   \ingroup alt
/**
In general it is used in AltGui_Dam_Change_Function_Dia.
*/
class AltGui_Id_Replacing_Wid : public QWidget, public Ui::AltGui_Id_Replacing_Wid
{

	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	AltGui_Id_Replacing_Wid(QWidget *parent = 0);
	///Default destructor
	~AltGui_Id_Replacing_Wid(void);


	//method
	///Set the combox with the current id items
	void set_combobox_current_items(QStringList list);
	///Set the combox with the new id items
	void set_combobox_new_items(QStringList list);

	///Get the pointer to the combo-box with the current id items
	QComboBox *get_ptr_combo_box_current(void);
	///Get the pointer to the combo-box with the new id items
	QComboBox *get_ptr_combo_box_new(void);
};
#endif
