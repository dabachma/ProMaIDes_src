#pragma once
/**\class Sys_Export_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef SYS_EXPORT_DIA_H
#define SYS_EXPORT_DIA_H

//QT libs
#include <QDialog>

//forms
#include "ui_Sys_Export_Dia.h"
//system sys
#include "Common_Const.h"


///Enumerator for the type of export \ingroup sys
enum _sys_export_type{
	///Export to the tecplot format
	exp_tecplot,
	///Export to excel format with tab as separator
	exp_excel_tab,
	///Export to excel format with semicolon as separator
	exp_excel_comma,
};

///Dialog-class for an export of table-views or graphs to ascii-files \ingroup sys
/**

*/
class Sys_Export_Dia  : public QDialog
{
friend class Sys_Plot_Wid;
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	Sys_Export_Dia(QWidget *parent = 0);
	///Default destructor
	~Sys_Export_Dia(void);

	//method
	///Set the header text label of the dialog
	void set_header_txt(const string txt);
	///Check whether all the items or only the selected should be exported
	bool getCheckBoxState(void);

	///Get the type of export, which is required (e.g. excel tab, tecplot etc.)
	_sys_export_type get_export_type(void);
	///Get output file and path
	string get_output_file(void);

	///Start the dialog
	bool start_dialog(void);

	///Get a pointer to the select-all check box
	QCheckBox *get_ptr_selectall_checkbox(void);
	///Get a pointer to the select-all group box
	QGroupBox *get_ptr_selectall_groupbox(void);
	///Get a pointer to the radio-button tecplot-output
	QRadioButton* get_ptr_tecplot_radio_button(void);

private slots:
	///Check the state of the Checkbox
	void isChecked(int state);

private:
	//members
	///Form class made with the QT-designer for the layout of the dialog
	Ui::Sys_Export_Dia ui;
};
#endif
