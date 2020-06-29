#pragma once
/**\class RiskGui_Optimisation_Prediction_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/

#ifndef RISKGUI_OPTIMISATION_PREDICTION_DIA_H
#define RISKGUI_OPTIMISATION_PREDICTION_DIA_HH

//Qt-libs
#include <QtGui>
#include <QDialog>
//forms
#include "ui_RiskGui_Optimisation_Prediction_Dia.h"

//forward declaration
struct _risk_ca_scenario_control;

///Dialog-class for asking the boundaries of a automatic optimisation for the catchment risk calculation \ingroup risk
/**

\see Risk_System

*/
class RiskGui_Optimisation_Prediction_Dia : public QDialog, public Ui::RiskGui_Optimisation_Prediction_Dia
{

//Macro for using signals and slots (Qt)in this class
Q_OBJECT 

public:
	///Default constructor
	RiskGui_Optimisation_Prediction_Dia(QWidget *parent = NULL);
	///Default destructor
	~RiskGui_Optimisation_Prediction_Dia(void);

	//method

	///Start the dialog; it returns true by acception, false by rejection
	bool start_dialog(void);

	///Set the value from the dialog
	void set_boundaries(_risk_ca_scenario_control *params);


private slots:

	///Change the status (disable/enable) of the economical spinbox corresponding the checkbox
	void change_status_ecn_spinbox(bool enable);
	///Change the status (disable/enable) of the ecological spinbox corresponding the checkbox
	void change_status_eco_spinbox(bool enable);
	///Change the status (disable/enable) of the people2risk affected people spinbox corresponding the checkbox
	void change_status_pop_affected_spinbox(bool enable);
	///Change the status (disable/enable) of the people2risk endangered people spinbox corresponding the checkbox
	void change_status_pop_endangered_spinbox(bool enable);
	///Change the status (disable/enable) of the psycho-social spinbox corresponding the checkbox
	void change_status_pys_spinbox(bool enable);

private:

	//members

	///Boundary value of the economical predicted risk
	double ecn_bound;
	///Boundary value of the ecological predicted risk
	double eco_bound;
	///Boundary value of the people2risk affected people risk
	double pop_aff_bound;
	///Boundary value of the people2risk endangered people risk
	double pop_end_bound;
	///Boundary value of the psycho-social risk
	double pys_bound;

};
#endif
