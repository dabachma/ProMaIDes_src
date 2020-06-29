#pragma once
/**\class DamGui_Raster_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/
#ifndef DAMGUI_RASTER_DIA_H
#define DAMGUI_RASTER_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>
//forms
#include "ui_DamGui_Raster_Dia.h"

//dam-system
#include "Dam_Ecn_Raster.h"
#include "Dam_Eco_Btype_Raster.h"
#include "Dam_Eco_Soil_Raster.h"
#include "Dam_People_Raster.h"
#include "Dam_Pys_Raster.h"


///Dialog-class for choosing damage raster, which should be handled \ingroup dam
/**
*/
class DamGui_Raster_Dia : public QDialog, public Ui::DamGui_Raster_Dia
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:



	///Default constructor
	DamGui_Raster_Dia(QWidget *parent = NULL);
	///Default destructor
	~DamGui_Raster_Dia(void);

	//methods
	///Start the dialog and set the current available economical raster as a selection; it returns true by acception, false by rejection
	bool start_dialog(Dam_Ecn_Raster *raster, const int number);
	///Start the dialog and set the current available ecological biotope raster as a selection; it returns true by acception, false by rejection
	bool start_dialog(Dam_Eco_Btype_Raster *raster, const int number);
	///Start the dialog and set the current available ecological soil erosion raster as a selection; it returns true by acception, false by rejection
	bool start_dialog(Dam_Eco_Soil_Raster *raster, const int number);
	///Start the dialog and set the current available people2risk raster as a selection; it returns true by acception, false by rejection
	bool start_dialog(Dam_People_Raster *raster, const int number);
	///Start the dialog and set the current available psycho-social raster as a selection; it returns true by acception, false by rejection
	bool start_dialog(Dam_Pys_Raster *raster, const int number);

	///Set a text to the text label of the dialog
	void set_txt(const string txt);


public slots:
	///Switch the selection between all and clear selection
	void all_selection(bool flag);


private:

	//members

	///The items of the list widget
	QListWidgetItem *items;

	///Header label
	string header_txt;

	//methods

	///Allocate the items of the list widget
	void allocate_list_item(const int number);




};
#endif
