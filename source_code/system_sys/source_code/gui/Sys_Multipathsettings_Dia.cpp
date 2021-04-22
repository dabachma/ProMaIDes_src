#include "Sys_Headers_Precompiled.h"
//#include "Sys_Multipathsettings_Dia.h"

//Constructor
Sys_Multipathsettings_Dia::Sys_Multipathsettings_Dia(QWidget *parent) : QDialog(parent){
	this->number_files=0;
	ui.setupUi(this);
	ui.tableWidget->setColumnCount(1);
    ui.tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui.tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Select Files"));
	ui.tableWidget->setAlternatingRowColors(true);
	this->filter = "";
	//slot connectiong
	QObject::connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(ui.spinBox, SIGNAL(valueChanged(int)), this, SLOT(spinboxValueChanged(int)));

	

	this->old_spinbox_value=0;
	this->ui.spinBox->setValue(1);
		//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Multipathsettings_Dia)), _sys_system_modules::SYS_SYS);
}
//Destructor
Sys_Multipathsettings_Dia::~Sys_Multipathsettings_Dia(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Multipathsettings_Dia), _sys_system_modules::SYS_SYS);
}
//___________
//public
//Returns all the selected files
QStringList Sys_Multipathsettings_Dia::getFileslist(void){
	return this->file_list;
}
//Get the filename for the given index
string Sys_Multipathsettings_Dia::get_filename(const int index){
	string name;
	if(index<0 || index>=this->number_files){
		name=label::not_set;
	}
	else{
		name=this->file_list.at(index).toStdString();

	}
	
	return name;
}
//Returns the number of selected files
int Sys_Multipathsettings_Dia::getFilesnumber(void){
	return this->number_files;
}
//Save all the selected files in a single logfile 
void Sys_Multipathsettings_Dia::saveAsFileslist(void){
	QString Fileslist = QFileDialog::getSaveFileName(this, 
		tr("Save Directories Logfile to"), QDir::currentPath());
	QFile file(Fileslist);
	if (Fileslist.isEmpty()) return;
	// If the file exist, make a backup of teh old file
	if(QFile::exists(Fileslist)){
		QString backupName = Fileslist;
		backupName.append("_bak");
		QFile::copy(backupName, Fileslist);
	}
	file.open(QIODevice::ReadWrite | QIODevice::Text);
	for (int i = 0; i < ui.tableWidget->rowCount(); i++){
		//! Use of qobject_cast...
		//! Convert every widgets from the FileChooser class as 
		//! single widgets, so the table could handle it.
		Sys_Filechooser_Dia* filename = qobject_cast<Sys_Filechooser_Dia*>(ui.tableWidget->cellWidget(i, 0));
		QString text = filename->uiFileChooser.fileLineEdit->text();
		//! If the lineEdit are empties, display error message
		//! If not, save every files in a logfile
		if(text.isEmpty()){
			QMessageBox::warning(this, tr("Warning Message"), 
				tr("Please select all the files or remove empties rows"), QMessageBox::Ok);
			return;
		}else{
			QTextStream logfile(&file);
			logfile << text << endl;
		}
	}
	file.close();
}


//Dialog for the paths and filenames
bool Sys_Multipathsettings_Dia::asked_path_filenames(void){
	int decision=this->exec();
	//rejected
	if(decision ==0){
		return false;
	}
	//accepted; read out the list and the number of files with the methods getFileslist(void), getFilesnumber(void)
	else{
		this->set_files2list();
		return true;
	}
}
//Set a widget to the layout for alraedy existing scenario
void Sys_Multipathsettings_Dia::set_widget2existing_scenario_layout(QWidget *scenario_wid){
	this->ui.verticalLayout_exist_sc->addWidget(scenario_wid);
}
//Set a text to header text label
void Sys_Multipathsettings_Dia::set_txt2headerlabel(const string txt, QIcon icon){
	this->setWindowIcon(icon);
	this->ui.label_header->setText(txt.c_str());
}
//Set file filters
void Sys_Multipathsettings_Dia::set_file_filters(QString filter) {
	this->filter = filter;
	for (int i = 0; i < ui.tableWidget->rowCount(); i++) {
		qobject_cast<Sys_Filechooser_Dia*>(ui.tableWidget->cellWidget(i, 0))->set_file_filters(this->filter);
	}
}
//_______
//private
//Set the FileChooser widget for every rows 
void Sys_Multipathsettings_Dia::setRowItem(const bool add){
	for (int i = 0; i < ui.tableWidget->rowCount(); i++){
		if(add==true){
			Sys_Filechooser_Dia *rowItem = new Sys_Filechooser_Dia(this);
			rowItem->set_file_filters(this->filter);
			ui.tableWidget->resizeColumnToContents(0);
			//! Set the FileChooser widget at every rows of the table
			ui.tableWidget->setCellWidget(ui.tableWidget->rowCount()-1, 0, rowItem);
			ui.tableWidget->setCurrentCell(ui.tableWidget->rowCount(), 0);
			ui.tableWidget->setRowHeight(i, 40);
		}
	}
}
//_________
//private slot
//The spin box value is changed 
void Sys_Multipathsettings_Dia::spinboxValueChanged(const int number){
	if(abs(number-this->old_spinbox_value)<=1){
		ui.tableWidget->setRowCount(number);
		if(number>this->old_spinbox_value){
			this->setRowItem(true);
		}
		else{
			this->setRowItem(false);
		}
		this->old_spinbox_value=number;
	}
	else{
		ui.spinBox->setValue(this->old_spinbox_value);
	}

}
//Set the selected files into a list
void Sys_Multipathsettings_Dia::set_files2list(void){
	for (int i = 0; i < ui.tableWidget->rowCount(); i++){
		Sys_Filechooser_Dia* filename = qobject_cast<Sys_Filechooser_Dia*>(ui.tableWidget->cellWidget(i, 0));
		QString text = filename->uiFileChooser.fileLineEdit->text();
		if(!((text.isEmpty())|| (text == NULL))){
			this->file_list.append(text);
		}
	}
	this->number_files=this->file_list.size();
}
