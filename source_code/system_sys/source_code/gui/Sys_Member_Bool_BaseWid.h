#pragma once
/**\class Sys_Member_Bool_BaseWid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_MEMBER_BOOL_BASEWID_H
#define SYS_MEMBER_BOOL_BASEWID_H



//forms
#include "ui_Sys_Member_Bool_BaseWid.h"

//qt-libs
#include <QMenu>
#include <QPoint>
#include <QIcon>


//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"


///Base-widget for displaying a boolean variable with a text label \ingroup sys
/**

*/
class Sys_Member_Bool_BaseWid : public QWidget, public Ui::boolForm
{
	Q_OBJECT //Macro for using signals and slots (Qt)in this class

public:

	///Default constructor
	Sys_Member_Bool_BaseWid(QWidget *parent=NULL);
	///Default destructor
	~Sys_Member_Bool_BaseWid(void);
	
	//methods

	///Changes the text of the QLabel
	void set_label_text(const string text);
	///Changes tooltip text
	void set_tooltip(const string tooltip);
	///Set the QCheckbox value
	void set_value(const bool val);
	///Get the QCheckbox value
	bool get_value(void);
	///Returns editable status
	bool is_editable(void);

public slots:

	///Change Edit Status (Read Only)
	void set_editable(const bool state);
};

#endif // SYS_MEMBER_BOOL_BASEWID_H
