#pragma once
#ifndef SYS_DIVERSE_TEXT_DIA_H
#define SYS_DIVERSE_TEXT_DIA_H
/**\class Sys_Diverse_Text_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

//Qt libs
#include <QMessageBox>
#include <QAbstractButton>

//system sys
#include "Common_Const.h"

///Dialog-class (Ok/Cancel) for reasking the user, if a choosed action should be really conducted  \ingroup sys
/**
 The text for asking can be defined by the developer, as well as the icon. It is based on the 
 QMessageBox. The Qtquestion-icon is the default-icon.

*/
class Sys_Diverse_Text_Dia : public QMessageBox
{
//Macro for using signals and slots (Qt)in this class	
Q_OBJECT

public:
	///Default constructor
	Sys_Diverse_Text_Dia(QWidget *parent = 0);
	///Constructor for a pure information box
	Sys_Diverse_Text_Dia(const bool just_info, QWidget *parent = 0);
	///Default destructor
	virtual ~Sys_Diverse_Text_Dia(void);

	//methods
	///Set the question of the dialog
	void set_dialog_question(const string text);

	///Set the window icon
	void set_window_icon(QIcon icon);

	///Start the dialog and return the user's decision (ok:=true, cancel:=false)
	virtual bool start_dialog(void);

	///Set button text
	void set_button_text(const string ok_butt, const string cancel_butt);



private:

	//member
	///Dialog text; it represents the question to the user
	string question;



};
#endif
