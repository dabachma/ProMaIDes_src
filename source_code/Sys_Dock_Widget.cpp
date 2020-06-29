#include "Main_Headers_Precompiled.h"
//#include "Sys_Dock_Widget.h"

//Default constructor
Sys_Dock_Widget::Sys_Dock_Widget(QWidget *parent): QDockWidget(parent){
}
//Default destructor
Sys_Dock_Widget::~Sys_Dock_Widget(void){
}
//________
//public
//Handle a close event
void Sys_Dock_Widget::closeEvent(QCloseEvent * close){
	emit close_procedure();
	QDockWidget::closeEvent(close);
}
