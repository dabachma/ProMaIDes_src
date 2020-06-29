#include "Sys_Headers_Precompiled.h"
//#include "FplGui_FaultTree_BaseWid.h"

//Default constructor
FplGui_FaultTree_BaseWid::FplGui_FaultTree_BaseWid(QWidget *parent) 
	: _Sys_Abstract_Base_Wid(parent) 
{

	view = new Sys_Elastic_GraphicsView(this);
	scene = new Sys_Elastic_GraphicsScene(this);
	view->setScene(scene);

	arrangeAction = new QAction(tr("Arrange Items"), this); 
	this->context_menu.addAction(arrangeAction); 
	//count memory
	ADD_MEM(FplGui_FaultTree_BaseWid, _sys_system_modules::SYS_SYS);
}

//Default destructor
FplGui_FaultTree_BaseWid::~FplGui_FaultTree_BaseWid(void) 
{
	//count memory
	MIN_MEM(FplGui_FaultTree_BaseWid, _sys_system_modules::SYS_SYS);
}

//______________
//public methods
//
QList<Sys_Elastic_GraphicsPixmapItem *> FplGui_FaultTree_BaseWid::get_items(void) const {
	return items;
}


