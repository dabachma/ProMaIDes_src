#include "Sys_Headers_Precompiled.h"
//#include "Sys_Postgres_Config_Dia.h"

//Default constructor
Sys_Postgres_Config_Dia::Sys_Postgres_Config_Dia(QWidget* parent) : QDialog(parent) {

	ui.setupUi(this);

	QSettings settings("AG_FRM", "MyProMaIDes");
	QString previous_controller_path = settings.value("systray_controller_path", "").toString();
	QString previous_database_path = settings.value("systray_database_path", "").toString();

	ui.controller_path->setText(previous_controller_path);
	ui.database_path->setText(previous_database_path);


	//QObject::connect(ui.button_controller, SIGNAL(clicked()), this, SLOT(on_button_controller_clicked()));

	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Postgres_Config_Dia), _sys_system_modules::SYS_SYS);

}
//Default destructor
Sys_Postgres_Config_Dia::~Sys_Postgres_Config_Dia(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Postgres_Config_Dia), _sys_system_modules::SYS_SYS);
}

//Get postgres path
QString Sys_Postgres_Config_Dia::postgres_path(void) const {
	return ui.controller_path->text();
}
//Get database folder
QString Sys_Postgres_Config_Dia::database_folder(void) const {
	return ui.database_path->text();
}

//______________________
//private slots
void Sys_Postgres_Config_Dia::on_button_controller_clicked(void) {

	QString file = QFileDialog::getOpenFileName(
		this,
		tr("Select pg_ctl.exe"),
		QDir::homePath(),
		"Pg_ctl.exe (pg_ctl.exe);;All files (*)"
	);

	if (!file.isEmpty()) {
		ui.controller_path->setText(file);
	}

}

void Sys_Postgres_Config_Dia::on_button_database_clicked(void) {


	QString folderPath = QFileDialog::getExistingDirectory(
		this,
		tr("Select Database Folder"),
		QDir::homePath(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);

	if (!folderPath.isEmpty()) {
		ui.database_path->setText(folderPath);
	}

}

//The dialog is accepted 
void Sys_Postgres_Config_Dia::on_okButton_clicked(void) {
	QSettings settings("AG_FRM", "MyProMaIDes");

	settings.setValue("systray_controller_path", ui.controller_path->text());
	settings.setValue("systray_database_path", ui.database_path->text());

	accept();
}
//The dialog is rejected
void Sys_Postgres_Config_Dia::on_cancelButton_clicked(void) {
	reject();
}