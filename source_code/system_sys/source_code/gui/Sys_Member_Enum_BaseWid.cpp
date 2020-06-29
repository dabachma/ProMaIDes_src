//#include "Sys_Member_Enum_BaseWid.h"
#include "Sys_Headers_Precompiled.h"

//Deafult Constructor
Sys_Member_Enum_BaseWid::Sys_Member_Enum_BaseWid(QWidget *parent) : QWidget(parent) {
	//Qt Stuff
	this->setupUi(this);

	//SELF
	this->edit_flag = false;
	this->set_editable(false);
	this->last_index = 0;

	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Member_Enum_BaseWid)), _sys_system_modules::SYS_SYS);
}
//Default Destructor
Sys_Member_Enum_BaseWid::~Sys_Member_Enum_BaseWid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Member_Enum_BaseWid), _sys_system_modules::SYS_SYS);
}
//____________
//public
//Changes the text of the QLabel
void Sys_Member_Enum_BaseWid::set_label_text(const string text) {
	QString qText = text.c_str();
	this->label->setText(qText);
}

//Changes tooltip text
void Sys_Member_Enum_BaseWid::set_tooltip(const string tooltip) {
	QString qTooltip = tooltip.c_str();
	this->label->setToolTip(qTooltip);
}

//Set the Combobox items
void Sys_Member_Enum_BaseWid::set_items(const string* items, const int len) {
	QStringList qItems;

	for (int i=0; i<len; i++) {
		QString qItem = items[i].c_str();
		qItems.append(qItem);
	}
	this->comboBox->addItems(qItems);
}

//Set the QCombobox value by item string
//void Sys_Member_Enum_BaseWid::set_current_value(const string val) {
//	//
//}

//Set the QCombobox value by index
void Sys_Member_Enum_BaseWid::set_current_value(const int index) {
	this->comboBox->setCurrentIndex(index);
	this->last_index = index;
}

//Get the QCombobox value (item, string)
string Sys_Member_Enum_BaseWid::get_current_value(void) {
	return this->comboBox->currentText().toStdString();
}

//Get the QCombobox value (index, int)	
int Sys_Member_Enum_BaseWid::get_current_index(void) {
	return this->comboBox->currentIndex();
}

//Change editable status	
void Sys_Member_Enum_BaseWid::set_editable(const bool edit) {
	this->edit_flag = edit;
	this->last_index = this->comboBox->currentIndex();
	if (edit) {
		QObject::disconnect(this->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_currentIndexChanged(int)));
	}
	else {
		QObject::connect(this->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_currentIndexChanged(int)));
	}
}

//Returns editable status	
bool Sys_Member_Enum_BaseWid::is_editable(void) {
	return this->edit_flag;
}

//Sets last_index to current index and then current to new_index
void Sys_Member_Enum_BaseWid::on_comboBox_currentIndexChanged(int new_index) {
	this->set_current_value(this->last_index);
	new_index = new_index + 1;
}