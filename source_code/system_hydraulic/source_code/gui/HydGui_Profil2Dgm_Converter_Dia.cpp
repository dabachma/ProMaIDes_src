#include "Hyd_Headers_Precompiled.h"
//#include "HydGui_Profil2Dgm_Converter_Dia.h"



//Default constructor
HydGui_Profil2Dgm_Converter_Dia::HydGui_Profil2Dgm_Converter_Dia(QWidget *parent) : QDialog(parent){
	this->file_browser=NULL;
	this->number_browser=0;
	ui.setupUi(this);
	this->ui.doubleSpinBox_dens->setValue(0.3);
	//slot connectiong
	QObject::connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(HydGui_Profil2Dgm_Converter_Dia)), _sys_system_modules::HYD_SYS);
}
//Default destructor
HydGui_Profil2Dgm_Converter_Dia::~HydGui_Profil2Dgm_Converter_Dia(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(HydGui_Profil2Dgm_Converter_Dia), _sys_system_modules::HYD_SYS);
	if(this->file_browser!=NULL){
		delete []this->file_browser;
		this->file_browser=NULL;
	}
}
//___________
//public
//Set the number of file-browser (Sys_One_Filechooser_Wid). They are also alocated and set to the dialog
void HydGui_Profil2Dgm_Converter_Dia::set_number_file_browser(const int number, QIcon icon){
	this->number_browser=number;
	this->setWindowIcon(icon);

	this->file_browser=new Sys_One_Filechooser_Wid[this->number_browser];

	for(int i=0; i< this->number_browser;i++){
		this->file_browser[i].setParent(this);
		this->browser_layout.addWidget(&this->file_browser[i]);
	}

	//connect the widgets for the path
	for(int i=0; i< this->number_browser; i++){
		for(int j=0;  j<this->number_browser; j++){
			QObject::connect(&(this->file_browser[i]),SIGNAL(send_current_path(QString )),&(this->file_browser[j]),SLOT(set_current_path(QString)));
		}

	}

	//add the layout to the dialog
	ui.chooser_frame->setLayout(&this->browser_layout);
	this->adjustSize();
}
//Get the number of file-browser (Sys_One_Filechooser_Wid) used in the dialog
int HydGui_Profil2Dgm_Converter_Dia::get_number_file_browser(void){
	return this->number_browser;
}
//Get a pointer to the file-browser given by the index
Sys_One_Filechooser_Wid* HydGui_Profil2Dgm_Converter_Dia::get_ptr_file_browser(const int index){
	if(index<0 || index >= this->number_browser){
		return NULL;
	}
	else{
		return &this->file_browser[index];
	}
}
//Get the text (path and filename) of the file-browser given by the indexd
string HydGui_Profil2Dgm_Converter_Dia::get_file_path(const int index){
	if(index<0 || index >= this->number_browser){
		return label::not_defined;
	}
	else{
		return this->file_browser[index].get_file_name();
	}
}
//Set the text of the main text-label
void HydGui_Profil2Dgm_Converter_Dia::set_main_text_label(const string text){
	ui.main_text->setText(text.c_str());
}
//Set the window title
void HydGui_Profil2Dgm_Converter_Dia::set_window_title(const string text){
	this->setWindowTitle(text.c_str());
}
//Get the number of additional streamlines
int HydGui_Profil2Dgm_Converter_Dia::get_number_additional_streamlines(void){
	return this->ui.spinBox_stream->value();
}
//Get the number of profiles in file
int HydGui_Profil2Dgm_Converter_Dia::get_number_profiles_in_file(void){
	return this->ui.spinBox_profil->value();
}
//Get density along stream lines
double HydGui_Profil2Dgm_Converter_Dia::get_dens_streamline(void){
	return this->ui.doubleSpinBox_dens->value();
}
//Get offset for x-coordinate
double HydGui_Profil2Dgm_Converter_Dia::get_offset_x(void){
	return this->ui.spinBox_off_x->value();
}
//Get offset for y-coordinate
double HydGui_Profil2Dgm_Converter_Dia::get_offset_y(void){
	return this->ui.spinBox_offy->value();
}
//Start the dialog
bool HydGui_Profil2Dgm_Converter_Dia::start_dialog(void){
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