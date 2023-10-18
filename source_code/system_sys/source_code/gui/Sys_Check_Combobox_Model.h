#pragma once
/** 
	\class Sys_Check_Combobox_Model
	\author Daniel Bachmann, Yann Labou et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/

#ifndef SYS_CHECK_COMBOBOX_MODEL_H
#define SYS_CHECK_COMBOBOX_MODEL_H


//qt-libs
#include <qstandarditemmodel.h>

///Class for an abstract interface for our model.  \ingroup sys
/**
This class provides an abstract interface for our model.
This model provides data to the view in the selection combobox.
\see Sys_CheckComboBoxView class
*/
class Sys_Check_Combobox_Model : public QStandardItemModel
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	Sys_Check_Combobox_Model(QObject* parent = NULL);
	///Default destructor
	~Sys_Check_Combobox_Model(void);


	//method
	///Get the flags of the model
	Qt::ItemFlags flags(const QModelIndex& index);


};
#endif
