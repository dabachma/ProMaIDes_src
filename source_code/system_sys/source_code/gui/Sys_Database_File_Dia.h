#pragma once
/**\class Sys_Database_File_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_DATABASE_FILE_DIA_H
#define SYS_DATABASE_FILE_DIA_H


//qt classes
#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "ui_Sys_Database_File_Dia.h"

//system classes
#include "Data_Base.h"

///Dialog-class for asking for the files needed for set-up a database-connection \ingroup sys
/**
This files are the connection-file with the connection parameters and
the tables-file, where the tables of the database are defined.
This class is not more required since the project management is implemented.
\see Sys_Project
*/
class Sys_Database_File_Dia: public QDialog
{
	//Macro for using signals and slots (Qt)in this class
    Q_OBJECT

public:
	///Default constructor
    Sys_Database_File_Dia(QWidget *parent = 0);
	///Default destructor
    ~Sys_Database_File_Dia(void);


	///Get the path and name of the file for the connection parameters
	QString db_file_dir(void) const;
	///Get the path and name of the file, where the tables are specified
	QString table_file_dir(void) const;

private slots:
	///The ok-button is clicked, it means accepted
    void on_okButton_clicked(void);
	///The cancel-button is clicked, it means rejected
    void on_cancelButton_clicked(void);
	///Activate the file-browser for the connection file
	void on_browseDbFileButton_clicked(void);
	///Activate the file-browser for the tables file
	void on_browseTableFileButton_clicked(void);

private:

	///Form-class made with the Qt-designer for this dialog
	Ui::Sys_Database_File_Dia ui;

};
#endif