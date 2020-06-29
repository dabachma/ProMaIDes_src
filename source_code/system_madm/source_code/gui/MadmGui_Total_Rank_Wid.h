#pragma once
/**\class MadmGui_Total_Rank_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef MADMGUI_TOTAL_RANK_WID_H
#define MADMGUI_TOTAL_RANK_WID_H


//Qt-libs
#include <QtGui>
#include <QWidget>
//forms
#include "ui_MadmGui_Total_Rank_Wid.h"

//system sys
#include "_Sys_Data_Wid.h"

///Widget-class for displaying more ranking with the diverse madm-approaches in a widget \ingroup madm
/**
Here the different rankings, each one is displayed with MadmGui_Rank_Wid, are displayed together in
one widget.
*/
class MadmGui_Total_Rank_Wid : public _Sys_Data_Wid, public Ui::MadmGui_Total_Rank_Wid
{

//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	MadmGui_Total_Rank_Wid(QWidget *parent = NULL);
	///Default destructor
	~MadmGui_Total_Rank_Wid(void);


	//methods
	///Set tooltip of the header
	void set_header_tooltip(const string tooltip, QTreeWidgetItem *item);
	///Set-up the widget
	void set_up_widget(QSqlDatabase *ptr_database, const int areastate, const int set_id, const _risk_type risktype); 

};
#endif
