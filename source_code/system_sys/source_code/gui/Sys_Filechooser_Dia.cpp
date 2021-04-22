//#include "Sys_Filechooser_Dia.h"
#include "Sys_Headers_Precompiled.h"

//constructor
Sys_Filechooser_Dia::Sys_Filechooser_Dia(QWidget *parent) : QWidget(parent){
	uiFileChooser.setupUi(this);
	uiFileChooser.fileLineEdit->setReadOnly(true);
	QObject::connect(uiFileChooser.browseButton, SIGNAL(clicked()),this, SLOT(chooseFile()));
	this->filter = "";
	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Filechooser_Dia)), _sys_system_modules::SYS_SYS);

}
//destructor
Sys_Filechooser_Dia::~Sys_Filechooser_Dia(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Filechooser_Dia), _sys_system_modules::SYS_SYS);

}
//________
//public
//Return the text (filename+path) displayed in the lineEdit of the FileChooser widget
QString Sys_Filechooser_Dia::fileName(void){
	return uiFileChooser.fileLineEdit->text();
}
//Slot for the pushButton of the FileChooser; it allows the user to browse a file on the
void Sys_Filechooser_Dia::chooseFile(void){
	QFileDialog my_dia;
	QDir my_dir;
	QString file = my_dia.getOpenFileName(this, tr("Select File"), Sys_Project::get_current_path().c_str(), this->filter);
	if(!file.isEmpty()){
		string buffer;
		uiFileChooser.fileLineEdit->setText(file);
		buffer=my_dir.absoluteFilePath(file).toStdString();
		buffer=functions::get_file_path(buffer);
		Sys_Project::set_current_path(buffer);

	}
}
//Set file filters
void Sys_Filechooser_Dia::set_file_filters(QString filter) {
	this->filter = filter;

}