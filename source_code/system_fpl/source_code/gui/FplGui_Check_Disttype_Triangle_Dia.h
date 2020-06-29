#pragma once
/**\class FplGui_Check_Disttype_Triangle_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPLGUI_CHECK_DISTTYPE_TRIANGLE_DIA_H
#define FPLGUI_CHECK_DISTTYPE_TRIANGLE_DIA_H


//Qt-libs
#include <QtGui>
#include <QDialog>
//forms
#include "ui_FplGui_Check_Disttype_Triangle_Dia.h"

//fpl-system

///Dialog-class for setting the parameter for a random generator and transformation check of the triangle distribution type\ingroup fpl
/**

\see Fpl_Check_Distribution, Fpl_Distribution_Class_Triangle
*/
class FplGui_Check_Disttype_Triangle_Dia: public QDialog, public Ui::FplGui_Check_Disttype_Triangle_Dia
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT
public:

	///Default constructor
	FplGui_Check_Disttype_Triangle_Dia(QWidget *parent = 0);
	///Default destructor
	~FplGui_Check_Disttype_Triangle_Dia(void);

	//methods
	///Start the dialog; it returns true by acception, false by rejection
	bool start_dialog(void);

	///Get the min value
	double get_min(void);
	///Get the max value
	double get_max(void);
	///Get the mid value
	double get_mid(void);
	///Get the constant
	double get_constant(void);

private slots:
	///Check if the mid-value has changed and set the maximum/minimum values of the spinboxes (min/max) dynamically 
	void check_mid_change(const double value);
};
#endif
