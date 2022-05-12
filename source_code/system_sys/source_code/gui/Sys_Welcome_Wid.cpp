#include "Sys_Headers_Precompiled.h"
//#include "Sys_Welcome_Wid.h"

//constructor
Sys_Welcome_Wid::Sys_Welcome_Wid(QWidget *parent) : QWidget(parent, Qt::FramelessWindowHint){

	this->ui.setupUi(this);
	this->setGeometry(350,150, 1250,700);

	this->ui.label->setGeometry(0, 0, 2000, 20);
	QString buff;
	buff = "<font color=\"black\">Welcome to ";
	buff.append(Sys_Project::get_version_number().c_str());
	buff.append("...</font>");
	
	this->ui.label->setText(buff);

	QTimer::singleShot(2500, this, SLOT(set_new_txt()));
	
	this->my_scene.setParent(this->ui.graphic);
	this->ui.graphic->setGeometry(0, 0, 1250, 700);
	
	this->my_scene.setSceneRect(this->ui.graphic->rect());
	//this->my_scene.setBackgroundBrush(QBrush(Qt::lightGray, Qt::CrossPattern));
	this->my_scene.setBackgroundBrush(QColor(238, 238, 238));
	//this->my_scene.setForegroundBrush(QColor(255, 255, 255, 127));
	
	this->ui.graphic->setScene(&this->my_scene);
	
	
	//QSize my_size=this->size();
	//this->my_jpg.scaled(my_size);
	this->my_jpg.load(":welcome_wid_v2.png");
	//this->my_jpg.fill(Qt::transparent);
	//this->my_jpg.scaledToHeight(700);

	this->my_scene.addPixmap(this->my_jpg.scaledToHeight(680));
	
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
	this->ui.label->setText("<font color=\"black\">ProMaIDes is loading...</font>");
}