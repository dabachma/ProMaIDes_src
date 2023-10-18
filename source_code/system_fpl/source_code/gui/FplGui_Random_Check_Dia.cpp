#include "Fpl_Headers_Precompiled.h"
//#include "FplGui_Random_Check_Dia.h"

//Default constructor
FplGui_Random_Check_Dia::FplGui_Random_Check_Dia(QWidget *parent): QDialog(parent){

	this->setupUi(this);

	this->spinBox_number_runs->setMinimum(100);
	this->spinBox_number_runs->setMaximum(100000000);
	this->spinBox_number_runs->setValue(100000);
	this->spinBox_number_runs->setSingleStep(5000);
	this->spinBox_number_class->setMinimum(10);
	this->spinBox_number_class->setMaximum(10000);
	this->spinBox_number_class->setValue(500);
	this->spinBox_number_class->setSingleStep(50);

	this->comboBox_generator_type->addItem(fpl_label::generator_ranmar.c_str());
	this->comboBox_generator_type->addItem(fpl_label::generator_zikurat.c_str());
	this->comboBox_generator_type->addItem(fpl_label::generator_MofAll.c_str());
	this->comboBox_generator_type->addItem(fpl_label::generator_sfmt.c_str());

	this->comboBox_dist_type->addItem(fpl_label::dist_mean.c_str());
	this->comboBox_dist_type->addItem(fpl_label::dist_triangle.c_str());


	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(FplGui_Random_Check_Dia), _sys_system_modules::FPL_SYS);
}
//Default destructor
FplGui_Random_Check_Dia::~FplGui_Random_Check_Dia(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(FplGui_Random_Check_Dia), _sys_system_modules::FPL_SYS);
}
//__________
//public
//Start the dialog; it returns true by acception, false by rejection
bool FplGui_Random_Check_Dia::start_dialog(void){
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
//Get the number of runs
int FplGui_Random_Check_Dia::get_number_runs(void){
	return this->spinBox_number_runs->value();
}
//Get number of classes
int FplGui_Random_Check_Dia::get_number_classes(void){
	return this->spinBox_number_class->value();
}
//Get random generator type
_fpl_random_generator_type FplGui_Random_Check_Dia::get_random_generator_type(void){
	string buffer;
	buffer=this->comboBox_generator_type->currentText().toStdString();

	return Fpl_Mc_Sim::convert_text2generator_type(buffer);
}
//Get distribution class type
_fpl_distribution_type FplGui_Random_Check_Dia::get_distribution_class_type(void){
	string buffer;
	buffer=this->comboBox_dist_type->currentText().toStdString();

	return _Fpl_Distribution_Class::convert_txt2dist_type(buffer);
}