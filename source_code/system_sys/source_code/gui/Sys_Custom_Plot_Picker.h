#pragma once
/** 
	\class Sys_Custom_Plot_Picker
	\author Daniel Bachmann, Yann Labou et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/

#ifndef SYS_CUSTOM_PLOT_PICKER_H
#define SYS_CUSTOM_PLOT_PICKER_H

//qwt-libs
#include <qwt_plot_picker.h>
#include <qwt_plot_canvas.h>

///Picker-class for a custom Qwt-plot plicker \ingroup sys
/**
Custom picker class that enable the picker to show only two digits after the decimal point.
Overload the trackerTextmethod. Inherits QwtPlotPicker.
*/
class Sys_Custom_Plot_Picker : public QwtPlotPicker
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT


public:
	///Default constructor
    Sys_Custom_Plot_Picker(QWidget *canvas);
	///Default destructor
	~Sys_Custom_Plot_Picker(void);


protected:

	//method
	///Get the tracker text
	virtual QwtText trackerText(const QPoint &pos);
};
#endif
