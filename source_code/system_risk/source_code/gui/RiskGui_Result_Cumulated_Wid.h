#pragma once
/**\class RiskGui_Result_Cumulated_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef RISKGUI_RESULT_CUMULATED_WID_H
#define RISKGUI_RESULT_CUMULATED_WID_H

//Qt-libs
#include <QtGui>
#include <QWidget>
//forms
#include "ui_RiskGui_Result_Cumulated_Wid.h"

//risk-system
#include "Risk_System.h"

//system sys
#include "Sys_Plot_Wid.h"
#include "Sys_Diverse_Text_Dia.h"
#include "_Sys_Data_Wid.h"

///Widget-class for displaying the cumulated risk results\ingroup risk
/**


\see Risk_System
*/

class RiskGui_Result_Cumulated_Wid : public _Sys_Data_Wid, Ui::RiskGui_Result_Cumulated_Wid
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	RiskGui_Result_Cumulated_Wid(QWidget *parent=NULL);
	///Default destructor
	~RiskGui_Result_Cumulated_Wid(void);

	//methods
	///Set tooltip of the header
	void set_header_tooltip(const string tooltip, QTreeWidgetItem *item);
	///Set the member of the widget
	void set_member(QSqlDatabase *ptr_database, const _sys_system_id id, const _risk_type type);

private:

	//members
	///Number of values to plot
	int number_data;

	///Array of x-values
	double *array_x;
	///Array of y-values
	double *array_y;


	//methods
	///Generate the plot for the economic risk
	void generate_ecn_plots(QSqlQueryModel *model);
	///Generate the plot for the ecological risk
	void generate_eco_plots(QSqlQueryModel *model);

	///Generate the plot for the monetary summary risk
	void generate_monetary_plots(QSqlQueryModel *model);

	///Generate the plot for the people2risk risk
	void generate_pop_plots(QSqlQueryModel *model);
	///Generate the plot for the psycho-social risk
	void generate_pys_plots(QSqlQueryModel *model);

	///Generate the plot for the simple counting risk
	void generate_sc_plots(QSqlQueryModel *model);

	///Generate the plot for the ci risk
	void generate_ci_plots(QSqlQueryModel *model);

	///Generate the plot for the maximum dicharge risk
	void generate_max_q_plot(QSqlQueryModel *model);

	///Alloacte data arrays
	void allocate_data(void);
	///Delete data arrays
	void delete_data(void);

	///Set error(s)
	Error set_error(const int err_type);
};
#endif
