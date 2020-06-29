#pragma once
/**\class Sys_Delete_Logfile_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_DELETE_LOGFILE_DIA_H
#define SYS_DELETE_LOGFILE_DIA_H
// Qt Libs
#include <QtGui>
#include <QDialog>
#include <QDirModel>


//forms
#include "ui_Sys_Delete_Logfile_Dia.h"
//system sys
#include "Common_Const.h"

///Dialog-class for asking the deleting of logfiles in the logfile-archiv \ingroup sys
/**


*/
class Sys_Delete_Logfile_Dia : public QDialog, public Ui::Sys_Delete_Logfile_Dia
{

//Macro for using signals and slots (Qt)in this class
Q_OBJECT 

public:
	///Default constructor
	Sys_Delete_Logfile_Dia(QWidget *parent = NULL);
	///Default destructor
	~Sys_Delete_Logfile_Dia(void);


	//method

	///Set up the dialog
	void set_dialog(QDirModel *archiv, QDir *path);
	///Start the dialog; it returns true by acception, false by rejection
	bool start_dialog(void);
	///Get the model of the tree view
	QTreeView *get_model(void);

private slots:
	///Select all entries
	void select_all_entries(bool flag);


};
#endif
