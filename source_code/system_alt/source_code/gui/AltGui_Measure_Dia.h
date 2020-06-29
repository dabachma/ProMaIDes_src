#pragma once
/**\class AltGui_Measure_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef ALTGUI_MEASURE_DIA_H
#define ALTGUI_MEASURE_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>
//forms
#include "ui_AltGui_Measure_Dia.h"

#include "AltGui_Alternative_Info_Dia.h"

#include "Common_Const.h"



///Dialog-class for setting the already implemented measure for select the measure state to switch  \ingroup alt
/**
*/
class AltGui_Measure_Dia : public QDialog, public Ui::AltGui_Measure_Dia
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT
public:
	///Default constructor
	AltGui_Measure_Dia(QWidget *parent = 0);
	///Default destructor
	~AltGui_Measure_Dia(void);


	//method
	///Set window title
	void set_window_title(const string txt);
	///Set heading
	void set_heading(const string txt);
	///Set for the dialog the measure identifier
	void set_dialog(const int number, _alt_general_info *measure_list, const int current_measure);
	///Set for the dialog the measure identifier for a deleting action
	void set_dialog_delete(const int number, _alt_general_info *measure_list, const int current_measure);
	///Start the dialog it returns true by acception, false by rejection
	bool start_dialog(void);

	///Get the selected informations, the measure state to switch 
	int get_selected_information(void);

private:

	//member
	///Measure state to switch
	int measure_state2switch;


};
#endif

