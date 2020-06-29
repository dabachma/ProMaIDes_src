//#include "HydGui_Rv_Profile_Overflow_Params_Wid.h"
#include "Hyd_Headers_Precompiled.h"

//Default constructor
HydGui_Rv_Profile_Overflow_Params_Wid::HydGui_Rv_Profile_Overflow_Params_Wid(DataRole role, QWidget *parent) : QWidget(parent) {
	this->setupUi(this);

	QObject::connect(this->overflow_coupling_left->checkBox, SIGNAL(clicked(bool)), this, SLOT(set_overflow_coupling_left(bool)));
	QObject::connect(this->overflow_coupling_right->checkBox, SIGNAL(clicked(bool)), this, SLOT(set_overflow_coupling_right(bool)));

	this->set_overflow_coupling_left(false);
	this->set_overflow_coupling_right(false);

	switch(role) {
		case 0: //DisplayRole
			this->set_editable(false);
			break;
		case 1: //EditRole
			this->set_editable(true);
	}

	Sys_Memory_Count::self()->add_mem(sizeof(HydGui_Rv_Profile_Overflow_Params_Wid), _sys_system_modules::HYD_SYS);
}
//Default destructor
HydGui_Rv_Profile_Overflow_Params_Wid::~HydGui_Rv_Profile_Overflow_Params_Wid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(HydGui_Rv_Profile_Overflow_Params_Wid), _sys_system_modules::HYD_SYS);
}
//______________
//public methods
//Set the widget editable
void HydGui_Rv_Profile_Overflow_Params_Wid::set_editable(const bool state) {
	this->overflow_coupling_left->set_editable(state);
	this->overflow_coupling_right->set_editable(state);
	this->poleni_fac_left->set_editable(state);
	this->poleni_fac_right->set_editable(state);
}
//Set default values
void HydGui_Rv_Profile_Overflow_Params_Wid::set_default_values(void) {
	this->overflow_coupling_left->set_value(false);
	this->overflow_coupling_right->set_value(false);
	this->poleni_fac_left->set_value(0.65);
	this->poleni_fac_right->set_value(0.65);
}
//Set values of the display widget
void HydGui_Rv_Profile_Overflow_Params_Wid::set_values(const bool r_ov, const double pol_r, const bool l_ov, const double pol_l){
	this->set_overflow_coupling_left(l_ov);
	this->set_overflow_coupling_right(r_ov);
	this->poleni_fac_left->set_value(pol_l);
	this->poleni_fac_right->set_value(pol_r);
}
//____________
//public slots
//Set the poleni factor left enabled
void HydGui_Rv_Profile_Overflow_Params_Wid::set_overflow_coupling_left(const bool state) {
	this->poleni_fac_left->setEnabled(state);
	this->overflow_coupling_left->set_value(state);
}
//Set the poleni factor right enabled
void HydGui_Rv_Profile_Overflow_Params_Wid::set_overflow_coupling_right(const bool state) {
	this->poleni_fac_right->setEnabled(state);
	this->overflow_coupling_right->set_value(state);
}