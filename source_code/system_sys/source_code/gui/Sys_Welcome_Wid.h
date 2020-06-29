#pragma once
/**\class Sys_Welcome_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_WELCOME_WID_H
#define SYS_WELCOME_WID_H

//qt class
#include <QWidget>
#include <QGraphicsScene>

//forms
#include "ui_Sys_Welcome_Wid.h"

///Widget-class for the welcome widget \ingroup sys
/** This widget is launched at the beginning of the application.
It is closed automatically after 2.5 seconds. The time to closing is set in Main_Wid::Main_Wid(void).
The layout of the widget is generated via the QtDesigner. The included picture is in the .jpg-fromat.
*/
class Sys_Welcome_Wid : public QWidget
{

//Macro for using signals and slots (Qt)in this class	
Q_OBJECT

public:
	///Default constructor
	Sys_Welcome_Wid(QWidget *parent = 0);;
	//destructor
	~Sys_Welcome_Wid(void);

private:
	///Form class made with the QtDesigner for the layout of the dialog
	Ui::Sys_Welcome_Wid ui;

	///A graphic scene for loading the pixmap class
	QGraphicsScene my_scene;
	///Pixmap classs where the jpg is loaded
	QPixmap my_jpg;

	
private slots:
	///Change the progress text after few mseconds
	void set_new_txt(void);
	

};

#endif
