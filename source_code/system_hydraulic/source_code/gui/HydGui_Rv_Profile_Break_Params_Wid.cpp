//#include "HydGui_Rv_Profile_Break_Params_Wid.h"
#include "Hyd_Headers_Precompiled.h"

//Default constructor
HydGui_Rv_Profile_Break_Params_Wid::HydGui_Rv_Profile_Break_Params_Wid(DataRole role, QWidget *parent) : QWidget(parent) {
	//Qt
	this->setupUi(this);

	//Self
	this->wall_resistance_left->set_label_text("Wall resistance [(m^0.5)s]");
	this->wall_resistance_left->set_tooltip("Resistance parameter for a discontinuous break like a wall break");

	this->wall_resistance_right->set_label_text("Wall resistance [(m^0.5)s]");
	this->wall_resistance_right->set_tooltip("Resistance parameter for a discontinuous break like a wall break");

	this->abrupt_breach_open_right->set_label_text("Abrupt breach open [m]");
	this->abrupt_breach_open_right->set_tooltip("Abrupt opening width after the resistance of a discontinuous break is exceeded");

	this->abrupt_breach_open_left->set_label_text("Abrupt breach open [m]");
	this->abrupt_breach_open_left->set_tooltip("brupt opening width after the resistance of a discontinuous break is exceeded");

	this->critical_velocity_right->set_label_text("Critical velocity [m/s]");
	this->critical_velocity_right->set_tooltip("Resistance parameter of a continuous break like a dike break");

	this->critical_velocity_left->set_label_text("Critical velocity [m/s]");
	this->critical_velocity_left->set_tooltip("Resistance parameter of a continuous break like a dike break");
	this->left_dike = false;
	this->left_wall = false;
	this->right_dike = false;
	this->right_wall = false;

	//Connect Signlas to Slots
	QObject::connect(this->break_left_check, SIGNAL(clicked(bool)), this, SLOT(break_left_check_clicked(bool)));
	QObject::connect(this->break_right_check, SIGNAL(clicked(bool)), this, SLOT(break_right_check_clicked(bool)));
	QObject::connect(this->wall_left_radio, SIGNAL(clicked(bool)), this, SLOT(wall_left_radio_clicked(bool)));
	QObject::connect(this->wall_right_radio, SIGNAL(clicked(bool)), this, SLOT(wall_right_radio_clicked(bool)));
	QObject::connect(this->dike_left_radio, SIGNAL(clicked(bool)), this, SLOT(dike_left_radio_clicked(bool)));
	QObject::connect(this->dike_right_radio, SIGNAL(clicked(bool)), this, SLOT(dike_right_radio_clicked(bool)));

	switch(role) {
		case 0: //DisplayRole
			this->set_editable(false);
		case 1: //EditRole
			this->set_editable(true);
	}

	Sys_Memory_Count::self()->add_mem(sizeof(HydGui_Rv_Profile_Break_Params_Wid), _sys_system_modules::HYD_SYS);
}
//Default destructor
HydGui_Rv_Profile_Break_Params_Wid::~HydGui_Rv_Profile_Break_Params_Wid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(HydGui_Rv_Profile_Break_Params_Wid), _sys_system_modules::HYD_SYS);
}
//______________
//public methods
void HydGui_Rv_Profile_Break_Params_Wid::set_editable(const bool state) {
	this->wall_resistance_left->set_editable(state);
	this->wall_resistance_right->set_editable(state);
	this->abrupt_breach_open_left->set_editable(state);
	this->abrupt_breach_open_right->set_editable(state);
	this->critical_velocity_left->set_editable(state);
	this->critical_velocity_right->set_editable(state);

	this->break_point_left->setReadOnly(!state);
	this->break_point_right->setReadOnly(!state);
	this->wall_left_radio->setEnabled(state);
	this->dike_left_radio->setEnabled(state);
	this->wall_right_radio->setEnabled(state);
	this->dike_right_radio->setEnabled(state);

	this->break_left_check->setEnabled(state);
	this->break_right_check->setEnabled(state);
}
//Set default values
void HydGui_Rv_Profile_Break_Params_Wid::set_default_values(void) {
	//general settings
	this->break_left_check->setChecked(false);
	this->break_left_check_clicked(false);
	this->break_right_check->setChecked(false);
	this->break_right_check_clicked(false);
	//wall settings
	this->wall_resistance_left->set_value(300.0);
	this->wall_resistance_right->set_value(300.0);
	this->abrupt_breach_open_left->set_value(5.0);
	this->abrupt_breach_open_right->set_value(5.0);
	//dike settings
	this->critical_velocity_left->set_value(0.3);
	this->critical_velocity_right->set_value(0.3);
}
//Set the left break parameters enabled
void HydGui_Rv_Profile_Break_Params_Wid::set_left_break(const bool state) {
	this->break_left_check_clicked(state);
	this->break_left_check->setChecked(state);
}
//Set the right break parameters enabled
void HydGui_Rv_Profile_Break_Params_Wid::set_right_break(const bool state) {
	this->break_right_check_clicked(state);
	this->break_right_check->setChecked(state);
}
//Set wall or dike as boundary building
void HydGui_Rv_Profile_Break_Params_Wid::set_abrupt_breach_left(const bool state) {
	this->left_wall = state;
	this->wall_left_radio_clicked(state);
	this->wall_left_radio->setChecked(state);

	this->left_dike = !state;	
	this->dike_left_radio_clicked(!state);
	this->dike_left_radio->setChecked(!state);
}
//Set wall or dike as boundary building
void HydGui_Rv_Profile_Break_Params_Wid::set_abrupt_breach_right(const bool state) {
	this->right_wall = state;
	this->wall_right_radio_clicked(state);
	this->wall_right_radio->setChecked(state);

	this->right_dike = !state;
	this->dike_right_radio_clicked(!state);
	this->dike_right_radio->setChecked(!state);
}
//Get state of abrupt breach left
bool HydGui_Rv_Profile_Break_Params_Wid::get_abrupt_breach_left(void) {
	return this->left_wall;
}
//Get state of abrupt breach right
bool HydGui_Rv_Profile_Break_Params_Wid::get_abrupt_breach_right(void) {
	return this->right_wall;
}
///Set the left break parameters
void HydGui_Rv_Profile_Break_Params_Wid::set_left_break_params(const bool left_break, _hyd_break_parameters *params, const int index_base){
	if(params==NULL){
		return;
	}
	this->set_abrupt_breach_left(params->abrupt_fails_flag);
	this->set_left_break(left_break);

	if(left_break==false){
		return;
	}
	
	//wall settings
	if(params->abrupt_fails_flag==true){
		this->wall_resistance_left->set_value(params->resistance);
		this->abrupt_breach_open_left->set_value(params->abrupt_opening);
	}
	else{
		//dike settings
		this->critical_velocity_left->set_value(params->resistance);
	}
	this->break_point_left->setValue(index_base);
	

}
//Set the right break parameters
void HydGui_Rv_Profile_Break_Params_Wid::set_right_break_params(const bool right_break, _hyd_break_parameters *params, const int index_base){
	if(params==NULL){
		return;
	}
	this->set_abrupt_breach_right(params->abrupt_fails_flag);
	this->set_right_break(right_break);
	if(right_break==false){
		return;
	}
	
	//wall settings
	if(params->abrupt_fails_flag==true){
		this->wall_resistance_right->set_value(params->resistance);
		this->abrupt_breach_open_right->set_value(params->abrupt_opening);
	}
	else{
		//dike settings
		this->critical_velocity_right->set_value(params->resistance);
	}
	this->break_point_right->setValue(index_base);
	
}
//_____________
//private slots
//Updates the widget to activate/deactivate the left break parameter
void HydGui_Rv_Profile_Break_Params_Wid::break_left_check_clicked(const bool state) {
	this->general_left_box->setEnabled(state);
	if (state == true) {
		if (this->left_dike == true) {
			this->dike_left_box->setEnabled(true);
			this->wall_left_box->setEnabled(false);
		}
		if (this->left_wall == true) {
			this->dike_left_box->setEnabled(false);
			this->wall_left_box->setEnabled(true);
		}
	}
	if (state == false) {
		this->dike_left_box->setEnabled(false);
		this->wall_left_box->setEnabled(false);
	}
}
//Updates the widget to activate/deactivate the right break parameter
void HydGui_Rv_Profile_Break_Params_Wid::break_right_check_clicked(const bool state) {
	this->general_right_box->setEnabled(state);
	if (state == true) {
		if (this->right_dike == true) {
			this->dike_right_box->setEnabled(true);
			this->wall_right_box->setEnabled(false);
		}
		if (this->right_wall == true) {
			this->dike_right_box->setEnabled(false);
			this->wall_right_box->setEnabled(true);
		}
	}
	if (state == false) {
		this->dike_right_box->setEnabled(false);
		this->wall_right_box->setEnabled(false);
	}
}
//Updates the widget to activate/deactivate the left break parameter for wall-break (discontinuous)
void HydGui_Rv_Profile_Break_Params_Wid::wall_left_radio_clicked(const bool state) {
	this->left_wall = !state;
	this->left_dike = state;
	this->wall_left_box->setEnabled(state);
	this->dike_left_box->setEnabled(!state);
}
//Updates the widget to activate/deactivate the left break parameter for dike-break (continuous)
void HydGui_Rv_Profile_Break_Params_Wid::dike_left_radio_clicked(const bool state) {
	this->left_dike = state;
	this->left_wall = !state;
	this->wall_left_box->setEnabled(!state);
	this->dike_left_box->setEnabled(state);
}
//Updates the widget to activate/deactivate the right break parameter for wall-break (discontinuous)
void HydGui_Rv_Profile_Break_Params_Wid::wall_right_radio_clicked(const bool state) {
	this->right_wall = !state;
	this->right_dike = state;
	this->wall_right_box->setEnabled(state);
	this->dike_right_box->setEnabled(!state);
}
//Updates the widget to activate/deactivate the right break parameter for dike-break (continuous)
void HydGui_Rv_Profile_Break_Params_Wid::dike_right_radio_clicked(const bool state) {
	this->right_dike = state;
	this->right_wall = !state;
	this->wall_right_box->setEnabled(!state);
	this->dike_right_box->setEnabled(state);
}