#pragma once
/**\class AltGui_Dam_Change_Function_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef ALTGUI_DAM_CHANGE_FUNCTION_DIA_H
#define ALTGUI_DAM_CHANGE_FUNCTION_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>
//forms
#include "ui_AltGui_Dam_Change_Function_Dia.h"
//alt-system
#include "AltGui_Id_Replacing_Wid.h"


///Dialog-class for changing the damage function (ECN or ECO) or categories (POP or PYS) as measure  \ingroup alt
/**
*/
class AltGui_Dam_Change_Function_Dia : public QDialog, public Ui::AltGui_Dam_Change_Function_Dia
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	AltGui_Dam_Change_Function_Dia(QWidget *parent = 0);
	///Default destructor
	~AltGui_Dam_Change_Function_Dia(void);


	//method
	///Set for the dialog the function/categories, which should be switched
	bool set_dialog(QSqlDatabase *database, const string detailed_identifier);

	///Get the selected informations 
	void get_selected_information(QStringList *file_list, QList<int> *id_list_current, QList<int> *id_list_new);


	///Start the dialog it returns true by acception, false by rejection
	bool start_dialog(void);

private slots:
	///Check the value of the spinbox
	void recieve_number_spin_box(int number);


private:


	//members
	///Old spin box value
	int old_spinbox_value;

	///List of strings for the current indices
	QStringList current_list;


	//methods
	///Set the ECN damage function to the combo box
	void set_ecn_damage_function2combobox(QSqlDatabase *database);
	///Add new ECN damage function to the database
	void add_ecn_damage_function2database(QSqlDatabase *database);
	///Set the ECO biotope-type function to the combo box
	void set_eco_btype_function2combobox(QSqlDatabase *database);
	///Add new ECO biotope-type functionn to the database
	void add_eco_btype_function2database(QSqlDatabase *database);
	///Set the ECO soil-type function to the combo box
	void set_eco_soiltype_function2combobox(QSqlDatabase *database);
	///Add new ECO soil-type-type functionn to the database
	void add_eco_soiltype_function2database(QSqlDatabase *database);
	///Set the POP categories to the combo box
	void set_pop_categories2combobox(QSqlDatabase *database);
	///Add new POP categories to the database
	void add_pop_categories2database(QSqlDatabase *database);
	///Set the PYS categories to the combo box
	void set_pys_categories2combobox(QSqlDatabase *database);
	///Add new PYS categories to the database
	void add_pys_categories2database(QSqlDatabase *database);

	///Set the widget for every rows
	void set_row_item(const bool add);

	///Set the items of the combo box after the generated string list
	void set_combox_items(const int index);


};

#endif