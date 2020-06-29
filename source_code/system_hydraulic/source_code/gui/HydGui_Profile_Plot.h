#pragma once
/**\class HydGui_Profile_Plot
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYDGUI_PROFILE_PLOT_H
#define HYDGUI_PROFILE_PLOT_H

//classes qwt
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
//system_sys_ classes
#include "_Sys_Common_System.h"
//system_hyd_classe
#include "Hyd_Profile_Segment.h"

///Plot-widget class to depict a river profile as cross-section and the profile points (_Hyd_River_Profile_Type) \ingroup hyd
/**
*/
class HydGui_Profile_Plot : public QwtPlot
{
public:
	///Default constructor
	HydGui_Profile_Plot(QWidget *parent=NULL);
	///Default destructor
	~HydGui_Profile_Plot(void);


	//methods

	///Set the titles of the plot, the x- and y-axis
	void set_titles(const string plot, const string x_axis, const string y_axis);
	
	///Set the number of curves, which should be plotted
	void set_number_curves(const int number_of_curves);

	///Set the curves with the title, color and the segments, which should be plotted
	void set_curves(const int curve_number, const string title, const int number_segments, QColor my_color, Hyd_Profile_Segment *segment2show);

	///Set symbols for each curve
	void set_symbols(const int curve_number, QwtSymbol::Style my_style);

	///Show the plot
	void show_plot(void);


private:
	//members

	///The curve(s), which should be plotted
	/**It is allocated in allocate_curves(void).
	*/
	QwtPlotCurve *curves;
	///The symbols for each curve
	QwtSymbol *symbols;

	///The legend of the plot
	QwtLegend *legend;
	
	//Number of curve, which should be plotted
	int number_curves;
	///A buffer for the x-value, which should be plotted; it is given for each curve to the Qwt-plot class
	double *x_values;
	///A buffer for the y-value, which should be plotted; it is given for each curve to the Qwt-plot class
	double *y_values;

	//methods

	///Allocate the curves, which should be plotted corresponding to the number of curves
	void allocate_curves(void);
	///Allocate the number of data points for each curve
	void allocate_data_points(const int number_of_points);

	

};
#endif