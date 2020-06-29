#pragma once
/**\class AltGui_Alternative_Base_On_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef ALTGUI_ALTERNATIVE_BASE_ON_DIA_H
#define ALTGUI_ALTERNATIVE_BASE_ON_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>

#include <QSqlDatabase>
#include "_Sys_Common_System.h"

//forms
#include "ui_AltGui_Alternative_Base_On_Dia.h"

///Dialog-class for setting the a base measure state for changing the paramater of one or multiple FPL-sections \ingroup alt
/**
*/
class AltGui_Alternative_Base_On_Dia : public QDialog, public Ui::AltGui_Alternative_Base_On_Dia
{

	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT


public:
	///Default constructor
	AltGui_Alternative_Base_On_Dia(QWidget *parent = 0);
	///Default destructor
	~AltGui_Alternative_Base_On_Dia(void);


	//methods
	///Set for the dialog the measure identifier
	void set_dialog(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Start the dialog it returns true by acception, false by rejection
	bool start_dialog(void);

	///Get the measure-state on which the new measure (change FPL_parameters) should base on
	int get_measurestate_base_on(void);

private:
	//members
	///Measure state on which a new measure should based on (used in Change FPL-Paramters)
	int measurestate_base;
};
#endif
