#include "Hyd_Headers_Precompiled.h"
//#include "HydGui_Bound_Scenario_Data_Dia.h"

//Default constructor
HydGui_Bound_Scenario_Data_Dia::HydGui_Bound_Scenario_Data_Dia(QWidget *parent): QDialog(parent){
	this->setupUi(this);
	this->number_scenario=0;
	this->number_sc_exist=0;
	this->number_sc_new=0;
	this->boundary_data=NULL;

	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydGui_Bound_Scenario_Data_Dia), _sys_system_modules::HYD_SYS);
}
//Default destructor
HydGui_Bound_Scenario_Data_Dia::~HydGui_Bound_Scenario_Data_Dia(void){
	this->delete_bound_scenario();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(HydGui_Bound_Scenario_Data_Dia), _sys_system_modules::HYD_SYS);
}
//________
//public
//Set the dialog an the appending table widget with the given scenario data
void HydGui_Bound_Scenario_Data_Dia::set_dialog(const int number, string *file_names, QSqlDatabase *ptr_database, const bool new_base_sc){
	this->delete_bound_scenario();
	this->number_sc_new=number;
	//set existing scenarios
	Hyd_Boundary_Szenario_Management my_manager;
	if(new_base_sc==false){
		my_manager.set_szenario_per_db(ptr_database);
		this->number_sc_exist=my_manager.get_number_sz();
	}
	else{
		this->number_sc_exist=0;	
	}

	this->number_scenario=this->number_sc_exist+this->number_sc_new;
	this->allocate_bound_scenario();

	//set data to the table widget
	this->tableWidget->set_rows(this->number_scenario);
	this->tableWidget->set_rows_existing_sc(this->number_sc_exist, my_manager.get_ptr_boundary_scenario());
	this->tableWidget->set_rows_new_sc(this->number_sc_new, file_names, this->number_sc_exist);
}
//Start the dialog; it returns true by acception, false by rejection
bool HydGui_Bound_Scenario_Data_Dia::start_dialog(void){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{	
		this->set_dialog_data2scenario();
		return true;
	}
}
//Get the data of a hydraulic boundary scenario
void HydGui_Bound_Scenario_Data_Dia::get_scenario_data(Hyd_Boundary_Szenario *data, const int index){
	if(index<0 || index>=this->number_sc_new){
		return;
	}
	else{
		data->set_members(&(this->boundary_data[index]));
	}
	
}
//Get a pointer to the scenario data table widget
QWidget *HydGui_Bound_Scenario_Data_Dia::get_ptr_scenario_data_widget(void){
	return this->tableWidget;
}
//Set new boundary scenario directly without a GUI dialog for an import via the hydraulic system
void HydGui_Bound_Scenario_Data_Dia::set_new_boundary_scenario_directly(Hyd_Boundary_Scenario_List *list){
	this->delete_bound_scenario();
	this->number_sc_new=list->get_number_in_list();
	this->allocate_bound_scenario();
	for(int i=0; i<this->number_sc_new; i++){
		this->boundary_data[i]=*list->get_ptr_scenario(i);
	}
}
//___________
//private
//Set the data in the dialog to the hydraulic boundary scenarios
void HydGui_Bound_Scenario_Data_Dia::set_dialog_data2scenario(void){
	for(int i=0; i< this->number_sc_new; i++){
		this->tableWidget->set_data2bound_scenario(i+this->number_sc_exist, &this->boundary_data[i]);
	}
}
//Allocate the hydraulic boundary scenarios
void HydGui_Bound_Scenario_Data_Dia::allocate_bound_scenario(void){
	try{
		this->boundary_data=new Hyd_Boundary_Szenario[this->number_sc_new];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the hydraulic boundary scenarios
void HydGui_Bound_Scenario_Data_Dia::delete_bound_scenario(void){
	if(this->boundary_data!=NULL){
		delete []this->boundary_data;
		this->boundary_data=NULL;
	}
}
//set the error
Error HydGui_Bound_Scenario_Data_Dia::set_error(const int err_type){
	string place="HydGui_Bound_Scenario_Data_Dia::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;
	switch (err_type){
		case 0://bad alloc
			place.append("allocate_bound_scenario(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;		
		default:
			place.append("set_error(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}