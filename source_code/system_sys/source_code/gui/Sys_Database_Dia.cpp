//#include "Sys_Database_Dia.h"
#include "Sys_Headers_Precompiled.h"

//Default constructor
Sys_Database_Dia::Sys_Database_Dia(QWidget *parent): QDialog(parent){

    ui.setupUi(this);
	this->ui.drivercombo->setCurrentIndex(2);

	//set tab-order
	QWidget::setTabOrder(this->ui.editHostname, this->ui.editDatabase);
	QWidget::setTabOrder(this->ui.editDatabase, this->ui.editUsername);
	QWidget::setTabOrder(this->ui.editUsername, this->ui.editPassword);
	QWidget::setTabOrder(this->ui.editPassword, this->ui.checkBox_save_pass);
	QWidget::setTabOrder(this->ui.checkBox_save_pass, this->ui.drivercombo);
	QWidget::setTabOrder(this->ui.drivercombo, this->ui.okButton);
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Database_Dia), _sys_system_modules::SYS_SYS);

}
//Default destructor
Sys_Database_Dia::~Sys_Database_Dia(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Database_Dia), _sys_system_modules::SYS_SYS);
}
//______________________________
//public
//Get the database name
QString Sys_Database_Dia::database_name(void) const{
    return ui.editDatabase->text();
}
//Get the user name
QString Sys_Database_Dia::user_name(void) const{
    return ui.editUsername->text();
}
//Get the password
QString Sys_Database_Dia::password(void) const{
    return ui.editPassword->text();
}
//Get the password save flag
bool Sys_Database_Dia::password_save_flag(void) const{
    return this->ui.checkBox_save_pass->isChecked();
}
//Get the host name
QString Sys_Database_Dia::host_name(void) const{
    return ui.editHostname->text();
}
//Get the directory of the table fil
QString Sys_Database_Dia::table_dir(void) const{
	return ui.editTableDir->text();
}
//Get the driver type as string
QString Sys_Database_Dia::get_driver(void) const{
	return this->ui.drivercombo->currentText();
}
//Hide the browser for the directory of the table file
void Sys_Database_Dia::hide_table_file_browser(void){
	this->ui.TableDirLabel->hide();
	this->ui.editTableDir->hide();
	this->ui.browseTableDirButton->hide();
	this->adjustSize();

}
//Set predefined parameters to the dialog
void Sys_Database_Dia::set_predef_parameter2dialog(_sys_database_params params){
	this->ui.editHostname->setText(params.host_name.c_str());
	this->ui.editDatabase->setText(params.database_name.c_str());
	this->ui.editUsername->setText(params.user_name.c_str());
	this->ui.editPassword->setText(params.password.c_str());
	if(params.driver_type==_sys_driver_type::MYSQL){
		this->ui.drivercombo->setCurrentIndex(1);
	}
	else if(params.driver_type==_sys_driver_type::POSTGRESQL){
		this->ui.drivercombo->setCurrentIndex(0);
	}
	else if(params.driver_type==_sys_driver_type::UNKNOWN_DR){
		this->ui.drivercombo->setCurrentIndex(2);
	}
	this->ui.checkBox_save_pass->setChecked(params.password_saved);

}
//Enable all just the password dialog and the password save flag
void Sys_Database_Dia::enable_for_password(void){
	this->ui.editHostname->setEnabled(false);
	this->ui.editDatabase->setEnabled(false);
	this->ui.editUsername->setEnabled(false);
	this->ui.drivercombo->setEnabled(false);
}
//______________________
//private slots
//The dialog is accepted 
void Sys_Database_Dia::on_okButton_clicked(void){
	accept();
}
//The dialog is rejected
void Sys_Database_Dia::on_cancelButton_clicked(void){
	reject();
}
//The directory is browsed via a file-dialog
void Sys_Database_Dia::on_browseTableDirButton_clicked(){
	QString currentpath = QDir::currentPath(); //Setzt den aktuellen Pfad zu dem Home-Verzeichnis 
	if(!ui.editTableDir->text().isEmpty()){ //Prüft, ob die Table-Pfad-Zeile leer ist und falls ja setzt default den home-Verzeichnis
		currentpath = ui.editTableDir->text();
	}
	QString fileName = QFileDialog::getOpenFileName(this, tr(""), //Dateiauswahldialog des Betriebssystems aufrufen
                                                 currentpath,
                                                 tr("Tables Files (*.tab)"));
	if (!fileName.isNull()){ //Prüft, ob den Pfad ausgewählt wurde
		 ui.editTableDir->setText(fileName); //Table-Pfad setzen
	}
}
