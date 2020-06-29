//#include "Sys_Elastic_GraphicsScene.h"
#include "Sys_Headers_Precompiled.h"

//
Sys_Elastic_GraphicsScene::Sys_Elastic_GraphicsScene(const QRectF &sceneRect, QObject *parent) 
	: QGraphicsScene(sceneRect, parent)
{
	ADD_MEM(Sys_Elastic_GraphicsScene, _sys_system_modules::SYS_SYS);
}
//
Sys_Elastic_GraphicsScene::Sys_Elastic_GraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent) 
	: QGraphicsScene(x, y, width, height, parent)
{
	ADD_MEM(Sys_Elastic_GraphicsScene, _sys_system_modules::SYS_SYS);
}
//
Sys_Elastic_GraphicsScene::Sys_Elastic_GraphicsScene(QObject *parent) 
	: QGraphicsScene(parent)
{
	ADD_MEM(Sys_Elastic_GraphicsScene, _sys_system_modules::SYS_SYS);
}
//Default constructor
Sys_Elastic_GraphicsScene::~Sys_Elastic_GraphicsScene(void)
{
	MIN_MEM(Sys_Elastic_GraphicsScene, _sys_system_modules::SYS_SYS);
}
