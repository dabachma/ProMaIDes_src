#pragma once
/**\class AltGui_Alternative_Info_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef ALTGUI_ALTERNATIVE_INFO_DIA_H
#define ALTGUI_ALTERNATIVE_INFO_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>

//forms
#include "ui_AltGui_Alternative_Info_Dia.h"
#include "_Sys_Common_System.h"


//alt sys
#include "AltGui_Alternative_Type_Dia.h"
#include "AltGui_Alternative_Base_On_Dia.h"

#include "AltGui_Dam_Resettlement_Dia.h"
#include "AltGui_Dam_Change_Function_Dia.h"


///Dialog-class for setting the general data of a specific alternative \ingroup alt
/**
*/
class AltGui_Alternative_Info_Dia : public QDialog, public Ui::AltGui_Alternative_Info_Dia
{

	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	AltGui_Alternative_Info_Dia(QWidget *parent = 0);
	///Default destructor
	~AltGui_Alternative_Info_Dia(void);

	//members
	///List of replacing/adding file names
	QStringList replacing_files;
	///Identifiers of replacing measure ids
	QList<int> replacing_ids;
	///Identifiers of current measure ids
	QList<int> current_ids;


	//method
	///Set for the dialog the measure identifier
	void set_dialog(const _sys_system_id id, const bool for_editing, const _alt_general_info data, const bool fpl_required);
	///Start the dialog it returns true by acception, false by rejection
	bool start_dialog(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Get the data of the measure
	_alt_general_info get_measure_general_data(void);

private:

	//members
	///Data of the measure
	_alt_general_info measure_data;
	///Flag if the dialog is just for editing the general measure data
	bool just_editing;



	///method


private slots:
	///Check the given name before
	void check_name(QString name);


	///Start the dialog for the detailed measure information
	void start_detailed_infodialog(void);

	///Start the dialog for the file information for measures
	bool start_measures_filedialog(QSqlDatabase *ptr_database);
};
#endif
