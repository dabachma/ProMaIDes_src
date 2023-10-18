#include "Alt_Headers_Precompiled.h"
//#include "AltGui_Alternative_Base_On_Dia.h"

//Default constructor
AltGui_Alternative_Base_On_Dia::AltGui_Alternative_Base_On_Dia(QWidget *parent): QDialog(parent){

	this->setupUi(this);

	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	this->measurestate_base=0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(AltGui_Alternative_Base_On_Dia), _sys_system_modules::ALT_SYS);
}
//Default destructor
AltGui_Alternative_Base_On_Dia::~AltGui_Alternative_Base_On_Dia(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(AltGui_Alternative_Base_On_Dia), _sys_system_modules::ALT_SYS);
}
//________
//public
//Set for the dialog the measure identifier
void AltGui_Alternative_Base_On_Dia::set_dialog(QSqlDatabase *ptr_database, const _sys_system_id id){


	ostringstream info;
	QStringList buffer2;
	info << "0 " << "Base state";
	buffer2.append(info.str().c_str());
	info.str("");
	string name;
	string descript;

	Alt_System::get_name_descript_measure_table(ptr_database, id, &name, &descript);
	info<<id.measure_nr<<"  "<<name;

	_alt_general_info buff_current;
	if(id.measure_nr>0){
		Alt_System::check_measure_is_set_database(ptr_database, id, &buff_current);
	}
	//set up combo box
	this->comboBox_base->clear();
	this->comboBox_base->setEnabled(true);

	//just take the base stae and the last measure state
	
	
	if(buff_current.detailed_type==alt_replace_fpl::section || buff_current.detailed_type==alt_replace_fpl::section_param ||
		buff_current.detailed_type==alt_add_fpl::section || buff_current.detailed_type==alt_combi_fpl::fpl_line){
		buffer2.append(info.str().c_str());
		this->comboBox_base->setToolTip("Choose between the base measure state or the current measure state");
	}
	else{
		this->comboBox_base->setToolTip("Just the base measure state is available. Either the current measure state \nis the base state or the current measure wasn't a measure concerning the FPL-section(s)");
	}
	
	this->comboBox_base->addItems(buffer2);



}
//Start the dialog it returns true by acception, false by rejection
bool AltGui_Alternative_Base_On_Dia::start_dialog(void){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{
		stringstream buff_str;
		buff_str << this->comboBox_base->currentText().toStdString();
		buff_str >> this->measurestate_base;
		return true;
	}
}
//Get the measurestate on which the new measure (change FPL_parameters) should base on
int AltGui_Alternative_Base_On_Dia::get_measurestate_base_on(void){
	return this->measurestate_base;
}