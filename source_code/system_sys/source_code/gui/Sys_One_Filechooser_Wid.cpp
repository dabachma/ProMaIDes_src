#include "Sys_Headers_Precompiled.h"
//#include "Sys_One_Filechooser_Wid.h"


//Default constructor
Sys_One_Filechooser_Wid::Sys_One_Filechooser_Wid(QWidget *parent) : QWidget(parent){
	ui.setupUi(this);
	ui.fileLineEdit->setReadOnly(true);
	QObject::connect(ui.browseButton, SIGNAL(clicked()),this, SLOT(choose_file()));
	this->file_name=label::not_set;
	this->path=label::not_set;
	this->text_label=label::not_set;

	this->flag_choose_exist_file=true;


	QString buffer;
	if(Sys_Project::get_current_path()==label::not_set){
		buffer=QDir::currentPath();
	}
	else{
		buffer=Sys_Project::get_current_path().c_str();
	}
	ui.fileLineEdit->setText(buffer);

	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_One_Filechooser_Wid)), _sys_system_modules::SYS_SYS);

}
//Default destructor
Sys_One_Filechooser_Wid::~Sys_One_Filechooser_Wid(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_One_Filechooser_Wid), _sys_system_modules::SYS_SYS);
}
//________
//public
//Set the text-label of the file-browser
void Sys_One_Filechooser_Wid::set_text_label(const string text){
	this->text_label=text;
	ui.txt_label->setText(text.c_str());
}
//Set the tool tip for the widget
void Sys_One_Filechooser_Wid::set_tooltip(const string tooltip){
	ui.txt_label->setToolTip(tooltip.c_str());
	ui.fileLineEdit->setToolTip(tooltip.c_str());
}
//Set flag if the filechooser is just for choosing an existing file (true) or creating a new file (false)
void Sys_One_Filechooser_Wid::set_choose_existing_file_flag(const bool flag){
	this->flag_choose_exist_file=flag;
}
//Clear the line edit
void Sys_One_Filechooser_Wid::clear_line_edit(void){
	this->ui.fileLineEdit->setText("");
}
//Transfer the file chooser into a path chooser (true) or other direction (false)
void Sys_One_Filechooser_Wid::transfer_file2path_chooser(const bool path_chooser_flag){
	QObject::disconnect(ui.browseButton, SIGNAL(clicked()),this, SLOT(choose_file()));
	if(path_chooser_flag==true){	
		QObject::connect(ui.browseButton, SIGNAL(clicked()),this, SLOT(choose_path()));
	}
	else{
		QObject::connect(ui.browseButton, SIGNAL(clicked()),this, SLOT(choose_file()));
	}
}
///Set filter for the file chooser
void Sys_One_Filechooser_Wid::set_filter(const string filter){
	this->filter_string=filter;
}
//Get the current path
string Sys_One_Filechooser_Wid::get_current_path(void){
	return this->path;
}
//Get the filename and the pathname 
string Sys_One_Filechooser_Wid::get_file_name(void){
	if(this->file_name.empty()==true){
		this->file_name=label::not_set;
	}
	return this->file_name;
}
//________
//public slots
//Set the current path 
void Sys_One_Filechooser_Wid::set_current_path(QString path){
	this->path=path.toStdString();	
}
//________
//private slots
//Slot for the pushButton of the file_browser; it allows the user to browse a file
void Sys_One_Filechooser_Wid::choose_file(void){
	QString file;
	
	ostringstream buffer;
	buffer << "Select " << this->text_label;

	if(this->flag_choose_exist_file==true){
		file=QFileDialog::getOpenFileName(this, buffer.str().c_str(),Sys_Project::get_current_path().c_str(), this->filter_string.c_str()) ;
	}
	else{
		file=QFileDialog::getSaveFileName(this, buffer.str().c_str(),Sys_Project::get_current_path().c_str(), this->filter_string.c_str()) ;
	}


	if(!file.isEmpty()){
		ui.fileLineEdit->setText(file);
		this->file_name=file.toStdString();
		QDir my_dir;
		this->path=my_dir.absoluteFilePath(this->file_name.c_str()).toStdString();
		this->path=functions::get_file_path(this->path);
		Sys_Project::set_current_path(this->path);
	}
}
//Slot for the pushButton of the path_browser; it allows the user to browse a path
void Sys_One_Filechooser_Wid::choose_path(void){

	QString buffer;
	if(Sys_Project::get_current_path()==label::not_set){
		buffer=QDir::currentPath();
	}
	else{
		buffer=Sys_Project::get_current_path().c_str();
	}
	this->path = QFileDialog::getExistingDirectory(this, tr("Select project path"),
		buffer, QFileDialog::ShowDirsOnly).toStdString();
	this->file_name="";
	ui.fileLineEdit->setText(this->path.c_str());
}