//#include "Sys_Database_File_Dia.h"
#include "Sys_Headers_Precompiled.h"

//Default constructor
Sys_Database_File_Dia::Sys_Database_File_Dia(QWidget *parent): QDialog(parent){
    ui.setupUi(this);
}
//Default destructor
Sys_Database_File_Dia::~Sys_Database_File_Dia(void){
}
//__________________
//public
//Get the path and name of the file for the connection parameters
QString Sys_Database_File_Dia::db_file_dir(void) const{
    return ui.editDbFile->text();
}
//Get the path and name of the file, where the tables are specified
QString Sys_Database_File_Dia::table_file_dir(void) const{
	return ui.editTableFile->text();
}
//___________________
//private slots
//The ok-button is clicked, it means accepted
void Sys_Database_File_Dia::on_okButton_clicked(void){
	accept();
}
//The cancel-button is clicked, it means rejected
void Sys_Database_File_Dia::on_cancelButton_clicked(void){
	reject();
}
//Activate the file-browser for the connection file
void Sys_Database_File_Dia::on_browseDbFileButton_clicked(void){
	QString currentpath = QDir::currentPath(); 
	if(!ui.editDbFile->text().isEmpty()){
		currentpath = ui.editDbFile->text();
	}

	QString fileName = QFileDialog::getOpenFileName(this, tr("Select File"), currentpath, tr("Connection Files (*.con)"));
	if (!fileName.isNull()){ //Prüft, ob den Pfad ausgewählt wurde
		 ui.editDbFile->setText(fileName); //Table-Pfad setzen
	}
}
//Activate the file-browser for the tables file
void Sys_Database_File_Dia::on_browseTableFileButton_clicked(void){
	QString currentpath = QDir::currentPath(); 
	if(!ui.editTableFile->text().isEmpty()){
		currentpath = ui.editTableFile->text();
	}
	
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select File"), currentpath, tr("Tables Files (*.tab)"));
	if (!fileName.isNull()){ //Prüft, ob den Pfad ausgewählt wurde
		 ui.editTableFile->setText(fileName); //Table-Pfad setzen
	}
}