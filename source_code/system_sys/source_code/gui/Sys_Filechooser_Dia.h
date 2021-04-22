#ifndef SYS_FILECHOOSER_DIA_H
#define SYS_FILECHOOSER_DIA_H
/**\class Sys_Filechooser_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

//Qt-libs
#include <QtGui>
#include <QDialog>
#include <QFileDialog>
#include <QString>
#include <QAbstractItemDelegate>
//forms
#include "ui_Sys_Filechooser_Dia.h"
//system sys
#include "Common_Const.h"

///This class creates a lineEdit and pushButton for every row of the multi path dialog (Sys_Multipathsettings_Dia) \ingroup sys
class Sys_Filechooser_Dia : public QWidget
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

//friend class
friend class Sys_Multipathsettings_Dia;

public:
	///Default constructor
	Sys_Filechooser_Dia(QWidget *parent = 0);
	///Default destructor
	~Sys_Filechooser_Dia(void);

	//methods
	///Return the text (filename+path) displayed in the lineEdit of the FileChooser widget
	QString fileName(void);

	///Set file filters
	void set_file_filters(QString filter);


private:
	//members
	///Form class made with the Qt Designer
	Ui::Sys_Filechooser_Dia uiFileChooser;

	///Filter for the files
	QString filter;


private slots:
	///Slot for the pushButton of the FileChooser; it allows the user to browse a file on the
	void chooseFile(void);
};
#endif