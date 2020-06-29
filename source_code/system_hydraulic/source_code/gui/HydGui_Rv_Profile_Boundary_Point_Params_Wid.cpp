//#include "HydGui_Rv_Profile_Boundary_Point_Params_Wid.h"
#include "Hyd_Headers_Precompiled.h"

//Default constructor
HydGui_Rv_Profile_Boundary_Point_Params_Wid::HydGui_Rv_Profile_Boundary_Point_Params_Wid(DataRole role, QWidget *parent) : QWidget(parent) {
	this->setupUi(this);
	this->sc_id = -1;
	this->previous_precision = 2;

	this->boundary_point_condition->set_label_text("Applied");
	this->boundary_point_condition->set_tooltip("Point boundary condition is applied?");
	this->boundary_point_stationary->set_label_text("Stationary");
	this->boundary_point_stationary->set_tooltip("Stationary discharge value or instationary boundary curve");
	this->boundary_point_value->set_label_text("Value [m³/s]");

	this->boundary_point_stationary->setEnabled(false);
	this->boundary_point_value->setEnabled(false);
	

	switch(role) {
		case 0: //DisplayRole
			break;
		case 1: //EditRole
			this->set_editable(true);
			this->set_condition(false);
	}

	QObject::connect(this->boundary_point_condition->checkBox, SIGNAL(clicked(bool)), this, SLOT(set_condition(bool)));
	QObject::connect(this->boundary_point_stationary->checkBox, SIGNAL(clicked(bool)), this, SLOT(set_stationary(bool)));

	this->set_default_values();
	//Count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydGui_Rv_Profile_Boundary_Point_Params_Wid), _sys_system_modules::HYD_SYS);
}
//Default destructor
HydGui_Rv_Profile_Boundary_Point_Params_Wid::~HydGui_Rv_Profile_Boundary_Point_Params_Wid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(HydGui_Rv_Profile_Boundary_Point_Params_Wid), _sys_system_modules::HYD_SYS);
}
//______________
//public methods
//Method to set the whole widget editable
void HydGui_Rv_Profile_Boundary_Point_Params_Wid::set_editable(const bool state) {
	this->boundary_point_condition->set_editable(state);
	this->boundary_point_stationary->set_editable(state);
	this->boundary_point_value->set_editable(state);
}
//Set the scenario id
void HydGui_Rv_Profile_Boundary_Point_Params_Wid::set_sc_id(const int id) {
	this->sc_id = id;
}
//Get the scenario id
int HydGui_Rv_Profile_Boundary_Point_Params_Wid::get_sc_id(void) {
	return this->sc_id;
}
//Set the widgets DataRole
void HydGui_Rv_Profile_Boundary_Point_Params_Wid::set_data_role(DataRole role) {
	this->role = role;
}
//Set the stationary and the value properties
void HydGui_Rv_Profile_Boundary_Point_Params_Wid::set_values(const bool stationary, const double value) {
	this->set_condition(true);
	this->set_stationary(stationary);
	this->boundary_point_value->set_value(value);
}
//Set default values
void HydGui_Rv_Profile_Boundary_Point_Params_Wid::set_default_values(void) {
	this->boundary_point_condition->set_value(false);
	this->boundary_point_stationary->set_value(false);
	this->boundary_point_value->set_value(0);
}
//____________
//public slots
//Set condition, configures the GUI 
void HydGui_Rv_Profile_Boundary_Point_Params_Wid::set_condition(const bool state) {
	this->boundary_point_stationary->setEnabled(state);
	this->boundary_point_value->setEnabled(state);
	this->boundary_point_condition->set_value(state);
}
//Set stationary, configures the GUI
void HydGui_Rv_Profile_Boundary_Point_Params_Wid::set_stationary(const bool state) {
	this->boundary_point_stationary->set_value(state);
	
	//stationary
	if (state==true) {
		//this->previous_precision = this->boundary_point_value->spinBox->decimals();
		this->boundary_point_value->set_precision(2);
		this->boundary_point_value->set_label_text("Value [m³/s]");
		this->boundary_point_value->set_range(-999999999.0, 9999999999.0);
		this->boundary_point_value->set_tooltip("Discharge value");
	}
	//instationary
	else {
		this->boundary_point_value->set_precision(0);
		this->boundary_point_value->set_label_text("Curve no. [-]");
		this->boundary_point_value->set_tooltip("Boundary curve number");
	}
}
