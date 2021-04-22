#pragma once
#ifndef SYS_MULTIPATHSETTINGS_DIA_H
#define SYS_MULTIPATHSETTINGS_DIA_H

/**\class Sys_Multipathsettings_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

//QT libs
#include <QDialog>
#include <QTableWidget>
#include <QFile>
#include <QStringList>
//forms
#include "ui_Sys_Multipathsettings_Dia.h"

//system sys
#include "Sys_Filechooser_Dia.h"
#include "Common_Const.h"

///Dialog-class for setting multiple inputs file, without any text-comment for each file-browser \ingroup sys
/**
The number of file-browser can be dynamically set by the user via a spin-box. 
The file-browser lines are managed in a list-tree. No extra text-label for each line is provided.
The list-tree is filled with Sys_Filechooser_Dia widgets via the spin-box. The lines of file-browser 
can also be deleted by by using the spin-box.

\see  Sys_Filechooser_Dia, Sys_Multi_Filechooser_Dia
*/
class Sys_Multipathsettings_Dia : public QDialog
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT


public:
	///Default constructor
	Sys_Multipathsettings_Dia(QWidget *parent = 0);
	///Default destructor
	~Sys_Multipathsettings_Dia(void);

	//methods
	///Returns all the selected files
	QStringList	getFileslist(void);
	///Get the filename for the given index
	string get_filename(const int index);
	///Returns the number of selected files
	int getFilesnumber(void);
	///Save all the selected files in a single logfile
	void saveAsFileslist(void);
	///Set a text to header text label
	void set_txt2headerlabel(const string txt, QIcon icon);

	///Dialog for the paths and filenames
	bool asked_path_filenames(void);

	///Set a widget to the layout for alraedy existing scenario
	void set_widget2existing_scenario_layout(QWidget *scenario_wid);

	///Set file filters
	void set_file_filters(QString filter);

private:
	//members
	///Form class made with the QT-designer for the layout of the dialog
	Ui::Sys_Multipathsettings_Dia ui;
	///List of the selected files
	QStringList file_list;
	///Number of files in list
	int number_files;
	///Old spin box value
	int old_spinbox_value;

	///Filter for the files
	QString filter;

	//method
	///Set the FileChooser widget for every rows
	void setRowItem(const bool add);




private slots:
	///The spin box value is changed
	void spinboxValueChanged(const int number);

	///Set the selected files into a list
	void set_files2list(void);
};

#endif