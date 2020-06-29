#pragma once
/**\class HydGui_System_State_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYDGUI_SYSTEM_STATE_DIA_H
#define HYDGUI_SYSTEM_STATE_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>
#include <QThread>
//system sys
#include "Sys_Project.h"
//forms
#include "ui_HydGui_System_State_Dia.h"



///Dialog-class for changing the hydraulic system data \ingroup hyd
/**
*/
class HydGui_System_State_Dia : public QDialog, public Ui::HydGui_System_State_Dia
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	HydGui_System_State_Dia(QWidget *parent = 0);
	///Default destructor
	~HydGui_System_State_Dia(void);

	///Start the dialog it returns true by acception, false by rejection
	bool start_dialog(const bool database_flag);

	///Set the current set parameters
	void set_current_system_data(const _hyd_state_data current);
	///Set the new parameters
	_hyd_state_data get_new_system_data(void);



};
#endif

