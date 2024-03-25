//#include "Sys_Member_Int_BaseWid.h"
#include "Sys_Headers_Precompiled.h"

//Deafult Constructor
Sys_Member_Int_BaseWid::Sys_Member_Int_BaseWid(QWidget *parent) : QWidget(parent), inc_step(5) {
	//Qt Stuff
	this->setupUi(this);

	//SELF
	this->set_editable(false);

	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Member_Int_BaseWid)), _sys_system_modules::SYS_SYS);
}
//Default Destructor
Sys_Member_Int_BaseWid::~Sys_Member_Int_BaseWid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Member_Int_BaseWid), _sys_system_modules::SYS_SYS);
}
//_________
//public
//Changes the text of the QLabel
void Sys_Member_Int_BaseWid::set_label_text(const string text) {
	QString qText = text.c_str();
	this->label->setText(qText);
}

//Changes tooltip text
void Sys_Member_Int_BaseWid::set_tooltip(const string tooltip) {
	QString qTooltip = tooltip.c_str();
	this->label->setToolTip(qTooltip);
}

//Set the range of the QSpinBox
void Sys_Member_Int_BaseWid::set_range(const int min, const int max) {
	this->spinBox->setRange(min, max);
	double buffer=(max-min)/10.0;
	int inc_buff=1;
	if (buffer<1.0){
		inc_buff=1;
	}
	else{
		inc_buff=(int)buffer;
	}
	this->set_increment(inc_buff);
}

//Set the incerement of QSpinBox
void Sys_Member_Int_BaseWid::set_increment(int range) {
	//int inc = range * (this->inc_step / 100.0);

	//if (inc < 1) {
	//	inc = 1;
	//}
	this->spinBox->setSingleStep(range);
}

//Set the QSpinBox value
void Sys_Member_Int_BaseWid::set_value(const int val) {
	this->spinBox->setValue(val);
}

//Get the QSpinBox value
int Sys_Member_Int_BaseWid::get_value(void) {
	return this->spinBox->value();
}

//Change editable status
void Sys_Member_Int_BaseWid::set_editable(bool edit) {
	this->spinBox->setReadOnly(!edit); 
}

//Returns editable status
bool Sys_Member_Int_BaseWid::is_editable(void) {
	return this->spinBox->isReadOnly();
}

//Changes the width of the combo box	
void Sys_Member_Int_BaseWid::set_box_width(const double width) {
	this->spinBox->setMinimumWidth(width);
}
