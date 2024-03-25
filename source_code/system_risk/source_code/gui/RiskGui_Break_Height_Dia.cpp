#include "Risk_Headers_Precompiled.h"
//#include "RiskGui_Break_Height_Dia.h"

//Default constructor
RiskGui_Break_Height_Dia::RiskGui_Break_Height_Dia(QWidget *parent): QDialog(parent){
	this->setupUi(this);

	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));


	Sys_Memory_Count::self()->add_mem(sizeof(RiskGui_Break_Height_Dia),_sys_system_modules::RISK_SYS);//count the memory
}
//Default destructor
RiskGui_Break_Height_Dia::~RiskGui_Break_Height_Dia(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(RiskGui_Break_Height_Dia),_sys_system_modules::RISK_SYS);//count the memory
}
//_________
//public
//Set the dialog for a given fpl-section; data are read from database
void RiskGui_Break_Height_Dia::set_section_id(QSqlDatabase *ptr_database, const int sec_id, const _sys_system_id id){

	int number=0;
	QSqlQueryModel model;
	number=Fpl_Section::select_relevant_section_database(&model, ptr_database,id, sec_id,  false);

	if(number==0){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info<<"Section id : " << sec_id << endl;
		msg.make_second_info(info.str());
		msg.output_msg(5);
		this->set_dialog_section_label("No FPL-section is found!");
		this->okButton->setEnabled(false);
		this->doubleSpinBox_break_height->setEnabled(false);
	}

	ostringstream txt;
	txt <<"Choose the breakheight for selected FPL-section " << model.record(0).value((Fpl_Section::table->get_column_name(fpl_label::section_name)).c_str()).toString().toStdString();
	this->set_dialog_section_label(txt.str());
	this->set_structure_height( model.record(0).value((Fpl_Section::table->get_column_name(fpl_label::structure_height)).c_str()).toDouble());
	this->set_break_height( model.record(0).value((Fpl_Section::table->get_column_name(fpl_label::section_start_breach_h)).c_str()).toDouble());


}
//Start the dialog; it returns true by acception, false by rejection
bool RiskGui_Break_Height_Dia::start_dialog(void){
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
//Get the break height
double RiskGui_Break_Height_Dia::get_break_height(void){
	return this->doubleSpinBox_break_height->value();
}
//_______
//private
//Set the dialog section label
void RiskGui_Break_Height_Dia::set_dialog_section_label(const string txt){
	this->label_section->setText(txt.c_str());
}
//Set the structure height
void RiskGui_Break_Height_Dia::set_structure_height(const double height){
	ostringstream buffer;
	buffer<<height;
	this->lineEdit_struct_height->setText(buffer.str().c_str());
}
//set the break height
void RiskGui_Break_Height_Dia::set_break_height(const double break_height){
	this->doubleSpinBox_break_height->setValue(break_height);
}

//Set warning(s)
Warning RiskGui_Break_Height_Dia::set_warning(const int warn_type){
	string place="RiskGui_Break_Height_Dia::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://results datas can not submitted
			place.append("set_section_id(QSqlDatabase *ptr_database, const int sec_id, const _sys_system_id id)");
			reason="Can not find the given section id in database";
			help="Check the database of the FPL-sections";
			type=2;
			break;
		default:
			place.append("set_warning(const int warn_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=5;
	}
	msg.set_msg(place,reason,help,reaction,type);
	msg.make_second_info(info.str());
	return msg;
}