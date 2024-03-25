#ifndef SYS_POSTGRES_CONFIG_DIA_H
#define SYS_POSTGRES_CONFIG_DIA_H

/**\class Sys_Postgres_Config_Dia
	\author Alaa Mroue
	\author produced by the Hochschule Magdeburg-Stendal
	\version #
	\date 2023
*/
//qt-libs
#include <QFile>
#include <QString>
#include <QSettings>
#include <QFileDialog>
#include <QDialog>
#include <QMessageBox>
//forms
#include "ui_Sys_Postgres_Config_Dia.h"

///Dialog-class for setting the database connection parameters \ingroup sys
/**

*/
class Sys_Postgres_Config_Dia : public QDialog
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT
public:
	///Default constructor
	Sys_Postgres_Config_Dia(QWidget* parent = 0);
	///Default destructor
	~Sys_Postgres_Config_Dia(void);

	//methods
	///Get postgres path
	QString postgres_path(void) const;
	///Get database folder
	QString database_folder(void) const;

private slots:
	///The controller select button is clicked
	void on_button_controller_clicked(void);
	///The database select button is clicked
	void on_button_database_clicked(void);
	///The dialog is accepted 
	void on_okButton_clicked(void);
	///The dialog is rejected
	void on_cancelButton_clicked(void);

private:
	//members
	///Form class made with the QT-designer for the layout of the dialog
	Ui::Sys_Postgres_Config_Dia ui;
};

#endif