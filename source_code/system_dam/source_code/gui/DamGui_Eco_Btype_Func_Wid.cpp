//#include "DamGui_Eco_Btype_Func_Wid.h"
#include "Dam_Headers_Precompiled.h"

//default constructor
DamGui_Eco_Btype_Func_Wid::DamGui_Eco_Btype_Func_Wid(DataRole role, QWidget *parent) : QWidget(parent) {
	//Qt Stuff
	this->setupUi(this);

	//impact_type
	this->impact_type->set_label_text("Impact type");
	this->impact_type->set_tooltip("The keyword for the typ of the impact value of the function (x-value)");
	string impact_types[] = {"IMPACT_H", "IMPACT_V", "IMPACT_D", "IMPACT_VH"};
	this->impact_type->set_items(impact_types, 4);

	//weight
	this->weight->set_label_text("Weight");
	this->weight->set_tooltip("Weight of the vulnerability value of the function to combining them to a total vulnerability value");
	this->weight->set_dataRole(role);

	//stepwise_flag
	this->stepwise_flag->set_label_text("Stepwise flag");
	this->stepwise_flag->set_tooltip("Flag if the function is handled as a stepwise function or as a continuous one with linear interpolation for inbetween values [optional, default = false => continuous]");

	switch(role) {
		case 0: //DisplayRole
			this->set_editable(false);
			break;
		case 1: //EditRole
			this->set_editable(true);
	}

	this->set_default_values();
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(DamGui_Eco_Btype_Func_Wid), _sys_system_modules::DAM_SYS);
}

//default destructor
DamGui_Eco_Btype_Func_Wid::~DamGui_Eco_Btype_Func_Wid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(DamGui_Eco_Btype_Func_Wid), _sys_system_modules::DAM_SYS);
}

//_______
//public
//method to set the title of the main groupbox
void DamGui_Eco_Btype_Func_Wid::set_groupBox_title(string text) {
	QString qtext = text.c_str();
	this->groupBox->setTitle(qtext);
}

//method to set the whole widget editable
void DamGui_Eco_Btype_Func_Wid::set_editable(const bool state) {
	this->editable = state;
	this->impact_type->set_editable(state);
	this->weight->set_editable(state);
	this->stepwise_flag->set_editable(state);
}

//common getter for private member 'editable'
bool DamGui_Eco_Btype_Func_Wid::is_editable(void) {
	return this->editable;
}

//Set default values
void DamGui_Eco_Btype_Func_Wid::set_default_values(void) {
	//damage function parameters
	this->impact_type->set_current_value(0);
	this->weight->set_value(0);
	this->stepwise_flag->set_value(false);
}

//Set members to the widget
void DamGui_Eco_Btype_Func_Wid::set_member2widget(Dam_Eco_Btype_Damage_Function *func){
	this->impact_type->set_editable(true);
	this->impact_type->set_current_value(func->impact_type);
	this->impact_type->set_editable(false);
	
	if(func->impact_type==_dam_impact_type::WATER_D){
		this->set_groupBox_title("Duration");
		this->widget->getPlotPtr()->set_axis_title("Duration [d]", "Damage factor [-]");
	}
	else if(func->impact_type==_dam_impact_type::WATER_H){
		this->set_groupBox_title("Waterlevel");
		this->widget->getPlotPtr()->set_axis_title("Waterlevel [m]", "Damage factor [-]");
	}
	else if(func->impact_type==_dam_impact_type::WATER_V){
		this->set_groupBox_title("Flow-velocity");
		this->widget->getPlotPtr()->set_axis_title("Velocity [m/s]", "Damage factor [-]");
	}
	
	this->weight->set_value(func->weight);
	this->stepwise_flag->set_value(func->stepwise_flag);

	//plot
	this->widget->getPlotPtr()->set_number_curves(1);
	this->widget->getPlotPtr()->set_curve_characteristics(0, "Impact", QwtPlotCurve::Lines, Qt::black);

	func->set_members2plot_wid(this->widget->getPlotPtr());

	this->widget->getPlotPtr()->show_plot();


}