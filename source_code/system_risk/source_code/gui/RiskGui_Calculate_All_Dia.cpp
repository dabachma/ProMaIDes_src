#include "Risk_Headers_Precompiled.h"
//#include "RiskGui_Calculate_All_Dia.h"

//Default constructor
RiskGui_Calculate_All_Dia::RiskGui_Calculate_All_Dia(QWidget *parent): QDialog(parent){
	this->setupUi(this);

	
	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QObject::connect(this->checkBox_ca_nobreak, SIGNAL(clicked(bool )), this, SLOT(check_ca_nobreak_change(bool )));
	QObject::connect(this->checkBox_ca_break_list, SIGNAL(clicked(bool )), this, SLOT(check_ca_list_generation_change(bool )));
	QObject::connect(this->checkBox_ca_one_break, SIGNAL(clicked(bool )), this, SLOT(check_ca_onebreak_change(bool )));
	QObject::connect(this->checkBox_ca_auto, SIGNAL(clicked(bool )), this, SLOT(check_ca_auto_change(bool )));

	Sys_Memory_Count::self()->add_mem(sizeof(RiskGui_Calculate_All_Dia),_sys_system_modules::RISK_SYS);//count the memory
}
//Default destructor
RiskGui_Calculate_All_Dia::~RiskGui_Calculate_All_Dia(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(RiskGui_Calculate_All_Dia),_sys_system_modules::RISK_SYS);//count the memory

}
//_________
//public
//Start the dialog; it returns true by acception, false by rejection
bool RiskGui_Calculate_All_Dia::start_dialog(void){
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
//Set the flags for calculation
void RiskGui_Calculate_All_Dia::set_flags(bool *ca_nobreak, bool *ca_list_gen, bool *ca_onebreak, bool *ca_auto){
	*ca_nobreak=this->checkBox_ca_nobreak->isChecked();
	*ca_list_gen=this->checkBox_ca_break_list->isChecked();
	*ca_onebreak=this->checkBox_ca_one_break->isChecked();
	*ca_auto=this->checkBox_ca_auto->isChecked();
}
//_________
//private slots
//Check the catchment risk nobreak calculation check box changed the status
void RiskGui_Calculate_All_Dia::check_ca_nobreak_change(bool flag){
	if(flag==false){
		this->checkBox_ca_break_list->setChecked(flag);
		this->checkBox_ca_one_break->setChecked(flag);
		this->checkBox_ca_auto->setChecked(flag);
	}
}
//Check the catchment risk scenario list generation check box changed the status
void RiskGui_Calculate_All_Dia::check_ca_list_generation_change(bool flag){
	if(flag==false){
		this->checkBox_ca_one_break->setChecked(flag);
		this->checkBox_ca_auto->setChecked(flag);
	}
	else{
		this->checkBox_ca_nobreak->setChecked(flag);
	}
}
//Check the catchment risk one-break calculation check box changed the status
void RiskGui_Calculate_All_Dia::check_ca_onebreak_change(bool flag){
	if(flag==false){
		this->checkBox_ca_auto->setChecked(flag);
	}
	else{
		this->checkBox_ca_break_list->setChecked(flag);
		this->checkBox_ca_nobreak->setChecked(flag);
	}
}
//Check the catchment risk auto calculation check box changed the status
void RiskGui_Calculate_All_Dia::check_ca_auto_change(bool flag){
	if(flag==true){
		this->checkBox_ca_break_list->setChecked(flag);
		this->checkBox_ca_one_break->setChecked(flag);
		this->checkBox_ca_nobreak->setChecked(flag);
	}
}