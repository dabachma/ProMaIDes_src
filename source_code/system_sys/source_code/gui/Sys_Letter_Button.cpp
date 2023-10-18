#include "Sys_Headers_Precompiled.h"
#include "Sys_Letter_Button.h"

//Default constructor
Sys_Letter_Button::Sys_Letter_Button(QWidget *parent): QPushButton(parent){

	QObject::connect(this, SIGNAL(clicked()), this, SLOT(slot2send_txt()));
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Letter_Button), _sys_system_modules::SYS_SYS);

}
//Default destructor
Sys_Letter_Button::~Sys_Letter_Button(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Letter_Button), _sys_system_modules::SYS_SYS);
}
//__________
//public slots
//A slot to send the button text
void Sys_Letter_Button::slot2send_txt(void){
	emit send_button_txt(this->text());
}