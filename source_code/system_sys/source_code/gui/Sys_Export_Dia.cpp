#include "Sys_Headers_Precompiled.h"
//#include "Sys_Export_Dia.h"

//Default constructor
Sys_Export_Dia::Sys_Export_Dia(QWidget *parent) : QDialog(parent){
	
	this->ui.setupUi(this);
	//slot connection
	QObject::connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(ui.selectAll_checkBox, SIGNAL(stateChanged(int)), this, SLOT(isChecked(int)));

	//filter to file chooser
	ostringstream buffer;
	buffer <<"Text files (*.txt *.dat *.csv);; All files (*)";
	this->ui.file_chooser->set_filter(buffer.str());

	this->ui.selectAll_checkBox->setChecked(false);
	this->ui.radioButton_excel_tab->setChecked(true);
	this->ui.file_chooser->set_text_label("Export data to file");
	this->ui.file_chooser->set_choose_existing_file_flag(false);

	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Export_Dia)), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Export_Dia::~Sys_Export_Dia(void){
	
	//count the memory
	Sys_Memory_Count::self()->minus_mem((sizeof(Sys_Export_Dia)), _sys_system_modules::SYS_SYS);
}
//_______________
//public
//Set the header text label of the dialog
void Sys_Export_Dia::set_header_txt(const string txt){
	this->ui.label_header->setTitle(txt.c_str());
}
//Check whether all the items or only the selected should be exported
bool Sys_Export_Dia::getCheckBoxState(void){
	if (this->ui.selectAll_checkBox->checkState() == Qt::Checked){
		return true;
	}else if(this->ui.selectAll_checkBox->checkState() == Qt::Unchecked){
		return false;
	}else{
		return false;
	}
}
//Get the which type of export is required
_sys_export_type Sys_Export_Dia::get_export_type(void){
	_sys_export_type buffer = _sys_export_type::exp_excel_tab;

	if(this->ui.radioButton_excel_tab->isChecked()==true){
		buffer=_sys_export_type::exp_excel_tab;
	}
	else if(this->ui.radioButton_excel_comma->isChecked()==true){
		buffer=_sys_export_type::exp_excel_comma;
	}
	else if(this->ui.radioButton_tecplot->isChecked()==true){
		buffer=_sys_export_type::exp_tecplot;
	}
	return buffer;
}
//Get output file and path
string Sys_Export_Dia::get_output_file(void){
	string buffer;
	buffer = this->ui.file_chooser->get_file_name();
	return buffer;
}

//Start the dialog
bool Sys_Export_Dia::start_dialog(void){
	this->ui.selectAll_checkBox->setChecked(false);
	this->ui.file_chooser->clear_line_edit();
	int decision = this->exec();
	//rejected
	if(decision == 0){
		return false;
	}
	//accepted; 
	else{
		return true;
	}
}
//Get a pointer to the select-all check box
QCheckBox *Sys_Export_Dia::get_ptr_selectall_checkbox(void){
	return this->ui.selectAll_checkBox;
}
//Get a pointer to the select-all group box
QGroupBox * Sys_Export_Dia::get_ptr_selectall_groupbox(void){
	return this->ui.selectAll_GroupBox;
}
//Get a pointer to the radio-button tecplot-output
QRadioButton* Sys_Export_Dia::get_ptr_tecplot_radio_button(void){
	return this->ui.radioButton_tecplot; 
}
//_______________
//private slots
//Change the state of the "select all rows" checkbox, always when the checkbox being clicked
void Sys_Export_Dia::isChecked(int state){
	if(state == Qt::Checked){
		this->getCheckBoxState();
	}else if(state == Qt::Unchecked){
		this->getCheckBoxState();
	}
}
