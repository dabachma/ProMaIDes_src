#pragma once
/**\class Sys_PlotSettings_Dia
	\author Daniel Bachmann, Yann Labou et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/

#ifndef SYS_PLOTSETTINGS_DIA_H
#define SYS_PLOTSETTINGS_DIA_H

//QT libs
#include <QDialog>
#include <QFontDialog>
#include <QColorDialog>

//forms
#include "ui_Sys_PlotSettings_Dia.h"
//system sys
#include "Common_Const.h"
#include "Sys_CurveSettings_Wid.h"



///Dialog-class for the editing of a plot widget \ingroup sys
/**

*/

class Sys_PlotSettings_Dia : public QDialog
{
	Q_OBJECT

public:
	///Default constructor
	Sys_PlotSettings_Dia(QWidget *parent = NULL);
	///Default destructor
	~Sys_PlotSettings_Dia(void);


	//method
	///Start the dialog
	bool start_dialog(void);

	///Set the title, x-axis name and y-axis name
	void set_title_x_y_axis(const string title, const string x_axis, const string y_axis);
	///Get the title
	string get_title(void);
	///Get the x-axis name
	string get_x_axis_name(void);
	///Get the y-axis name
	string get_y_axis_name(void);

	///Set the font of the title, x-axis name and y-axis name
	void set_font_title_x_y_axis(const QFont title, const QFont x_axis, const QFont y_axis);
	///Get the font of the title
	QFont get_font_title(void);
	///Get the font of the x-axis name
	QFont get_font_x_axis_name(void);
	///Get the font of the y-axis name
	QFont get_font_y_axis_name(void);

	///Set the color of the title, x-axis name and y-axis name
	void set_color_title_x_y_axis(const QColor title, const QColor x_axis, const QColor y_axis);
	///Get the color of the title
	QColor get_color_title(void);
	///Get the color of the x-axis name
	QColor get_color_x_axis_name(void);
	///Get the color of the y-axis name
	QColor get_color_y_axis_name(void);


	///Get number additional tab-widgets (number of curves)
	int get_number_additional_tab(void);

	///Set curve setting to tab widgets
	void set_curve_setting2tabwidget(const string title);



private:
	//members
	///Form class made with the QT-designer for the layout of the dialog
	Ui::Sys_PlotSettings_Dia ui;


	//members
	///Font settings of the title
	QFont f_title;
	///Font settings of the x-axis
	QFont f_x_axis;
	///Font settings of the y-axis
	QFont f_y_axis;
	///Color settings of the title
	QColor c_title;
	///Color settings of the x-axis
	QColor c_x_axis;
	///Color settings of the y-axis
	QColor c_y_axis;

	///Number of additional tab-widgets
	int number_add_tab;

	//method





private slots:

	///Start font dialog of the title
	void start_font_dia_title(void);
	///Start font dialog of the x-axis
	void start_font_dia_x_axis(void);
	///Start font dialog of the y-axis
	void start_font_dia_y_axis(void);

	///Start color dialog of the title
	void start_color_dia_title(void);
	///Start color dialog of the x-axis
	void start_color_dia_x_axis(void);
	///Start color dialog of the y-axis
	void start_color_dia_y_axis(void);

};
#endif