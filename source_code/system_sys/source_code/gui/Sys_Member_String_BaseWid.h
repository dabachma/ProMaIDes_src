#pragma once
/**\class Sys_Member_String_BaseWid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef SYS_MEMBER_STRING_BASEWID_H
#define SYS_MEMBER_STRING_BASEWID_H

//forms
#include "ui_Sys_Member_String_BaseWid.h"

//qt-libs
#include <QMenu>
#include <QPoint>
#include <QIcon>

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"


///Base-widget for displaying an string variable with a text label \ingroup sys
/**

*/
class Sys_Member_String_BaseWid : public QWidget, public Ui::stringForm
{
	Q_OBJECT //Macro for using signals and slots (Qt)in this class

public:

	///Default Constructor
	Sys_Member_String_BaseWid(QWidget *parent = 0);
	///Default Destructor
	~Sys_Member_String_BaseWid(void);

	///Changes the text of the QLabel
	void set_label_text(const string text);
	///Changes tooltip text
	void set_tooltip(const string tooltip);
	///Set the QLineEdit value
	void set_text(const string text);
	///Get the QLineEdit value
	string get_text(void);
	///Returns editable status
	bool is_editable(void);

public slots:

	///Change editable status
	void set_editable(bool edit);
};

#endif // SYS_MEMBER_STRING_BASEWID_H
