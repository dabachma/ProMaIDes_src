#pragma once
#ifndef SYS_MULTI_FILECHOOSER_DIA_H
#define SYS_MULTI_FILECHOOSER_DIA_H

/**\class Sys_Multi_Filechooser_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

//QT libs
#include <QDialog>
#include <QFile>
#include <QStringList>
//forms
#include "ui_Sys_Multi_Filechooser_Dia.h"

//system sys
#include "Sys_One_Filechooser_Wid.h"
#include "Common_Const.h"

///Dialog-class for setting multiple inputs file, with text-comments for each file-browser \ingroup sys
/**
The number of file-browser can be set by the developer in the code. 
A extra text-label for each file-browser line is provided, e.g. for marking the type of file, which should be
read in. Each file-browser line is a Sys_One_Filechooser_Wid. A dynamical handling of the file-browser lines
is not possible. For this purpose use Sys_Multipathsettings_Dia.

\see Sys_One_Filechooser_Wid, Sys_Multipathsettings_Dia
*/
class Sys_Multi_Filechooser_Dia : public QDialog
{

//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	Sys_Multi_Filechooser_Dia(QWidget *parent = 0);
	///Default destructor
	~Sys_Multi_Filechooser_Dia(void);

	///Set the number of file-browser (Sys_One_Filechooser_Wid). They are also allocated and set to the dialog.
	void set_number_file_browser(const int number, QIcon icon);

	///Get the number of file-browser (Sys_One_Filechooser_Wid) used in the dialog
	int get_number_file_browser(void);

	///Get a pointer to the file-browser given by the index
	Sys_One_Filechooser_Wid* get_ptr_file_browser(const int index);

	///Get the text (path and filename) of the file-browser given by the index
	string get_file_path(const int index);

	///Set the text of the main text-label
	void set_main_text_label(const string text);
	///Set the window title
	void set_window_title(const string text);

	///Start the dialog
	bool start_dialog(void);


private:
	//members
	///Form class made with the QT-designer for the layout of the dialog
	Ui::Sys_Multi_Filechooser_Dia ui;

	///Pointer to the single file-browser lines
	Sys_One_Filechooser_Wid *file_browser;

	///Number of file-browser in the dialog
	int number_browser;

	///Layout for the line-browser
	QVBoxLayout browser_layout;


};
#endif
