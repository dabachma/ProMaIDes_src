#pragma once
#ifndef HYDGUI_PROFIL2DGM_CONVERTER_DIA_H
#define HYDGUI_PROFIL2DGM_CONVERTER_DIA_H

/**\class HydGui_Profil2Dgm_Converter_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2013 
*/

//QT libs
#include <QDialog>
#include <QFile>
#include <QStringList>
//forms
#include "ui_HydGui_Profil2Dgm_Converter_Dia.h"

//system sys
#include "Sys_One_Filechooser_Wid.h"
#include "Common_Const.h"

///Dialog-class for the input data for a conversion of profile data to river DGM-W data \ingroup sys
/**
This dialog class reads in: two streamline representing the left and right river bank, a file of profile data,
the number of profiles in file and the number of extra stream lines for interpolation.

\see Sys_One_Filechooser_Wid
*/


class HydGui_Profil2Dgm_Converter_Dia : public QDialog
{

//Macro for using signals and slots (Qt)in this class
Q_OBJECT


public:
	///Default constructor
	HydGui_Profil2Dgm_Converter_Dia(QWidget *parent = 0);
	///Default destructor
	~HydGui_Profil2Dgm_Converter_Dia(void);

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

	///Get the number of additional streamlines
	int get_number_additional_streamlines(void);
	///Get the number of profiles in file
	int get_number_profiles_in_file(void);
	///Get density along stream lines
	double get_dens_streamline(void);
	///Get offset for x-coordinate
	double get_offset_x(void);
	///Get offset for y-coordinate
	double get_offset_y(void);

	///Start the dialog
	bool start_dialog(void);


private:
	//members
	///Form class made with the QT-designer for the layout of the dialog
	Ui::HydGui_Profil2Dgm_Converter_Dia ui;

	///Pointer to the single file-browser lines
	Sys_One_Filechooser_Wid *file_browser;

	///Number of file-browser in the dialog
	int number_browser;

	///Layout for the line-browser
	QVBoxLayout browser_layout;

};
#endif

