#pragma once
/**\class _Sys_Abstract_Base_Wid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef _SYS_ABSTRACT_BASE_WID_H
#define _SYS_ABSTRACT_BASE_WID_H

//Just a little macro for easier work, you can paste that in your show_as_dialog 
//method with current classname as argument and youre done with the method...
#define SHOW_AS_DIALOG(className) { \
	Sys_Base_Edit_Dia dialog(NULL, this); \
	className inDiaWid(DataRole::EditRole, &dialog); \
	inDiaWid.set_member(this); \
	dialog.add_child(&inDiaWid); \
	dialog.setWindowTitle(this->head_label->text()); \
	QIcon icon; \
	icon.addPixmap(*this->head_pixmap->pixmap()); \
	dialog.setWindowIcon(icon); \
	dialog.exec(); \
}

//Macro to keep the constructors of inheriting classes clean
#define DATA_ROLE_CONSTRUCTOR_STUFF() { \
	switch(role) { \
	case 0: \
		this->set_editable(false); \
		this->head_pixmap->setPixmap(QPixmap(":/default_config")); \
		break; \
	case 1: \
		this->set_editable(true); \
		this->headWidget->hide(); \
		QObject::disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint))); \
	} \
	this->set_default_values(); \
}

//Same reason here
#define ADD_MEM(className, moduleName) { \
	Sys_Memory_Count::self()->add_mem(sizeof(className), moduleName); \
}

//And same reason again
#define MIN_MEM(className, moduleName) { \
	Sys_Memory_Count::self()->minus_mem(sizeof(className), moduleName); \
}

//forms
#include "ui__Sys_Abstract_Base_Wid.h"

//system-sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "_Sys_Data_Wid.h"

//qt-libs
#include <QObject>
#include <QMenu>
#include <QTreeWidgetItem>

///Enumeratur to specify the role of the widget
enum DataRole {
	///Widget is for display
	DisplayRole,
	///Widget is for editing
	EditRole
};

///Abstract Class for several data formulars and dialogs \ingroup sys
/**
	
*/
class _Sys_Abstract_Base_Wid : public _Sys_Data_Wid, public Ui::Sys_Abstract_Base_Wid
{
	Q_OBJECT //Qt-macro

public:
	///Default constructor
	_Sys_Abstract_Base_Wid(QWidget *parent = 0);
	///Default destructor
	virtual ~_Sys_Abstract_Base_Wid(void);

	//methods
	///Set the possibility to edit the data to the given state
	void set_edit_action_disabled(const bool state);
	///Set the member, which are displayed in the widget
	virtual void set_member(_Sys_Abstract_Base_Wid *ptr) = 0;
	///Changes the editable state for all child widgets in the formular/dialog
	virtual void set_editable(const bool state) = 0;
	///Set the spinbox range in the head widget
	virtual void set_head_spinBox_range(const int max, const int min = 0);
	///Set the head spin box value
	void set_head_spinBox_value(const int value);
	///Get a pointer to the head spin box
	QSpinBox* get_ptr_head_spinbox(void);
	///Set the spinbox text in the head widget
	virtual void set_head_spinBox_text(const string text);
	///Common getter for editable state
	bool is_editable(void);
	///This method sets the text of the big label on top of the widget
	virtual void set_headLabel_text(const string title);
	///This method sets the icon on top of the widget (left)
	virtual void set_headPixmap(const QPixmap pic);
	///Set default values
	virtual void set_default_values(void) = 0;
	///Set the child widget
	void set_child(QWidget *child);
	///Set current scroll bars position (vertical, horizontal)
	void set_current_scroll_bar(const int ver_pos, const int hor_pos);

public slots:
	///Open the dialog to edit values in the widget
	virtual void show_as_dialog(void) = 0;

signals:
	///Send the number of the spin box and the pointer to the tree widget item
	void send_change_widget(int index, QTreeWidgetItem *item, int pos_ver, int pos_hor);
	
protected:
	//member
	///Editable state
	bool editable;
	///Action for the edit entry
	QAction *edit_action;
	///Icon for the edit entry
	QIcon edit_icon;
	///Pointer to the database
	QSqlDatabase *ptr_database;

private slots:
	///Recieve if the head spin box value is changed
	void recieve_head_spin_box_changed(int index);
};

#endif //_SYS_ABSTRACT_BASE_WID_H
