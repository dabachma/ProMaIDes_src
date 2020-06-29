#pragma once
/**\class AltGui_File_Replacing_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef ALTGUI_FILE_REPLACING_WID_H
#define ALTGUI_FILE_REPLACING_WID_H

//Qt-libs
#include <QtGui>
#include <QWidget>
//forms
#include "ui_AltGui_File_Replacing_Wid.h"

//system alt


///Widget-class for displaying and setting one file, which replaces specific system characteristics as measure like FPL-sections  \ingroup alt
/**
In general it is used in AltGui_Measure_Replacing_File_Dia.
*/
class AltGui_File_Replacing_Wid : public QWidget, public Ui::AltGui_File_Replacing_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	AltGui_File_Replacing_Wid(QWidget *parent = 0);
	///Default destructor
	~AltGui_File_Replacing_Wid(void);

	///Set the combox items
	void set_combobox_items(QStringList list);

	///Get the pointer to the combo-box
	QComboBox *get_ptr_combo_box(void);
	///Get choosen the file-name 
	QString get_file_name(void);

};
#endif

