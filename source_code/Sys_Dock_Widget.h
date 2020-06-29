#pragma once
/**\class Sys_Dock_Widgetd
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/
#ifndef SYS_DOCK_WIDGET_H
#define SYS_DOCK_WIDGET_H

//qt class
#include <QDockWidget>

class Sys_Dock_Widget : public QDockWidget
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	Sys_Dock_Widget(QWidget *parent);
	///Default Destructor
	~Sys_Dock_Widget(void);

	///Handle a close event
	void closeEvent(QCloseEvent * close);

signals:
	///Emit a closing procdure
	void close_procedure(void);
};
#endif