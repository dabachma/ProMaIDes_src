#pragma once
/**\class Sys_Member_Dbl_Prec_BaseWid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_MEMBER_DBL_PREC_BASEWID_H
#define SYS_MEMBER_DBL_PREC_BASEWID_H

//forms
#include "ui_Sys_Member_Dbl_Prec_BaseDia.h"

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"

///Set precision dialog for the doubleSpinBox in table-widgets \ingroup sys
/**

*/
class Sys_Member_Dbl_Prec_BaseWid : public QDialog, public Ui::prec_dia 
{
	Q_OBJECT //Macro for using signals and slots (Qt)in this class

public:

	///Default Constructor
	Sys_Member_Dbl_Prec_BaseWid(int currPrec, QWidget *parent = 0);
	///Default Destructor
	~Sys_Member_Dbl_Prec_BaseWid(void);
	
private:

	///Pixmap used in the Dialog
	QPixmap icon;

signals:

	///Emitted when the Apply Button is clicked
	void precApplied(int);

public slots:

	///Gains the current SpinBox value and gives it to precApplied signal
	void apply_clicked();
};

#endif // SYS_MEMBER_DBL_PREC_BASEWID_H
