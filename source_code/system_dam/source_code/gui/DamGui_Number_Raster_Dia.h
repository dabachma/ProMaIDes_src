#pragma once
/**\class DamGui_Number_Raster_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef DAMGUI_NUMBER_RASTER_DIA_H
#define DAMGUI_NUMBER_RASTER_DIA_H

//qt libs
#include <QDialog>

//syslibs
#include "Common_Const.h"

//forms
#include "ui_DamGui_Number_Raster_Dia.h"


///Class for asking the number of damage raster, which should be imported \ingroup dam
/**
Thus, the import of the damage raster gets more comfortable, because of an complete automatical
import of the raster. The path of the part raster for each total raster are asked with Sys_Multi_Filechooser_Dia. 

\see Sys_Multi_Filechooser_Dia
*/
class DamGui_Number_Raster_Dia : public QDialog, public Ui::DamGui_Number_Raster_Dia
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	DamGui_Number_Raster_Dia( QWidget *parent = 0);
	///Default destructor
	~DamGui_Number_Raster_Dia(void);

	///Get the number of raster
	int get_number_raster(void);

	///Start the dialog
	bool start_dialog(void);

	///Set the icon of the dialog
	void set_icon(QIcon icon);

	///Set window title
	void change_window_title(string title, QIcon icon);
	///Set main text in the dialog
	void change_main_text(string txt);


private:

	///Number of raster
	int number_raster;
};
#endif
