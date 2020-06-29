#pragma once
/**\class Sys_ComboBox_Delegate
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_COMBOBOX_DELEGATE_H
#define SYS_COMBOBOX_DELEGATE_H

//qt-libs
#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QComboBox>
#include <QtGui>

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"

///ComboBox as a item for a table-widget \ingroup sys
/**

*/
class Sys_ComboBox_Delegate : public QItemDelegate
{
	Q_OBJECT //Macro for using signals and slots (Qt)in this class

public:

	///Default constructor
	Sys_ComboBox_Delegate(QObject *parent = 0);
	///Default destructor
	~Sys_ComboBox_Delegate(void);

	//methods

	///Create the editor widget
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	///Set the editors data
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	///Set the models data
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	///Update the editors geometry
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;


	///Set the string list for the combobox items
	void set_string_list(QStringList items);

private slots:

	///Emit the commited data
	void emitCommitData(void);

private:
	//member
	///String list for the combobox items
	QStringList items;
};

#endif //SYS_COMBOBOX_DELEGATE_H
