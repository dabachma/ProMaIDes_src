#include "Alt_Headers_Precompiled.h"
//#include "AltGui_File_Replacing_Wid.h"

//Default constructor
AltGui_File_Replacing_Wid::AltGui_File_Replacing_Wid(QWidget *parent): QWidget(parent){
	this->setupUi(this);
	this->comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(AltGui_File_Replacing_Wid), _sys_system_modules::ALT_SYS);
}
//Default destructo
AltGui_File_Replacing_Wid::~AltGui_File_Replacing_Wid(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(AltGui_File_Replacing_Wid), _sys_system_modules::ALT_SYS);
}
//_______
//public
//Set the combox items
void AltGui_File_Replacing_Wid::set_combobox_items(QStringList list){
		this->comboBox->clear();
		this->comboBox->addItems(list);
		if(list.count()>0 && list.at(0)==alt_label::adding_item.c_str()){
			this->comboBox->insertSeparator(1);
		}
		
}
//Get the pointer to the combo-box
QComboBox * AltGui_File_Replacing_Wid::get_ptr_combo_box(void){
	return this->comboBox;

}
//Get choosen the file-name 
QString AltGui_File_Replacing_Wid::get_file_name(void){
	return this->widget->fileName();
}