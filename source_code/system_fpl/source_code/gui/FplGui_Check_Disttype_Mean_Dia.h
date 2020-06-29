#pragma once
/**\class FplGui_Check_Disttype_Mean_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPLGUI_CHECK_DISTTYPE_MEAN_DIA_H
#define FPLGUI_CHECK_DISTTYPE_MEAN_DIA_H


//Qt-libs
#include <QtGui>
#include <QDialog>
//forms
#include "ui_FplGui_Check_Disttype_Mean_Dia.h"

//fpl-system
//#include "_Fpl_Distribution_Class.h"

enum  _fpl_distribution_type : short;
enum _fpl_mean_distribution_type : short;



///Dialog-class for setting the parameter for a random generator and transformation check of the mean distribution type\ingroup fpl
/**

\see Fpl_Check_Distribution, Fpl_Distribution_Class_Mean
*/
class FplGui_Check_Disttype_Mean_Dia : public QDialog, public Ui::FplGui_Check_Disttype_Mean_Dia
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:

	///Default constructor
	FplGui_Check_Disttype_Mean_Dia(QWidget *parent = 0);
	///Default destructor
	~FplGui_Check_Disttype_Mean_Dia(void);

	//methods
	///Start the dialog; it returns true by acception, false by rejection
	bool start_dialog(void);

	///Get the mean value
	double get_mean(void);
	///Get the standard deviation
	double get_std_dev(void);
	///Get the constant
	double get_constant(void);
	///Get mean distribution type
	_fpl_mean_distribution_type get_mean_dist_type(void);

private slots:
	///Check the distribution type and change dialog dynamically
	void change_dialog_by_disttype(const int index);

};
#endif
