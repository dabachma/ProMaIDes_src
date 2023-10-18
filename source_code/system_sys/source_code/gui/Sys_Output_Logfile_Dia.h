#ifndef SYS_OUTPUT_LOGFILE_DIA_H
#define SYS_OUTPUT_LOGFILE_DIA_H

/**\class Sys_Output_Logfile_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

//Qt libs
#include <QDialog>
#include <QAbstractButton>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QDir>
//forms
#include "ui_Sys_Output_Logfile_Dia.h"
//system sys
#include "Common_Const.h"

///Dialog-class for asking the user the new logfile for the output for the different modules \ingroup sys
class Sys_Output_Logfile_Dia : public QDialog
{
//Macro for using signals and slots (Qt)in this class	
Q_OBJECT

public:
	///Default constructor
	Sys_Output_Logfile_Dia(QWidget *parent = 0);
	///Default destructor
	~Sys_Output_Logfile_Dia(void);


	//methods

	///Set the text for which moduls the outputflag should be changed
	void set_txt_modul_type(_sys_system_modules type);

	///Set a new output file name via dialog; if it is true the old file can be closed and a new one can be opened
	bool setOutputLogfile(string *_new_filename, const string old_file_name);

private:
	//members
	///Text to be displayed as Output type
	QString text;
	///Type of module for which the logfile should be changed
	_sys_system_modules modul_type;
	///String for the old path
	string old_path;

	///Form class made with the QT-designer for the layout of the dialog
	Ui::Sys_Output_Logfile_Dia ui;

	//method
	///Set the current output path
	void setOutputPath(QString outputpath);
	///Get the output type of the logfile
	QString getOutputType(void);
	///Get the name of the logfile
	QString getOutputLogfile(void);
	///Get the path where the logfile should be later saved
	QString getOutputPath(void);

private slots:
	///Open the dialog to select the directory
	void browseButtonClicked(void);


};
#endif