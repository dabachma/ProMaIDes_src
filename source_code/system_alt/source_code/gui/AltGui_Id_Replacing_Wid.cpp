#include "Alt_Headers_Precompiled.h"
//#include "AltGui_Id_Replacing_Wid.h"


//Default constructor
AltGui_Id_Replacing_Wid::AltGui_Id_Replacing_Wid(QWidget *parent): QWidget(parent){

	this->setupUi(this);
	this->comboBox_current->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	this->comboBox_new->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(AltGui_Id_Replacing_Wid), _sys_system_modules::ALT_SYS);
}
//Default destructor
AltGui_Id_Replacing_Wid::~AltGui_Id_Replacing_Wid(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(AltGui_Id_Replacing_Wid), _sys_system_modules::ALT_SYS);
}
//_________
//public
//Set the combox with the current id items
void AltGui_Id_Replacing_Wid::set_combobox_current_items(QStringList list){
	this->comboBox_current->clear();
	this->comboBox_current->addItems(list);

}
//Set the combox with the new id items
void AltGui_Id_Replacing_Wid::set_combobox_new_items(QStringList list){
	this->comboBox_new->clear();
	this->comboBox_new->addItems(list);
}
//Get the pointer to the combo-box with the current id items
QComboBox *AltGui_Id_Replacing_Wid::get_ptr_combo_box_current(void){
	return this->comboBox_current;
}
//Get the pointer to the combo-box with the new id items
QComboBox *AltGui_Id_Replacing_Wid::get_ptr_combo_box_new(void){
	return this->comboBox_new;
}