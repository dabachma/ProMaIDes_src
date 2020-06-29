//#include "Sys_Member_Dbl_Prec_BaseWid.h"
#include "Sys_Headers_Precompiled.h"

// Default Constructor
Sys_Member_Dbl_Prec_BaseWid::Sys_Member_Dbl_Prec_BaseWid(int currPrec, QWidget *parent) : QDialog(parent) {
	this->setupUi(this);
	
	this->spinBox->setValue(currPrec);
	this->icon = QPixmap(":/hydro/Preferences");
	this->label->setPixmap(icon);

	QObject::connect(this->applyButton, SIGNAL(clicked()), this, SLOT(apply_clicked()));

	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Member_Dbl_Prec_BaseWid)), _sys_system_modules::SYS_SYS);
}

// Default Destructor
Sys_Member_Dbl_Prec_BaseWid::~Sys_Member_Dbl_Prec_BaseWid(void) {
	Sys_Memory_Count::self()->minus_mem((sizeof(Sys_Member_Dbl_Prec_BaseWid)), _sys_system_modules::SYS_SYS);
}

// Gains the current SpinBox value and gives it to precApplied signal
void Sys_Member_Dbl_Prec_BaseWid::apply_clicked() {
	emit precApplied(this->spinBox->value());
	this->done(0);
}
