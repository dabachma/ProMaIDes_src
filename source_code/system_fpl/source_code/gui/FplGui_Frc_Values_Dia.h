#pragma once
/**\class FplGui_Random_Check_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPLGUI_FRC_VALUES_DIA_H
#define FPLGUI_FRC_VALUES_DIA_H
//qt-libs
#include <QDialog>
#include <QTableView>
#include <QList>
//qwt libs
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
//#include <qwt_data.h>
//#include <qwt_array.h>
//#include <qwt_compat.h>
#include <qwt_symbol.h>

//system sys libs
#include "Common_Const.h"
#include "Sys_DoubleSpinBox_Delegate.h"
#include "Sys_Diverse_Text_Dia.h"
#include "Error.h"

//forms 
#include "ui_FplGui_Frc_Values_Dia.h"


///Dialog-class for setting the values of a fragility curve via a dialog \ingroup fpl
/**

*/
class FplGui_Frc_Values_Dia : public QDialog
{
//Macro for using signals and slots (Qt)in this class
   Q_OBJECT

public:
	///Default constructor
    FplGui_Frc_Values_Dia(QWidget *parent = 0);
	///Default destructor
	~FplGui_Frc_Values_Dia(void);

	// Methods
	///Return the number of points, which are set
	int get_number_points(void);
	///Set data of a given fragility curve
	void set_data(const double water_h, const double prob);

	///Set the maximum waterlevel
	void set_maximum_waterlevel(const double max_h);

	///Return the waterlevel for the given index
	double get_water_level(const int index);
	///Return the failure probability for the given index
	double get_fail_prob(const int index);
	///Show fpl-section informations to the dialog header
	void show_frc_infos(const QString txt);
	///Start the dialog and return the user decision (ok:=true, cancel:=false)
	bool start_dialog(void);
	///Return the monte-carlo annuality of failure probability
	double get_annuality_failure(void);
	///Set the annaulity of failure
	void set_annuality_failure(const double annu);


private slots:
	///Add a new row to the table
	void addPointButton_clicked(void);
	///Remove the active row in the table
	void delPointButton_clicked(void);
	///Add and delete rows with using the spinbox
	void PointSpinBox_valueChanged(int val);
	///Check all cells to correctness
	void checkCells(int row, int col);
	///Check cell to correctness
	void checkCells(double value);
	///Check if the curve are ready to be plotted and plot them
	void okButton_clicked(void);

private:
	// Members

	///Summarizes all widget in the dialog; produced with the QtDesigner
	Ui::FplGui_Frc_Values_Dia ui;
	
	///Array of x values (water level)
	QVector<double> x;
	///Array of y values (failure probability)
	QVector<double> y;
	///Text that should appear in the FRC Info Box
	QString frc_infos;

	///Maximum value for the probability
	const double prob_max;
	///Minimum value for the probability
	const double prob_min;
	///Maximum value for the waterlevel
	double waterlevel_max;
	///Minimum value for the waterlevel
	const double waterlevel_min;

	///The number of rows in the table
	int number_rows;
	///The number of columns in the table
	int number_columns;

	///Curve for ploting the fragility curve
	QwtPlotCurve *frc_curve;

	//methods
	///Confirm the deleting of a row
	bool warningMessage(void);
	///Append the fixed and in the table stored values in the arrays 
	void get_values(void);

	///Set the plot curve for the fragility curve
	void set_fragility2plotcurve(void);
	///Allocate the fragility curve for ploting
	void allocate_plot_curve(void);
	///Delete the plot curve for the fragility curve
	void delete_plot_curve(void);

	///Set the plot parameters
	void set_plot(void);

	///Set error(s)
	Error set_error(const int err_type);
	
};
#endif
