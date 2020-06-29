#pragma once
/**\class Sys_DoubleSpinBox_Delegate
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_DOUBLESPINBOX_DELEGATE_H
#define SYS_DOUBLESPINBOX_DELEGATE_H

//qt-libs
#include <QtGui>
#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QDoubleSpinBox>

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"

///SpinBox (double) as a item for a table-widget \ingroup sys
/**

*/
class Sys_DoubleSpinBox_Delegate : public QItemDelegate
{
	Q_OBJECT //Macro for using signals and slots (Qt)in this class

public:

	///Default constructor
	Sys_DoubleSpinBox_Delegate(QObject *parent = 0);
	///Default destructor
	~Sys_DoubleSpinBox_Delegate(void);

	//methods

	///Create the editor widget
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	///Set the editors data
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
	///Set the models data
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	///Update the editors geometry
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const;
	///Set the spefic values for a spin-box
	void set_spin_box_values(const double max, const double min, const double interval, const int precision);

private:

	//members

	///Maximum value of the spinbox
	double max_value;
	///Minimum value of the spinbox
	double min_value;
	///Minimum value of the spinbox
	double interval_value;
	///Precision of the spinbox
	int precison;
};

#endif //SYS_DOUBLESPINBOX_DELEGATE_H
