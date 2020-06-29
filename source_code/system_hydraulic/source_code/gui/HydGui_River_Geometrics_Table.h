#pragma once
/**\class HydGui_River_Geometrics_Table
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef HYDGUI_RIVER_GEOMETRICS_TABLE_H
#define HYDGUI_RIVER_GEOMETRICS_TABLE_H

//sys-system
#include "Sys_ComboBox_Delegate.h"
#include "Sys_SpinBox_Delegate.h"
#include "Sys_DoubleSpinBox_Delegate.h"
#include "_Sys_Abstract_Base_Wid.h"

//qt-libs
#include <QTableView>
#include <QList>
#include <QStandardItemModel>
#include <QWidget>
#include <QHeaderView>

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"

///Table widget for displaying the river profile points \ingroup hyd
/**
	\see _Hyd_River_Profile_Type
*/
class HydGui_River_Geometrics_Table : public QTableView
{
	Q_OBJECT //Macro for using signals and slots (Qt)in this class
	
public:

	///Default constructor
	HydGui_River_Geometrics_Table(QWidget *parent = 0);
	///Default destructor
	~HydGui_River_Geometrics_Table(void);

	//methods

	///Set the number of rows, should do that at the beginning, before rows are filled
	void set_rows(int rowCount);
	///Remove a certain row, characterized by the given index
	void remove_row(int index);
	///Add a point (fill a row) to the table, if row doesent exist, a new row is beeing added (attention: index could differ then)
	void set_point_data(int row, double global_x, double global_y, double global_geo_height, int material_id, double dist_between_points, int channel_type);
	///Get the values of the specified row
	void get_point_data(int row);


	///Returns the rowCount
	int rows(void);
	///Returns the columnCount, which is constantly 6
	int columns(void);

public slots:

	///Append a row at the end of the table, connected to add_row button
	void add_row(void);
	///Remove the selected line from the table, connected to remove_row button
	void remove_row(void);
	///Set the current selected line
	void selection_changed(const QModelIndex &index);

private:

	//members

	///Index of the selected row
	QModelIndex current_index;
	///Internal row counter
	int rowCount;
	///Number of columns, fix
	const int columnCount;
	///The standarditemmodel used to display the tableview items
	QStandardItemModel *model;
	///Needed for last argument in the tableview 'channel_type', which is beeing converted from a string to an int
	enum channel_type {LEFT, MIDDLE, RIGHT};
	///Delegate item used in the table model: combo-box
	Sys_ComboBox_Delegate *cmbDelegate;
	///Delegate item used in the table model: spin-box for an integer value
	Sys_SpinBox_Delegate *intDelegate;
	///Delegate item used in the table model: spin-box for an double value
	Sys_DoubleSpinBox_Delegate *dblDelegate;
};

#endif // HydGui_River_Geometrics_Table_H
