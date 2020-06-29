#include "Sys_Elastic_GraphicsObject.h"

Sys_Elastic_GraphicsObject::Sys_Elastic_GraphicsObject(QGraphicsItem *parentItem, QObject *parent)
	: QObject(parent), QGraphicsItem(parentItem)
{
	ADD_MEM(Sys_Elastic_GraphicsObject, _sys_system_modules::SYS_SYS);
}

Sys_Elastic_GraphicsObject::Sys_Elastic_GraphicsObject(QObject *parent) 
	: QObject(parent)
{
	ADD_MEM(Sys_Elastic_GraphicsObject, _sys_system_modules::SYS_SYS);
}

Sys_Elastic_GraphicsObject::~Sys_Elastic_GraphicsObject(void) 
{
	MIN_MEM(Sys_Elastic_GraphicsObject, _sys_system_modules::SYS_SYS);
}

