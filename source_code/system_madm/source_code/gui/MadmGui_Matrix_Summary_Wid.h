#pragma once
/**\class MadmGui_Matrix_Summary_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef MADMGUI_MATRIX_SUMMARY_WID_H
#define MADMGUI_MATRIX_SUMMARY_WID_H


//Qt-libs
#include <QtGui>

//system sys
#include "_Sys_Result_Summary_Wid.h"

//system risk
#include "Risk_System.h"



///Widget-class for displaying the madm-matrix in a table widget as result \ingroup madm
/**

*/
class MadmGui_Matrix_Summary_Wid : public _Sys_Result_Summary_Wid
{

	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	MadmGui_Matrix_Summary_Wid(QWidget *parent = 0);
	///Default destructor
	~MadmGui_Matrix_Summary_Wid(void);

	//method
	///Set-up the widget
	void set_up_widget(QSqlDatabase *ptr_database, const  _sys_system_id id);

	///Set the risk type
	void set_risk_type(const _risk_type type);


private:
	//member
	///Risk type for the results
	_risk_type risk_type;
};
#endif
