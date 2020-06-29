#pragma once
/**\class AltGui_Measure_Replacing_File_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef ALTGUI_MEASURE_REPLACING_FILE_DIA_H
#define ALTGUI_MEASURE_REPLACING_FILE_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>
//forms
#include "ui_AltGui_Measure_Replacing_File_Dia.h"

//system alt
#include "AltGui_File_Replacing_Wid.h"
#include "AltGui_Alternative_Info_Dia.h"

//system hyd
#include "Hyd_Boundary_Szenario_Management.h"
//system fpl
#include "Fpl_Section.h"

///Dialog-class for setting the file(s), which stored information replaces specific system characteristics as measure  \ingroup alt
/**
*/
class AltGui_Measure_Replacing_File_Dia : public QDialog, public Ui::AltGui_Measure_Replacing_File_Dia
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	AltGui_Measure_Replacing_File_Dia(QWidget *parent = 0);
	///Default destructor
	~AltGui_Measure_Replacing_File_Dia(void);

	///Set for the dialog the measure identifier
	void set_dialog(const _alt_general_info data);
	///Start the dialog it returns true by acception, false by rejection
	bool start_dialog(void);

	///Get the selected informations 
	void get_selected_information(QStringList *file_list, QList<int> *id_list);


	///Set the pointer to the database
	void set_ptr_database(QSqlDatabase *database);


private slots:
	///Check the value of the spinbox
	void recieve_number_spin_box(int number);
	
private:

	//members

	///String list for the combo box
	QStringList combo_list;

	///Tooltip for the combo box
	QString tooltip_combo;

	///List of the id's, which will be replaced
	QList<int> id_list;

	///Pointer to the database
	QSqlDatabase *ptr_database;
	///System identifier
	_sys_system_id system_id;

	///Data of the measure, which should be set
	_alt_general_info measure_info;

	///Old spin box value
	int old_spinbox_value;

	//method
	///Set the measure specific data for the dialog
	void set_measure_type2dia(void);

	///Set the widget for every rows
	void set_row_item(const bool add);

	///Set the items of the combo box after the generated string list
	void set_combox_items(const int index);

	///Set the fpl-sections to the combo-box list
	void set_fpl_section2list(_sys_system_id id);

	///Set the hyd-2d-dikeline to the combo-box list
	void set_hyd_dikeline2list(const _sys_system_id id);

	///Set the hyd-river models to the combo-box list
	void set_hyd_rv_model2list(const _sys_system_id id);


	///Set error(s)
	Error set_error(const int err_type);

};

#endif
