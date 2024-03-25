#include "Risk_Headers_Precompiled.h"
//#include "RiskGui_Catchment_Scenario_Dia.h"


//Default constructor
RiskGui_Catchment_Scenario_Dia::RiskGui_Catchment_Scenario_Dia(QWidget *parent): QDialog(parent){
	this->setupUi(this);

	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(RiskGui_Catchment_Scenario_Dia), _sys_system_modules::RISK_SYS);

}
//Default destructor
RiskGui_Catchment_Scenario_Dia::~RiskGui_Catchment_Scenario_Dia(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(RiskGui_Catchment_Scenario_Dia), _sys_system_modules::RISK_SYS);
}
//________
//public
//Set up the dialog
void RiskGui_Catchment_Scenario_Dia::set_up_dialog(Risk_Catchment_Break_Scenario_List *list){
	this->tableWidget->set_up_widget(list);
	this->list=list;
}
//Start the dialog it returns true by acception, false by rejection
bool RiskGui_Catchment_Scenario_Dia::start_dialog(void){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{
		this->list->set_all_in_list_selected(false);
		QModelIndexList my_list=this->tableWidget->get_ptr_table_widget()->selectionModel()->selectedRows(0);
		int buffer=0;
		for(int i=0; i<my_list.count(); i++){
			buffer=this->tableWidget->get_ptr_table_widget()->item(my_list.at(i).row(),0)->data(Qt::DisplayRole).toInt();
			this->list->get_ptr_scenario(buffer)->set_is_selected(true);
		}
		
		return true;
	}

}