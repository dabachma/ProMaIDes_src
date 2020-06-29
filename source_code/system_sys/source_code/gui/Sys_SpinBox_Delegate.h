#pragma once
/**\class Sys_SpinBox_Delegate
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef SYS_SPINBOX_DELEGATE_H
#define SYS_SPINBOX_DELEGATE_H

//qt-libs
#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QSpinBox>
#include <QtGui>

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"

///SpinBox (int) as a item for a table-widget \ingroup sys
/**

*/
class Sys_SpinBox_Delegate : public QItemDelegate
{
    Q_OBJECT //Macro for using signals and slots (Qt)in this class

public:

	///Default constructor
    Sys_SpinBox_Delegate(QObject *parent = 0);
	///Default destructor
	~Sys_SpinBox_Delegate(void);

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
	void set_spin_box_values(const int max, const int min, const int interval);

private:

	//members

	///Maximum value of the spinbox
	int max_value;
	///Minimum value of the spinbox
	int min_value;
	///Minimum value of the spinbox
	int interval_value;
};

#endif //SYS_SPINBOX_DELEGATE_H
