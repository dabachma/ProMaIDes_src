#include "Cost_Headers_Precompiled.h"
//#include "CostGui_Alternative_Ecn_Cost_Dia.h"


//Default constructor
CostGui_Alternative_Ecn_Cost_Dia::CostGui_Alternative_Ecn_Cost_Dia(QWidget *parent): QDialog(parent){
	this->setupUi(this);

	this->lineEdit_alt_name->setText("Unknown alternative");
	this->textEdit_alt_description->setText("No alternative description availabe");

	this->cost_data.area_cost=0.0;
	this->cost_data.building_cost=0.0;
	this->cost_data.eco_trade_off=2.0;
	this->cost_data.eco_trade_off_cost=0.0;
	this->cost_data.matching_coeff=1.0;
	this->cost_data.planning_cost=0.0;
	this->cost_data.planning_cost_hoai=true;
	this->cost_data.total_cost=0.0;

	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(this->checkBox_planning_cost_hoai, SIGNAL(toggled(bool )), this, SLOT(check_planning_cost(bool )));
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(CostGui_Alternative_Ecn_Cost_Dia), _sys_system_modules::COST_SYS);

}
//Default destructor
CostGui_Alternative_Ecn_Cost_Dia::~CostGui_Alternative_Ecn_Cost_Dia(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(CostGui_Alternative_Ecn_Cost_Dia), _sys_system_modules::COST_SYS);

}
//________
//public
//Set the costs to the dialog and the alternative data to the dialog
void CostGui_Alternative_Ecn_Cost_Dia::set_dialog(const string alt_name, const string alt_descript, const _cost_ecn_cost_data cost_data){
	this->lineEdit_alt_name->setText(alt_name.c_str());
	if(alt_descript.empty()==true){
		this->textEdit_alt_description->setText("No alternative description availabe");
	}
	else{
		this->textEdit_alt_description->setText(alt_descript.c_str());
	}

	//set the already given cost data

	this->cost_data=cost_data;

	this->doubleSpinBox_area_cost->setValue(this->cost_data.area_cost);
	this->doubleSpinBox_build_cost->setValue(this->cost_data.building_cost);
	this->doubleSpinBox_eco_trade_off->setValue(this->cost_data.eco_trade_off);
	this->doubleSpinBox_matching_coef->setValue(this->cost_data.matching_coeff);
	this->checkBox_planning_cost_hoai->setChecked(this->cost_data.planning_cost_hoai);
	if(this->cost_data.planning_cost_hoai==false){
		this->doubleSpinBox_planning_cost->setValue(this->cost_data.planning_cost);
	}

}
//Start the dialog it returns true by acception, false by rejection
bool CostGui_Alternative_Ecn_Cost_Dia::start_dialog(void){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{
		//set the cost data
		this->cost_data.area_cost=this->doubleSpinBox_area_cost->value();
		this->cost_data.building_cost=this->doubleSpinBox_build_cost->value();
		this->cost_data.eco_trade_off=this->doubleSpinBox_eco_trade_off->value();
		this->cost_data.matching_coeff=this->doubleSpinBox_matching_coef->value();
		this->cost_data.planning_cost_hoai=this->checkBox_planning_cost_hoai->isChecked();
		if(this->cost_data.planning_cost_hoai==false){
			this->cost_data.planning_cost=this->doubleSpinBox_planning_cost->value();
		}
		else{
			this->cost_data.planning_cost=0.0;
		}
		return true;
	}

}
//Get the cost data, which are set
_cost_ecn_cost_data CostGui_Alternative_Ecn_Cost_Dia::get_cost_data(void){
	return this->cost_data;
}
//________
//private slot
//Check planning cost settings
void CostGui_Alternative_Ecn_Cost_Dia::check_planning_cost(bool flag){
	if(flag==true){
		this->doubleSpinBox_planning_cost->setValue(0.0);
	}

	this->doubleSpinBox_planning_cost->setEnabled(!flag);
	this->label_8->setEnabled(!flag);

}