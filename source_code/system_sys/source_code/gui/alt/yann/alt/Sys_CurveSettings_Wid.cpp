#include "source_code\Sys_Headers_Precompiled.h"
//#include "Sys_CurveSettings_Wid.h"


//Default constructor
Sys_CurveSettings_Wid::Sys_CurveSettings_Wid(QWidget *parent){

	this->ui.setupUi(this);
	this->setParent(parent);

	Sys_Memory_Count::self()->add_mem(sizeof(Sys_CurveSettings_Wid),_sys_system_modules::SYS_SYS);

}
//Default destructor
Sys_CurveSettings_Wid::~Sys_CurveSettings_Wid(void){


Sys_Memory_Count::self()->minus_mem(sizeof(Sys_CurveSettings_Wid),_sys_system_modules::SYS_SYS);
}
//_________
//public