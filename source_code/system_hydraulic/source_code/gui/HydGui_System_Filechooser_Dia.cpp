#include "Hyd_Headers_Precompiled.h"
//#include "HydGui_System_Filechooser_Dia.h"


//Default constructor
HydGui_System_Filechooser_Dia::HydGui_System_Filechooser_Dia(QWidget *parent): QDialog(parent){
	ui.setupUi(this);
	this->name=label::not_set;
	QObject::connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(ui.browseButton, SIGNAL(clicked()), this, SLOT(open_browser()));
}
//Default destructor
HydGui_System_Filechooser_Dia::~HydGui_System_Filechooser_Dia(void){
}
//_____________
//public
//Get the file-path and the file-name, which is set in the dialog
string HydGui_System_Filechooser_Dia::get_file_name(void){
	return this->name;
}
//Start the dialog
bool HydGui_System_Filechooser_Dia::start_dialog(void){
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
//Set a text to the text label of the dialog
void HydGui_System_Filechooser_Dia::set_txt(const string txt){
	this->ui.txt_label->setText(txt.c_str());

}
//____________
//private slots
//Open the browse-dialog
void HydGui_System_Filechooser_Dia::open_browser(void){
	QString file;

	file=QFileDialog::getOpenFileName(this, tr("Select file(s)"),Sys_Project::get_current_path().c_str(), tr("Global hydraulic data files (*.ilm)")) ;


	if(!file.isEmpty()){
		this->ui.fileLineEdit->setText(file);
		this->name=file.toStdString();
		QDir my_dir;
		string path;
		path=my_dir.absoluteFilePath(this->name.c_str()).toStdString();
		path=functions::get_file_path(path);
		Sys_Project::set_current_path(path);
	}

}