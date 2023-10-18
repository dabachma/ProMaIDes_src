#pragma once
/** 
	\class Sys_Check_Combobox_View
	\author Daniel Bachmann, Yann Labou et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/

#ifndef SYS_CHECK_COMBOBOX_VIEW_H
#define SYS_CHECK_COMBOBOX_VIEW_H


//qt-libs
#include <qlistview.h>

///Display class  \ingroup sys
/**
The Sys_CheckComboBoxView class allow us to display data provided by our Model.
\see Sys_Check_ComboBox_Model 
*/
class Sys_Check_Combobox_View : public QListView
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:

	///Default constructor
	Sys_Check_Combobox_View(QWidget* parent = NULL);
	///Default destructor
	~Sys_Check_Combobox_View(void);


	//method
	///Install an event filter on the selection combobox view to handle only clicks over the checkboxes in the combobox.
	bool eventFilter(QObject* , QEvent* event);



};
#endif

