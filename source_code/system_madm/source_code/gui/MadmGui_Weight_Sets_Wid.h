#pragma once
/**\class MadmGui_Weight_Sets_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef MADMGUI_WEIGHT_SETS_WID_H
#define MADMGUI_WEIGHT_SETS_WID_H


//Qt-libs
#include <QtGui>
#include <QWidget>
//forms
#include "ui_MadmGui_Weight_Sets_Wid.h"

//system sys
#include "_Sys_Data_Wid.h"

///Widget-class for displaying the existings madm-weighting sets in a table widget \ingroup madm
/**

\see Madm_System
*/
class MadmGui_Weight_Sets_Wid : public _Sys_Data_Wid, public Ui::MadmGui_Weight_Sets_Wid
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	MadmGui_Weight_Sets_Wid(QWidget *parent = NULL);
	///Default destructor
	~MadmGui_Weight_Sets_Wid(void);

	//method
	///Set-up the widget
	void set_up_widget(QSqlDatabase *ptr_database);
};
#endif
