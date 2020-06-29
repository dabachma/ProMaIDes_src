#pragma once
#ifndef FPL_DATA_RESULTS_H
#define FPL_DATA_RESULTS_H


//class for the chart
#include <qwt_plot.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_data.h>
#include <qwt_text.h>
#include <qwt_symbol.h>

//class
#include "_common_system.h"
#include "Fpl_Data_Section.h"
#include "Fpl_Frc_Sim.h"



class Fpl_Data_Results : public _Common_System
{
public:
	//constructor
	Fpl_Data_Results(void);
	//destructor
	~Fpl_Data_Results(void);

	//decide per console which action is desired
	void action(void);
	//show frc-table
	QTableView* show_frc_table(void);
	//frc- table to graph
	QWidget* show_graph_frc(const int actuel_sec_num);


private:
	
	//show a frc-table for one section
	QTableView* show_frc_table(const int actuel_sec_num);
	//ask which section
	int ask_section(const string section_specifier);


	//members
	Database_Viewer *my_browser_all;
	Database_Viewer *my_browser_one;
	QwtPlot *frc_plot;
	QwtPlotCurve *frccurve;
	QWidget *win_result;
	QBoxLayout *my_layout;

	//build the error msg
	Error set_error(const int err_type);

};
#endif