#include "Sys_Headers_Precompiled.h"
//#include "Sys_Custom_Plot_Picker.h"
#include <QPointF>

//Default constructor
Sys_Custom_Plot_Picker::Sys_Custom_Plot_Picker(QWidget *canvas):QwtPlotPicker(canvas){
}
//Default destructor
Sys_Custom_Plot_Picker::~Sys_Custom_Plot_Picker(void){
}
//_______
//protected
//Get the tracker text (virtual)
QwtText Sys_Custom_Plot_Picker::trackerText(const QPoint &pos){
    const QPointF p = invTransform(pos);
    const QPointF curvePos(p.x(), p.y());
	QString tracker;
	tracker.sprintf("%.2f, %.2f", curvePos.x(), curvePos.y());
	return QwtText(tracker);
}
