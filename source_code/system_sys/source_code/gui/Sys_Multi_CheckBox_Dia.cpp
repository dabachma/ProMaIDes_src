#include "Sys_Headers_Precompiled.h"
//#include "Sys_Multi_CheckBox_Dia.h"

//Default constructor
Sys_Multi_CheckBox_Dia::Sys_Multi_CheckBox_Dia(QWidget *parent) : QDialog(parent){

	this->number_boxes=0;
	this->check_boxes=NULL;
	ui.setupUi(this);
	//slot connectiong
	QObject::connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Multi_CheckBox_Dia)), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Multi_CheckBox_Dia::~Sys_Multi_CheckBox_Dia(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Multi_CheckBox_Dia), _sys_system_modules::SYS_SYS);
	if(this->check_boxes!=NULL){
		delete []this->check_boxes;
		this->check_boxes=NULL;
	}
}
//____________
//public
//Set the number of check-boxes. They are also allocated and set to the dialog.
void Sys_Multi_CheckBox_Dia::set_number_check_boxes(const int number, QIcon icon){
	this->number_boxes=number;
	this->setWindowIcon(icon);

	this->check_boxes=new QCheckBox[this->number_boxes];

	for(int i=0; i< this->number_boxes;i++){
		this->check_boxes[i].setParent(this);
		this->check_boxes[i].setChecked(false);
		this->check_box_layout.addWidget(&this->check_boxes[i]);
	}

	//add the layout to the dialog
	ui.chooser_frame->setLayout(&this->check_box_layout);
	this->adjustSize();
}
//Get the number of check-boxes (QCheckBox) used in the dialog
int Sys_Multi_CheckBox_Dia::get_number_check_boxes(void){
	return this->number_boxes;
}
//Set button text 
void Sys_Multi_CheckBox_Dia::set_button_txt(const string ok, const string cancel){
	ui.okButton->setText(ok.c_str());
	ui.cancelButton->setText(cancel.c_str());
}
//Get a pointer to the check-box given by the index
QCheckBox* Sys_Multi_CheckBox_Dia::get_ptr_check_box(const int index){
	if(index<0 || index >= this->number_boxes){
		return NULL;
	}
	else{
		return &this->check_boxes[index];
	}
}
//Get the boolean of the check-box given by the index
bool Sys_Multi_CheckBox_Dia::get_bool_check_box(const int index){
	bool buffer=false;
	if(index<0 || index >= this->number_boxes){
		return NULL;
	}
	else{
		buffer=this->check_boxes[index].isChecked();
		return buffer;
	}
}
//Set the text of the main text-label
void Sys_Multi_CheckBox_Dia::set_main_text_label(const string text){
	ui.main_text->setText(text.c_str());
}
//Set the window title
void Sys_Multi_CheckBox_Dia::set_window_title(const string text){
	this->setWindowTitle(text.c_str());
}
//Start the dialog
bool Sys_Multi_CheckBox_Dia::start_dialog(void){
	this->adjustSize();
	int decision=this->exec();
	//rejected
	if(decision ==0){
		return false;
	}
	//accepted; read the path and file name out via get_file_path(const int index) 
	else{
		return true;
	}
}