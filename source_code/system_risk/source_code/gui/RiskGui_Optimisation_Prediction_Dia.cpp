#include "Risk_Headers_Precompiled.h"
//#include "RiskGui_Optimisation_Prediction_Dia.h"


//Default constructor
RiskGui_Optimisation_Prediction_Dia::RiskGui_Optimisation_Prediction_Dia(QWidget *parent): QDialog(parent){
	this->setupUi(this);

	this->ecn_bound=0.0;
	this->eco_bound=0.0;
	this->pop_aff_bound=0.0;
	this->pop_end_bound=0.0;
	this->pys_bound=0.0;


	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QObject::connect(this->checkBox_ecn, SIGNAL(clicked(bool )), this, SLOT(change_status_ecn_spinbox(bool )));
	QObject::connect(this->checkBox_eco, SIGNAL(clicked(bool )), this, SLOT(change_status_eco_spinbox(bool )));
	QObject::connect(this->checkBox_pys, SIGNAL(clicked(bool )), this, SLOT(change_status_pys_spinbox(bool )));
	QObject::connect(this->checkBox_pop_aff, SIGNAL(clicked(bool )), this, SLOT(change_status_pop_affected_spinbox(bool )));
	QObject::connect(this->checkBox_pop_end, SIGNAL(clicked(bool )), this, SLOT(change_status_pop_endangered_spinbox(bool )));



	Sys_Memory_Count::self()->add_mem(sizeof(RiskGui_Optimisation_Prediction_Dia),_sys_system_modules::RISK_SYS);//count the memory
}
//Default destructor
RiskGui_Optimisation_Prediction_Dia::~RiskGui_Optimisation_Prediction_Dia(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(RiskGui_Optimisation_Prediction_Dia),_sys_system_modules::RISK_SYS);//count the memory
}
//_________
//public
//Start the dialog; it returns true by acception, false by rejection
bool RiskGui_Optimisation_Prediction_Dia::start_dialog(void){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{

		this->ecn_bound=this->doubleSpinBox_ecn->value();
		this->eco_bound=this->doubleSpinBox_eco->value();
		this->pop_aff_bound=this->doubleSpinBox_pop_aff->value();
		this->pop_end_bound=this->doubleSpinBox_pop_end->value();
		this->pys_bound=this->doubleSpinBox_pys->value();
		return true;
	}
}
//Set the value from the dialog
void RiskGui_Optimisation_Prediction_Dia::set_boundaries(_risk_ca_scenario_control *params){

	params->boundary_ecn_risk=this->ecn_bound;
	params->boundary_eco_risk=this->eco_bound;
	params->boundary_pop_affected_risk=this->pop_aff_bound;
	params->boundary_pop_endangered_risk=this->pop_end_bound;
	params->boundary_pys_risk=this->pys_bound;
}
//_________
//public slots
//Change the status (disable/enable) of the economical spinbox corresponding the checkbox
void RiskGui_Optimisation_Prediction_Dia::change_status_ecn_spinbox(bool enable){
	if(enable==false){
		this->ecn_bound=this->doubleSpinBox_ecn->value();
		this->doubleSpinBox_ecn->setEnabled(false);
		this->doubleSpinBox_ecn->setValue(0.0);
	}
	else{
		this->doubleSpinBox_ecn->setValue(this->ecn_bound);
		this->doubleSpinBox_ecn->setEnabled(true);
	}
}
//Change the status (disable/enable) of the ecological spinbox corresponding the checkbox
void RiskGui_Optimisation_Prediction_Dia::change_status_eco_spinbox(bool enable){
	if(enable==false){
		this->eco_bound=this->doubleSpinBox_eco->value();
		this->doubleSpinBox_eco->setEnabled(false);
		this->doubleSpinBox_eco->setValue(0.0);
	}
	else{
		this->doubleSpinBox_eco->setValue(this->eco_bound);
		this->doubleSpinBox_eco->setEnabled(true);
	}
}
//Change the status (disable/enable) of the people2risk affected people spinbox corresponding the checkbox
void RiskGui_Optimisation_Prediction_Dia::change_status_pop_affected_spinbox(bool enable){
	if(enable==false){
		this->pop_aff_bound=this->doubleSpinBox_pop_aff->value();
		this->doubleSpinBox_pop_aff->setEnabled(false);
		this->doubleSpinBox_pop_aff->setValue(0.0);
	}
	else{
		this->doubleSpinBox_pop_aff->setValue(this->pop_aff_bound);
		this->doubleSpinBox_pop_aff->setEnabled(true);
	}
}
//Change the status (disable/enable) of the people2risk endangered people spinbox corresponding the checkbox
void RiskGui_Optimisation_Prediction_Dia::change_status_pop_endangered_spinbox(bool enable){
	if(enable==false){
		this->pop_end_bound=this->doubleSpinBox_pop_end->value();
		this->doubleSpinBox_pop_end->setEnabled(false);
		this->doubleSpinBox_pop_end->setValue(0.0);
	}
	else{
		this->doubleSpinBox_pop_end->setValue(this->pop_end_bound);
		this->doubleSpinBox_pop_end->setEnabled(true);
	}
}
//Change the status (disable/enable) of the psycho-social spinbox corresponding the checkbox
void RiskGui_Optimisation_Prediction_Dia::change_status_pys_spinbox(bool enable){
	if(enable==false){
		this->pys_bound=this->doubleSpinBox_pys->value();
		this->doubleSpinBox_pys->setEnabled(false);
		this->doubleSpinBox_pys->setValue(0.0);
	}
	else{
		this->doubleSpinBox_pys->setValue(this->pys_bound);
		this->doubleSpinBox_pys->setEnabled(true);
	}
}