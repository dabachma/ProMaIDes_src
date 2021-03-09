#pragma once
/**\class HydGui_Boundary_Scenario_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPLGUI_SECTION_DIA_H
#define FPLGUI_SECTION_DIA_H


//Qt-libs
#include <QtGui>
#include <QDialog>
//forms
#include "ui_FplGui_Section_Dia.h"

//fpl-system
#include "Fpl_Section.h"

///Enumerator to distinguish the type of the thread, e.g. peforming a determistic calculation or a fragility curve calculation \ingroup fpl
enum _fpl_thread_type{

    ///Thread for importing fpl-section(s) from file into database
    fpl_import_file,

    ///Thread for performing a deterministc calculation of selected sections
    fpl_determ_selected,

    ///Thread for performing a monte-carlo calculation of selected sections
    fpl_mc_selected,

    ///Thread for performing a fragility curve calculation of selected sections
    fpl_frc_selected,

    ///Thread for testing the random-generator and the distribution,
    fpl_test_random,

    ///Thread for setting the frc by the user
    fpl_frc_by_hand,

    ///Thread is creating the fpl tables for a database project
    fpl_create_tab,
    ///Thread is checking the fpl tables of a database project
    fpl_check_tab,

    ///Thread is checking fpl-section(s) per file
    fpl_check_sec_file,
    ///Thread is checking fpl-section(s) per database
    fpl_check_sec_db,

	///Thread is exporting determinstic results of a FPL-section
	fpl_export_determ_sec,
	///Thread is exporting MC-results of a FPL-section
	fpl_export_mc_sec,
	///Thread is exporting FRC-results of a FPL-section
	fpl_export_frc_sec,

    ///Thread is combining the hydraulic system with the fpl-system
    fpl_combine_fpl2hyd,
    ///Thread is checking the idealization of the hydraulic system by the fpl-system
    fpl_check_ideal_fpl2hyd,

    ///Thread is deleting selected fpl section and their appending tables
    fpl_del_section,
    ///Thread is restoring default values in the tables
    fpl_restore_default,

    ///Thread for a unknown action
    fpl_thread_unknown


};

///Dialog-class for choosing multiple fpl-sections \ingroup fpl
/**
This dialog class uses a combo-box for the selction of different fpl-section. 
\see Fpl_Section
*/
class FplGui_Section_Dia : public QDialog, public Ui::FplGui_Section_Dia
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	FplGui_Section_Dia(QWidget *parent = NULL);
	///Default destructor
	~FplGui_Section_Dia(void);

	///Set the dialog, like the database, system_id etc.
	void set_dialog(QSqlDatabase *ptr_database, _sys_system_id sys_id,  const _fpl_thread_type type);
	///Start the dialog and set the current available sections as a selection; it returns true by acception, false by rejection
	bool start_dialog(int  **list_section, int *number);
	///Set a text to the text label of the dialog
	void set_txt(const string txt);
	///Set the dialog header text
	void set_dia_header_txt(const string header_title);

public slots:
	///Switch the selection between all and clear selection
	void all_selection(bool flag);

private:

	//members

	///The items of the list widget
	QListWidgetItem *items;

	//methods

	///Allocate the items of the list widget
	void allocate_list_item(const int number);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
