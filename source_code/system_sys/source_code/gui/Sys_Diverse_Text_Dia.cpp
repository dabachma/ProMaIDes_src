#include "Sys_Headers_Precompiled.h"
//#include "Sys_Diverse_Text_Dia.h"

//Default constructor
Sys_Diverse_Text_Dia::Sys_Diverse_Text_Dia(QWidget *parent) : QMessageBox(parent){
	this->question="No question is set!";
	this->setInformativeText(this->question.c_str());
	this->setIcon(QMessageBox::Question);
	QPushButton *buffer=NULL;

	QIcon my_icon8;
	my_icon8.addFile(":prom_icon");
	this->setWindowIcon(my_icon8);

	buffer=this->addButton(QMessageBox::Ok);
	QObject::connect(buffer, SIGNAL(clicked()), this, SLOT(accept()));
	buffer=this->addButton(QMessageBox::Cancel);
	QObject::connect(buffer, SIGNAL(clicked()), this, SLOT(reject()));
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Diverse_Text_Dia), _sys_system_modules::SYS_SYS);
}
//Constructor for a pure information box
Sys_Diverse_Text_Dia::Sys_Diverse_Text_Dia( bool just_info, QWidget *parent): QMessageBox(parent){
	this->question="No question is set!";
	this->setInformativeText(this->question.c_str());
	this->setIcon(QMessageBox::Information);
	QPushButton *buffer=NULL;
	buffer=this->addButton(QMessageBox::Ok);
	QObject::connect(buffer, SIGNAL(clicked()), this, SLOT(accept()));
	if(just_info==false){
		buffer=this->addButton(QMessageBox::Cancel);
		QObject::connect(buffer, SIGNAL(clicked()), this, SLOT(reject()));
	}
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Diverse_Text_Dia), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Diverse_Text_Dia::~Sys_Diverse_Text_Dia(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Diverse_Text_Dia), _sys_system_modules::SYS_SYS);
}
//___________
//public
//Set the question of the dialog
void Sys_Diverse_Text_Dia::set_dialog_question(const string text){
	this->question=text;
	//set text to the label
	this->setInformativeText(this->question.c_str());
}
//Start the dialog and return the user decision (ok:=true, cancel:=false)
bool Sys_Diverse_Text_Dia::start_dialog(void){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{
		return true;
	}
}
//Set the window icon
void Sys_Diverse_Text_Dia::set_window_icon(QIcon icon){
	this->setWindowIcon(icon);
}
//Set button text
void Sys_Diverse_Text_Dia::set_button_text(const string ok_butt, const string cancel_butt){
	this->button(QMessageBox::Ok)->setText(ok_butt.c_str());
	this->button(QMessageBox::Cancel)->setText(cancel_butt.c_str());
}