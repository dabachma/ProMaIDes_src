#pragma once
/**\class HydGui_Bound_Scenario_Table_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYDGUI_BOUND_SCENARIO_TABLE_WID_H
#define HYDGUI_BOUND_SCENARIO_TABLE_WID_H

//sys-system
#include "Sys_SpinBox_Delegate.h"
#include "Sys_DoubleSpinBox_Delegate.h"
//qt-libs
#include <QTableView>
#include <QList>
#include <QStandardItemModel>
#include <QWidget>
#include <QHeaderView>
//system sys
#include "Common_Const.h"
//hyd-sys
#include "Hyd_Boundary_Szenario.h"


///Table widget for displaying the hydraulic boundary scenario data \ingroup hyd
/**
	\see Hyd_Boundary_Szenario
*/
class HydGui_Bound_Scenario_Table_Wid : public QTableView
{

	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT 

public:
	///Default constructor
	HydGui_Bound_Scenario_Table_Wid(QWidget *parent=NULL);
	///Default destructor
	~HydGui_Bound_Scenario_Table_Wid(void);

	///Set the number of rows; it have to be done before the rows are filled
	void set_rows(const int rowCount);
	///Set rows of existing scenarios
	void set_rows_existing_sc(const int number, Hyd_Boundary_Szenario *scenario);

	///Set file names to the table widget
	void set_rows_new_sc(const int number, string *file_name, const int number_old);

	///Set the data of a row to a given hydraulic boundary scenario
	void set_data2bound_scenario(const int index, Hyd_Boundary_Szenario *scenario); 

private:
	//members

	///Internal row counter
	int row_count;
	///Number of columns
	const int column_count;
	///The standarditemmodel used to display the tableview items
	QStandardItemModel *model;
	///Gray color for already existing scenarios
	QBrush my_color;

	///Delegate item used in the table model: spin-box for an integer value
	Sys_SpinBox_Delegate *int_delegate;
	///Delegate item used in the table model: spin-box for an double value
	Sys_DoubleSpinBox_Delegate *dbl_delegate;
};
#endif
