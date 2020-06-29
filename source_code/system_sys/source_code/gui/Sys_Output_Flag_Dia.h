#pragma once
#ifndef SYS_OUTPUT_FLAG_DIA_H
#define SYS_OUTPUT_FLAG_DIA_H
/**\class Sys_Output_Flag_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

// Qt Libs
#include <QDialog>
#include <QAbstractButton>
#include <QCheckBox>
#include <QString>
//forms
#include "ui_Sys_Output_Flag_Dia.h"
//system sys
#include "Common_Const.h"

///Dialog-class for asking the user the flag for the output to the display for the different modules (detailed or not detailed) \ingroup sys
class Sys_Output_Flag_Dia : public QDialog
{
//Macro for using signals and slots (Qt)in this class	
Q_OBJECT

public:
	///Default constructor
	Sys_Output_Flag_Dia(QWidget *parent = 0);
	///Default destructor
	~Sys_Output_Flag_Dia(void);

	//method
	///Set the text for which moduls the outputflag should be changed
	void set_txt_modul_type(_sys_system_modules type);
	///Set the current output flag
	void set_current_flag(const bool flag);

	///Make the dialog and get the new detailed flag
	bool get_new_detailed_flag(void);


private:

	//members
	///Form class for the gui layout
	Ui::Sys_Output_Flag_Dia ui;

	///Text to be displayed as Output type
	QString text;
	///Old output flag
	bool old_flag;
	///New output flag
	bool new_flag;
};
#endif