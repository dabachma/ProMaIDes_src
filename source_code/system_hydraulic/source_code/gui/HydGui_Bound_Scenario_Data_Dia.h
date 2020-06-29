#pragma once
/**\class HydGui_Bound_Scenario_Data_Dia
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYDGUI_BOUND_SCENARIO_DATA_DIA_H
#define HYDGUI_BOUND_SCENARIO_DATA_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>
//forms
#include "ui_HydGui_Bound_Scenario_Data_Dia.h"
//hyd-sys
#include "HydGui_Bound_Scenario_Table_Wid.h"
#include "Hyd_Boundary_Scenario_List.h"



///Dialog-class for setting the data of an hydraulic boundary scenario, e.g. name, annuality etc. \ingroup hyd
/**
*/
class HydGui_Bound_Scenario_Data_Dia : public QDialog, public Ui::HydGui_Bound_Scenario_Data_Dia
{

	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	HydGui_Bound_Scenario_Data_Dia(QWidget *parent = 0);
	///Default destructor
	~HydGui_Bound_Scenario_Data_Dia(void);

	//methods
	///Set the dialog an the appending table widget with the given scenario data
	void set_dialog(const int number, string *file_names, QSqlDatabase *ptr_database, const bool new_base_sc);

	///Start the dialog; it returns true by acception, false by rejection
	bool start_dialog(void);

	///Get the data of a hydraulic boundary scenario
	void get_scenario_data(Hyd_Boundary_Szenario *data, const int index);

	///Get a pointer to the scenario data table widget
	QWidget *get_ptr_scenario_data_widget(void);

	///Set new boundary scenario directly without a GUI dialog for an import via the hydraulic system
	void set_new_boundary_scenario_directly(Hyd_Boundary_Scenario_List *list);


private:

	//members
	///Pointer to the hydraulic boundary scenario data, which is set by the user per dialog
	Hyd_Boundary_Szenario *boundary_data;
	///Number of hydraulic boundary scenarios
	int number_scenario;

	///Number of already existing scenarios
	int number_sc_exist;
	///Number of new scenarios
	int number_sc_new;

	//methods
	///Set the data in the dialog to the hydraulic boundary scenarios
	void set_dialog_data2scenario(void);
	///Allocate the hydraulic boundary scenarios
	void allocate_bound_scenario(void);
	///Delete the hydraulic boundary scenarios
	void delete_bound_scenario(void);

	///Set the error(s)
	Error set_error(const int err_type);

};
#endif
