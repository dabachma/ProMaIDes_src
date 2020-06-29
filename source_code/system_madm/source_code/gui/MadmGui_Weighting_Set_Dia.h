#pragma once
/**\class MadmGui_Weighting_Set_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef MADMGUI_WEIGHTING_SET_DIA_H
#define MADMGUI_WEIGHTING_SET_DIA_H


//Qt-libs
#include <QtGui>
#include <QDialog>
#include <QSqlDatabase>
//sys lib
#include "Common_Const.h"
#include "Error.h"
//forms
#include "ui_MadmGui_Weighting_Set_Dia.h"



///Dialog-class for choosing multiple madm-weighting sets \ingroup madm
/**
This dialog class uses a combo-box for the selction of different madm-weighting sets. 
\see Madm_System
*/
class MadmGui_Weighting_Set_Dia : public QDialog, public Ui::MadmGui_Weighting_Set_Dia
{

//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	MadmGui_Weighting_Set_Dia(QWidget *parent = NULL);
	///Default destructor
	~MadmGui_Weighting_Set_Dia(void);

	///Set the dialog, like the database, system_id etc.
	int set_dialog(QSqlDatabase *ptr_database);
	///Start the dialog and set the current available sections as a selection; it returns true by acception, false by rejection
	bool start_dialog(int  **list_sets, int *number);
	///Set a text to the text label of the dialog
	void set_txt(const string txt);
	///Set the dialog header text
	void set_dia_header_txt(const string header_title);

public slots:
	///Switch the selection between all and clear selection
	void all_selection(bool flag);

private:

	//members

	//methods
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
