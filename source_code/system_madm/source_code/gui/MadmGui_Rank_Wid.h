#pragma once
/**\class MadmGui_Rank_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef MADMGUI_RANK_WID_H
#define MADMGUI_RANK_WID_H


//Qt-libs
#include <QtGui>
#include <QWidget>
//forms
#include "ui_MadmGui_Rank_Wid.h"

//madm system
#include "Madm_Analysis.h" 

//system sys
#include "_Sys_Data_Wid.h"


///Widget-class for displaying one ranking with one madm-approach in a table-widget \ingroup madm
/**
This widget class uses a table-widget for displaying the rank of floodprotection 
measures implemented into the system. 
\see Madm_Analysis, _Madm_Solver
*/
class MadmGui_Rank_Wid : public _Sys_Data_Wid, public Ui::MadmGui_Rank_Wid
{

//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	MadmGui_Rank_Wid(QWidget *parent = NULL);
	///Default destructor
	~MadmGui_Rank_Wid(void);

	//method

	///Set-up the widget
	void set_up_widget(QSqlDatabase *ptr_database, const int areastate, const int set_id, const _risk_type risktype, const _madm_approach_type ana_type); 
};
#endif
