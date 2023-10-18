#include "Dam_Headers_Precompiled.h"
//#include "DamGui_Number_Raster_Dia.h"

//Default constructor
DamGui_Number_Raster_Dia::DamGui_Number_Raster_Dia( QWidget *parent) : QDialog(parent){

	this->setupUi(this);
	//slot connectiong
	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	this->number_raster=0;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(DamGui_Number_Raster_Dia), _sys_system_modules::DAM_SYS);
}
//Default destructor
DamGui_Number_Raster_Dia::~DamGui_Number_Raster_Dia(void){


	
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(DamGui_Number_Raster_Dia), _sys_system_modules::DAM_SYS);
}
//____________
//public
//Get the number of raster
int DamGui_Number_Raster_Dia::get_number_raster(void){

	return this->number_raster;
}
//Start the dialog
bool DamGui_Number_Raster_Dia::start_dialog(void){

	int decision=this->exec();
	//rejected
	if(decision ==0){
		return false;
	}
	//accepted; read the path and file name out via get_file_path(const int index) 
	else{
		this->number_raster=this->spinBox_raster_num->value();
		return true;
	}
}
//Set the icon of the dialog
void DamGui_Number_Raster_Dia::set_icon(QIcon icon){
	this->setWindowIcon(icon);
}
//Set window title
void DamGui_Number_Raster_Dia::change_window_title(string title, QIcon icon){
	this->setWindowTitle(title.c_str());
	this->setWindowIcon(icon);

}
//Set main text in the dialog
void DamGui_Number_Raster_Dia::change_main_text(string txt){
	this->label->setText(txt.c_str());
	
}
//____________
//private