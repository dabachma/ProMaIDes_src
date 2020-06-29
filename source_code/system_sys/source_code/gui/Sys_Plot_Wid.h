#pragma once
/** 
	\class Sys_Plot_Wid
	\author Daniel Bachmann, Yann Labou et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2012
*/

#ifndef SYS_PLOT_WID_H
#define SYS_PLOT_WID_H

//Qt Libs
#include <QtGui>
#include <QObject>
#include <QSvgGenerator>
#include <QPrintDialog>

//Qwt Libs
#include <qwt_legend.h>
//#include <qwt_legend_item.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_zoomer.h>
#include <qwt_symbol.h>
#include <qwt_dyngrid_layout.h>
#include <qwt_plot_renderer.h>

//System Libs
#include "Common_Const.h"
#include "Error.h"
#include "Sys_Export_Dia.h"
#include "Sys_PlotSettings_Dia.h"
#include "Sys_CurveSettings_Wid.h"
#include "Sys_Custom_Plot_Picker.h"

//Forward Declaration of class Sys_Plot_Frame because this class would be used but is not yet defined
class Sys_Plot_Frame;
//Forward Declaration of class Sys_Custom_Picker because this class would be used but is not yet defined
class Sys_Custom_Picker;

///Widget-class for ploting data to a plot \ingroup sys
/**

*/
class Sys_Plot_Wid : public QwtPlot
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:

	///Default constructor
	Sys_Plot_Wid(QWidget *parent = NULL);
	///Default destructor
	~Sys_Plot_Wid(void);

	//members
	///Type of data export
	_sys_export_type export_type;

	///Action for painting plot to printer
	QAction *printAct;
	///Action for printing plot as pdf
    QAction *printPDFAct;
	///Action  for saving plot as image
    QAction *saveasAct;
	///Action for enabling zooming functionality
    QAction *zoomAct;
	///Action for restoring to original size
	QAction *restoresizeAct;
	///Action to enable grid on the plot
	QAction *showGridAct;
	///Action for the export of selected data to a file
	QAction *exportAct;
	///Action for editing plot curves
	QAction *editPlotAct;

	//methods
	///Return pointer on the frame. This method provides an interface to access Sys_Plot_Frame functions
	Sys_Plot_Frame *getFramePtr(void);

	///Set a pointer to the parent frame
	void setFramePtr(Sys_Plot_Frame *parent_frame);
	///Set the number of curves and allocate them
	void set_number_curves(const int number);
	///Get the number of curves
	int get_number_curves(void);
	///Set plot title
	void set_plot_title(const string title);
	///Set title of x- and y-axis
	void set_axis_title(const string x_axis, const string y_axis);
	///Set curve characteristics
	void set_curve_characteristics(const int index, const string title, const QwtPlotCurve::CurveStyle style, const QColor color);
	///Overloaded method. See also set_curve_characteristics(const int index, const string title, const QwtPlotCurve::CurveStyle style, const QColor color)
	void set_curve_characteristics(const int index, const QwtText title, const QwtPlotCurve::CurveStyle style, const QColor color, const int width, const Qt::PenStyle pen_style);
	///Set curve data
	void set_curve_data(const int index, const int number_point, double *xdata, double *ydata);
	///Set curve symbols
	void set_curve_symbols(const int index, const int size = 5 ,QwtSymbol::Style style = QwtSymbol::Style::NoSymbol);
	///Overloaded method. See also set_curve_symbols(const int index, const int size = 5 ,QwtSymbol::Style style = QwtSymbol::Style::NoSymbol)
	void set_curve_symbols(const int index, const int size, QwtSymbol::Style style, const QColor);
	///Set the legend
	void set_legend(const bool flag);
	///Set the scale for the y-axis
	void set_scale_y_axis(const double min, const double max);
	///Show the plot
	void show_plot(void);
	///Clear the plot
	void clear_plot(void);
	///Add curve names into the model stored in the selection combobox
	void addItem(bool checked);
	///Returns the names of all the QwtPlotCurve items on the plot, as a string list
	const QStringList curvesList(void);

	///Return the color of a QwtPlotCurve items on the plot
	//QColor getCurveColor(const int index);

	///This method is intended to organize the legend in one column if the legend position is left or right. 
	///Otherwise the legend items will be placed in #curves rows from left to right
	///The method QwtPlot::insert(QwtLegend*, QwtPlot::LegendPosition, double) should handle this but do not.
	///Maybe a bug?
	void updateMyLegend(bool update) const;
	///Export selected data to file in ascii-format separated with tabs
	void setOutputTypeAsciiTab(void);
	///Export selected data to file in ascii-format separated with comma
	void setOutputTypeAsciiComma(void);
	///Export selected data to file in tecplot-format
	void setOutputTypeTecplot(void);

private:

	//members
	///Filename for the data export
	string filename;
	///Number of curves
	int number_curve;
	///Flag if the plot is with legend
	bool legend_flag;
	///Color of the curve and the symbols
	QColor my_color;
	///Width of the curve
	int my_width;
	

	///Pointer to the legend
	QwtLegend *legend;
	///Provides zooming by magnifying in steps using the mouse wheel
	QwtPlotMagnifier *plotMagnifier;
	///Provides panning of a plot canvas
	QwtPlotPanner *plotPanner;
	///Provides stacked zooming for a plot widget
	QwtPlotZoomer *plotZoomer;
	///Provides selections on a plot canvas
	QwtPlotPicker *picker;
	///Grid of the plot
	QwtPlotGrid *grid;
	///List of pointers to curves of the plot
	QwtPlotCurve *curves;
	///Pointer to the parent frame
	Sys_Plot_Frame *frame;


	//methods
	///Create the action of the frame widget
	void createActions(void);
	///Connect the actions of the frame widget per signal and slot
	void connectActions(void);

	///Exclude the path from the file name
	QString strippedName(QString fullFileName);
	///Exclude the path from the absolute path name
	QString strippedPathName(QString fullFileName);
	///Allocate the curves, symbols and legend
	void allocate_curves(void);
	///Delete the curves, symbols and legend
	void delete_curves(void);
	///Context menu, provides some functionalities by mouse right click
    void contextMenuEvent(QContextMenuEvent *event);
	///Set error(s)
	Error set_error(const int err_type);

private slots:
	///Paint the plot on a printer
	void printPlot(void);
	///Paint the plot as PDF
	void printPDF(void);
	///Save the plot as Image (Currently available formats are .jpg and .png)
	void saveAS(void);
	///Provides zooming functionality
	void enableZoomMode(bool);
	///Restore plot size to original dimensions
	void restoreSize(void);
	///Show or hide the plot's grid
	void setShowGrid(bool);
	///Ensure that Zoom Button is checked as soon as Zoom Action is being checked
	void checkButtons(bool);
	///Open the export dialog
	void export_curve_data(void);
	///Provides various possibilities to edit the plot curves
	void editPlot(void);
	///Show or hide selected curves by checking or not the checkboxes in the selection combobox
	void showCurves(const QModelIndex&);
	
};
#endif
