#pragma once
/**\class Sys_Sqlmodel_Loader
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_DATA_WIDGET_LOADER_H
#define SYS_DATA_WIDGET_LOADER_H

//qt libs
#include <QObject>

//foward declaration
class Sys_Data_Tree_Wid;

///Thread-class for loading data widget to the data widget with an extra thread \ingroup sys
/**

*/
class Sys_Data_Widget_Loader : public QThread 
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT 

public:
	///Default constructor
	Sys_Data_Widget_Loader(void);
	///Default destructor
	~Sys_Data_Widget_Loader(void);

	//method
	///Set the pointer to the data tree widget
	void set_ptr_data_tree(Sys_Data_Tree_Wid *ptr);

	///Set the tree widget item, which should be loaded as widget
	void set_ptr_item2load(QTreeWidgetItem *load);

	void set_widget(QWidget *widget);

	///Run the thread; the pointer to the data tree widget has to be set before running the thread
	void run(void);

signals:
	///Emit if the data tree idget should be enabled or not
	void enable_data_tree(bool flag);
	///Emit the widget to the data tree widget, which should be displayed
	void widget2show(QWidget *wid);

private:
	//member
	///Pointer to the data tree widget
	Sys_Data_Tree_Wid *ptr_datatree;

	///Pointer to the tree widget item, which should be loaded as widget
	QTreeWidgetItem *item2load;

	QWidget *widget;

	//method
	///Set widget to the hydraulic view
	void widget2hyd(QTreeWidgetItem *item);
	///Set widget to the damage view
	void widget2dam(QTreeWidgetItem *item);
	///Set widget to the fpl view
	void widget2fpl(QTreeWidgetItem *item);
	///Set widget to the madm view
	void widget2madm(QTreeWidgetItem *item);
	///Set widget to the risk view
	void widget2risk(QTreeWidgetItem *item);
	///Set widget to the alt view
	void widget2alt(QTreeWidgetItem *item);
	///Set widget to the cost view
	void widget2cost(QTreeWidgetItem *item);
	//set up for further modules

};
#endif
