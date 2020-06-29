#include "Sys_Headers_Precompiled.h"
//#include "Sys_Welcome_Wid.h"

//constructor
Sys_Welcome_Wid::Sys_Welcome_Wid(QWidget *parent) : QWidget(parent, Qt::FramelessWindowHint){

	this->ui.setupUi(this);
	this->setGeometry(400,330, 400,376);
	this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	this->ui.label->setText("<font color=\"white\">Welcome to ProMaIDes</font>");

	QTimer::singleShot(2000, this, SLOT(set_new_txt()));
	
	this->my_scene.setParent(this->ui.graphic);
	this->my_scene.setSceneRect(this->ui.graphic->rect());
	this->ui.graphic->setScene(&this->my_scene);
	
	//QSize my_size=this->size();
	//this->my_jpg.scaled(my_size);
	this->my_jpg.load(":welcome_wid_v1.png");

	this->my_scene.addPixmap(this->my_jpg);
	
	this->ui.graphic->show();
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Welcome_Wid), _sys_system_modules::SYS_SYS);

}
//destructor
Sys_Welcome_Wid::~Sys_Welcome_Wid(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Welcome_Wid), _sys_system_modules::SYS_SYS);
}
//______________
//private slots
//Change the welcome text after few mseconds
void Sys_Welcome_Wid::set_new_txt(void){
	/**
	The welcome text is changed with this slot. The time for changing and the connecting of the slot is launched in the constructor
	*/
	this->ui.label->setText("<font color=\"white\">ProMaIDes is loading...</font>");
}