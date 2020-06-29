#pragma once
/**\class HydGui_Boundary_Scenario_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYDGUI_BOUNDARY_SCENARIO_DIA_H
#define HYDGUI_BOUNDARY_SCENARIO_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>
//forms
#include "ui_HydGui_Boundary_Scenario_Dia.h"
//hyd-sys
#include "Hyd_Boundary_Szenario.h"

///Dialog-class for choosing multiple hydraulic boundary scenarios, which should be calculated or checked \ingroup hyd
/**
*/
class HydGui_Boundary_Scenario_Dia : public QDialog, public Ui::HydGui_Boundary_Scenario_Dia
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	HydGui_Boundary_Scenario_Dia(QWidget *parent = 0);
	///Default destructor
	~HydGui_Boundary_Scenario_Dia(void);


	//methods

	///Start the dialog and set the current available scenarios as a selection; it returns true by acception, false by rejection
	bool start_dialog(Hyd_Boundary_Szenario *ptr_scenarios, const int number);
	///Set a text to the text label of the dialog
	void set_txt(const string txt);


public slots:
	///Switch the selection between all and clear selection
	void all_selection(bool flag);


private:

	//members

	///The items of the list widget
	QListWidgetItem *items;

	///Header label
	string header_txt;

	//methods

	///Allocate the items of the list widget
	void allocate_list_item(const int number);


};
#endif
