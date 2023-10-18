#include "Sys_Headers_Precompiled.h"
//#include "Sys_Reconnect_Db_Dia.h"

//Default constructor
Sys_Reconnect_Db_Dia::Sys_Reconnect_Db_Dia(QWidget *parent) : Sys_Diverse_Text_Dia(true, parent){

	this->setIcon(QMessageBox::Warning);
	this->setModal(true);
	this->good_close=false;
	QObject::disconnect(this->button(QMessageBox::Ok), SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->button(QMessageBox::Ok), SIGNAL(clicked()), this, SLOT(ok_button_is_clicked()));
	
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Reconnect_Db_Dia), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Reconnect_Db_Dia::~Sys_Reconnect_Db_Dia(void){

	
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Reconnect_Db_Dia), _sys_system_modules::SYS_SYS);
}
//_________
//public
//Start the dialog and return the user decision (ok:=true, cancel:=false)
bool Sys_Reconnect_Db_Dia::start_dialog(void){
	
	this->exec();

	return false;

}
//___________
//public slots
//Close the dialog
void Sys_Reconnect_Db_Dia::close_dialog(void){
	this->good_close=true;
	this->close();
}
//Ok-button pressed actions
void Sys_Reconnect_Db_Dia::ok_button_is_clicked(void){
	Data_Base::set_reconnection_flag(false);
	this->good_close=true;
	this->close();
}
//________
//private
//Handles the close event by the close-button of the dialog
void Sys_Reconnect_Db_Dia::closeEvent(QCloseEvent *close){
	if(this->good_close==true){
		this->close();
	}
	else{
		close->ignore();
	}
}