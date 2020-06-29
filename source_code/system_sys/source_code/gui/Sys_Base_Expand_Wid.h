#pragma once
/**\class Sys_Base_Expand_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_BASE_EXPAND_WID
#define SYS_BASE_EXPAND_WID

//forms
#include "ui_Sys_Base_Expand_Wid.h"

//qt-libs
#include <QWidget>

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"

///Base-widget for displaying widgets in an expandable container widget \ingroup sys
/**

*/
class Sys_Base_Expand_Wid : public QWidget, public Ui::Sys_Base_Expand
{
	Q_OBJECT //Qt-macro

public:

	///Default constructor
	Sys_Base_Expand_Wid(QWidget *parent = 0);
	///Default destructor
	~Sys_Base_Expand_Wid(void);

	//methods

	///Sets the text of the label in the head_layout 
	void set_title(const string text);
	///Sets the text of the label in the head_layout 
	string get_title(void);
	///Adds a child widget to the container layout
	void add_child(QWidget *child);
    ///Adds a child layout to the container layout
    //void add_child(QLayout *child);
	///Removes the child widget
	void remove_child(QWidget *child);
	///Sets the frame inside the container widget visible or not visible
	void set_frame_visible(const bool state);
	///Sets the head widget visible or not visible
	void set_head_widget_visible(const bool state);
	///Sets the head widgets icon
	void set_head_pixmap(const QPixmap pic);

public slots:

	///Sets the container widget visible
	void set_expanded(const bool state);
	///Sets the container widget editable
	void set_editable(const bool state);

private:

	//methods

	///Sets up class members and atrributes
	void setupRest(void);

};

#endif //SYS_BASE_EXPAND_WID
