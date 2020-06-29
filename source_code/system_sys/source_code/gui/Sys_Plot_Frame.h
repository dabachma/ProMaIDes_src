#pragma once
/**\class Sys_Plot_Frame
	\author Yann Labou et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef SYS_PLOT_FRAME_H
#define SYS_PLOT_FRAME_H

//Qt Libs
#include <QtGui>
#include <QObject>

//Forms
#include "ui_Sys_Plot_Frame.h"

//System Libs
#include "Common_Const.h"
#include "Sys_Plot_Wid.h"


//Forward Declaration of class Sys_Plot_Wid because this class would be used but is not yet defined
//class Sys_Plot_Wid;

///Frame-class, where a plot-widget is included \ingroup sys
/**
The Sys_Plot_Frame class provides a dialog (frame) that allow users to plot curves and 
performs user-specific operations like saving, printing and export data in various 
formats, zooming or panning plots; show or hide selected curves.
Inherits QFrame.

	\see Sys_plot_Wid
*/
class Sys_Plot_Frame : public QFrame
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Constructor
	Sys_Plot_Frame(QWidget *parent = 0);
	///Default destructor
	~Sys_Plot_Frame(void);

	//method
	///Return pointer on the plot. This method provides an interface to access Sys_Plot_Wid functions
	Sys_Plot_Wid *getPlotPtr(void);
	///Set data to the model stored in the selection combobox. See Sys_CheckComboBoxModel class
	void setComboBoxData(int, bool);
	///Check whether the checkboxes in the ComboBox are checked or not
	bool isChecked(int);
	///Set the curve names in the combobox as default text
	void editComboBoxTextChanged(void);
	///Set title for the groupbox containing tool buttons and plot
	void set_groupbox_title(const string title);
	
	//members
	/**
	The Ui::Sys_Plot_Frame is an interface description object from the 
	ui_Sys_Plot_Frame.h file that sets up the Frame and the connections 
	between its signals and slots.
	*/
	Ui::Sys_Plot_Frame ui;

private:
	//members
	///Template that store every pairs (index, curve name) of the combobox
	QMap<int, QString> map;
	///Connect all the buttons in one method, that should be call once in the Sys_Plot_Frame constructor
	void connectButtons(void);

    //members
    ///Plot
    //Sys_Plot_Wid *my_plot;


private slots:
	///Ensure that Zoom Action is checked as soon as Zoom Button is being checked
	void checkActions(bool on);
	///Update the displayed text of the Combobox after every user selections
	void editComboBoxTextChanged(const QModelIndex & index);
	///Change the visible text after a item is just activated
	void change_visible_text(const QString );
};


#endif
