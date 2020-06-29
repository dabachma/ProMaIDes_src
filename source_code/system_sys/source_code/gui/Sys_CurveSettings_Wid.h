#pragma once
/** 
	\class Sys_Plot_Wid
	\author Daniel Bachmann, Yann Labou et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2012
*/

#ifndef SYS_CURVESETTINGS_WID_H
#define SYS_CURVESETTINGS_WID_H

//Qt Libs
#include <QtGui>
#include <QObject>
#include <QWidget>
#include <qwt_text.h>
#include <qwt_symbol.h>

//forms
#include "ui_Sys_CurveSettings_Wid.h"
//system sys
#include "Common_Const.h"

///Widget-class for editing the curves of a plot widget \ingroup sys
/**

*/
class Sys_CurveSettings_Wid : public QWidget
{

	Q_OBJECT

public:

	///Default constructor
	Sys_CurveSettings_Wid(QWidget *parent = NULL);
	///Default destructor
	~Sys_CurveSettings_Wid(void);

	//members
	///Form class made with the QT-designer for the layout of the dialog
	Ui::Sys_CurveSettings_Wid ui;
	
	//methods
	///Get the title of the curve
	QwtText getCurveTitle(void);
	///Get the style of the curve. Possible curve styles are Lines, Sticks, Steps and Dots. Default is set to Lines.
	Qt::PenStyle getCurveStyle(void);
	///Get the style of the symbol. Possible symbol styles are Ellipse, Rect, Diamond, Triangle, Cross, XCross, HLine, VLine, Star1, Star2 and Hexagon. Default is set to Rect.
	QwtSymbol::Style getSymbolStyle(void);
	///Get the line width of the curve
	int getCurveWidth(void);
	///Get the symbol size
	int getSymbolSize(void);
	///Get the curve color
	QColor getCurveColor(void);
	///Get the symbol color
	QColor getSymbolColor(void);

	///Set the curve color
	void set_curve_col(QColor col);
	///Set the symbol color
	void set_sym_col(QColor col);


private:
	//members
	///List for the curve style
	QStringList curveStyleList;
	///List for the symbol style
	QStringList symbolStyleList;
	///The symbol of the curve
	QwtSymbol curveSymbol;

	///Color of the curve
	QColor curveColor;
	///Color of the symbol
	QColor symbolColor;

private slots:
	///Button for changing the curve color is pushed
	void on_curveColorPushButton_clicked(void);
	///Button for changing the symbol color is pushed
	void on_symbolColorPushButton_clicked(void);
};
#endif
