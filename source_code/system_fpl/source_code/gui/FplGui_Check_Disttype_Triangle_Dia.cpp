#include "Fpl_Headers_Precompiled.h"
#include "FplGui_Check_Disttype_Triangle_Dia.h"

//Default constructor
FplGui_Check_Disttype_Triangle_Dia::FplGui_Check_Disttype_Triangle_Dia(QWidget *parent): QDialog(parent){

	this->setupUi(this);

	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(this->doubleSpinBox_mid, SIGNAL(valueChanged(double )), this, SLOT(check_mid_change(double )));
	this->doubleSpinBox_mid->setValue(0.0);

	
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(FplGui_Check_Disttype_Triangle_Dia), _sys_system_modules::FPL_SYS);
}
//Default destructor
FplGui_Check_Disttype_Triangle_Dia::~FplGui_Check_Disttype_Triangle_Dia(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(FplGui_Check_Disttype_Triangle_Dia), _sys_system_modules::FPL_SYS);
}
//__________
//public
//Start the dialog; it returns true by acception, false by rejection
bool FplGui_Check_Disttype_Triangle_Dia::start_dialog(void){
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
//Get the min value
double FplGui_Check_Disttype_Triangle_Dia::get_min(void){
	return this->doubleSpinBox_min->value();
}
//Get the max value
double FplGui_Check_Disttype_Triangle_Dia::get_max(void){
	return this->doubleSpinBox_max->value();
}
//Get the mid value
double FplGui_Check_Disttype_Triangle_Dia::get_mid(void){
	return this->doubleSpinBox_mid->value();
}
//Get the constant
double FplGui_Check_Disttype_Triangle_Dia::get_constant(void){
	return this->doubleSpinBox_constant->value();
}
//__________
//private slots
//Check if the mid-value has changed and set the maximum/minimum values of the spinboxes (min/max) dynamically 
void FplGui_Check_Disttype_Triangle_Dia::check_mid_change(const double value){
	this->doubleSpinBox_max->setMinimum(value);
	this->doubleSpinBox_min->setMaximum(value);
}