//#include "Sys_Elastic_GraphicsView.h"
#include "Sys_Headers_Precompiled.h"

Sys_Elastic_GraphicsView::Sys_Elastic_GraphicsView(QWidget *parent) 
	: QGraphicsView(parent)
{
	this->setRenderHint(QPainter::Antialiasing);

	ADD_MEM(Sys_Elastic_GraphicsView, _sys_system_modules::SYS_SYS);
}

Sys_Elastic_GraphicsView::Sys_Elastic_GraphicsView(Sys_Elastic_GraphicsScene *scene, QWidget *parent) :
        QGraphicsView(scene, parent)
{
	this->setRenderHint(QPainter::Antialiasing);

	ADD_MEM(Sys_Elastic_GraphicsView, _sys_system_modules::SYS_SYS);
}

Sys_Elastic_GraphicsView::~Sys_Elastic_GraphicsView(void)
{
	MIN_MEM(Sys_Elastic_GraphicsView, _sys_system_modules::SYS_SYS);
}
