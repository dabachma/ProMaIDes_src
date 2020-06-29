#pragma once
/**\class HydGui_System_Filechooser_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYDGUI_SYSTEM_FILECHOOSER_DIA_H
#define HYDGUI_SYSTEM_FILECHOOSER_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>
#include <QFileDialog>
#include <QString>
#include <QAbstractItemDelegate>
//forms
#include "ui_HydGui_System_Filechooser_Dia.h"
//system sys
#include "Common_Const.h"
#include "Sys_Project.h"

///Dialog-class for choosing one global hydraulic system file (.ilm), which should be imported to the database \ingroup hyd
/**
*/
class HydGui_System_Filechooser_Dia : public QDialog
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	HydGui_System_Filechooser_Dia(QWidget *parent = 0);
	///Default destructor
	~HydGui_System_Filechooser_Dia(void);

	//methods

	///Get the file-path and the file-name, which is set in the dialog
	string get_file_name(void);
	///Start the dialog; it returns true by acception, false by rejection
	bool start_dialog(void);
	///Set a text to the text label of the dialog
	void set_txt(const string txt);



private:
	//members

	///File-path and file name
	string name;
	///Form class for the gui layout
	Ui::HydGui_System_Filechooser_Dia ui;

private slots:
	///Open the browse-dialog
	void open_browser(void);

};
#endif
