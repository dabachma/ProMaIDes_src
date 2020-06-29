//#include "Exception_Sender.h"
#include "Sys_Headers_Precompiled.h"

//init static members
Sys_Exception_Sender *Sys_Exception_Sender::ptr_class=NULL;

//Default destructor
Sys_Exception_Sender::~Sys_Exception_Sender(void){
	if (ptr_class!=NULL){
		delete ptr_class;
		ptr_class=NULL;
	}
}
//_____________________________
//public
//It returns the static pointer to this unique object (singleton pattern); if it is not yet allocated, it will be allocated
Sys_Exception_Sender* Sys_Exception_Sender::self(void){
	if (ptr_class==NULL){
		ptr_class = new Sys_Exception_Sender();
	}
	return ptr_class;
}
//______________________
//public signals
//Sender for the warning number
void Sys_Exception_Sender::set_fatal_errortxt(const QString txt){
	//send it
	emit send_fatal_error(txt);
}
//Sender for the errornumber
void Sys_Exception_Sender::set_error_number(const int number, const QString tooltip){
	//send it
	emit send_error_number(number, tooltip);
}
//Sender for the fatal error text
void Sys_Exception_Sender::set_warning_number(const int number, const QString tooltip){
	//send it
	emit send_warning_number(number, tooltip);
}
//Set to open the reconnection dialog
void Sys_Exception_Sender::set_open_reconnection_dia(QString time){
	emit send_reconnection_try_dia_open(time);
}
//Set to close the reconnection dialog
void Sys_Exception_Sender::set_close_reconnection_dia(void){
	emit send_reconnection_try_dia_close();
}
//____________________
//private
//Default constructor as private (singleton pattern)
Sys_Exception_Sender::Sys_Exception_Sender(void){
}