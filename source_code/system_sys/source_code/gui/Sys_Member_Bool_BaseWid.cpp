//#include "Sys_Member_Bool_BaseWid.h"
#include "Sys_Headers_Precompiled.h"

//Default Constructor
Sys_Member_Bool_BaseWid::Sys_Member_Bool_BaseWid(QWidget *parent) : QWidget(parent) {
	//Qt Stuff
	this->setupUi(this);
	
	//SELF
	this->set_editable(false);
	
	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Member_Bool_BaseWid)), _sys_system_modules::SYS_SYS);
}
//Default Destructor
Sys_Member_Bool_BaseWid::~Sys_Member_Bool_BaseWid(void) {
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Member_Bool_BaseWid), _sys_system_modules::SYS_SYS);
}
//_______
//public
//Changes the text of the QLabel
void Sys_Member_Bool_BaseWid::set_label_text(const string text) {
	QString qText = text.c_str();
	this->label->setText(qText);
}
//Changes tooltip text	
void Sys_Member_Bool_BaseWid::set_tooltip(const string tooltip) {
	QString qTooltip = tooltip.c_str();
	this->label->setToolTip(qTooltip);
}
//Set the QCheckbox value	
void Sys_Member_Bool_BaseWid::set_value(const bool val) {
	this->checkBox->setChecked(val);
}
//Get the QCheckbox value	
bool Sys_Member_Bool_BaseWid::get_value(void) {
	return this->checkBox->isChecked();
}

//Returns editable status	
bool Sys_Member_Bool_BaseWid::is_editable(void) {
	return this->checkBox->isEnabled();
}
//___________
//public slots

//Change editable status	
void Sys_Member_Bool_BaseWid::set_editable(const bool state) {
	this->checkBox->setEnabled(state);
}