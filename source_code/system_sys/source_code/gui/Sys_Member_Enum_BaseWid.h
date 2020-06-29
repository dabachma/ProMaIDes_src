#pragma once
/**\class Sys_Member_Enum_BaseWid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_MEMBER_ENUM_BASEWID_H
#define SYS_MEMBER_ENUM_BASEWID_H

//forms
#include "ui_Sys_Member_Enum_BaseWid.h"

//qt-libs
#include <QMenu>
#include <QPoint>
#include <QIcon>

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"

///Base-widget for displaying a enum variable with a text label \ingroup sys
/**

*/
class Sys_Member_Enum_BaseWid : public QWidget, public Ui::enumForm
{
	Q_OBJECT //Macro for using signals and slots (Qt)in this class

public:

	///Default Constructor
	Sys_Member_Enum_BaseWid(QWidget *parent = 0);
	///Default Destructor
	~Sys_Member_Enum_BaseWid(void);

	///Changes the text of the QLabel
	void set_label_text(const string);
	///Changes tooltip text
	void set_tooltip(const string);
	///Set the Combobox items
	void set_items(const string*, const int);
	// Set the QCombobox value by item string
	//void set_current_value(const string val);
	///Set the QCombobox value by index
	void set_current_value(const int);
	///Get the QCombobox value (item, string)
	string get_current_value(void);
	///Get the QCombobox value (index, int)
	int get_current_index(void);
	///Returns editable status
	bool is_editable(void);

private:

	///Saves the last active ComboBox Index
	int last_index;
	///Saves current Edit Status
	bool edit_flag;

public slots:

	///Sets last_index to current index and then current to new_index
	void on_comboBox_currentIndexChanged(int);
	///Change editable status
	void set_editable(bool);
};

#endif // SYS_MEMBER_ENUM_BASEWID_H
