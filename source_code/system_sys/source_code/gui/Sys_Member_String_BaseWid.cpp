//#include "Sys_Member_String_BaseWid.h"
#include "Sys_Headers_Precompiled.h"

//Default Constructor
Sys_Member_String_BaseWid::Sys_Member_String_BaseWid(QWidget *parent) : QWidget(parent) {
	//Qt Stuff
	this->setupUi(this);

	//SELF
	this->set_editable(false);
	this->set_text(" - ");
	
	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Member_String_BaseWid)), _sys_system_modules::SYS_SYS);
}
//Default Destructor
Sys_Member_String_BaseWid::~Sys_Member_String_BaseWid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Member_String_BaseWid), _sys_system_modules::SYS_SYS);
}
//_____________
//public
//Changes the text of the QLabel
void Sys_Member_String_BaseWid::set_label_text(const string text) {
	QString qText = text.c_str();
	this->label->setText(qText);
}

//Changes tooltip text
void Sys_Member_String_BaseWid::set_tooltip(const string tooltip) {
	QString qTooltip = tooltip.c_str();
	this->label->setToolTip(qTooltip);
}

//Set the QLineEdit value
void Sys_Member_String_BaseWid::set_text(const string text) {
	QString qText = text.c_str();
	this->lineEdit->setText(qText);
}

//Get the QLineEdit value
string Sys_Member_String_BaseWid::get_text(void) {
	return this->lineEdit->text().toStdString();
}

//Change editable status
void Sys_Member_String_BaseWid::set_editable(const bool edit) {
	this->lineEdit->setReadOnly(!edit);
}

//returns editable status
bool Sys_Member_String_BaseWid::is_editable(void) {
	return this->lineEdit->isReadOnly();
}
