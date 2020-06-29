#pragma once
/** 
	\class Sys_Plot_Wid
	\author Daniel Bachmann, Yann Labou et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011
*/

#ifndef SYS_CURVESETTINGS_WID_H
#define SYS_CURVESETTINGS_WID_H

//Qt Libs
#include <QtGui>
#include <QObject>
#include <QWidget>

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
};
#endif
