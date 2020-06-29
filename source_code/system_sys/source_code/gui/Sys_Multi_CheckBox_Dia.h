#pragma once
#ifndef SYS_MULTI_CHECKBOX_DIA_H
#define SYS_MULTI_CHECKBOX_DIA_H

/**\class Sys_Multi_CheckBox_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

//QT libs
#include <QDialog>
#include <QCheckBox>

//forms
#include "ui_Sys_Multi_Filechooser_Dia.h"

//system sys
#include "Common_Const.h"

///Dialog-class for setting multiple check-boxes, with text-comments for each box \ingroup sys
/**
The number of check-boxes can be set by the developer in the code. 
A  text-label for each check-boxes line is provided, e.g. for marking the type of boolean, which should be
read in. Each check-box line is a QCheckBox. A dynamical handling of the checkboxes
is not possible. For this purpose use Sys_Multipathsettings_Dia.
*/
class Sys_Multi_CheckBox_Dia : public QDialog
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	Sys_Multi_CheckBox_Dia(QWidget *parent = 0);
	///Default destructor
	virtual ~Sys_Multi_CheckBox_Dia(void);


	//method
	///Set the number of check-boxes. They are also allocated and set to the dialog.
	void set_number_check_boxes(const int number, QIcon icon);

	///Get the number of check-boxes (QCheckBox) used in the dialog
	int get_number_check_boxes(void);

	///Set button text 
	void set_button_txt(const string ok, const string cancel);

	///Get a pointer to the check-box given by the index
	QCheckBox* get_ptr_check_box(const int index);

	///Get the boolean of the check-box given by the index
	bool get_bool_check_box(const int index);

	///Set the text of the main text-label
	void set_main_text_label(const string text);
	///Set the window title
	void set_window_title(const string text);

	///Start the dialog
	virtual bool start_dialog(void);


private:

	//members
	///Number of check-boxes
	int number_boxes;

	///The check-boxes, which are displayed
	QCheckBox *check_boxes;

	///Form class made with the QT-designer for the layout of the dialog
	Ui::Sys_Multi_Filechooser_Dia ui;

	///Layout for the check-boxes
	QVBoxLayout check_box_layout;
};
#endif
