#include "Hyd_Headers_Precompiled.h"
//#include "HydGui_System_State_Dia.h"


//Default constructor
HydGui_System_State_Dia::HydGui_System_State_Dia(QWidget *parent): QDialog(parent){
	this->setupUi(this);
	this->spinBox_threads->setMinimum(1);
	this->spinBox_threads->setSingleStep(1);
	this->spinBox_threads->setMaximum(constant::max_threads);

	ostringstream buffer;
	buffer << "Maximum threads "<< constant::max_threads<< ", ideal threads ";
	int thread=1;
	if((QThread::idealThreadCount()-1)>thread){
		thread=(QThread::idealThreadCount()-1);
	}
	buffer << thread;
	this->spinBox_threads->setToolTip(buffer.str().c_str());

	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydGui_System_State_Dia), _sys_system_modules::HYD_SYS);
}
//Default destructor
HydGui_System_State_Dia::~HydGui_System_State_Dia(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(HydGui_System_State_Dia), _sys_system_modules::HYD_SYS);
}
//________
//public
//Start the dialog it returns true by acception, false by rejection
bool HydGui_System_State_Dia::start_dialog(const bool database_flag){
	if(database_flag==false){
		this->checkBox_file_output->setEnabled(false);
	}
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
//Set the current set parameters
void HydGui_System_State_Dia::set_current_system_data(const _hyd_state_data current){
	this->checkBox_file_output->setChecked(current.file_output_required);
	this->spinBox_threads->setValue(current.number_threads);
}
//Set the new parameters
_hyd_state_data HydGui_System_State_Dia::get_new_system_data(void){
	_hyd_state_data buffer;
	buffer.file_output_required=this->checkBox_file_output->isChecked();
	buffer.number_threads=this->spinBox_threads->value();

	return buffer;
}
