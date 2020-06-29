#ifndef SYS_DATABASE_DIA_H
#define SYS_DATABASE_DIA_H

/**\class Sys_Database_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
//qt-libs
#include <QDialog>
#include <QMessageBox>
//forms
#include "ui_Sys_Database_Dia.h"

//forward declaration
struct _sys_database_params;

///Dialog-class for setting the database connection parameters \ingroup sys
/**

*/
class Sys_Database_Dia: public QDialog
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT
public:
	///Default constructor
    Sys_Database_Dia(QWidget *parent = 0);
	///Default destructor
    ~Sys_Database_Dia(void);

	//methods
	///Get the host name
    QString host_name(void) const;
	///Get the database name
	QString database_name(void) const;
	///Get the user name
    QString user_name(void) const;
	///Get the password
    QString password(void) const;
	///Get the password save flag
	bool password_save_flag(void) const;
	///Get the directory of the table file
	QString table_dir(void) const;
	///Get the driver type as string
	QString get_driver(void) const;

	///Hide the browser for the directory of the table file
	void hide_table_file_browser(void);
	///Set predefined parameters to the dialog
	void set_predef_parameter2dialog(_sys_database_params params);

	///Enable all just the password dialog and the password save flag
	void enable_for_password(void);
    
private slots:
	///The dialog is accepted 
	void on_okButton_clicked(void);
	///The dialog is rejected
    void on_cancelButton_clicked(void);
	///The directory is browsed via a file-dialog
	void on_browseTableDirButton_clicked(void);

private:

	//members
	///Form class made with the QT-designer for the layout of the dialog
	Ui::Sys_Database_Dia ui;
};

#endif