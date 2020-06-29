//#include "Sys_Output_Logfile_Dia.h"
#include "Sys_Headers_Precompiled.h"

//Constructor
Sys_Output_Logfile_Dia::Sys_Output_Logfile_Dia(QWidget *parent) : QDialog(parent){

	this->modul_type=_sys_system_modules::SYS_SYS;

	ui.setupUi(this);
	ui.typeLineEdit->setReadOnly(true);
	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(ui.browseButton, SIGNAL(clicked()), this, SLOT(browseButtonClicked()));
	QObject::connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));

	//set the current path
	ui.pathLineEdit->setText(QDir::currentPath());

	this->old_path=Sys_Project::get_current_path();

	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Output_Logfile_Dia)), _sys_system_modules::SYS_SYS);
}
// Destructor
Sys_Output_Logfile_Dia::~Sys_Output_Logfile_Dia(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Output_Logfile_Dia), _sys_system_modules::SYS_SYS);
}
//_____________
//public
//Set the text for which moduls the outputflag should be changed
void Sys_Output_Logfile_Dia::set_txt_modul_type(_sys_system_modules type){
	this->modul_type=type;
	QIcon icon;
	switch (type){
		case _sys_system_modules::SYS_SYS:
			this->text = "Change the logfile for the modul SYS";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":prom_icon");
			this->setWindowIcon(icon);
			break;
		case _sys_system_modules::FPL_SYS:
			this->text = "Change the logfile for the modul FPL";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":fpl_icon");
			this->setWindowIcon(icon);
			break;
		case _sys_system_modules::HYD_SYS:
			this->text = "Change the logfile for the modul HYD";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":hyd_icon");
			this->setWindowIcon(icon);
			break;
		case _sys_system_modules::MADM_SYS:
			this->text = "Change the logfile for the modul MADM";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":madm_icon");
			this->setWindowIcon(icon);
			break;
		case _sys_system_modules::DAM_SYS:
			this->text = "Change the logfile for the modul DAM";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":dam_icon");
			this->setWindowIcon(icon);
			break;
		case _sys_system_modules::RISK_SYS:
			this->text = "Change the logfile for the modul RISK";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":risk_icon");
			this->setWindowIcon(icon);
			break;
		case _sys_system_modules::ALT_SYS:
			this->text = "Change the logfile for the modul ALT";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":alt_icon");
			this->setWindowIcon(icon);
			break;
		case _sys_system_modules::COST_SYS:
			this->text = "Change the logfile for the modul COST";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":cost_icon");
			this->setWindowIcon(icon);
			break;
		default:
			this->text = "Change the logfile for the Modul NOTKNOWN";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
	}
}
//Set a new output file name via dialog; if it is true the old file can be closed and a new one can be opened
bool Sys_Output_Logfile_Dia::setOutputLogfile(string *new_filename, const string old_file_name){
	int decision=this->exec();

	 //rejected
	if(decision ==0){
		//nothing happen
		return false;
	}
	else{
		this->old_path=old_file_name;
		//check if path and name is set
		if(ui.pathLineEdit->text().isEmpty()==true){
			ostringstream text;
			text << "No valid path is set"<<endl;
			QMessageBox::warning(this,"Warning",text.str().c_str());

			return false;
		}
		if(ui.nameLineEdit->text().isEmpty()==true){
			ostringstream text;
			text << "No valid filename is set"<<endl;
			QMessageBox::warning(this,"Warning",text.str().c_str());

			return false;
		}

		//check if there is a file with the same name
		QDir dir = this->getOutputPath();
		QString	logfile = this->getOutputLogfile();
		QString test_name=logfile;
		test_name.append(".dat");
		QFile file(dir.filePath(test_name));
		// If the file already exists, create a backup file with the extension "_bak" and copy the content of the old file in the new one.
		if(file.exists(test_name)==true){
			QString logfileBackup = test_name;
			logfileBackup.append("_bak");
			file.copy(test_name, logfileBackup);
		}
		
		new_filename->clear();
		//set the new name
		new_filename->append(logfile.toStdString());


		return true;
	}
}
//_____________
//private
//Open the dialog to select the directory (slot)
void Sys_Output_Logfile_Dia::browseButtonClicked(void){
	QString output_path;
	this->setOutputPath(output_path);
}
// Set the current output path
void Sys_Output_Logfile_Dia::setOutputPath(QString outputpath){
	outputpath = QFileDialog::getExistingDirectory(this, 
		tr("Open the path where the logfile should be saved"),
		this->old_path.c_str(), QFileDialog::ShowDirsOnly);
	//Sys_Project::set_current_path(outputpath.toStdString());
	ui.pathLineEdit->setText(outputpath);
}
//Get the output type of the logfile
QString Sys_Output_Logfile_Dia::getOutputType(void){
	QString outputtype;
	switch (this->modul_type){
		case _sys_system_modules::SYS_SYS:
			outputtype = "SYS_";

			break;
		case _sys_system_modules::FPL_SYS:
			outputtype = "FPL_";

			break;
		case _sys_system_modules::HYD_SYS:
			outputtype = "HYD_";

			break;
		case _sys_system_modules::MADM_SYS:
			outputtype = "MADM_";

			break;
		case _sys_system_modules::DAM_SYS:
			outputtype = "DAM_";
			break;
		default:
			outputtype = "NOTKNOWN_";
	}
	return outputtype;
}
// Get the name of the logfile
QString Sys_Output_Logfile_Dia::getOutputLogfile(){
	QString logfile;
	logfile = ui.nameLineEdit->text().prepend((ui.pathLineEdit->text()).append("/%1").arg(this->getOutputType()));
	return logfile;
}
// Get the path where the logfile should be later saved
QString Sys_Output_Logfile_Dia::getOutputPath(){
	QString OutputPath;
	OutputPath = ui.pathLineEdit->text(); 
	return OutputPath; 
}