#include "Sys_Headers_Precompiled.h"
//#include "Sys_Output_Flag_Dia.h"


// Constructor
Sys_Output_Flag_Dia::Sys_Output_Flag_Dia(QWidget *parent) : QDialog(parent){
	this->old_flag=false;
	this->new_flag=false;
	
	ui.setupUi(this);
	ui.checkBox->setChecked(this->old_flag);
	QObject::connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

		//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Output_Flag_Dia)), _sys_system_modules::SYS_SYS);

}
// Destructor
Sys_Output_Flag_Dia::~Sys_Output_Flag_Dia(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Output_Flag_Dia), _sys_system_modules::SYS_SYS);
}
//___________
//public
//Set the text for which moduls the outputflag should be changed
void Sys_Output_Flag_Dia::set_txt_modul_type(_sys_system_modules type){

	QIcon icon;
	switch (type){
		case _sys_system_modules::SYS_SYS:
			this->text = "Change the flag for the Modul SYS";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":prom_icon");
			this->setWindowIcon(icon);
			break;
		case _sys_system_modules::FPL_SYS:
			this->text = "Change the flag for the Modul FPL";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":fpl_icon");
			this->setWindowIcon(icon);
			break;
		case _sys_system_modules::HYD_SYS:
			this->text = "Change the flag for the Modul HYD";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":hyd_icon");
			this->setWindowIcon(icon);
			break;
		case _sys_system_modules::MADM_SYS:
			this->text = "Change the flag for the Modul MADM";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":madm_icon");
			this->setWindowIcon(icon);
			break;
		case _sys_system_modules::DAM_SYS:
			this->text = "Change the flag for the Modul DAM";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":dam_icon");
			this->setWindowIcon(icon);
			break;
		case _sys_system_modules::RISK_SYS:
			this->text = "Change the flag for the Modul RISK";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":risk_icon");
			this->setWindowIcon(icon);
			break;
		case _sys_system_modules::ALT_SYS:
			this->text = "Change the flag for the Modul ALT";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":alt_icon");
			this->setWindowIcon(icon);
			break;
		case _sys_system_modules::COST_SYS:
			this->text = "Change the flag for the Modul COST";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
			icon.addFile(":cost_icon");
			this->setWindowIcon(icon);
			break;
		default:
			this->text = "Change the flag for the Modul NOTKNOWN";
			ui.typeLineEdit->setText(text);
			ui.typeLineEdit->setReadOnly(true);
	}
}
//Set the current output flag
void Sys_Output_Flag_Dia::set_current_flag(const bool flag){
	this->old_flag=flag;
	ui.checkBox->setChecked(this->old_flag);
}
//Make the dialog and get the new detailed flag
bool Sys_Output_Flag_Dia::get_new_detailed_flag(void){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return this->old_flag;
	}
	//accepted
	else{
		this->new_flag=ui.checkBox->isChecked();
		return this->new_flag;
	}
}