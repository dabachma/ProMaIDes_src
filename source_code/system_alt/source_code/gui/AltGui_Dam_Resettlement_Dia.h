#pragma once
/**\class AltGui_Dam_Resettlement_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef ALTGUI_DAM_RESETTLEMENT_DIA_H
#define ALTGUI_DAM_RESETTLEMENT_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>

//forms
#include "ui_AltGui_Dam_Resettlement_Dia.h"

///Dialog-class for changing the elements in the given polygon(s) by resetting the economic values and the people \ingroup alt
/**
*/
class AltGui_Dam_Resettlement_Dia : public QDialog, public Ui::AltGui_Dam_Resettlement_Dia
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	AltGui_Dam_Resettlement_Dia(QWidget *parent = 0);
	///Default destructor
	~AltGui_Dam_Resettlement_Dia(void);


	//method
	///Set for the dialog the function/categories, which should be switched
	void set_dialog(QSqlDatabase *database);
	
	///Get the selected informations 
	void get_selected_information(QStringList *file_list, QList<int> *id_list);

	///Start the dialog it returns true by acception, false by rejection
	bool start_dialog(void);
};
#endif
