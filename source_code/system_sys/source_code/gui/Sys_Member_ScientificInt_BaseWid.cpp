//#include "Sys_Member_ScientificInt_BaseWid.h"
#include "Sys_Headers_Precompiled.h"

//Deafult Constructor
Sys_Member_ScientificInt_BaseWid::Sys_Member_ScientificInt_BaseWid(QWidget *parent) : QWidget(parent), inc_step(5) {
	//Qt Stuff
	this->setupUi(this);

	//SELF
	this->set_editable(false);
	this->base_spinBox->setRange(0, 9);

	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Member_ScientificInt_BaseWid)), _sys_system_modules::SYS_SYS);
}
//Default Destructor
Sys_Member_ScientificInt_BaseWid::~Sys_Member_ScientificInt_BaseWid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Member_ScientificInt_BaseWid), _sys_system_modules::SYS_SYS);
}
//__________
//public
//Changes the text of the QLabel
void Sys_Member_ScientificInt_BaseWid::set_label_text(const string text) {
	QString qText = text.c_str();
	this->label->setText(qText);
}

//Changes tooltip text
void Sys_Member_ScientificInt_BaseWid::set_tooltip(const string tooltip) {
	QString qTooltip = tooltip.c_str();
	this->label->setToolTip(qTooltip);
}

//Set the range of the QSpinBox
void Sys_Member_ScientificInt_BaseWid::set_range(const int min, const int max) {
	this->exp_spinBox->setRange(min, max);
	this->set_increment(max-min);
}

//Set the incerement of QSpinBox
void Sys_Member_ScientificInt_BaseWid::set_increment(int range) {
	int inc = range * (this->inc_step / 100.0);

	if (inc < 1) {
		inc = 1;
	}
	this->exp_spinBox->setSingleStep(inc);
}

//Set the QSpinBox value
void Sys_Member_ScientificInt_BaseWid::set_value(int val) {
	int exp = 0;
	if (val < 1 && val > 0) {
		while (val < 1) {
			val = val * 10;
			exp++;
		}
		this->base_spinBox->setValue(val);
		this->exp_spinBox->setValue(-exp);
	}
	else if (val > 1) {
		while (val > 10) {
			val = val / 10;
			exp++;
		}
		this->base_spinBox->setValue(val);
		this->exp_spinBox->setValue(exp);
	}
	else if (val > -1 && val < 0) {
		while (val > -1) {
			val = val * 10;
			exp++;
		}
		this->base_spinBox->setValue(val);
		this->exp_spinBox->setValue(-exp);
	}
	else if (val < -1) {
		while (val < -10) {
			val = val / 10;
			exp++;
		}
		this->base_spinBox->setValue(val);
		this->exp_spinBox->setValue(exp);
	}
}

//Get the QSpinBox value
int Sys_Member_ScientificInt_BaseWid::get_value(void) {
	return this->base_spinBox->value() * pow(10.0, this->exp_spinBox->value());
}

//Change editable status
void Sys_Member_ScientificInt_BaseWid::set_editable(bool edit) {
	this->exp_spinBox->setReadOnly(!edit);
	this->base_spinBox->setReadOnly(!edit);
}

//Returns editable status
bool Sys_Member_ScientificInt_BaseWid::is_editable(void) {
	if (this->base_spinBox->isReadOnly() && this->exp_spinBox->isReadOnly()) {
		return false;
	}
	else return true;
}