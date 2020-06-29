#pragma once
/**\class FplGui_Random_Check_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPLGUI_RANDOM_CHECK_DIA_H
#define FPLGUI_RANDOM_CHECK_DIA_H


//Qt-libs
#include <QtGui>
#include <QDialog>
//forms
#include "ui_FplGui_Random_Check_Dia.h"

//fpl-system
#include "_Fpl_Distribution_Class.h"

///Enumerator for the random generator, which are used for a probabilistic simulation \ingroup fpl
enum  _fpl_random_generator_type{
    ///Ranmar generator is used (_Fpl_Distribution_Class)
    gen_type_ranmar,
    ///Mother-of-all generator is used
    gen_type_MofAll,
    ///SFMT generator is used
    gen_type_sfmt,
    ///Zikurat generator is used
    gen_type_zikurat,
};
//enum  _fpl_distribution_type;


///Dialog-class for setting the parameter for a random generator and transformation check \ingroup fpl
/**

\see Fpl_Check_Distribution, Mc_Sim
*/
class FplGui_Random_Check_Dia : public QDialog, public Ui::FplGui_Random_Check_Dia
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	FplGui_Random_Check_Dia(QWidget *parent=NULL);
	///Default destructor
	~FplGui_Random_Check_Dia(void);


	//methods
	///Start the dialog; it returns true by acception, false by rejection
	bool start_dialog(void);
	///Get the number of runs
	int get_number_runs(void);
	///Get number of classes
	int get_number_classes(void);
	///Get random generator type
	_fpl_random_generator_type get_random_generator_type(void);
	///Get distribution class type
	_fpl_distribution_type get_distribution_class_type(void);
};
#endif
